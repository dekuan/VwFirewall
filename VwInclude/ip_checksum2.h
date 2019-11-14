//数据包结构体
#pragma pack(1) 

//	物理帧头结构 14 字节
typedef struct
{
	BYTE desmac[6];		//	目的MAC地址
	BYTE srcmac[6];		//	源MAC地址
	USHORT ethertype;	//	帧类型
}Dlc_Header;

//	Arp帧结构
typedef struct
{
	USHORT hw_type;		//	硬件类型Ethernet:0x1
	USHORT prot_type;	//	上层协议类型IP:0x0800
	BYTE hw_addr_len;	//	硬件地址长度:6
	BYTE prot_addr_len;	//	协议地址(IP地址)的长度:4
	USHORT flag;		//	1表示请求,2表示应答
	BYTE send_hw_addr[6];	//	源MAC地址
	UINT send_prot_addr;	//	源IP地址
	BYTE targ_hw_addr[6];	//	目的MAC地址
	UINT targ_prot_addr;	//	目的IP地址
	BYTE padding[18];	//	填充数据  
}Arp_Frame;

//	ARP包=DLC头+ARP帧
typedef struct
{
	Dlc_Header dlcheader;	//DLC头
	Arp_Frame arpframe;	//ARP帧
}ARP_Packet;

//	IP报头结构
typedef struct
{
	BYTE  ver_len;       //IP包头部长度,单位：4字节
	BYTE  tos;           //服务类型TOS
	USHORT total_len;    //IP包总长度 
	USHORT ident;        //标识
	USHORT frag_and_flags;  //标志位
	BYTE ttl;           //生存时间
	BYTE proto;         //协议
	USHORT checksum;    //IP首部校验和
	UINT  sourceIP;  //源IP地址(32位)
	UINT  destIP;    //目的IP地址(32位)
}Ip_Header;

//	TCP报头结构
typedef struct {
	USHORT srcport;   // 源端口
	USHORT dstport;   // 目的端口
	UINT seqnum;      // 顺序号
	UINT acknum;      // 确认号
	BYTE dataoff;     // TCP头长
	BYTE flags;       // 标志（URG、ACK等）
	USHORT window;    // 窗口大小
	USHORT chksum;    // 校验和
	USHORT urgptr;    // 紧急指针
}Tcp_Header;

//	TCP伪首部 用于进行TCP校验和的计算,保证TCP效验的有效性
typedef struct
{
	ULONG  sourceip;    //源IP地址
	ULONG  destip;      //目的IP地址
	BYTE mbz;           //置空(0)
	BYTE ptcl;          //协议类型(IPPROTO_TCP)
	USHORT tcpl;        //TCP包的总长度(单位:字节)
}Tcp_Psd_Header;

//	UDP报头
typedef struct
{
	USHORT srcport;     // 源端口
	USHORT dstport;     // 目的端口
	USHORT total_len;   // 包括UDP报头及UDP数据的长度(单位:字节)
	USHORT chksum;      // 校验和
}Udp_Header;

//	UDP伪首部-仅用于计算校验和
typedef struct tsd_hdr 
{ 
	ULONG  sourceip;    //源IP地址
	ULONG  destip;      //目的IP地址
	BYTE  mbz;           //置空(0)
	BYTE  ptcl;          //协议类型(IPPROTO_UDP)
	USHORT udpl;         //UDP包总长度(单位:字节) 
}Udp_Psd_Header;

//	ICMP报头
typedef struct
{
	BYTE i_type;     //类型 类型是关键:0->回送应答(Ping应答) 8->回送请求(Ping请求)
	BYTE i_code;     //代码 这个与类型有关 当类型为0或8时这里都是0
	USHORT i_cksum;  //ICMP包校验和
	USHORT i_id;     //识别号(一般用进程ID作为标识号)
	USHORT i_seq;    //报文序列号(一般设置为0)
	//UINT timestamp;  //时间戳
	BYTE padding[32];//填充数据
}Icmp_Header;

//	ICMP数据包
typedef struct
{
	Dlc_Header dlc_header;		//	以太帧
	Ip_Header  ip_header;		//	IP头
	Icmp_Header icmp_header;	//	ICMP帧
}Icmp_Packet;

//	攻击信息
typedef struct
{
	unsigned char flag;		//	攻击数据包类型1-arp,2-tcp,3-udp
	unsigned int srcip;		//	攻击者IP
	unsigned char code[33];		//	攻击特征码
}Attack_Infor;

