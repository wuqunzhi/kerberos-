#include <stdio.h>
#include <winsock2.h>
#define BUFFSIZE 1024   // TCP���仺������С
#define TCPQUEUECOUNT 2 // TCP�Ŷ����ӵ�������
#pragma comment(lib, "ws2_32.lib")
//������Ϣ��return�Է����ص���Ϣ
char *TPC_Client_Send_and_Recv(char *buf1, int bufLen1, const char *ip, const int port) {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        printf("WSA startup failed\n");

    //����soocket----------------------------------------------------------
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //�ͻ��˿ɲ���bind���˿ں����
    struct sockaddr_in remote = {0};
    remote.sin_family = AF_INET;
    remote.sin_port = htons(port);
    remote.sin_addr.s_addr = inet_addr(ip);

    //���ӷ�����------------------------------------------------------------
    int yy = connect(sock, (void *)&remote, sizeof(remote));

    //������Ϣ--------------------------------------------------------------
    send(sock, buf1, bufLen1, 0);

    //������Ϣ--------------------------------------------------------------
    char *buf2 = (char *)malloc(BUFFSIZE);
    recv(sock, buf2, BUFFSIZE, 0);

    if (WSAGetLastError() != 0)
        printf("TCPerror = %d\n", WSAGetLastError());

    closesocket(sock);
    WSACleanup();
    return buf2;
}

//������Ϣ���������ٷ��ͻ�ȥ
void TCP_Server_Recv_and_Send(char *buf1, int bufLen1, const int port,
                              char *deal(char *str, char *K1, char *K2), char *K1, char *K2) {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        printf("WSA startup failed\n");

    //����soocket----------------------------------------------------------
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //Э����AF_INET��ipv4+�˿ں�
    struct sockaddr_in s = {0};
    s.sin_family = AF_INET;
    s.sin_port = htons(port);
    s.sin_addr.s_addr = inet_addr("127.0.0.1"); //����

    //��(socket,����ip�Ͷ˿�)------------------------------------------------
    bind(sock, (void *)&s, sizeof(s));

    //�����˿�----------------------------------------------------------------
    listen(sock, TCPQUEUECOUNT); //����Ŷ�2��tcp����

    //��������----------------------------------------------------------------
    struct sockaddr_in RemoteAddr = {0}; //�Է���ַ��Ϣ
    int rsocklen = sizeof(RemoteAddr);
    SOCKET rsock = accept(sock, (void *)&RemoteAddr, &rsocklen); //����ʽ

    //������Ϣ----------------------------------------------------------------
    recv(rsock, buf1, bufLen1, 0);

    //������Ϣ----------------------------------------------------------------
    char *buf2 = deal(buf1, K1, K2); //�����յ�����Ϣ��
    send(rsock, buf2, BUFFSIZE, 0);

    if (WSAGetLastError() != 0) {
        printf("TCPerror = %d\n", WSAGetLastError());
        exit(-1);
    }
    closesocket(rsock);
    closesocket(sock);
}

/*��λ�ȡsocket��ip?
SOCKADDR_IN sockAddr;
int iLen=sizeof(sockAddr);
getpeername(m_lClient,(struct sockaddr *)&sockAddr,&iLen);//�õ�Զ��IP��ַ�Ͷ˿ں�
getsockname(m_lClient,(struct sockaddr *)&sockAddr,&iLen);//�õ����ص�IP��ַ�Ͷ˿ں�
strAddr = inet_ntoa(sockAddr.sin_addr);//IP
uIPPort = sockAddr.sin_port;��//�˿ں�
*/
