/*****************************************************************
|
|      Neptune - Network :: BSD Implementation
|
|      (c) 2001-2016 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
//#include <net/if_arp.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "../../Core/NptConfig.h"
#include "../../Core/NptTypes.h"
#include "../../Core/NptStreams.h"
#include "../../Core/NptThreads.h"
#include "../../Core/NptNetwork.h"
#include "../../Core/NptUtils.h"
#include "../../Core/NptConstants.h"
#include "../../Core/NptSockets.h"

#if defined(NPT_CONFIG_HAVE_NET_IF_DL_H)
#include <net/if_dl.h>
#endif
#if defined(NPT_CONFIG_HAVE_NET_IF_TYPES_H)
#include <net/if_types.h>
#endif

#if defined(NPT_CONFIG_HAVE_GETIFADDRS)
#include <ifaddrs.h>
#endif

/*----------------------------------------------------------------------
|   platform adaptation
+---------------------------------------------------------------------*/
#if !defined(IFHWADDRLEN)
#define IFHWADDRLEN 6 // default to 48 bits
#endif
#if !defined(ARPHRD_ETHER)
#define ARPHRD_ETHER 1
#endif

#if defined(_SIZEOF_ADDR_IFREQ)
#define NPT_IFREQ_SIZE(ifr) _SIZEOF_ADDR_IFREQ(*ifr)
#elif defined(NPT_CONFIG_HAVE_SOCKADDR_SA_LEN)
#define NPT_IFREQ_SIZE(ifr) (sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len)
#else
#define NPT_IFREQ_SIZE(ifr) sizeof(*ifr)
#endif

/*----------------------------------------------------------------------
|   IPv6 support
+---------------------------------------------------------------------*/
#if defined(NPT_CONFIG_HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif

/*----------------------------------------------------------------------
|   NPT_IpAddress::Any and NPT_IpAddress::Loopback
+---------------------------------------------------------------------*/
#if defined(NPT_CONFIG_ENABLE_IPV6)
const NPT_IpAddress NPT_IpAddress::Any(NPT_IpAddress::IPV6, in6addr_any.s6_addr, 16);
const NPT_IpAddress NPT_IpAddress::Loopback(NPT_IpAddress::IPV6, in6addr_loopback.s6_addr, 16);
#else
const NPT_IpAddress NPT_IpAddress::Any;
const NPT_IpAddress NPT_IpAddress::Loopback(127,0,0,1);
#endif

#if defined(NPT_CONFIG_HAVE_INET_NTOP)
/*----------------------------------------------------------------------
|   NPT_IpAddress::ToString
+---------------------------------------------------------------------*/
NPT_String
NPT_IpAddress::ToString() const
{
    NPT_String address;
    char workspace[128];
    int af = AF_INET;
#if defined(NPT_CONFIG_ENABLE_IPV6)
    if (m_Type == IPV6) {
        af = AF_INET6;
    }
#endif
    const char* string = inet_ntop(af, &m_Address[0], workspace, sizeof(workspace));
    if (string) {
        address = string;
    }
    
    return address;
}
#else
/*----------------------------------------------------------------------
|   NPT_IpAddress::ToString
+---------------------------------------------------------------------*/
NPT_String
NPT_IpAddress::ToString() const
{
    NPT_String address;
    address.Reserve(16);
    address += NPT_String::FromInteger(m_Address[0]);
    address += '.';
    address += NPT_String::FromInteger(m_Address[1]);
    address += '.';
    address += NPT_String::FromInteger(m_Address[2]);
    address += '.';
    address += NPT_String::FromInteger(m_Address[3]);

    return address;
}
#endif

