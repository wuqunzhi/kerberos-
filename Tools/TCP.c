#include <stdio.h>
#include <winsock2.h>
#define BUFFSIZE 1024   // TCP传输缓冲区大小
#define TCPQUEUECOUNT 2 // TCP排队连接的最大个数
#pragma comment(lib, "ws2_32.lib")
//发送信息，return对方返回的信息
char *TPC_Client_Send_and_Recv(char *buf1, int bufLen1, const char *ip, const int port) {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        printf("WSA startup failed\n");

    //创建soocket----------------------------------------------------------
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //客户端可不用bind，端口号随机
    struct sockaddr_in remote = {0};
    remote.sin_family = AF_INET;
    remote.sin_port = htons(port);
    remote.sin_addr.s_addr = inet_addr(ip);

    //连接服务器------------------------------------------------------------
    int yy = connect(sock, (void *)&remote, sizeof(remote));

    //发送信息--------------------------------------------------------------
    send(sock, buf1, bufLen1, 0);

    //接收信息--------------------------------------------------------------
    char *buf2 = (char *)malloc(BUFFSIZE);
    recv(sock, buf2, BUFFSIZE, 0);

    if (WSAGetLastError() != 0)
        printf("TCPerror = %d\n", WSAGetLastError());

    closesocket(sock);
    WSACleanup();
    return buf2;
}

//接受信息，处理完再发送回去
void TCP_Server_Recv_and_Send(char *buf1, int bufLen1, const int port,
                              char *deal(char *str, char *K1, char *K2), char *K1, char *K2) {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        printf("WSA startup failed\n");

    //创建soocket----------------------------------------------------------
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //协议族AF_INET，ipv4+端口号
    struct sockaddr_in s = {0};
    s.sin_family = AF_INET;
    s.sin_port = htons(port);
    s.sin_addr.s_addr = inet_addr("127.0.0.1"); //本机

    //绑定(socket,本机ip和端口)------------------------------------------------
    bind(sock, (void *)&s, sizeof(s));

    //监听端口----------------------------------------------------------------
    listen(sock, TCPQUEUECOUNT); //最多排队2个tcp连接

    //接受连接----------------------------------------------------------------
    struct sockaddr_in RemoteAddr = {0}; //对方地址信息
    int rsocklen = sizeof(RemoteAddr);
    SOCKET rsock = accept(sock, (void *)&RemoteAddr, &rsocklen); //阻塞式

    //接收信息----------------------------------------------------------------
    recv(rsock, buf1, bufLen1, 0);

    //发送信息----------------------------------------------------------------
    char *buf2 = deal(buf1, K1, K2); //处理收到的信息。
    send(rsock, buf2, BUFFSIZE, 0);

    if (WSAGetLastError() != 0) {
        printf("TCPerror = %d\n", WSAGetLastError());
        exit(-1);
    }
    closesocket(rsock);
    closesocket(sock);
}

/*如何获取socket的ip?
SOCKADDR_IN sockAddr;
int iLen=sizeof(sockAddr);
getpeername(m_lClient,(struct sockaddr *)&sockAddr,&iLen);//得到远程IP地址和端口号
getsockname(m_lClient,(struct sockaddr *)&sockAddr,&iLen);//得到本地的IP地址和端口号
strAddr = inet_ntoa(sockAddr.sin_addr);//IP
uIPPort = sockAddr.sin_port;　//端口号
*/
