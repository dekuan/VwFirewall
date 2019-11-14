//////////////////////////////////////////////////////////////////////////
//	HttpServerHeaders.h
//

#ifndef __HTTPSERVERHEADERS_HEADER__
#define __HTTPSERVERHEADERS_HEADER__



/**
 *	Http server headers
 */
static TCHAR * g_ArrHttpServerHeaders[] =
{
	"Apache-Coyote/1.1",
	"Apache Tomcat/4.1.12(HTTP/1.1 Connector)",
	"Apache/1.3.19(Unix)(Red-Hat/Linux)mod_ssl/2.8.1 OpenSSL/0.9.6 DA",
	"Apache/1.3.23(Unix)",
	"Apache/1.3.28(Unix)PHP/4.3.2",
	"Apache/1.3.33(Unix)",
	"Apache/1.3.36(Unix)mod_auth_passthrough/1.8 mod_log_bytes/1.2 mod",
	"Apache/1.3.9(Mac OS X Server)",
	"Apache/1.3.9(Unix) Debian/GNU",
	"Apache/2.0",
	"Apache/2.0.40(Unix) DAV/2",
	"Apache/2.0.52(Red hat)",
	"Apache/2.0.52(Red Hat) mod_perl/1.99_16 Perl/v5.8.5 DAV/2",
	"Apache/2.0.54(Fedora)",
	"Apache/2.2.0(Unix) mod_ssl/2.2.0 OpenSSL/0.9.7g",
	"Apache/2.2.4(Unix) PHP/4.4.5 mod_perl/2.02 Perl/v5.8.8",
	"Apache/2.2.6(Unix)",
	"Apache/2.2.9(Unix)",
	"GWS/2.1",
	"HPWB/4.3.6(Win32) ApacheJServ/1.0b1",
	"IBM_HTTP_Server",
	"IBM_HTTP_Server/2.0.47.1 Apache/2.0.47(Unix)",
	"iTP Secure WebServer/5.0",
	"Jetty/5.1 5rc1(SunOS/5.8 sparc java/1.4.2_05)",
	"lighttpd/1.3.16",
	"lighttpd/1.4.19",
	"LiveCache/2.4a",
	"LiveServer/5.2",
	"Lotus-Domino",
	"Lotus-Domino/0",
	"Lotus-Domino/5.0.8",
	"Netscape-Enterprise/3.6 SP3",
	"Netscape-Enterprise/4.1",
	"Netscape-Enterprise/6.0",
	"Oracle-Application-Server-10g/10.1.2.0.2 Oracle-HTTP-Server OracleAS",
	"Oracle9iAS/9.0.2 Oracle HTTP Server Powered by Apache/1.3.19(Unix)",
	"SAP J2EE Engine/6.40",
	"Stronghold/2.4.2 Apache/1.3.6 C2NetEU/2412(Unix)",
	"Stronghold/3.0 Apache/1.3.19 RedHat/3014c mod_fastcgi/2.2.10",
	"Sun-ONE-Web-Server/6.1",
	"WebLogic 5.1.0 Service Pack 10 07/11/2001 21:04:48 #126882",
	"WebSphere Application Server/5.1",
	"WebSphere Application Server/6.0",
	"WebSTAR/4.5(SSL)",
	"Zeus/4.1",
	"Zeus/4.3",
	"Zope.server.http(HTTP)",
	"Zope/(Zope 2.5.1 (binary release, python 2.1, linux2-x86), python 2.1.3, linux2) ZServer/1.1b1"	
};

#define HTTPSERVERHEADERS_COUNT		( sizeof(g_ArrHttpServerHeaders) / sizeof(g_ArrHttpServerHeaders[0]) )


/**
 *	检测是否是有效的索引值
 */
static BOOL HttpServerHeaders_IsValidIndex( LONG lnIndex )
{
	return ( lnIndex >= 0 && lnIndex < HTTPSERVERHEADERS_COUNT );
}







#endif	// __HTTPSERVERHEADERS_HEADER__