#if defined(NPT_CONFIG_HAVE_INET_PTON)
/*----------------------------------------------------------------------
|   NPT_IpAddress::Parse
+---------------------------------------------------------------------*/
NPT_Result
NPT_IpAddress::Parse(const char* name)
{
    int result;
    
    // check the name
    if (name == NULL) return NPT_ERROR_INVALID_PARAMETERS;

    // clear the address
    NPT_SetMemory(&m_Address[0], 0, sizeof(m_Address));

#if defined(NPT_CONFIG_ENABLE_IPV6)
    // try IPv6 first
    result = inet_pton(AF_INET6, name, &m_Address[0]);
    if (result > 0) {
        m_Type = IPV6;
        return NPT_SUCCESS;
    }
#endif

    // try IPv4 next
    result = inet_pton(AF_INET, name, &m_Address[0]);
    if (result > 0) {
        m_Type = IPV4;
        return NPT_SUCCESS;
    }
    
    if (result == 0) {
        return NPT_ERROR_INVALID_SYNTAX;
    } else {
        return NPT_FAILURE;
    }
}
#else
/*----------------------------------------------------------------------
|   NPT_IpAddress::Parse
+---------------------------------------------------------------------*/
NPT_Result
NPT_IpAddress::Parse(const char* name)
{
    // check the name
    if (name == NULL) return NPT_ERROR_INVALID_PARAMETERS;

    // clear the address
    NPT_SetMemory(&m_Address[0], 0, sizeof(m_Address));

    // parse
    unsigned int  fragment;
    bool          fragment_empty = true;
    unsigned char address[4];
    unsigned int  accumulator;
    for (fragment = 0, accumulator = 0; fragment < 4; ++name) {
        if (*name == '\0' || *name == '.') {
            // fragment terminator
            if (fragment_empty) return NPT_ERROR_INVALID_SYNTAX;
            address[fragment++] = accumulator;
            if (*name == '\0') break;
            accumulator = 0;
            fragment_empty = true;
        } else if (*name >= '0' && *name <= '9') {
            // numerical character
            accumulator = accumulator*10 + (*name - '0');
            if (accumulator > 255) return NPT_ERROR_INVALID_SYNTAX;
            fragment_empty = false; 
        } else {
            // invalid character
            return NPT_ERROR_INVALID_SYNTAX;
        }
    }

    if (fragment == 4 && *name == '\0' && !fragment_empty) {
        m_Address[0] = address[0];
        m_Address[1] = address[1];
        m_Address[2] = address[2];
        m_Address[3] = address[3];
        return NPT_SUCCESS;
    } else {
        return NPT_ERROR_INVALID_SYNTAX;
    }
}
#endif

#if defined(NPT_CONFIG_HAVE_GETIFADDRS)
/*----------------------------------------------------------------------
|   NPT_NetworkInterface::GetNetworkInterfaces
+---------------------------------------------------------------------*/
NPT_Result
NPT_NetworkInterface::GetNetworkInterfaces(NPT_List<NPT_NetworkInterface*>& interfaces)
{
    interfaces.Clear();
    
    struct ifaddrs* addrs = NULL;
    int result = getifaddrs(&addrs);
    if (result != 0) {
        return NPT_ERROR_BASE_UNIX-errno;
    }
    
    for (struct ifaddrs* addr = addrs;
                         addr;
                         addr = addr->ifa_next) {

        // get detailed info about the interface
        NPT_Flags flags = 0;
        // process the flags
        if ((addr->ifa_flags & IFF_UP) == 0) {
            // the interface is not up, ignore it
            continue;
        }
        if (addr->ifa_flags & IFF_BROADCAST) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_BROADCAST;
        }
        if (addr->ifa_flags & IFF_LOOPBACK) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_LOOPBACK;
        }
#if defined(IFF_POINTOPOINT)
        if (addr->ifa_flags & IFF_POINTOPOINT) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_POINT_TO_POINT;
        }