#pragma pack()





//-------------------------------------------------------------------------
// PacketCheckSum
// 计算数据包的校验和
// 参数:packet-待处理数据(将封装好的数据包的指针)
//-------------------------------------------------------------------------
void PacketCheckSum(unsigned char packet[])
{
	Dlc_Header * pdlc_header	= NULL;		//	以太头指针
	Ip_Header * pip_header		= NULL;		//	IP头指针
	unsigned short attachsize	= 0;		//	传输层协议头以及附加数据的总长度

	pdlc_header	= (Dlc_Header *)packet;

	//	判断 ethertype, 如果不是 IP 包则不予处理
	if ( ntohs( pdlc_header->ethertype ) != 0x0800 )
	{
		return;
	}

	pip_header	= (Ip_Header*)( packet + 14 );


	if ( 0x06 == pip_header->proto )
	{
		//
		//	TCP包
		//
		Tcp_Header * ptcp_header		= NULL; //TCP头指针
		Tcp_Psd_Header * ptcp_psd_header	= NULL;
		UINT uIpHeaderLen;
		UINT uTcpHeaderOffset;

		//	IP头长度
		uIpHeaderLen		= ( ( pip_header->ver_len ) & 15 ) * 4;
		uTcpHeaderOffset	= 14 + uIpHeaderLen;
		ptcp_header		= (Tcp_Header*)( packet + uTcpHeaderOffset );

		//	传输层协议头以及附加数据的总长度
		//	TCP头部+TCP数据整个长度
		attachsize		= ntohs( pip_header->total_len ) - uIpHeaderLen;
		ptcp_psd_header		= (Tcp_Psd_Header*)malloc( attachsize + sizeof(Tcp_Psd_Header) );

		if ( ptcp_psd_header )
		{
			memset( ptcp_psd_header, 0, attachsize + sizeof(Tcp_Psd_Header) );

			//	填充伪 TCP 头
			ptcp_psd_header->destip		= pip_header->destIP;
			ptcp_psd_header->sourceip	= pip_header->sourceIP;
			ptcp_psd_header->mbz		= 0;
			ptcp_psd_header->ptcl		= 0x06;
			ptcp_psd_header->tcpl		= htons( attachsize );

			//	计算TCP校验和
			ptcp_header->chksum		= 0;
			memcpy( (unsigned char*)ptcp_psd_header + sizeof(Tcp_Psd_Header), (unsigned char*)ptcp_header, attachsize );
			ptcp_header->chksum		= checksum( (unsigned short *)ptcp_psd_header, attachsize+sizeof(Tcp_Psd_Header) );

			//	计算ip头的校验和
			pip_header->checksum		= 0;
			pip_header->checksum		= checksum( (unsigned short *)pip_header, 20 );
		}
	}
	else if ( 0x11 == pip_header->proto )
	{
		//
		//	UDP包
		//
		Udp_Header * pudp_header		= NULL;	//	UDP头指针
		Udp_Psd_Header * pudp_psd_header	= NULL;
		pudp_header	= (Udp_Header*)( packet + 14 + ((pip_header->ver_len)&15 ) * 4 );
		attachsize	= ntohs( pip_header->total_len ) - ( ( pip_header->ver_len ) & 15 ) * 4;
		pudp_psd_header	= (Udp_Psd_Header*)malloc( attachsize+sizeof(Udp_Psd_Header) );
		if ( pudp_psd_header )
		{
			memset( pudp_psd_header, 0, attachsize+sizeof(Udp_Psd_Header) );
			
			//	填充伪UDP头
			pudp_psd_header->destip		= pip_header->destIP;
			pudp_psd_header->sourceip	= pip_header->sourceIP;
			pudp_psd_header->mbz		= 0;
			pudp_psd_header->ptcl		= 0x11;
			pudp_psd_header->udpl		= htons(attachsize);

			//	计算UDP校验和
			pudp_header->chksum		= 0;
			memcpy( (unsigned char *)pudp_psd_header+sizeof(Udp_Psd_Header), (unsigned char *)pudp_header, attachsize );
			pudp_header->chksum		= checksum( (unsigned short *)pudp_psd_header, attachsize + sizeof(Udp_Psd_Header) );

			//	计算ip头的校验和
			pip_header->checksum		= 0;
			pip_header->checksum		= checksum( (unsigned short *)pip_header, 20 );
		}
	}

	return;
}
