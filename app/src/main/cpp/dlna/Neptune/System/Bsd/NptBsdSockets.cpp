/*****************************************************************
|
|   Neptune - Sockets :: BSD/Winsock Implementation
|
|   (c) 2001-2008 Gilles Boccon-Gibod
|   Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#if (defined(_WIN32) || defined(_WIN32_WCE) || defined(_XBOX)) && !defined(__SYMBIAN32__)
#if !defined(__WINSOCK__) 
#define __WINSOCK__ 
#endif
#endif

#if defined(__WINSOCK__) && !defined(_XBOX)
#define STRICT
#define NPT_WIN32_USE_WINSOCK2
#ifdef NPT_WIN32_USE_WINSOCK2
/* it is important to include this in this order, because winsock.h and ws2tcpip.h */
/* have different definitions for the same preprocessor symbols, such as IP_ADD_MEMBERSHIP */
#include <winsock2.h>
#include <ws2tcpip.h> 
#else
#include <winsock.h>
#endif
#include <windows.h>

// XBox
#elif defined(_XBOX)
#include <xtl.h>
#include <winsockx.h>

#elif defined(__TCS__)

// Trimedia includes
#include <sockets.h>

#elif defined(__PSP__)

// PSP includes
#include <psptypes.h>
#include <kernel.h>
#include <pspnet.h>
#include <pspnet_error.h>
#include <pspnet_inet.h>
#include <pspnet_resolver.h>
#include <pspnet_apctl.h>
#include <pspnet_ap_dialog_dummy.h>
#include <errno.h>
#include <wlan.h>
#include <pspnet/sys/socket.h>
#include <pspnet/sys/select.h>
#include <pspnet/netinet/in.h>

#elif defined(__PPU__)

// PS3 includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <netex/net.h>
#include <netex/errno.h>

#else

// default includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#if !defined(__SYMBIAN32__)
#include <netinet/tcp.h>
#endif
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#endif 


#include "../../Core/NptConfig.h"
#include "../../Core/NptTypes.h"
#include "../../Core/NptStreams.h"
#include "../../Core/NptThreads.h"
#include "../../Core/NptSockets.h"
#include "../../Core/NptUtils.h"
#include "../../Core/NptConstants.h"
#include "../../Core/NptLogging.h"

/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
NPT_SET_LOCAL_LOGGER("neptune.sockets.bsd")

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const int NPT_TCP_SERVER_SOCKET_DEFAULT_LISTEN_COUNT = 20;

/*----------------------------------------------------------------------
|   WinSock adaptation layer
+---------------------------------------------------------------------*/
#if defined(__WINSOCK__)
#if defined(_XBOX)
#include "NptXboxNetwork.h"
#define SO_ERROR    0x1007          /* unsupported */
#else
#include "NptWin32Network.h"
#endif
// force a reference to the initializer so that the linker does not optimize it out
static NPT_WinsockSystem& WinsockInitializer = NPT_WinsockSystem::Initializer; 

#if defined(SetPort)
#undef SetPort
#endif

#if !defined(EWOULDBLOCK)
#define EWOULDBLOCK  WSAEWOULDBLOCK
#endif
#if !defined(EINPROGRESS)
#define EINPROGRESS  WSAEINPROGRESS
#endif
#if !defined(ECONNREFUSED)
#define ECONNREFUSED WSAECONNREFUSED
#endif
#if !defined(ECONNABORTED)
#define ECONNABORTED WSAECONNABORTED
#endif
#if !defined(ECONNRESET)
#define ECONNRESET   WSAECONNRESET
#endif
#if !defined(ETIMEDOUT)
#define ETIMEDOUT    WSAETIMEDOUT
#endif
#if !defined(ENETRESET)
#define ENETRESET    WSAENETRESET
#endif
#if !defined(EADDRINUSE)
#define EADDRINUSE   WSAEADDRINUSE
#endif
#if !defined(ENETDOWN)
#define ENETDOWN     WSAENETDOWN
#endif
#if !defined(ENETUNREACH)
#define ENETUNREACH  WSAENETUNREACH
#endif
#if !defined(EHOSTUNREACH)
#define EHOSTUNREACH WSAEHOSTUNREACH
#endif
#if !defined(ENOTCONN)
#define ENOTCONN     WSAENOTCONN
#endif
#if !defined(EAGAIN)
#define EAGAIN       WSAEWOULDBLOCK 
#endif
#if !defined(EINTR)
#define EINTR        WSAEINTR
#endif
#if !defined(SHUT_RDWR)
#define SHUT_RDWR SD_BOTH 
#endif

#if !defined(__MINGW32__)
typedef int         ssize_t;
#endif
typedef int         socklen_t;
typedef char*       SocketBuffer;
typedef const char* SocketConstBuffer;
typedef char*       SocketOption;
typedef SOCKET      SocketFd;

#define GetSocketError()                 WSAGetLastError()
#define NPT_BSD_SOCKET_IS_INVALID(_s)    ((_s) == INVALID_SOCKET)
#define NPT_BSD_SOCKET_CALL_FAILED(_e)   ((_e) == SOCKET_ERROR)
#define NPT_BSD_SOCKET_SELECT_FAILED(_e) ((_e) == SOCKET_ERROR)
#define NPT_BSD_SOCKET_INVALID_HANDLE    INVALID_SOCKET

/*----------------------------------------------------------------------
|   Trimedia adaptation layer
+---------------------------------------------------------------------*/
#elif defined(__TCS__)  // trimedia PSOS w/ Target TCP
typedef void*       SocketBuffer;
typedef const void* SocketConstBuffer;
typedef void*       SocketOption;
typedef int         SocketFd;

#define GetSocketError()                 errno
#define NPT_BSD_SOCKET_IS_INVALID(_s)    ((_s)  < 0)
#define NPT_BSD_SOCKET_CALL_FAILED(_e)   ((_e)  < 0)
#define NPT_BSD_SOCKET_SELECT_FAILED(_e) ((_e)  < 0)
#define NPT_BSD_SOCKET_INVALID_HANDLE    (-1)

/*----------------------------------------------------------------------
|   PSP adaptation layer
+---------------------------------------------------------------------*/
#elif defined(__PSP__)
typedef SceNetInetSocklen_t socklen_t;
#define timeval SceNetInetTimeval
#define inet_addr sceNetInetInetAddr
#define select sceNetInetSelect
#define socket sceNetInetSocket
#define connect sceNetInetConnect
#define bind sceNetInetBind
#define accept sceNetInetAccept
#define getpeername sceNetInetGetpeername
#define getsockopt sceNetInetGetsockopt
#define setsockopt sceNetInetSetsockopt
#define listen sceNetInetListen
#define getsockname sceNetInetGetsockname
#define sockaddr SceNetInetSockaddr
#define sockaddr_in SceNetInetSockaddrIn
#define in_addr SceNetInetInAddr
#define send  sceNetInetSend
#define sendto sceNetInetSendto
#define recv  sceNetInetRecv
#define recvfrom sceNetInetRecvfrom
#define closesocket sceNetInetClose
#define htonl sceNetHtonl
#define htons sceNetHtons
#define ntohl sceNetNtohl
#define ntohs sceNetNtohs
#define SOL_SOCKET SCE_NET_INET_SOL_SOCKET
#define AF_INET SCE_NET_INET_AF_INET
#define SOCK_STREAM SCE_NET_INET_SOCK_STREAM
#define SOCK_DGRAM SCE_NET_INET_SOCK_DGRAM
#define SO_BROADCAST SCE_NET_INET_SO_BROADCAST
#define SO_ERROR SCE_NET_INET_SO_ERROR
#define IPPROTO_IP SCE_NET_INET_IPPROTO_IP
#define IP_ADD_MEMBERSHIP SCE_NET_INET_IP_ADD_MEMBERSHIP
#define IP_MULTICAST_IF SCE_NET_INET_IP_MULTICAST_IF
#define IP_MULTICAST_TTL SCE_NET_INET_IP_MULTICAST_TTL
#define SO_REUSEADDR SCE_NET_INET_SO_REUSEADDR
#define INADDR_ANY SCE_NET_INET_INADDR_ANY
#define ip_mreq SceNetInetIpMreq
#ifdef fd_set
#undef fd_set
#endif
#define fd_set SceNetInetFdSet
#ifdef FD_ZERO
#undef FD_ZERO
#endif
#define FD_ZERO SceNetInetFD_ZERO
#ifdef FD_SET
#undef FD_SET
#endif
#define FD_SET SceNetInetFD_SET
#ifdef FD_CLR
#undef FD_CLR
#endif
#define FD_CLR SceNetInetFD_CLR
#ifdef FD_ISSET
#undef FD_ISSET
#endif
#define FD_ISSET SceNetInetFD_ISSET

#define RESOLVER_TIMEOUT (5 * 1000 * 1000)
#define RESOLVER_RETRY    5

typedef void*       SocketBuffer;
typedef const void* SocketConstBuffer;
typedef void*       SocketOption;
typedef int         SocketFd;

#define GetSocketError()                 sceNetInetGetErrno()
#define NPT_BSD_SOCKET_IS_INVALID(_s)    ((_s) < 0)
#define NPT_BSD_SOCKET_CALL_FAILED(_e)   ((_e) < 0)
#define NPT_BSD_SOCKET_SELECT_FAILED(_e) ((_e) < 0)
#define NPT_BSD_SOCKET_INVALID_HANDLE    (-1)