#endif // defined(IFF_POINTOPOINT)
        if (addr->ifa_flags & IFF_PROMISC) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_PROMISCUOUS;
        }
        if (addr->ifa_flags & IFF_MULTICAST) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_MULTICAST;
        }

        // get a pointer to an interface we've looped over before
        // or create a new one
        NPT_NetworkInterface* interface = NULL;
        for (NPT_List<NPT_NetworkInterface*>::Iterator iface_iter = interfaces.GetFirstItem();
                                                       iface_iter;
                                                     ++iface_iter) {
            if ((*iface_iter)->GetName() == (const char*)addr->ifa_name) {
                interface = *iface_iter;
                break;
            }
        }
        if (interface == NULL) {
            // create a new interface object
            interface = new NPT_NetworkInterface(addr->ifa_name, flags);

            // add the interface to the list
            interfaces.Add(interface);
        }
        
        if (addr->ifa_addr == NULL) {
            continue;
        }
        switch (addr->ifa_addr->sa_family) {
            case AF_INET: {
                // primary address
                NPT_IpAddress primary_address(ntohl(((struct sockaddr_in*)addr->ifa_addr)->sin_addr.s_addr));

                // broadcast address
                NPT_IpAddress broadcast_address;
                if (addr->ifa_broadaddr) {
                    broadcast_address.Set(ntohl(((struct sockaddr_in*)addr->ifa_broadaddr)->sin_addr.s_addr));
                }

                // point to point address
                NPT_IpAddress destination_address;
                if (addr->ifa_dstaddr) {
                    destination_address.Set(ntohl(((struct sockaddr_in*)addr->ifa_dstaddr)->sin_addr.s_addr));
                }

                // netmask
                NPT_IpAddress netmask(0xFFFFFFFF);
                if (addr->ifa_netmask) {
                    netmask.Set(ntohl(((struct sockaddr_in*)addr->ifa_netmask)->sin_addr.s_addr));
                }

                // add the address to the interface
                NPT_NetworkInterfaceAddress iface_address(
                    primary_address,
                    broadcast_address,
                    destination_address,
                    netmask);
                interface->AddAddress(iface_address);  
                
                break;
            }

#if defined(NPT_CONFIG_ENABLE_IPV6)
            case AF_INET6: {
                // primary address
                const struct sockaddr_in6* ipv6_address = (const struct sockaddr_in6*)addr->ifa_addr;
                NPT_IpAddress primary_address(NPT_IpAddress::IPV6, ipv6_address->sin6_addr.s6_addr, 16, ipv6_address->sin6_scope_id);

                // empty broadcast address (no broadcast address for IPv6)
                NPT_IpAddress broadcast_address(NPT_IpAddress::IPV6);

                // point to point address
                NPT_IpAddress destination_address(NPT_IpAddress::IPV6);
                if (flags & NPT_NETWORK_INTERFACE_FLAG_POINT_TO_POINT) {
                    if (addr->ifa_dstaddr) {
                        ipv6_address = (const struct sockaddr_in6*)addr->ifa_dstaddr;
                        destination_address.Set(ipv6_address->sin6_addr.s6_addr, 16, ipv6_address->sin6_scope_id);
                    }
                }

                // empty netmask (does not work for IPv6)
                NPT_IpAddress netmask((NPT_IpAddress::IPV6));

                // add the address to the interface
                NPT_NetworkInterfaceAddress iface_address(
                    primary_address,
                    broadcast_address,
                    destination_address,
                    netmask);
                interface->AddAddress(iface_address);
                
                break;
            }
#endif
            
#if defined(AF_LINK) && defined(NPT_CONFIG_HAVE_SOCKADDR_DL)
            case AF_LINK: {
                struct sockaddr_dl* mac_addr = (struct sockaddr_dl*)addr->ifa_addr;
                NPT_MacAddress::Type mac_addr_type = NPT_MacAddress::TYPE_UNKNOWN;
                switch (mac_addr->sdl_type) {
#if defined(IFT_LOOP)
                    case IFT_LOOP:  mac_addr_type = NPT_MacAddress::TYPE_LOOPBACK; break;
#endif
#if defined(IFT_ETHER)
                    case IFT_ETHER: mac_addr_type = NPT_MacAddress::TYPE_ETHERNET; break;
#endif
#if defined(IFT_PPP)
                    case IFT_PPP:   mac_addr_type = NPT_MacAddress::TYPE_PPP;      break;
#endif                    
                }
                interface->SetMacAddress(mac_addr_type, 
                                         (const unsigned char*)(&mac_addr->sdl_data[mac_addr->sdl_nlen]),
                                         mac_addr->sdl_alen);
                break;
            }
#endif
        }
    }
    
    freeifaddrs(addrs);
    
    return NPT_SUCCESS;
}
#else
const unsigned int NPT_BSD_NETWORK_MAX_IFCONF_SIZE = 1<<20;

