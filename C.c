#include "Tools/DES.c"
#include "Tools/MD5.c"
#include "Tools/TCP.c"
#include "config.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define K_C "C#######"    // 通过口令HASH得到的C与AS共享的密钥(这里省略了HASH这一步)
#define ID_TGS "TGS00001" // 我要访问的TGS的ID
#define ID_V "SERVICE1"   // 我要访问的SERVICE的ID
#define AD_C "12700001"   // 我的IP地址(实际没用到)

char userName[100];     //保存用户名
char userPassword[100]; //保存用户口令

char *userLogin() {
    printf("请输入用户名：");
    scanf("%s", userName);
    printf("请输入密码：");
    scanf("%s", userPassword);
    printf("登陆成功！\n\n");
    return substr(strcatPro(3, "C_", userName, "###########"), 0, 8);
}
// TCP传输的IP和端口号
// C(127.0.0.1,随机)
// AS(127.0.0.1 ,1111)
// TGS(127.0.0.1,2222)
// V(127.0.0.1,3333)
// ID_c向AS服务器(AS_ip,AS_port)请求获得和ID_tgs通信的会话密钥K_c_tgs和票据ticket_tgs
void *authenticateToAS(char *ID_c, char *K_c, char *ID_tgsS, char *AS_ip, int AS_port, char **K_c_tgs,
                       char **ticket_tgs) {
    char *TS_1 = timeToChars(time(NULL));

    char *M1 = strcatPro(3, ID_c, ID_tgsS, TS_1); // C -> AS (M1)
    printf("C -> AS (M1): %s \n", M1);
    char *M2 = TPC_Client_Send_and_Recv(M1, strlen(M1), AS_ip, AS_port); // C <- AS (M2)
    printf("C <- AS (M2): %s \n", M2);
    char *decode_M2 = des_decode(K_c, M2, strlen(M2)); //(decode_M2)
    printf("(decode_M2) : %s \n", decode_M2);

    printf("验证AS服务器：");
    char *TS_2 = substr(decode_M2, 16, 8);
    if (CharsToTime(TS_2) - CharsToTime(TS_1) > TRUST) {
        printf("时间戳错误，不信任服务器AS！程序退出!\n");
        exit(-1);
    }
    //认证AS
    printf("验证AS服务器通过，获得与TGS的会话密钥和票据!\n");

    *K_c_tgs = substr(decode_M2, 0, 8);      //获得与TGS通信的会话密钥!
    *ticket_tgs = substr(decode_M2, 32, 56); //获得请求TGS的服务的票据!
    printf("获得与TGS通信的会话密钥:%s\n", *K_c_tgs);
    printf("获得请求TGS的服务的票据:%s\n\n", *ticket_tgs);
}
char *authenticateToTGS(char *ID_c, char *K_c_tgs, char *ticket_tgs, char *TGS_ip, int TGS_port, char **K_c_v,
                        char **ticket_v) {

    char *TS_3 = timeToChars(time(NULL));
    char *authenticator_c = strcatPro(3, ID_c, AD_C, TS_3);
    char *encode_authenticator_c = des_encode(K_c_tgs, authenticator_c, strlen(authenticator_c));
    char *M3 = strcatPro(3, ID_V, ticket_tgs, encode_authenticator_c);

    printf("C -> TGS (M3): %s \n", M3);
    char *M4 = TPC_Client_Send_and_Recv(M3, strlen(M3), TGS_IP, TGS_PORT);
    printf("C <- TGS (M4): %s \n", M4);
    char *decode_M4 = des_decode(K_c_tgs, M4, strlen(M4));
    printf("(decode_M4)  : %s \n", decode_M4);

    //认证TGS
    printf("验证TGS服务器：");
    char *TS_4 = substr(decode_M4, 16, 8);
    printf("%d %d", CharsToTime(TS_4), CharsToTime(TS_3));
    if (CharsToTime(TS_4) - CharsToTime(TS_3) > TRUST) {

        printf("时间戳错误，不信任服务器TGS！程序退出!\n");
        exit(-1);
    }
    printf("验证TGS服务器通过，获得与V的会话密钥和票据!\n");

    *K_c_v = substr(decode_M4, 0, 8);      //获得与V通信的会话密钥!
    *ticket_v = substr(decode_M4, 24, 56); //获得请求V的服务的票据!
    printf("获得与V通信的会话密钥:%s\n", *K_c_v);
    printf("获得请求V的服务的票据:%s\n\n", *ticket_v);
}
char *authenticateToV(char *ID_c, char *K_c_v, char *ticket_v, char *V_ip, int V_port) {
    char *TS_5 = timeToChars(time(NULL));
    char *authenticator_c = strcatPro(3, ID_c, AD_C, TS_5);
    char *encode_authenticator_c = des_encode(K_c_v, authenticator_c, strlen(authenticator_c));
    char *M5 = strcatPro(2, ticket_v, encode_authenticator_c);

    printf("C -> V (M5): %s \n", M5);
    char *M6 = TPC_Client_Send_and_Recv(M5, strlen(M5), V_ip, V_port);
    printf("C <- V (M6): %s \n", M6);
    char *decode_M6 = des_decode(K_c_v, M6, strlen(M6));
    printf("(decode_M6): %s \n", decode_M6);

    //认证V
    printf("验证V服务器：");
    char *TS_6 = substr(decode_M6, 0, 8);
    if (CharsToTime(TS_6) - CharsToTime(TS_5) != 1) {
        printf("时间戳错误，不信任服务器V！程序退出!\n");
        exit(-1);
    }
    printf("验证V服务器通过，认证结束!\n\n");
}
int main() {
    printf("------------------进程Client:------------------\n");
    char *ID_c = userLogin();

    char *K_c_tgs, *ticket_tgs;
    authenticateToAS(ID_c, K_C, ID_TGS, AS_IP, AS_PORT, &K_c_tgs, &ticket_tgs);
    char *K_c_v, *ticket_v;
    authenticateToTGS(ID_c, K_c_tgs, ticket_tgs, TGS_IP, TGS_PORT, &K_c_v, &ticket_v);
    authenticateToV(ID_c, K_c_v, ticket_v, V_IP, V_PORT);
}