/*----------------------------------------------------------------------
|   PS3 adaptation layer
+---------------------------------------------------------------------*/
#elif defined(__PPU__)
#undef EWOULDBLOCK    
#undef ECONNREFUSED  
#undef ECONNABORTED  
#undef ECONNRESET    
#undef ETIMEDOUT     
#undef ENETRESET     
#undef EADDRINUSE    
#undef ENETDOWN      
#undef ENETUNREACH   
#undef EAGAIN        
#undef EINTR     
#undef EINPROGRESS

#define EWOULDBLOCK   SYS_NET_EWOULDBLOCK 
#define ECONNREFUSED  SYS_NET_ECONNREFUSED
#define ECONNABORTED  SYS_NET_ECONNABORTED
#define ECONNRESET    SYS_NET_ECONNRESET
#define ETIMEDOUT     SYS_NET_ETIMEDOUT
#define ENETRESET     SYS_NET_ENETRESET
#define EADDRINUSE    SYS_NET_EADDRINUSE
#define ENETDOWN      SYS_NET_ENETDOWN
#define ENETUNREACH   SYS_NET_ENETUNREACH
#define EAGAIN        SYS_NET_EAGAIN
#define EINTR         SYS_NET_EINTR
#define EINPROGRESS   SYS_NET_EINPROGRESS

typedef void*        SocketBuffer;
typedef const void*  SocketConstBuffer;
typedef void*        SocketOption;
typedef int          SocketFd;

#define closesocket  socketclose
#define select       socketselect

#define GetSocketError()                 sys_net_errno
#define NPT_BSD_SOCKET_IS_INVALID(_s)    ((_s) < 0)
#define NPT_BSD_SOCKET_CALL_FAILED(_e)   ((_e) < 0)
#define NPT_BSD_SOCKET_SELECT_FAILED(_e) ((_e) < 0)
#define NPT_BSD_SOCKET_INVALID_HANDLE    (-1)

// network initializer 
static struct NPT_Ps3NetworkInitializer {
    NPT_Ps3NetworkInitializer() {
        sys_net_initialize_network();
    }
    ~NPT_Ps3NetworkInitializer() {
        sys_net_finalize_network();
    }
} Ps3NetworkInitializer;

/*----------------------------------------------------------------------
|   Default adaptation layer
+---------------------------------------------------------------------*/
#else  
typedef void*       SocketBuffer;
typedef const void* SocketConstBuffer;
typedef void*       SocketOption;
typedef int         SocketFd;

#define closesocket  close
#define ioctlsocket  ioctl

#define GetSocketError()                 errno
#define NPT_BSD_SOCKET_IS_INVALID(_s)    ((_s)  < 0)
#define NPT_BSD_SOCKET_CALL_FAILED(_e)   ((_e)  < 0)
#define NPT_BSD_SOCKET_SELECT_FAILED(_e) ((_e)  < 0)
#define NPT_BSD_SOCKET_INVALID_HANDLE    (-1)

#endif

/*----------------------------------------------------------------------
|   IPv6 support
+---------------------------------------------------------------------*/
#if defined(NPT_CONFIG_ENABLE_IPV6)
#include <arpa/inet.h>

#define NPT_SOCKETS_PF_INET PF_INET6
typedef union {
    struct sockaddr         sa;
    struct sockaddr_in      sa_in;
    struct sockaddr_in6     sa_in6;
    struct sockaddr_storage sa_storage;
} NPT_sockaddr_in;

#else

#define NPT_SOCKETS_PF_INET PF_INET
typedef union {
    struct sockaddr    sa;
    struct sockaddr_in sa_in;
} NPT_sockaddr_in;

#endif

#if defined(NPT_CONFIG_ENABLE_IPV6)
/*----------------------------------------------------------------------
|   SocketAddressToInetAddress
+---------------------------------------------------------------------*/
static void
SocketAddressToInetAddress(const NPT_SocketAddress& socket_address, 
                           NPT_sockaddr_in&         inet_address,
                           socklen_t&               inet_address_length)
{
    // initialize the structure
    NPT_SetMemory(&inet_address, 0, sizeof(inet_address));
    
    // setup the structure
    inet_address_length = sizeof(sockaddr_in6);
    inet_address.sa_in6.sin6_len      = inet_address_length;
    inet_address.sa_in6.sin6_family   = AF_INET6;
    inet_address.sa_in6.sin6_port     = htons(socket_address.GetPort());
    inet_address.sa_in6.sin6_scope_id = socket_address.GetIpAddress().GetScopeId();

    NPT_IpAddress::Type type = socket_address.GetIpAddress().GetType();
    if (type == NPT_IpAddress::IPV6) {
        NPT_CopyMemory(inet_address.sa_in6.sin6_addr.s6_addr, socket_address.GetIpAddress().AsBytes(), 16);
    } else {
        NPT_SetMemory(&inet_address.sa_in6.sin6_addr.s6_addr[0], 0, 10);
        inet_address.sa_in6.sin6_addr.s6_addr[10] = 0xFF;
        inet_address.sa_in6.sin6_addr.s6_addr[11] = 0xFF;
        NPT_CopyMemory(&inet_address.sa_in6.sin6_addr.s6_addr[12], socket_address.GetIpAddress().AsBytes(), 4);
    }
}

/*----------------------------------------------------------------------
|   InetAddressToSocketAddress
+---------------------------------------------------------------------*/
static void
InetAddressToSocketAddress(const NPT_sockaddr_in& inet_address,
                           NPT_SocketAddress&     socket_address)
{
    // setup the structure
    socket_address.SetPort(inet_address.sa_in6.sin6_port);
    if (inet_address.sa.sa_family == AF_INET6) {
        socket_address.SetPort(ntohs(inet_address.sa_in6.sin6_port));
        socket_address.SetIpAddress(NPT_IpAddress(NPT_IpAddress::IPV6, inet_address.sa_in6.sin6_addr.s6_addr, 16));
    } else {
        socket_address.SetPort(ntohs(inet_address.sa_in.sin_port));
        socket_address.SetIpAddress(NPT_IpAddress(ntohl(inet_address.sa_in.sin_addr.s_addr)));
    }
}
#else
/*----------------------------------------------------------------------
|   SocketAddressToInetAddress
+---------------------------------------------------------------------*/
static void
SocketAddressToInetAddress(const NPT_SocketAddress& socket_address, 
                           NPT_sockaddr_in&         inet_address,
                           socklen_t&               inet_address_length)
{
    // initialize the structure
    NPT_SetMemory(&inet_address, 0, sizeof(inet_address));
    inet_address_length = sizeof(sockaddr_in);
    
#if defined(NPT_CONFIG_HAVE_SOCKADDR_IN_SIN_LEN)
    inet_address.sa_in.sin_len = sizeof(inet_address);
#endif

    // setup the structure
    inet_address.sa_in.sin_family      = AF_INET;
    inet_address.sa_in.sin_port        = htons(socket_address.GetPort());
    inet_address.sa_in.sin_addr.s_addr = htonl(socket_address.GetIpAddress().AsLong());
}

/*----------------------------------------------------------------------
|   InetAddressToSocketAddress
+---------------------------------------------------------------------*/
static void
InetAddressToSocketAddress(const NPT_sockaddr_in& inet_address,
                           NPT_SocketAddress&     socket_address)
{
    socket_address.SetPort(ntohs(inet_address.sa_in.sin_port));
    socket_address.SetIpAddress(NPT_IpAddress(ntohl(inet_address.sa_in.sin_addr.s_addr)));
}
#endif

/*----------------------------------------------------------------------
|   MapErrorCode
+---------------------------------------------------------------------*/
static NPT_Result 
MapErrorCode(int error)
{
    switch (error) {
        case ECONNRESET:
        case ENETRESET:
            return NPT_ERROR_CONNECTION_RESET;

        case ECONNABORTED:
            return NPT_ERROR_CONNECTION_ABORTED;

        case ECONNREFUSED:
            return NPT_ERROR_CONNECTION_REFUSED;

        case ETIMEDOUT:
            return NPT_ERROR_TIMEOUT;

        case EADDRINUSE:
            return NPT_ERROR_ADDRESS_IN_USE;

        case ENETDOWN:
            return NPT_ERROR_NETWORK_DOWN;

        case ENETUNREACH:
            return NPT_ERROR_NETWORK_UNREACHABLE;
            
        case EHOSTUNREACH:
            return NPT_ERROR_HOST_UNREACHABLE;

        case EINPROGRESS:
        case EAGAIN:
#if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
        case EWOULDBLOCK:
#endif
            return NPT_ERROR_WOULD_BLOCK;

#if defined(EPIPE)
        case EPIPE:
            return NPT_ERROR_CONNECTION_RESET;
#endif

#if defined(ENOTCONN)
        case ENOTCONN:
            return NPT_ERROR_NOT_CONNECTED;
#endif

#if defined(EINTR)
        case EINTR:
            return NPT_ERROR_INTERRUPTED;
#endif

#if defined(EACCES)
        case EACCES:
            return NPT_ERROR_PERMISSION_DENIED;
#endif

        default:
            return NPT_ERROR_ERRNO(error);
    }
}

#if defined(_XBOX)

struct hostent {
    char    * h_name;           /* official name of host */
    char    * * h_aliases;      /* alias list */
    short   h_addrtype;         /* host address type */
    short   h_length;           /* length of address */
    char    * * h_addr_list;    /* list of addresses */
#define h_addr  h_addr_list[0]  /* address, for backward compat */
};

typedef struct {
    struct hostent server;
    char name[128];
    char addr[16];
    char* addr_list[4];
} HostEnt;