/*----------------------------------------------------------------------
|   NPT_NetworkInterface::GetNetworkInterfaces
+---------------------------------------------------------------------*/
NPT_Result
NPT_NetworkInterface::GetNetworkInterfaces(NPT_List<NPT_NetworkInterface*>& interfaces)
{
//#if defined(NPT_CONFIG_ENABLE_IPV6)
//    int net = socket(PF_INET6, SOCK_DGRAM, 0);
//#else
    int net = socket(PF_INET, SOCK_DGRAM, 0);
//#endif
    if (net < 0) {
        return NPT_ERROR_BASE_UNIX-errno;
    }
    
    // Try to get the config until we have enough memory for it
    // According to "Unix Network Programming", some implementations
    // do not return an error when the supplied buffer is too small
    // so we need to try, increasing the buffer size every time, 
    // until we get the same size twice. We cannot assume success when
    // the returned size is smaller than the supplied buffer, because
    // some implementations can return less that the buffer size if
    // another structure does not fit.
    unsigned int buffer_size = 4096; // initial guess
    unsigned int last_size = 0;
    struct ifconf config;
    unsigned char* buffer = NULL;
    for (;buffer_size < NPT_BSD_NETWORK_MAX_IFCONF_SIZE;) {
        buffer = new unsigned char[buffer_size];
        config.ifc_len = buffer_size;
        config.ifc_buf = (char*)buffer;
        if (ioctl(net, SIOCGIFCONF, &config) < 0) {
            if (errno != EINVAL || last_size != 0) {
                delete[] buffer;
                close(net);
                return NPT_ERROR_BASE_UNIX-errno;
            }
        } else {
            if ((unsigned int)config.ifc_len == last_size) {
                // same size, we can use the buffer
                break;
            }
            // different size, we need to reallocate
            last_size = config.ifc_len;
        } 
        
        // supply 4096 more bytes more next time around
        buffer_size += 4096;
        delete[] buffer;
        buffer = NULL;
    }
    if (buffer == NULL) {
        close(net);
        return NPT_ERROR_NOT_ENOUGH_SPACE;
    }
    
    // iterate over all objects
    unsigned char *entries;
    for (entries = (unsigned char*)config.ifc_req; entries < (unsigned char*)config.ifc_req+config.ifc_len;) {
        struct ifreq* entry = (struct ifreq*)entries;
                
        // point to the next entry
        entries += NPT_IFREQ_SIZE(entry);
        
        // ignore anything except AF_INET, AF_INET6 (if enabled) and AF_LINK addresses
        if (entry->ifr_addr.sa_family != AF_INET
#if defined(NPT_CONFIG_ENABLE_IPV6)
            && entry->ifr_addr.sa_family != AF_INET6
#endif
#if defined(AF_LINK)
            && entry->ifr_addr.sa_family != AF_LINK 
#endif
        ) {
            continue;
        }
        
        // get detailed info about the interface
        NPT_Flags flags = 0;
#if defined(SIOCGIFFLAGS)
        struct ifreq query = *entry;
        if (ioctl(net, SIOCGIFFLAGS, &query) < 0) continue;
        
        // process the flags
        if ((query.ifr_flags & IFF_UP) == 0) {
            // the interface is not up, ignore it
            continue;
        }
        if (query.ifr_flags & IFF_BROADCAST) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_BROADCAST;
        }
        if (query.ifr_flags & IFF_LOOPBACK) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_LOOPBACK;
        }
#if defined(IFF_POINTOPOINT)
        if (query.ifr_flags & IFF_POINTOPOINT) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_POINT_TO_POINT;
        }
#endif // defined(IFF_POINTOPOINT)
        if (query.ifr_flags & IFF_PROMISC) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_PROMISCUOUS;
        }
        if (query.ifr_flags & IFF_MULTICAST) {
            flags |= NPT_NETWORK_INTERFACE_FLAG_MULTICAST;
        }
