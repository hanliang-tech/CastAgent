# CastAgent
运行在接收端的投屏协议（DLNA）代理，支持发送端指定DMP，支持配置接收策略

# 抓包分析
POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:14 GMT
Content-Length: 448
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>NO_MEDIA_PRESENT</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:14 GMT
Content-Length: 448
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>NO_MEDIA_PRESENT</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#SetAVTransportURI"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 1875
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:SetAVTransportURI xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID><CurrentURI>http://cache.m.iqiyi.com/mus/6299107589907401/489063831396bfba637c59d437756a8f/afbe8fd3d73448c9/1566528573/20220822/ef/82/ff687b78cac36d751adaf92341133c0e.m3u8?vt=2&amp;previewTime=6&amp;qd_originate=sys&amp;k_uid=5c141e8a3dd2aeae51161a84e636ee641019&amp;_lnt=0&amp;qd_time=1661154434999&amp;qd_p=7166ef80&amp;ve=d6238e8a1d134ecfb37b22b89bb3151c&amp;bid=300&amp;bossStatus=2&amp;code=2&amp;ff=ts&amp;preIdAll=bb196bccc4d541a48dec3ee8869bde05_0&amp;prv=0&amp;px=f7PRDlt9kJfq0oEKL00pHxkPl5S4qIm3EEYm33HilQPL7m3nbKkMxrcdsGGLpIv6zm2qYj5d&amp;tvid=4101975334356600&amp;qd_vip=1&amp;src=02032001010000000000-04000000001000000000-01&amp;qd_uri=dash&amp;tm=1661154434746&amp;previewType=1&amp;rpt=2&amp;sgti=12_5c141e8a3dd2aeae51161a84e636ee641019_1661154434746&amp;vf=e80778620471d9abd45b9cf8e8555113</CurrentURI><CurrentURIMetaData>&lt;DIDL-Lite xmlns=&quot;urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/&quot; xmlns:dc=&quot;http://purl.org/dc/elements/1.1/&quot; xmlns:upnp=&quot;urn:schemas-upnp-org:metadata-1-0/upnp/&quot;&gt;&lt;item id=&quot;...............8......8... ..........................................&quot; parentID=&quot;-1&quot; restricted=&quot;1&quot;&gt;&lt;upnp:genre&gt;Unknown&lt;/upnp:genre&gt;&lt;upnp:storageMedium&gt;UNKNOWN&lt;/upnp:storageMedium&gt;&lt;upnp:writeStatus&gt;UNKNOWN&lt;/upnp:writeStatus&gt;&lt;upnp:class&gt;object.item.videoItem.movie&lt;/upnp:class&gt;&lt;dc:title&gt;...............8......8... ..........................................&lt;/dc:title&gt;&lt;/item&gt;&lt;/DIDL-Lite&gt;</CurrentURIMetaData></u:SetAVTransportURI></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:16 GMT
Content-Length: 277
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:SetAVTransportURIResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"/></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#Play"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 306
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:Play xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID><Speed>1</Speed></u:Play></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:16 GMT
Content-Length: 264
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:PlayResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"/></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:16 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:17 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:18 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:19 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:20 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:21 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:22 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:23 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>POST /AVTransport/ffffffff-8182-6fa4-75b3-19f8729c8a30/control.xml HTTP/1.1
SOAPAction: "urn:schemas-upnp-org:service:AVTransport:1#GetTransportInfo"
User-Agent: UPnP/1.0 IQIYIDLNA/iqiyidlna/NewDLNA/1.0
Connection: keep-alive
Host: 192.168.3.71:1422
Content-Length: 314
Content-Type: text/xml; charset="utf-8"

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfo xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><InstanceID>0</InstanceID></u:GetTransportInfo></s:Body></s:Envelope>HTTP/1.1 200 OK
Ext: 
Date: Mon, 22 Aug 2022 07:47:24 GMT
Content-Length: 439
Content-Type: text/xml; charset="utf-8"
Server: UPnP/1.0 DLNADOC/1.50 Platinum/1.0.5.13

<?xml version="1.0" encoding="UTF-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><u:GetTransportInfoResponse xmlns:u="urn:schemas-upnp-org:service:AVTransport:1"><CurrentTransportState>PLAYING</CurrentTransportState><CurrentTransportStatus>OK</CurrentTransportStatus><CurrentSpeed>1</CurrentSpeed></u:GetTransportInfoResponse></s:Body></s:Envelope>