/*----------------------------------------------------------------------
|   gethostbyname
+---------------------------------------------------------------------*/
static struct hostent* 
gethostbyname(const char* name)
{
    struct hostent* host = NULL;
    HostEnt*        host_entry = new HostEnt;
    WSAEVENT        hEvent = WSACreateEvent();
    XNDNS*          pDns = NULL;

    INT err = XNetDnsLookup(name, hEvent, &pDns);
    WaitForSingleObject(hEvent, INFINITE);
    if (pDns) {
        if (pDns->iStatus == 0) {
            strcpy(host_entry->name, name);
            host_entry->addr_list[0] = host_entry->addr;
            memcpy(host_entry->addr, &(pDns->aina[0].s_addr), 4);
            host_entry->server.h_name = host_entry->name;
            host_entry->server.h_aliases = 0;
            host_entry->server.h_addrtype = AF_INET;
            host_entry->server.h_length = 4;
            host_entry->server.h_addr_list = new char*[4];

            host_entry->server.h_addr_list[0] = host_entry->addr_list[0];
            host_entry->server.h_addr_list[1] = 0;

            host = (struct hostent*)host_entry;
        }
        XNetDnsRelease(pDns);
    }
    WSACloseEvent(hEvent);
    return host;
};

#endif // _XBOX

#if defined(__WINSOCK__)
/*----------------------------------------------------------------------
|   socketpair
+---------------------------------------------------------------------*/
static int
socketpair(int, int, int, SOCKET sockets[2]) // we ignore the first two params: we only use this for a strictly limited case
{
	int result = 0;

	// initialize with default values
	sockets[0] = INVALID_SOCKET;
	sockets[1] = INVALID_SOCKET;

	// create a listener socket and bind to the loopback address, any port
	SOCKET listener = socket(PF_INET, SOCK_STREAM, 0);
	if (listener == INVALID_SOCKET) goto fail;

	// bind the listener and listen for connections
	struct sockaddr_in inet_address;
	memset(&inet_address, 0, sizeof(inet_address));
	inet_address.sin_family = AF_INET;
	inet_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	int reuse = 1;
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
	result = bind(listener, (const sockaddr*)&inet_address, sizeof(inet_address));
	if (result != 0) goto fail;
	listen(listener, 1);

	// read the port that was assigned to the listener socket
    socklen_t name_length = sizeof(inet_address);
    result = getsockname(listener, (struct sockaddr*)&inet_address, &name_length); 
	if (result != 0) goto fail;

	// create the first socket 
	sockets[0] = socket(PF_INET, SOCK_STREAM, 0);
	if (sockets[0] == INVALID_SOCKET) goto fail;

	// connect the first socket
	result = connect(sockets[0], (const sockaddr*)&inet_address, sizeof(inet_address));
	if (result != 0) goto fail;

	// accept the connection, resulting in the second socket
	name_length = sizeof(inet_address);
	sockets[1] = accept(listener, (sockaddr*)&inet_address, &name_length);
	if (result != 0) goto fail;

	// we don't need the listener anymore
	closesocket(listener);
	return 0;

fail:
	result = MapErrorCode(GetSocketError());
	if (listener   != INVALID_SOCKET) closesocket(listener);
	if (sockets[0] != INVALID_SOCKET) closesocket(sockets[0]);
	if (sockets[1] != INVALID_SOCKET) closesocket(sockets[1]);
	sockets[0] = sockets[1] = INVALID_SOCKET;
	return result;
}
#endif

/*----------------------------------------------------------------------
|   NPT_Hash<NPT_Thread::ThreadId>
+---------------------------------------------------------------------*/
template <>
struct NPT_Hash<NPT_Thread::ThreadId>
{
    NPT_UInt32 operator()(NPT_Thread::ThreadId id) const {
        return (NPT_UInt32)(id & 0xFFFFFFFF);
    }
};