#endif // defined(SIOCGIFFLAGS)
  
        // get a pointer to an interface we've looped over before
        // or create a new one
        NPT_NetworkInterface* interface = NULL;
        for (NPT_List<NPT_NetworkInterface*>::Iterator iface_iter = interfaces.GetFirstItem();
                                                       iface_iter;
                                                     ++iface_iter) {
            if ((*iface_iter)->GetName() == (const char*)entry->ifr_name) {
                interface = *iface_iter;
                break;
            }
        }
        if (interface == NULL) {
            // create a new interface object
            interface = new NPT_NetworkInterface(entry->ifr_name, flags);

            // add the interface to the list
            interfaces.Add(interface);   

            // get the mac address        
#if defined(SIOCGIFHWADDR)
            if (ioctl(net, SIOCGIFHWADDR, &query) == 0) {
                NPT_MacAddress::Type mac_addr_type;
                unsigned int         mac_addr_length = IFHWADDRLEN;
                switch (query.ifr_addr.sa_family) {
#if defined(ARPHRD_ETHER)
                    case ARPHRD_ETHER:
                        mac_addr_type = NPT_MacAddress::TYPE_ETHERNET;
                        break;
#endif

#if defined(ARPHRD_LOOPBACK)
                    case ARPHRD_LOOPBACK:
                        mac_addr_type = NPT_MacAddress::TYPE_LOOPBACK;
                        length = 0;
                        break;
#endif
                          
#if defined(ARPHRD_PPP)
                    case ARPHRD_PPP:
                        mac_addr_type = NPT_MacAddress::TYPE_PPP;
                        mac_addr_length = 0;
                        break;
#endif
                    
#if defined(ARPHRD_IEEE80211)
                    case ARPHRD_IEEE80211:
                        mac_addr_type = NPT_MacAddress::TYPE_IEEE_802_11;
                        break;
#endif
                                   
                    default:
                        mac_addr_type = NPT_MacAddress::TYPE_UNKNOWN;
                        mac_addr_length = sizeof(query.ifr_addr.sa_data);
                        break;
                }
                
                interface->SetMacAddress(mac_addr_type, (const unsigned char*)query.ifr_addr.sa_data, mac_addr_length);
            }
#endif
        }
          
        switch (entry->ifr_addr.sa_family) {
            case AF_INET: {
                // primary address
                NPT_IpAddress primary_address(ntohl(((struct sockaddr_in*)&entry->ifr_addr)->sin_addr.s_addr));

                // broadcast address
                NPT_IpAddress broadcast_address;
#if defined(SIOCGIFBRDADDR)
                if (flags & NPT_NETWORK_INTERFACE_FLAG_BROADCAST) {
                    if (ioctl(net, SIOCGIFBRDADDR, &query) == 0) {
                        broadcast_address.Set(ntohl(((struct sockaddr_in*)&query.ifr_addr)->sin_addr.s_addr));
                    }
                }
#endif

                // point to point address
                NPT_IpAddress destination_address;
#if defined(SIOCGIFDSTADDR)
                if (flags & NPT_NETWORK_INTERFACE_FLAG_POINT_TO_POINT) {
                    if (ioctl(net, SIOCGIFDSTADDR, &query) == 0) {
                        destination_address.Set(ntohl(((struct sockaddr_in*)&query.ifr_addr)->sin_addr.s_addr));
                    }
                }
#endif

                // netmask
                NPT_IpAddress netmask(0xFFFFFFFF);
#if defined(SIOCGIFNETMASK)
                if (ioctl(net, SIOCGIFNETMASK, &query) == 0) {
                    netmask.Set(ntohl(((struct sockaddr_in*)&query.ifr_addr)->sin_addr.s_addr));
                }
#endif

                // add the address to the interface
                NPT_NetworkInterfaceAddress iface_address(
                    primary_address,
                    broadcast_address,
                    destination_address,
                    netmask);
                interface->AddAddress(iface_address);  
                
                break;
            }

#if defined(NPT_CONFIG_ENABLE_IPV6)
            case AF_INET6: {
                // primary address
                const struct sockaddr_in6* ipv6_address = (const struct sockaddr_in6*)&entry->ifr_addr;
                NPT_IpAddress primary_address(NPT_IpAddress::IPV6, ipv6_address->sin6_addr.s6_addr, 16, ipv6_address->sin6_scope_id);

                // empty broadcast address (no broadcast address for IPv6)
                NPT_IpAddress broadcast_address(NPT_IpAddress::IPV6);

                // point to point address
                NPT_IpAddress destination_address(NPT_IpAddress::IPV6);
#if defined(SIOCGIFDSTADDR)
                if (flags & NPT_NETWORK_INTERFACE_FLAG_POINT_TO_POINT) {
                    if (ioctl(net, SIOCGIFDSTADDR, &query) == 0) {
                        ipv6_address = (const struct sockaddr_in6*)&query.ifr_addr;
                        destination_address.Set(ipv6_address->sin6_addr.s6_addr, 16, ipv6_address->sin6_scope_id);
                    }
                }
#endif

                // empty netmask (does not work for IPv6)
                NPT_IpAddress netmask((NPT_IpAddress::IPV6));

                // add the address to the interface
                NPT_NetworkInterfaceAddress iface_address(
                    primary_address,
                    broadcast_address,
                    destination_address,
                    netmask);
                interface->AddAddress(iface_address);
                
                break;
            }
#endif

#if defined(AF_LINK) && defined(NPT_CONFIG_HAVE_SOCKADDR_DL)
            case AF_LINK: {
                struct sockaddr_dl* mac_addr = (struct sockaddr_dl*)&entry->ifr_addr;
                NPT_MacAddress::Type mac_addr_type = NPT_MacAddress::TYPE_UNKNOWN;
                switch (mac_addr->sdl_type) {
#if defined(IFT_LOOP)
                    case IFT_LOOP:  mac_addr_type = NPT_MacAddress::TYPE_LOOPBACK; break;
#endif
#if defined(IFT_ETHER)
                    case IFT_ETHER: mac_addr_type = NPT_MacAddress::TYPE_ETHERNET; break;
#endif
#if defined(IFT_PPP)
                    case IFT_PPP:   mac_addr_type = NPT_MacAddress::TYPE_PPP;      break;
#endif                    
                }
                interface->SetMacAddress(mac_addr_type, 
                                         (const unsigned char*)(&mac_addr->sdl_data[mac_addr->sdl_nlen]),
                                         mac_addr->sdl_alen);
                break;
            }
#endif
        }
    }

    // free resources
    delete[] buffer;
    close(net);
    
    return NPT_SUCCESS;
}
#endif
