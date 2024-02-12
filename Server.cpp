#include <stdio.h>  
#include <winsock2.h>  
#pragma comment(lib,"ws2_32.lib")	//把ws2_32.lib加到Link页的连接库  
#define PORT 19132					//通信的端口（指服务器端）
#define ERROR 0  
#define BUFFER_SIZE 1024			//注意：此Server端数据接收缓冲区 >= Client端数据发送缓冲区 ，否则造成缓冲区溢出
/*
	服务端原理：
		1、服务器进程创建套接字
		2、将本地地址绑定到所创建的套接字上，以三元组{<通信协议>,<IP地址>,<端口号>}在网络上标识该套接字
		3、将套接字置入监听模式，并准备接受连接请求
*/
int main()  
{  
	WSADATA WSAData;  
	if(WSAStartup(MAKEWORD(2,0),&WSAData)==SOCKET_ERROR)  //启动winsock ，WSAStartup()函数对Winsock DLL进行初始化
	{  
		printf("Socket initialize fail!\n");  
		exit(1);  
	}  
	SOCKET sock;										//服务进程创建套接字句柄（用于监听）
	if((sock=socket(AF_INET,SOCK_STREAM,0))==ERROR)		//调用socket()函数创建一个流套接字，参数（网络地址类型，套接字类型，网络协议）
	{ 
		printf("Socket create!\n");  
		WSACleanup();  
		exit(1);  
	}  
	struct sockaddr_in ServerAddr;			//sockaddr_in结构用来标识TCP/IP协议下的地址，可强制转换为sockaddr结构
	ServerAddr.sin_family=AF_INET;			//sin_family字段必须设为AF_INET，表示该Socket处于Internet域
	ServerAddr.sin_port=htons(PORT);		//sin_port字段用于指定服务端口，注意避免冲突
	ServerAddr.sin_addr.s_addr=INADDR_ANY;  //sin_addr字段用于把一个IP地址保存为一个4字节的数，无符号长整型，根据不同用法还可表示本地或远程IP地址
	if(bind(sock,(LPSOCKADDR)&ServerAddr,sizeof(ServerAddr))==SOCKET_ERROR)  //调用bind()函数将本地地址绑定到所创建的套接字上，以在网络上标识该套接字
	{   
		printf("Bind fail!\n");  
		closesocket(sock);  
		WSACleanup();  
		exit(1);  
	}  
	printf("Server Socket Port:%d\n",ntohs(ServerAddr.sin_port));  
	if(listen(sock,10)==SOCKET_ERROR)		//调用listen()函数将套接字置入监听模式并准备接受连接请求，参数（已捆绑未连接的套接字描述字，正在等待连接的最大队列长度）
	{ 
		printf("Listen fail!\n");  
		closesocket(sock);  
		WSACleanup();  
		exit(1);  
	}  
 
	SOCKET msgsock;			//创建一个新的套接字（用于接收accept函数的返回值，即表示已经接受的那个客户端的连接，进而接收Client发来的数据）
	char buf[BUFFER_SIZE];  //数据接收缓冲区
	while(1)  
	{  
		if((msgsock=accept(sock,(LPSOCKADDR)0,(int *)0))==INVALID_SOCKET)  //进入监听状态后，调用accept()函数接收客户端的连接请求，并把连接传给msgsock套接字，原sock套接字继续监听其他客户机连接请求
		{  
			printf("Accept fail!\n");  
			continue;  
		}  
		memset(buf,0,sizeof(buf));											//初始化数据接收缓冲区
		recv(msgsock,buf,BUFFER_SIZE,0);									//接收客户端发送过来的数据  
		if(buf[0]=='e' && buf[1]=='x' && buf[2]=='i' && buf[3]=='t')		//"exit"命令，退出程序
		{  
			printf("The End.\n");  
			break;  
		}
		printf(":%s",buf);  
		closesocket(msgsock);  
	}  
 
	closesocket(sock); //关闭套接字  
	WSACleanup();	   //终止对Winsock DLL的使用，并释放资源
	return 0;
}   