/*----------------------------------------------------------------------
|   NPT_IpAddress::ResolveName
+---------------------------------------------------------------------*/
NPT_Result
NPT_IpAddress::ResolveName(const char* name, NPT_Timeout)
{
    // check parameters
    if (name == NULL || name[0] == '\0') return NPT_ERROR_HOST_UNKNOWN;

#if !defined(NPT_CONFIG_ENABLE_IPV6)
    // handle numerical addrs
    NPT_IpAddress numerical_address;
    if (NPT_SUCCEEDED(numerical_address.Parse(name))) {
        /* the name is a numerical IP addr */
        *this = numerical_address;
        return NPT_SUCCESS;
    }
#endif

    // resolve the name into a list of addresses
    NPT_List<NPT_IpAddress> addresses;
    NPT_Result result = NPT_NetworkNameResolver::Resolve(name, addresses);
    if (NPT_FAILED(result)) return result;
    if (addresses.GetItemCount() < 1) {
        return NPT_ERROR_NO_SUCH_NAME;
    }
    
    // pick the first address
    *this = *(addresses.GetFirstItem());
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketFd
+---------------------------------------------------------------------*/
class NPT_BsdSocketFd
{
public:
    // constructors and destructor
    NPT_BsdSocketFd(SocketFd fd, NPT_Flags flags) : 
      m_SocketFd(fd), 
      m_ReadTimeout(NPT_TIMEOUT_INFINITE), 
      m_WriteTimeout(NPT_TIMEOUT_INFINITE),
      m_Position(0),
      m_Cancelled(false),
      m_Cancellable((flags & NPT_SOCKET_FLAG_CANCELLABLE) != 0) {
        // always use non-blocking mode
        SetBlockingMode(false); 
        
        // cancellation support
        if (flags & NPT_SOCKET_FLAG_CANCELLABLE) {
            int result = socketpair(AF_UNIX, SOCK_DGRAM, 0, m_CancelFds);
            if (result != 0) {
                NPT_LOG_WARNING_1("socketpair failed (%d)", GetSocketError());
                m_CancelFds[0] = m_CancelFds[1] = -1;
                m_Cancellable = false;
            }
        } else {
            m_CancelFds[0] = m_CancelFds[1] = NPT_BSD_SOCKET_INVALID_HANDLE;
        }
    }
    ~NPT_BsdSocketFd() {
        if (m_Cancellable) {
            if (!NPT_BSD_SOCKET_IS_INVALID(m_CancelFds[0])) closesocket(m_CancelFds[0]);
            if (!NPT_BSD_SOCKET_IS_INVALID(m_CancelFds[1])) closesocket(m_CancelFds[1]);
        }
        closesocket(m_SocketFd);
    }

    // methods
    NPT_Result SetBlockingMode(bool blocking);
    NPT_Result WaitUntilReadable();
    NPT_Result WaitUntilWriteable();
    NPT_Result WaitForCondition(bool readable, bool writeable, bool async_connect, NPT_Timeout timeout);
    NPT_Result Cancel(bool do_shutdown);

    // members
    SocketFd          m_SocketFd;
    NPT_Timeout       m_ReadTimeout;
    NPT_Timeout       m_WriteTimeout;
    NPT_Position      m_Position;
    volatile bool     m_Cancelled;
    bool              m_Cancellable;
    SocketFd          m_CancelFds[2];

private:
    // methods
    friend class NPT_BsdTcpServerSocket;
    friend class NPT_BsdTcpClientSocket;
};

typedef NPT_Reference<NPT_BsdSocketFd> NPT_BsdSocketFdReference;

/*----------------------------------------------------------------------
|   NPT_BsdBlockerSocket
+---------------------------------------------------------------------*/
class NPT_BsdBlockerSocket {
public:
    NPT_BsdBlockerSocket(NPT_BsdSocketFd* fd) {
        Set(NPT_Thread::GetCurrentThreadId(), fd);
    }
    ~NPT_BsdBlockerSocket() {
        Set(NPT_Thread::GetCurrentThreadId(), NULL);
    }

    static NPT_Result Cancel(NPT_Thread::ThreadId id);

private:
    static NPT_Result Set(NPT_Thread::ThreadId id, NPT_BsdSocketFd* fd);

    static NPT_Mutex MapLock;
    static NPT_HashMap<NPT_Thread::ThreadId, NPT_BsdSocketFd*> Map;
};

/*----------------------------------------------------------------------
|   NPT_BsdBlockerSocket::MapLock
+---------------------------------------------------------------------*/
NPT_Mutex NPT_BsdBlockerSocket::MapLock;

/*----------------------------------------------------------------------
|   NPT_BsdBlockerSocket::Map
+---------------------------------------------------------------------*/
NPT_HashMap<NPT_Thread::ThreadId, NPT_BsdSocketFd*> NPT_BsdBlockerSocket::Map;

/*----------------------------------------------------------------------
|   NPT_BsdBlockerSocket::Set
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdBlockerSocket::Set(NPT_Thread::ThreadId id, NPT_BsdSocketFd* fd)
{
    NPT_AutoLock synchronized(MapLock);

    if (fd) {
        return Map.Put(id, fd);
    } else {
        return Map.Erase(id);
    }
}

/*----------------------------------------------------------------------
|   NPT_BsdBlockerSocket::Cancel
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdBlockerSocket::Cancel(NPT_Thread::ThreadId id)
{
    NPT_AutoLock synchronized(MapLock);
    
    NPT_BsdSocketFd** fd = NULL;
    NPT_Result result = Map.Get(id, fd);
    if (NPT_SUCCEEDED(result) && fd && *fd) {
        (*fd)->Cancel(true);
    }

    return result;
}

/*----------------------------------------------------------------------
|   NPT_Socket::CancelBlockerSocket
+---------------------------------------------------------------------*/
NPT_Result
NPT_Socket::CancelBlockerSocket(NPT_Thread::ThreadId thread_id)
{
    return NPT_BsdBlockerSocket::Cancel(thread_id);
}


#if defined(__WINSOCK__) || defined(__TCS__)
/*----------------------------------------------------------------------
|   NPT_BsdSocketFd::SetBlockingMode
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::SetBlockingMode(bool blocking)
{
    unsigned long args = blocking?0:1;
    if (ioctlsocket(m_SocketFd, FIONBIO, &args)) {
        return NPT_ERROR_SOCKET_CONTROL_FAILED;
    }
    return NPT_SUCCESS;
}
#elif defined(__PSP__) || defined(__PPU__)
/*----------------------------------------------------------------------
|   NPT_BsdSocketFd::SetBlockingMode
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::SetBlockingMode(bool blocking)
{
    int args = blocking?0:1;
    if (setsockopt(m_SocketFd, SOL_SOCKET, SO_NBIO, &args, sizeof(args))) {
        return NPT_ERROR_SOCKET_CONTROL_FAILED;
    }
    return NPT_SUCCESS;
}
#else
/*----------------------------------------------------------------------
|   NPT_BsdSocketFd::SetBlockingMode
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::SetBlockingMode(bool blocking)
{
    int flags = fcntl(m_SocketFd, F_GETFL, 0);
    if (blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    if (fcntl(m_SocketFd, F_SETFL, flags)) {
        return NPT_ERROR_SOCKET_CONTROL_FAILED;
    }
    return NPT_SUCCESS;
}
#endif

/*----------------------------------------------------------------------
|   NPT_BsdSocketFd::WaitUntilReadable
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::WaitUntilReadable()
{
    return WaitForCondition(true, false, false, m_ReadTimeout);
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketFd::WaitUntilWriteable
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::WaitUntilWriteable()
{
    return WaitForCondition(false, true, false, m_WriteTimeout);
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketFd::WaitForCondition
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::WaitForCondition(bool        wait_for_readable, 
                                  bool        wait_for_writeable, 
                                  bool        async_connect,
                                  NPT_Timeout timeout)
{
    // wait for incoming connection
    NPT_Result result = NPT_SUCCESS;
    int        max_fd = (int)m_SocketFd;
    fd_set read_set;
    fd_set write_set;
    fd_set except_set;
    FD_ZERO(&read_set);
    if (wait_for_readable) FD_SET(m_SocketFd, &read_set);
    FD_ZERO(&write_set);
    if (wait_for_writeable) FD_SET(m_SocketFd, &write_set);
    FD_ZERO(&except_set);
    FD_SET(m_SocketFd, &except_set);

    // if the socket is cancellable, add it to the blocker map so a thread can cancel it
    NPT_BsdBlockerSocket blocker(this);
    
    // setup the cancel fd
    if (m_Cancellable && timeout) {
        if ((int)m_CancelFds[1] > max_fd) max_fd = m_CancelFds[1];
        FD_SET(m_CancelFds[1], &read_set);
    }
    
    struct timeval timeout_value;
    if (timeout != NPT_TIMEOUT_INFINITE) {
        timeout_value.tv_sec = timeout/1000;
        timeout_value.tv_usec = 1000*(timeout-1000*(timeout/1000));
    };
    
    NPT_LOG_FINER_2("waiting for condition (%s %s)",
                    wait_for_readable?"read":"",
                    wait_for_writeable?"write":"");
    int io_result;
    do {
        io_result = select(max_fd+1,
                           &read_set, &write_set, &except_set, 
                           timeout == NPT_TIMEOUT_INFINITE ? 
                           NULL : &timeout_value);
        NPT_LOG_FINER_1("select returned %d", io_result);
    } while (NPT_BSD_SOCKET_SELECT_FAILED(io_result) && GetSocketError() == EINTR);
    
    if (m_Cancelled) return NPT_ERROR_CANCELLED;

    if (io_result == 0) {
        if (timeout == 0) {
            // non-blocking call
            result = NPT_ERROR_WOULD_BLOCK;
        } else {
            // timeout
            result = NPT_ERROR_TIMEOUT;
        }
    } else if (NPT_BSD_SOCKET_SELECT_FAILED(io_result)) {
        result = MapErrorCode(GetSocketError());
    } else if ((wait_for_readable  && FD_ISSET(m_SocketFd, &read_set)) ||
               (wait_for_writeable && FD_ISSET(m_SocketFd, &write_set))) {
        if (async_connect) {
            // get error status from socket
            // (some systems return the error in errno, others
            //  return it in the buffer passed to getsockopt)
            int error = 0;
            socklen_t length = sizeof(error);
            io_result = getsockopt(m_SocketFd, 
                                   SOL_SOCKET, 
                                   SO_ERROR, 
                                   (SocketOption)&error, 
                                   &length);
            if (NPT_BSD_SOCKET_CALL_FAILED(io_result)) {
                result = MapErrorCode(GetSocketError());
            } else if (error) {
                result = MapErrorCode(error);
            } else {
                result = NPT_SUCCESS;
            }
        } else {
            result = NPT_SUCCESS;
        }
    } else if (FD_ISSET(m_SocketFd, &except_set)) {
        NPT_LOG_FINE("select socket exception is set");

        int error = 0;
        socklen_t length = sizeof(error);
        io_result = getsockopt(m_SocketFd, 
                                SOL_SOCKET, 
                                SO_ERROR, 
                                (SocketOption)&error, 
                                &length);
        if (NPT_BSD_SOCKET_CALL_FAILED(io_result)) {
            result = MapErrorCode(GetSocketError());
        } else if (error) {
            result = MapErrorCode(error);
        } else {
            result = NPT_FAILURE;
        }
    } else {
        // should not happen
        NPT_LOG_FINE("unexected select state");
        result = NPT_ERROR_INTERNAL;
    }

    if (NPT_FAILED(result)) {
        NPT_LOG_FINER_1("select result = %d", result);
    }
    return result;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketFd::Cancel
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketFd::Cancel(bool do_shutdown)
{
    // mark the socket as cancelled
    m_Cancelled = true;
    
    // force a shutdown if requested
    if (do_shutdown) {
        int result = shutdown(m_SocketFd, SHUT_RDWR);
        if (NPT_BSD_SOCKET_CALL_FAILED(result)) {
            NPT_LOG_FINE_1("shutdown failed (%d)", MapErrorCode(GetSocketError()));
        }
    }
    
    // unblock waiting selects
    if (m_Cancellable) {
        char dummy = 0;
        send(m_CancelFds[0], &dummy, 1, 0);
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketStream
+---------------------------------------------------------------------*/
class NPT_BsdSocketStream
{
 public:
    // methods
    NPT_BsdSocketStream(NPT_BsdSocketFdReference& socket_fd) :
       m_SocketFdReference(socket_fd) {}

    // NPT_InputStream and NPT_OutputStream methods
    NPT_Result Seek(NPT_Position) { return NPT_ERROR_NOT_SUPPORTED; }
    NPT_Result Tell(NPT_Position& where) {
        where = 0;
        return NPT_SUCCESS;
    }

 protected:
    // destructor
     virtual ~NPT_BsdSocketStream() {}

    // members
    NPT_BsdSocketFdReference m_SocketFdReference;
};

/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream
+---------------------------------------------------------------------*/
class NPT_BsdSocketInputStream : public NPT_InputStream,
                                 private NPT_BsdSocketStream
{
public:
    // constructors and destructor
    NPT_BsdSocketInputStream(NPT_BsdSocketFdReference& socket_fd) :
      NPT_BsdSocketStream(socket_fd) {}

    // NPT_InputStream methods
    NPT_Result Read(void*     buffer, 
                    NPT_Size  bytes_to_read, 
                    NPT_Size* bytes_read);
    NPT_Result Seek(NPT_Position offset) { 
        return NPT_BsdSocketStream::Seek(offset); }
    NPT_Result Tell(NPT_Position& where) {
        return NPT_BsdSocketStream::Tell(where);
    }
    NPT_Result GetSize(NPT_LargeSize& size);
    NPT_Result GetAvailable(NPT_LargeSize& available);
};

/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream::Read
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketInputStream::Read(void*     buffer, 
                               NPT_Size  bytes_to_read, 
                               NPT_Size* bytes_read)
{
    // if we're blocking, wait until the socket is readable
    if (m_SocketFdReference->m_ReadTimeout) {
        NPT_Result result = m_SocketFdReference->WaitUntilReadable();
        if (result != NPT_SUCCESS) return result;
    }

    // read from the socket
    NPT_LOG_FINEST_1("reading %d from socket", (int)bytes_to_read);
    ssize_t nb_read = recv(m_SocketFdReference->m_SocketFd, 
                           (SocketBuffer)buffer, 
                           bytes_to_read, 0);
    NPT_LOG_FINEST_1("recv returned %d", (int)nb_read);
    
    if (nb_read <= 0) {
        if (bytes_read) *bytes_read = 0;
        if (m_SocketFdReference->m_Cancelled) return NPT_ERROR_CANCELLED;
            
        if (nb_read == 0) {
            NPT_LOG_FINE("socket end of stream");
            return NPT_ERROR_EOS;
        } else {
            NPT_Result result = MapErrorCode(GetSocketError());
            NPT_LOG_FINE_1("socket result = %d", result);
            return result;
        }
    }
    
    // update position and return
    if (bytes_read) *bytes_read = (NPT_Size)nb_read;
    m_SocketFdReference->m_Position += nb_read;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream::GetSize
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketInputStream::GetSize(NPT_LargeSize& size)
{
    // generic socket streams have no size
    size = 0;
    return NPT_ERROR_NOT_SUPPORTED;
}

#if defined(__PPU__)
/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream::GetAvailable
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketInputStream::GetAvailable(NPT_LargeSize&)
{
    return NPT_ERROR_NOT_SUPPORTED;
}
#else
/*----------------------------------------------------------------------
|   NPT_BsdSocketInputStream::GetAvailable
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketInputStream::GetAvailable(NPT_LargeSize& available)
{
    unsigned long ready = 0;
    int io_result = ioctlsocket(m_SocketFdReference->m_SocketFd, FIONREAD, &ready); 
    if (NPT_BSD_SOCKET_CALL_FAILED(io_result)) {
        available = 0;
        return NPT_ERROR_SOCKET_CONTROL_FAILED;
    } else {
        available = ready;
        if (available == 0) {
            // check if the socket is disconnected
            NPT_Result result = m_SocketFdReference->WaitForCondition(true, false, false, 0);
            if (result == NPT_ERROR_WOULD_BLOCK) {
                return NPT_SUCCESS;
            } else {
                available = 1; // pretend that there's data available
            }
        }
        return NPT_SUCCESS;
    }
}
#endif

/*----------------------------------------------------------------------
|   NPT_BsdSocketOutputStream
+---------------------------------------------------------------------*/
class NPT_BsdSocketOutputStream : public NPT_OutputStream,
                                  private NPT_BsdSocketStream
{
public:
    // constructors and destructor
    NPT_BsdSocketOutputStream(NPT_BsdSocketFdReference& socket_fd) :
        NPT_BsdSocketStream(socket_fd) {}

    // NPT_OutputStream methods
    NPT_Result Write(const void* buffer, 
                     NPT_Size    bytes_to_write, 
                     NPT_Size*   bytes_written);
    NPT_Result Seek(NPT_Position offset) { 
        return NPT_BsdSocketStream::Seek(offset); }
    NPT_Result Tell(NPT_Position& where) {
        return NPT_BsdSocketStream::Tell(where);
    }
    NPT_Result Flush();
};

/*----------------------------------------------------------------------
|   NPT_BsdSocketOutputStream::Write
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketOutputStream::Write(const void*  buffer, 
                                 NPT_Size     bytes_to_write, 
                                 NPT_Size*    bytes_written)
{
    // FIXME: A temporary hack to get to the Cancel method
    if (buffer == NULL) {
        NPT_LOG_INFO("Cancelling BSD socket output stream through write...");
    
        m_SocketFdReference->Cancel(true);

        NPT_LOG_INFO("Done cancelling BSD socket output stream through write.");
        return NPT_SUCCESS;
    }

    int tries_left = 100;
    for (;;) {
        // if we're blocking, wait until the socket is writeable
        if (m_SocketFdReference->m_WriteTimeout) {
            NPT_Result result = m_SocketFdReference->WaitUntilWriteable();
            if (result != NPT_SUCCESS) return result;
        }

        int flags = 0;
    #if defined(MSG_NOSIGNAL)
        // for some BSD stacks, ask for EPIPE to be returned instead
        // of sending a SIGPIPE signal to the process
        flags |= MSG_NOSIGNAL;
    #endif

        // write to the socket
        NPT_LOG_FINEST_1("writing %d to socket", (int)bytes_to_write);
        ssize_t nb_written = send(m_SocketFdReference->m_SocketFd, 
                                  (SocketConstBuffer)buffer, 
                                  bytes_to_write, flags);
        NPT_LOG_FINEST_1("send returned %d", (int)nb_written);
        
        if (nb_written <= 0) {
            if (bytes_written) *bytes_written = 0;
            if (m_SocketFdReference->m_Cancelled) return NPT_ERROR_CANCELLED;

            if (nb_written == 0) {
                NPT_LOG_FINE("connection reset");
                return NPT_ERROR_CONNECTION_RESET;
            } else {
                NPT_Result result = MapErrorCode(GetSocketError());
                NPT_LOG_FINE_1("socket result = %d", result);
                    if (m_SocketFdReference->m_WriteTimeout && result == NPT_ERROR_WOULD_BLOCK) {
                        // Well, the socket was writeable but then failed with "would block"
                        // Loop back and try again, a certain number of times only...
                        NPT_LOG_FINE_1("Socket failed with 'would block' even though writeable?! Tries left: %d", tries_left);
                        if (--tries_left < 0) {
                            NPT_LOG_SEVERE("Failed to send any data, send kept returning with 'would block' even though timeout is not 0");
                            return NPT_ERROR_WRITE_FAILED;
                        }
                        continue;
                    }
                return result;
            }
        }
        
        // update position and return
        if (bytes_written) *bytes_written = (NPT_Size)nb_written;
        m_SocketFdReference->m_Position += nb_written;
        return NPT_SUCCESS;
    }
}

/*----------------------------------------------------------------------
|   NPT_BsdSocketOutputStream::Flush
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocketOutputStream::Flush()
{
    int       args = 0;
    socklen_t size = sizeof(args);

    NPT_LOG_FINEST("flushing socket");
    
    // get the value of the nagle algorithm
    if (getsockopt(m_SocketFdReference->m_SocketFd, 
                  IPPROTO_TCP, 
                  TCP_NODELAY, 
                  (char*)&args, 
                  &size)) {
        return NPT_ERROR_GETSOCKOPT_FAILED;
    }

    // return now if nagle was already off
    if (args == 1) return NPT_SUCCESS;

    // disable the nagle algorithm
    args = 1;
    if (setsockopt(m_SocketFdReference->m_SocketFd, 
                   IPPROTO_TCP, 
                   TCP_NODELAY, 
                   (const char*)&args, 
                   sizeof(args))) {
        return NPT_ERROR_SETSOCKOPT_FAILED;
    }

    // send an empty buffer to flush
    int flags = 0;
#if defined(MSG_NOSIGNAL)
    // for some BSD stacks, ask for EPIPE to be returned instead
    // of sending a SIGPIPE signal to the process
    flags |= MSG_NOSIGNAL;
#endif
    char dummy = 0;
    send(m_SocketFdReference->m_SocketFd, &dummy, 0, flags); 

    // restore the nagle algorithm to its original setting
    args = 0;
    if (setsockopt(m_SocketFdReference->m_SocketFd, 
                   IPPROTO_TCP, 
                   TCP_NODELAY, 
                   (const char*)&args, 
                   sizeof(args))) {
        return NPT_ERROR_SETSOCKOPT_FAILED;
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket
+---------------------------------------------------------------------*/
class NPT_BsdSocket : public NPT_SocketInterface
{
 public:
    // constructors and destructor
             NPT_BsdSocket(SocketFd fd, NPT_Flags flags);
    virtual ~NPT_BsdSocket();

    // methods
    NPT_Result RefreshInfo();

    // NPT_SocketInterface methods
    NPT_Result Bind(const NPT_SocketAddress& address, bool reuse_address = true);
    NPT_Result Connect(const NPT_SocketAddress& address, NPT_Timeout timeout);
    NPT_Result WaitForConnection(NPT_Timeout timeout);
    NPT_Result GetInputStream(NPT_InputStreamReference& stream);
    NPT_Result GetOutputStream(NPT_OutputStreamReference& stream);
    NPT_Result GetInfo(NPT_SocketInfo& info);
    NPT_Result SetReadTimeout(NPT_Timeout timeout);
    NPT_Result SetWriteTimeout(NPT_Timeout timeout);
    NPT_Result Cancel(bool shutdown);

 protected:
    // members
    NPT_BsdSocketFdReference m_SocketFdReference;
    NPT_SocketInfo           m_Info;
};

/*----------------------------------------------------------------------
|   NPT_BsdSocket::NPT_BsdSocket
+---------------------------------------------------------------------*/
NPT_BsdSocket::NPT_BsdSocket(SocketFd fd, NPT_Flags flags) : 
    m_SocketFdReference(new NPT_BsdSocketFd(fd, flags))
{
    // disable the SIGPIPE signal
#if defined(SO_NOSIGPIPE)
    int option = 1;
    (void)setsockopt(m_SocketFdReference->m_SocketFd,
                     SOL_SOCKET,
                     SO_NOSIGPIPE,
                     (SocketOption)&option,
                     sizeof(option));
#elif defined(SIGPIPE)
    signal(SIGPIPE, SIG_IGN);
#endif

    RefreshInfo();
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::~NPT_BsdSocket
+---------------------------------------------------------------------*/
NPT_BsdSocket::~NPT_BsdSocket()
{
    // release the socket fd reference
    m_SocketFdReference = NULL;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::Bind
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::Bind(const NPT_SocketAddress& address, bool reuse_address)
{
    // set socket options
    if (reuse_address) {
        NPT_LOG_FINE("setting SO_REUSEADDR option on socket");
        int option = 1;
        if (setsockopt(m_SocketFdReference->m_SocketFd,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       (SocketOption)&option,
                       sizeof(option))) {
            return MapErrorCode(GetSocketError());
        }
    }
    
    // convert the address
    NPT_sockaddr_in inet_address;
    socklen_t       inet_address_length;
    SocketAddressToInetAddress(address, inet_address, inet_address_length);
    
#if defined(_XBOX)
    if( address.GetIpAddress().AsLong() != NPT_IpAddress::Any.AsLong() ) {
        // Xbox can't bind to specific address, defaulting to ANY
        SocketAddressToInetAddress(NPT_SocketAddress(NPT_IpAddress::Any, address.GetPort()), &inet_address);
    }
#endif

    // bind the socket
    if (bind(m_SocketFdReference->m_SocketFd, 
             &inet_address.sa,
             inet_address_length) < 0) {
        return MapErrorCode(GetSocketError());
    }
    
    // refresh socket info
    RefreshInfo();

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdSocket::Connect(const NPT_SocketAddress&, NPT_Timeout)
{
    // this is unsupported unless overridden in a derived class
    return NPT_ERROR_NOT_SUPPORTED;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::WaitForConnection
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdSocket::WaitForConnection(NPT_Timeout)
{
    // this is unsupported unless overridden in a derived class
    return NPT_ERROR_NOT_SUPPORTED;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::GetInputStream
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::GetInputStream(NPT_InputStreamReference& stream)
{
    // default value
    stream = NULL;

    // check that we have a socket
    if (m_SocketFdReference.IsNull()) return NPT_ERROR_INVALID_STATE;

    // create a stream
    stream = new NPT_BsdSocketInputStream(m_SocketFdReference);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::GetOutputStream
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::GetOutputStream(NPT_OutputStreamReference& stream)
{
    // default value
    stream = NULL;

    // check that the file is open
    if (m_SocketFdReference.IsNull()) return NPT_ERROR_INVALID_STATE;

    // create a stream
    stream = new NPT_BsdSocketOutputStream(m_SocketFdReference);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::GetInfo
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::GetInfo(NPT_SocketInfo& info)
{
    // return the cached info
    info = m_Info;
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::RefreshInfo
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::RefreshInfo()
{
    // check that we have a socket
    if (m_SocketFdReference.IsNull()) return NPT_ERROR_INVALID_STATE;

    // get the local socket addr
    NPT_sockaddr_in inet_address;
    socklen_t       inet_address_length = sizeof(inet_address);
    if (getsockname(m_SocketFdReference->m_SocketFd, 
                    &inet_address.sa,
                    &inet_address_length) == 0) {
        InetAddressToSocketAddress(inet_address, m_Info.local_address);
    }

    // get the peer socket addr
    if (getpeername(m_SocketFdReference->m_SocketFd,
                    &inet_address.sa,
                    &inet_address_length) == 0) {
        InetAddressToSocketAddress(inet_address, m_Info.remote_address);
    }

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::SetReadTimeout
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::SetReadTimeout(NPT_Timeout timeout)
{
    m_SocketFdReference->m_ReadTimeout = timeout;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::SetWriteTimeout
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::SetWriteTimeout(NPT_Timeout timeout)
{
    m_SocketFdReference->m_WriteTimeout = timeout;
    setsockopt(m_SocketFdReference->m_SocketFd,
               SOL_SOCKET,
               SO_SNDTIMEO,
               (SocketOption)&timeout,
               sizeof(timeout));
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdSocket::Cancel
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdSocket::Cancel(bool do_shutdown)
{
    return m_SocketFdReference->Cancel(do_shutdown);
}

/*----------------------------------------------------------------------
|   NPT_Socket::~NPT_Socket
+---------------------------------------------------------------------*/
NPT_Socket::~NPT_Socket()
{
    delete m_SocketDelegate;
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket
+---------------------------------------------------------------------*/
class NPT_BsdUdpSocket : public    NPT_UdpSocketInterface,
                         protected NPT_BsdSocket
                         
{
 public:
    // constructor and destructor
             NPT_BsdUdpSocket(NPT_Flags flags);
    virtual ~NPT_BsdUdpSocket() {}

    // NPT_SocketInterface methods
    NPT_Result Bind(const NPT_SocketAddress& address, bool reuse_address = true);
    NPT_Result Connect(const NPT_SocketAddress& address,
                       NPT_Timeout              timeout);

    // NPT_UdpSocketInterface methods
    NPT_Result Send(const NPT_DataBuffer&    packet, 
                    const NPT_SocketAddress* address);
    NPT_Result Receive(NPT_DataBuffer&     packet, 
                       NPT_SocketAddress*  address);

    // friends
    friend class NPT_UdpSocket;
};

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::NPT_BsdUdpSocket
+---------------------------------------------------------------------*/
NPT_BsdUdpSocket::NPT_BsdUdpSocket(NPT_Flags flags) :
    NPT_BsdSocket(socket(NPT_SOCKETS_PF_INET, SOCK_DGRAM, 0), flags)
{
    // set default socket options
    int option = 1;
    (void)setsockopt(m_SocketFdReference->m_SocketFd,
                     SOL_SOCKET,
                     SO_BROADCAST,
                     (SocketOption)&option,
                     sizeof(option));

#if defined(_XBOX)
    // set flag on the socket to allow sending of multicast
    if (!NPT_BSD_SOCKET_IS_INVALID(m_SocketFdReference->m_SocketFd)) {
        *(DWORD*)((char*)m_SocketFdReference->m_SocketFd+0xc) |= 0x02000000;
    }
#endif
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::Bind
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpSocket::Bind(const NPT_SocketAddress& address, bool reuse_address)
{
    if (reuse_address) {
#if defined(SO_REUSEPORT) 
        // some implementations (BSD 4.4) need this in addition to SO_REUSEADDR
        NPT_LOG_FINE("setting SO_REUSEPORT option on socket");
        int option = 1;
        if (setsockopt(m_SocketFdReference->m_SocketFd,
                       SOL_SOCKET,
                       SO_REUSEPORT,
                       (SocketOption)&option,
                       sizeof(option))) {
            return MapErrorCode(GetSocketError());
        }
#endif
    }
    // call the inherited method
    return NPT_BsdSocket::Bind(address, reuse_address);
} 

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpSocket::Connect(const NPT_SocketAddress& address, 
                          NPT_Timeout /* ignored */)
{
    // setup an address structure
    NPT_sockaddr_in inet_address;
    socklen_t       inet_address_length;
    SocketAddressToInetAddress(address, inet_address, inet_address_length);

    // connect so that we can have some addr bound to the socket
    NPT_LOG_FINE_2("connecting to %s, port %d",
                   address.GetIpAddress().ToString().GetChars(),
                   address.GetPort());
    int io_result = connect(m_SocketFdReference->m_SocketFd, 
                            &inet_address.sa,
                            inet_address_length);
    if (NPT_BSD_SOCKET_CALL_FAILED(io_result)) { 
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("socket error %d", result);
        return result;
    }
    
    // refresh socket info
    RefreshInfo();

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::Send
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdUdpSocket::Send(const NPT_DataBuffer&    packet, 
                       const NPT_SocketAddress* address) 
{
    // get the packet buffer
    const NPT_Byte* buffer        = packet.GetData();
    ssize_t         buffer_length = packet.GetDataSize();

    // if we're blocking, wait until the socket is writeable
    if (m_SocketFdReference->m_WriteTimeout) {
        NPT_Result result = m_SocketFdReference->WaitUntilWriteable();
        if (result != NPT_SUCCESS) return result;
    }

    // send the packet buffer
    ssize_t io_result;
    if (address) {
        // send to the specified address

        // setup an address structure
        NPT_sockaddr_in inet_address;
        socklen_t       inet_address_length;
        SocketAddressToInetAddress(*address, inet_address, inet_address_length);
        
        // send the data
        NPT_LOG_FINEST_2("sending datagram to %s port %d",
                         address->GetIpAddress().ToString().GetChars(),
                         address->GetPort());
        io_result = sendto(m_SocketFdReference->m_SocketFd, 
                           (SocketConstBuffer)buffer, 
                           buffer_length, 
                           0, 
                           &inet_address.sa,
                           inet_address_length);
    } else {
        int flags = 0;
#if defined(MSG_NOSIGNAL)
        // for some BSD stacks, ask for EPIPE to be returned instead
        // of sending a SIGPIPE signal to the process
        flags |= MSG_NOSIGNAL;
#endif

        // send to whichever addr the socket is connected
        NPT_LOG_FINEST("sending datagram");
        io_result = send(m_SocketFdReference->m_SocketFd, 
                         (SocketConstBuffer)buffer, 
                         buffer_length,
                         flags);
    }

    // check result
    NPT_LOG_FINEST_1("send/sendto returned %d", (int)io_result);
    if (m_SocketFdReference->m_Cancelled) return NPT_ERROR_CANCELLED;
    if (NPT_BSD_SOCKET_CALL_FAILED(io_result)) {
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("socket error %d", result);
        return result;
    }

    // update position and return
    m_SocketFdReference->m_Position += buffer_length;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpSocket::Receive
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdUdpSocket::Receive(NPT_DataBuffer&    packet, 
                          NPT_SocketAddress* address)
{
    // get the packet buffer
    NPT_Byte* buffer      = packet.UseData();
    ssize_t   buffer_size = packet.GetBufferSize();

    // check that we have some space to receive
    if (buffer_size == 0) return NPT_ERROR_INVALID_PARAMETERS;

    // if we're blocking, wait until the socket is readable
    if (m_SocketFdReference->m_ReadTimeout) {
        NPT_Result result = m_SocketFdReference->WaitUntilReadable();
        if (result != NPT_SUCCESS) return result;
    }

    // receive a packet
    ssize_t io_result = 0;
    if (address) {
        NPT_sockaddr_in inet_address;
        socklen_t       inet_address_length = sizeof(inet_address);

        NPT_LOG_FINEST_2("receiving dagagram from %s port %d",
                         address->GetIpAddress().ToString().GetChars(),
                         address->GetPort());
        io_result = recvfrom(m_SocketFdReference->m_SocketFd, 
                             (SocketBuffer)buffer, 
                             buffer_size, 
                             0, 
                             &inet_address.sa,
                             &inet_address_length);

        // convert the address format
        if (!NPT_BSD_SOCKET_CALL_FAILED(io_result)) {
            InetAddressToSocketAddress(inet_address, *address);
        }
    } else {
        NPT_LOG_FINEST("receiving datagram");
        io_result = recv(m_SocketFdReference->m_SocketFd,
                         (SocketBuffer)buffer,
                         buffer_size,
                         0);
    }

    // check result
    NPT_LOG_FINEST_1("recv/recvfrom returned %d", (int)io_result);
    if (m_SocketFdReference->m_Cancelled) {
        packet.SetDataSize(0);
        return NPT_ERROR_CANCELLED;
    }
    if (NPT_BSD_SOCKET_CALL_FAILED(io_result)) {
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("socket error %d", result);
        packet.SetDataSize(0);
        return result;
    } 
    
    // update position and return
    packet.SetDataSize((NPT_Size)io_result);
    m_SocketFdReference->m_Position += io_result;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_UdpSocket::NPT_UdpSocket
+---------------------------------------------------------------------*/
NPT_UdpSocket::NPT_UdpSocket(NPT_Flags flags)
{
    NPT_BsdUdpSocket* delegate = new NPT_BsdUdpSocket(flags);
    m_SocketDelegate    = delegate;
    m_UdpSocketDelegate = delegate;
}

/*----------------------------------------------------------------------
|   NPT_UdpSocket::NPT_UdpSocket
+---------------------------------------------------------------------*/
NPT_UdpSocket::NPT_UdpSocket(NPT_UdpSocketInterface* delegate) :
    m_UdpSocketDelegate(delegate)
{
}

/*----------------------------------------------------------------------
|   NPT_UdpSocket::~NPT_UdpSocket
+---------------------------------------------------------------------*/
NPT_UdpSocket::~NPT_UdpSocket()
{
    delete m_UdpSocketDelegate;

    // set the delegate pointers to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_UdpSocketDelegate = NULL;
    m_SocketDelegate    = NULL;
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket
+---------------------------------------------------------------------*/
class NPT_BsdUdpMulticastSocket : public    NPT_UdpMulticastSocketInterface,
                                  protected NPT_BsdUdpSocket
                                  
{
 public:
    // methods
     NPT_BsdUdpMulticastSocket(NPT_Flags flags);
    ~NPT_BsdUdpMulticastSocket();

    // NPT_UdpMulticastSocketInterface methods
    NPT_Result JoinGroup(const NPT_IpAddress& group,
                         const NPT_IpAddress& iface);
    NPT_Result LeaveGroup(const NPT_IpAddress& group,
                          const NPT_IpAddress& iface);
    NPT_Result SetTimeToLive(unsigned char ttl);
    NPT_Result SetInterface(const NPT_IpAddress& iface);

    // friends 
    friend class NPT_UdpMulticastSocket;
};

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::NPT_BsdUdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_BsdUdpMulticastSocket::NPT_BsdUdpMulticastSocket(NPT_Flags flags) :
    NPT_BsdUdpSocket(flags)
{
#if !defined(_XBOX)
    int option = 1;
    setsockopt(m_SocketFdReference->m_SocketFd, 
               IPPROTO_IP, 
               IP_MULTICAST_LOOP,
               (SocketOption)&option,
               sizeof(option));
#endif
}

/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::~NPT_BsdUdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_BsdUdpMulticastSocket::~NPT_BsdUdpMulticastSocket()
{
}

#if defined(_XBOX)
/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::JoinGroup
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::JoinGroup(const NPT_IpAddress& group,
                                     const NPT_IpAddress& iface)
{
    return NPT_SUCCESS;
}
#else
/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::JoinGroup
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::JoinGroup(const NPT_IpAddress& group,
                                     const NPT_IpAddress& iface)
{
#if defined(NPT_CONFIG_ENABLE_IPV6)
    struct ipv6_mreq mreq;

    // set the interface index
    mreq.ipv6mr_interface = 0; // FIXME: hardcoded to 0 for now
    
    // set the group address
    if (group.GetType() == NPT_IpAddress::IPV6) {
        NPT_CopyMemory(&mreq.ipv6mr_multiaddr.s6_addr[0], group.AsBytes(), 16);
    } else {
        NPT_SetMemory(&mreq.ipv6mr_multiaddr.s6_addr[0], 0, 10);
        mreq.ipv6mr_multiaddr.s6_addr[10] = 0xFF;
        mreq.ipv6mr_multiaddr.s6_addr[11] = 0xFF;
        NPT_CopyMemory(&mreq.ipv6mr_multiaddr.s6_addr[12], group.AsBytes(), 4);
    }

    // set socket option
    NPT_LOG_FINE_2("joining multicast addr %s group %s",
                   iface.ToString().GetChars(), group.ToString().GetChars());
    if (setsockopt(m_SocketFdReference->m_SocketFd,
                   IPPROTO_IPV6, IPV6_JOIN_GROUP,
                   (SocketOption)&mreq, sizeof(mreq))) {
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("setsockopt error %d", result);
        return result;
    }
    return NPT_SUCCESS;
#else
    struct ip_mreq mreq;

    // set the interface address
    mreq.imr_interface.s_addr = htonl(iface.AsLong());

    // set the group address
    mreq.imr_multiaddr.s_addr = htonl(group.AsLong());

    // set socket option
    NPT_LOG_FINE_2("joining multicast addr %s group %s",
                   iface.ToString().GetChars(), group.ToString().GetChars());
    if (setsockopt(m_SocketFdReference->m_SocketFd,
                   IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (SocketOption)&mreq, sizeof(mreq))) {
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("setsockopt error %d", result);
        return result;
    }
    return NPT_SUCCESS;
#endif
}
#endif

#if defined(_XBOX)
/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::LeaveGroup
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::LeaveGroup(const NPT_IpAddress& group,
                                      const NPT_IpAddress& iface)
{
    return NPT_SUCCESS;
}
#else
/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::LeaveGroup
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::LeaveGroup(const NPT_IpAddress& group,
                                      const NPT_IpAddress& iface)
{
#if defined(NPT_CONFIG_ENABLE_IPV6)
    struct ipv6_mreq mreq;

    // set the interface index
    mreq.ipv6mr_interface = 0; // FIXME: hardcoded to 0 for now
    
    // set the group address
    if (group.GetType() == NPT_IpAddress::IPV6) {
        NPT_CopyMemory(&mreq.ipv6mr_multiaddr.s6_addr[0], group.AsBytes(), 16);
    } else {
        NPT_SetMemory(&mreq.ipv6mr_multiaddr.s6_addr[0], 0, 10);
        mreq.ipv6mr_multiaddr.s6_addr[10] = 0xFF;
        mreq.ipv6mr_multiaddr.s6_addr[11] = 0xFF;
        NPT_CopyMemory(&mreq.ipv6mr_multiaddr.s6_addr[12], group.AsBytes(), 4);
    }

    // set socket option
    NPT_LOG_FINE_2("leaving multicast addr %s group %s",
                   iface.ToString().GetChars(), group.ToString().GetChars());
    if (setsockopt(m_SocketFdReference->m_SocketFd,
                   IPPROTO_IPV6, IPV6_LEAVE_GROUP,
                   (SocketOption)&mreq, sizeof(mreq))) {
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("setsockopt error %d", result);
        return result;
    }
#else
    struct ip_mreq mreq;

    // set the interface address
    mreq.imr_interface.s_addr = htonl(iface.AsLong());

    // set the group address
    mreq.imr_multiaddr.s_addr = htonl(group.AsLong());

    // set socket option
    NPT_LOG_FINE_2("leaving multicast addr %s group %s",
                   iface.ToString().GetChars(), group.ToString().GetChars());
    if (setsockopt(m_SocketFdReference->m_SocketFd,
                   IPPROTO_IP, IP_DROP_MEMBERSHIP,
                   (SocketOption)&mreq, sizeof(mreq))) {
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("setsockopt error %d", result);
        return result;
    }
#endif
    
    return NPT_SUCCESS;
}
#endif

#if defined(_XBOX)
/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::SetInterface
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::SetInterface(const NPT_IpAddress& iface)
{
    return NPT_SUCCESS;
}
#else
/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::SetInterface
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::SetInterface(const NPT_IpAddress& iface)
{
#if defined(NPT_CONFIG_ENABLE_IPV6)
    unsigned int ifindex = 0; // FIXME: hardcoded to 0 for now
    
    // set socket option
    NPT_LOG_FINE_1("setting multicast interface %s", iface.ToString().GetChars());
    if (setsockopt(m_SocketFdReference->m_SocketFd,
                   IPPROTO_IPV6, IPV6_MULTICAST_IF,
                   (char*)&ifindex, sizeof(ifindex))) {
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("setsockopt error %d", result);
        return result;
    }
#else
    struct in_addr iface_addr;
    // set the interface address
    iface_addr.s_addr = htonl(iface.AsLong());

    // set socket option
    NPT_LOG_FINE_1("setting multicast interface %s", iface.ToString().GetChars());
    if (setsockopt(m_SocketFdReference->m_SocketFd,
                   IPPROTO_IP, IP_MULTICAST_IF,
                   (char*)&iface_addr, sizeof(iface_addr))) {
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("setsockopt error %d", result);
        return result;
    }
#endif
    
    return NPT_SUCCESS;
}
#endif

#if defined(_XBOX)
/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::SetTimeToLive
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::SetTimeToLive(unsigned char ttl)
{
    return NPT_ERROR_NOT_IMPLEMENTED;
}
#else
/*----------------------------------------------------------------------
|   NPT_BsdUdpMulticastSocket::SetTimeToLive
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdUdpMulticastSocket::SetTimeToLive(unsigned char ttl)
{

    // set socket option
    NPT_LOG_FINE_1("setting multicast TTL to %d", (int)ttl); 
#if defined(NPT_CONFIG_ENABLE_IPV6)
    int ttl_opt = ttl;
    if (setsockopt(m_SocketFdReference->m_SocketFd,
                   IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
                   (SocketOption)&ttl_opt, sizeof(ttl_opt))) {
#else
    unsigned char ttl_opt = ttl;
    if (setsockopt(m_SocketFdReference->m_SocketFd,
                   IPPROTO_IP, IP_MULTICAST_TTL,
                   (SocketOption)&ttl_opt, sizeof(ttl_opt))) {
#endif
        NPT_Result result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("setsockopt error %d", result);
        return result;
    }
    
    return NPT_SUCCESS;
}
#endif

/*----------------------------------------------------------------------
|   NPT_UdpMulticastSocket::NPT_UdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_UdpMulticastSocket::NPT_UdpMulticastSocket(NPT_Flags flags) :
    NPT_UdpSocket((NPT_UdpSocketInterface*)0)
{
    NPT_BsdUdpMulticastSocket* delegate = new NPT_BsdUdpMulticastSocket(flags);
    m_SocketDelegate             = delegate;
    m_UdpSocketDelegate          = delegate;
    m_UdpMulticastSocketDelegate = delegate;
}

/*----------------------------------------------------------------------
|   NPT_UdpMulticastSocket::~NPT_UdpMulticastSocket
+---------------------------------------------------------------------*/
NPT_UdpMulticastSocket::~NPT_UdpMulticastSocket()
{
    delete m_UdpMulticastSocketDelegate;

    // set the delegate pointers to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_SocketDelegate             = NULL;
    m_UdpSocketDelegate          = NULL;
    m_UdpMulticastSocketDelegate = NULL;
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket
+---------------------------------------------------------------------*/
class NPT_BsdTcpClientSocket : protected NPT_BsdSocket
{
 public:
    // methods
     NPT_BsdTcpClientSocket(NPT_Flags flags);
    ~NPT_BsdTcpClientSocket();

    // NPT_SocketInterface methods
    NPT_Result Connect(const NPT_SocketAddress& address,
                       NPT_Timeout              timeout);
    NPT_Result WaitForConnection(NPT_Timeout timeout);

protected:
    // friends
    friend class NPT_TcpClientSocket;
};

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::NPT_BsdTcpClientSocket
+---------------------------------------------------------------------*/
NPT_BsdTcpClientSocket::NPT_BsdTcpClientSocket(NPT_Flags flags) : 
    NPT_BsdSocket(socket(NPT_SOCKETS_PF_INET, SOCK_STREAM, 0), flags)
{
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::~NPT_BsdTcpClientSocket
+---------------------------------------------------------------------*/
NPT_BsdTcpClientSocket::~NPT_BsdTcpClientSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::Connect
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdTcpClientSocket::Connect(const NPT_SocketAddress& address, 
                                NPT_Timeout              timeout)
{
    // convert the address
    NPT_sockaddr_in inet_address;
    socklen_t       inet_address_length;
    SocketAddressToInetAddress(address, inet_address, inet_address_length);

    // initiate connection
    NPT_LOG_FINE_2("connecting to %s, port %d",
                   address.GetIpAddress().ToString().GetChars(),
                   address.GetPort());
    int io_result;
    io_result = connect(m_SocketFdReference->m_SocketFd, 
                        &inet_address.sa,
                        inet_address_length);
    if (io_result == 0) {
        // immediate connection
        NPT_LOG_FINE("immediate connection");
        
        // get socket info
        RefreshInfo();

        return NPT_SUCCESS;
    }

    // test for errors
    NPT_Result result = MapErrorCode(GetSocketError());
    
    // if we're blocking, wait for a connection unless there was an error
    if (timeout && result == NPT_ERROR_WOULD_BLOCK) {
        return WaitForConnection(timeout);
    }

    return result;
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpClientSocket::WaitForConnection
+---------------------------------------------------------------------*/
NPT_Result 
NPT_BsdTcpClientSocket::WaitForConnection(NPT_Timeout timeout)
{
    NPT_Result result = m_SocketFdReference->WaitForCondition(true, true, true, timeout);
    
    // get socket info
    RefreshInfo();

    return result;
}

/*----------------------------------------------------------------------
|   NPT_TcpClientSocket::NPT_TcpClientSocket
+---------------------------------------------------------------------*/
NPT_TcpClientSocket::NPT_TcpClientSocket(NPT_Flags flags) :
    NPT_Socket(new NPT_BsdTcpClientSocket(flags))
{
}

/*----------------------------------------------------------------------
|   NPT_TcpClientSocket::NPT_TcpClientSocket
+---------------------------------------------------------------------*/
NPT_TcpClientSocket::~NPT_TcpClientSocket()
{
    delete m_SocketDelegate;

    // set the delegate pointer to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_SocketDelegate = NULL;
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket
+---------------------------------------------------------------------*/
class NPT_BsdTcpServerSocket : public    NPT_TcpServerSocketInterface,
                               protected NPT_BsdSocket
                               
{
 public:
    // methods
     NPT_BsdTcpServerSocket(NPT_Flags flags);
    ~NPT_BsdTcpServerSocket();

    // NPT_SocketInterface methods
    NPT_Result GetInputStream(NPT_InputStreamReference& stream) {
        // no stream
        stream = NULL;
        return NPT_ERROR_NOT_SUPPORTED;
    }
    NPT_Result GetOutputStream(NPT_OutputStreamReference& stream) {
        // no stream
        stream = NULL;
        return NPT_ERROR_NOT_SUPPORTED;
    }

    // NPT_TcpServerSocket methods
    NPT_Result Listen(unsigned int max_clients);
    NPT_Result WaitForNewClient(NPT_Socket*& client, 
                                NPT_Timeout  timeout,
                                NPT_Flags    flags);

protected:
    // members
    unsigned int m_ListenMax;

    // friends
    friend class NPT_TcpServerSocket;
};

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket::NPT_BsdTcpServerSocket
+---------------------------------------------------------------------*/
NPT_BsdTcpServerSocket::NPT_BsdTcpServerSocket(NPT_Flags flags) : 
    NPT_BsdSocket(socket(NPT_SOCKETS_PF_INET, SOCK_STREAM, 0), flags),
    m_ListenMax(0)
{
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket::~NPT_BsdTcpServerSocket
+---------------------------------------------------------------------*/
NPT_BsdTcpServerSocket::~NPT_BsdTcpServerSocket()
{
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket::Listen
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdTcpServerSocket::Listen(unsigned int max_clients)
{
    // listen for connections
    if (listen(m_SocketFdReference->m_SocketFd, max_clients) < 0) {
        m_ListenMax = 0;
        return NPT_ERROR_LISTEN_FAILED;
    }   
    m_ListenMax = max_clients;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_BsdTcpServerSocket::WaitForNewClient
+---------------------------------------------------------------------*/
NPT_Result
NPT_BsdTcpServerSocket::WaitForNewClient(NPT_Socket*& client, 
                                         NPT_Timeout  timeout,
                                         NPT_Flags    flags)
{
    // default value
    client = NULL;

    // check that we are listening for clients
    if (m_ListenMax == 0) {
        Listen(NPT_TCP_SERVER_SOCKET_DEFAULT_LISTEN_COUNT);
    }

    // wait until the socket is readable or writeable
    NPT_LOG_FINER("waiting until socket is readable or writeable");
    NPT_Result result = m_SocketFdReference->WaitForCondition(true, true, false, timeout);
    if (result != NPT_SUCCESS) return result;

    NPT_LOG_FINER("accepting connection");
    NPT_sockaddr_in inet_address;
    socklen_t       inet_address_length = sizeof(inet_address);
    SocketFd socket_fd = accept(m_SocketFdReference->m_SocketFd, &inet_address.sa, &inet_address_length);
    if (NPT_BSD_SOCKET_IS_INVALID(socket_fd)) {
        if (m_SocketFdReference->m_Cancelled) return NPT_ERROR_CANCELLED;
        result = MapErrorCode(GetSocketError());
        NPT_LOG_FINE_1("socket error %d", result);
        return result;
    } else {
        client = new NPT_Socket(new NPT_BsdSocket(socket_fd, flags));
    }

    // done
    return result;    
}

/*----------------------------------------------------------------------
|   NPT_TcpServerSocket::NPT_TcpServerSocket
+---------------------------------------------------------------------*/
NPT_TcpServerSocket::NPT_TcpServerSocket(NPT_Flags flags)
{
    NPT_BsdTcpServerSocket* delegate = new NPT_BsdTcpServerSocket(flags);
    m_SocketDelegate          = delegate;
    m_TcpServerSocketDelegate = delegate;
}

/*----------------------------------------------------------------------
|   NPT_TcpServerSocket::NPT_TcpServerSocket
+---------------------------------------------------------------------*/
NPT_TcpServerSocket::~NPT_TcpServerSocket()
{
    delete m_TcpServerSocketDelegate;

    // set the delegate pointers to NULL because it is shared by the
    // base classes, and we only want to delete the object once
    m_SocketDelegate          = NULL;
    m_TcpServerSocketDelegate = NULL;
}
