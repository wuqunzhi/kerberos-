#include "Tools/DES.c"
#include "Tools/MD5.c"
#include "Tools/TCP.c"
#include "config.c"
#include <stdio.h>
#define K_V "V#######" // TGS和V共享的加密密钥
char *deal(char *M5, char *K_v, char *K_else1) {
    printf("V <- C (M5)       :%s \n", M5);
    // M5 = ticket_v || Authenticator_c;

    // 1) ticket_v = Encode(K_v, K_c_v || ID_c || AD_c || ID_v || TS_4 || Lifetime4);
    char *ticket_v = substr(M5, 0, 56);
    char *decode_ticket_v = des_decode(K_v, ticket_v, strlen(ticket_v));
    char *K_c_v = substr(decode_ticket_v, 0, 8);
    char *ID_c_inTicket = substr(decode_ticket_v, 8, 8);
    char *AD_c_inTicket = substr(decode_ticket_v, 16, 8);
    char *ID_V_inTicket = substr(decode_ticket_v, 24, 8);
    char *TS_4_inTicket = substr(decode_ticket_v, 32, 8);
    char *Lifetime4_inTicket = substr(decode_ticket_v, 40, 8);

    // 2) Authenticator_c = Encode(K_c_v, ID_c || AD_c || TS_5);
    char *authenticator_c = substr(M5, 56, 32);
    char *decode_authenticator_c = des_decode(K_c_v, authenticator_c, strlen(authenticator_c));
    char *ID_c_inAuthenticator_c = substr(decode_authenticator_c, 0, 8);
    char *AD_c_inAuthenticator_c = substr(decode_authenticator_c, 8, 8);
    char *TS_5_inAuthenticator_c = substr(decode_authenticator_c, 16, 8);

    //认证：
    if (time(NULL) % 10000000 - CharsToTime(TS_5_inAuthenticator_c) > TRUST ||
        time(NULL) % 10000000 - CharsToTime(TS_5_inAuthenticator_c) < 0) {
        printf("%s\n", TS_5_inAuthenticator_c);
        printf("%d %d\n", time(NULL) % 10000000, CharsToTime(TS_5_inAuthenticator_c));
        printf("时间戳不满足误差，认证失败！\n");
        exit(-1);
    }
    if (time(NULL) % 10000000 - CharsToTime(TS_4_inTicket) > CharsToTime(Lifetime4_inTicket)) {
        printf("票据过期，拒绝授权！\n");
        exit(-1);
    }
    if (strcmp(ID_c_inAuthenticator_c, ID_c_inTicket) != 0 ||
        strcmp(AD_c_inAuthenticator_c, AD_c_inTicket) != 0) {
        printf("用户身份与票据不对应，拒绝授权！\n");
        exit(-1);
    }
    printf("认证通过！\n");

    // M6 = Encode(K_c_v, TS_6); //TS_6 = TS_5 + 1
    char *M6 = timeToChars(CharsToTime(TS_5_inAuthenticator_c) + 1);
    char *encode_M6 = des_encode(K_c_v, M6, strlen(M6));

    printf("V -> C (encode_M6):%s \n", encode_M6);

    return encode_M6;
}
int main() {
    printf("------------------进程V:------------------\n");
    while (1) {
        printf("等待服务请求中...\n");
        char str[BUFFSIZE] = {0};
        TCP_Server_Recv_and_Send(str, BUFFSIZE, V_PORT, deal, K_V, NULL);
        printf("服务完成！\n");
    }
}