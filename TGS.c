#include "Tools/DES.c"
#include "Tools/MD5.c"
#include "Tools/TCP.c"
#include "config.c"
#include <stdio.h>
#define ID_TGS "TGS####1"    // 我（TGS）的ID
#define K_TGS "TGS#####"     // TGS和AS共享的密钥
#define K_V "V#######"       // TGS和V共享的密钥
#define LIFETIME4 "00003600" // ticket_v票据有效期，1hour
char *deal(char *M3, char *K_v, char *K_tgs) {
    printf("TGS <- C (M4)       :%s \n", M3);
    // M3 = ID_v || Ticket_tgs || Authenticator;

    // 1) ID_V
    char *ID_v = substr(M3, 0, 8);

    // 2) Ticket_tgs = Encode(K_tgs, K_c_tgs || ID_c || AD_c || ID_tgs || TS2 || LIFETIME2);
    char *ticket_tgs = substr(M3, 8, 56);
    char *decode_ticket_tgs = des_decode(K_tgs, ticket_tgs, strlen(ticket_tgs));

    char *K_c_tgs = substr(decode_ticket_tgs, 0, 8);
    char *ID_c_inTicket = substr(decode_ticket_tgs, 8, 8);
    char *AD_c_inTicket = substr(decode_ticket_tgs, 16, 8);
    char *ID_tgs_inTicket = substr(decode_ticket_tgs, 24, 8);
    char *TS_2_inTicket = substr(decode_ticket_tgs, 32, 8);
    char *Lifetime2_inTicket = substr(decode_ticket_tgs, 40, 8);

    // 3) Authenticator_c = Encode(K_c_tgs, ID_c || AD_c || TS3);
    char *authenticator_c = substr(M3, 64, 32);
    char *decode_authenticator_c = des_decode(K_c_tgs, authenticator_c, strlen(authenticator_c));

    char *ID_c_inAuthenticator_c = substr(decode_authenticator_c, 0, 8);
    char *AD_c_inAuthenticator_c = substr(decode_authenticator_c, 8, 8);
    char *TS_3_inAuthenticator_c = substr(decode_authenticator_c, 16, 8);

    //认证：
    if (time(NULL) % 10000000 - CharsToTime(TS_3_inAuthenticator_c) > TRUST ||
        time(NULL) % 10000000 - CharsToTime(TS_3_inAuthenticator_c) < 0) {
        printf("时间戳不满足误差，认证失败！\n");
        exit(-1);
    }
    if (time(NULL) % 10000000 - CharsToTime(TS_2_inTicket) > CharsToTime(Lifetime2_inTicket)) {
        printf("票据过期，拒绝授权！\n");
        exit(-1);
    }
    if (strcmp(ID_c_inAuthenticator_c, ID_c_inTicket) != 0 ||
        strcmp(AD_c_inAuthenticator_c, AD_c_inTicket) != 0) {
        printf("用户身份与票据不对应，拒绝授权！\n");
        exit(-1);
    }
    printf("认证通过！\n");
    // M4 = Encode(K_c_tgs, K_c_v || ID_v || TS_4 || Ticket_v);
    // Ticket_v = Encode(K_v, K_c_v || ID_c || AD_c || ID_v || TS_4 || LIFETIME4);
    char *ID_c = ID_c_inTicket;
    char *AD_c = AD_c_inTicket;
    char *TS_4 = timeToChars(time(NULL));
    char *K_c_v = timeToChars(rand() * rand() % 100000000); //随机生成会话密钥
    char *ticket_v = strcatPro(6, K_c_v, ID_c, AD_c, ID_v, TS_4, LIFETIME4);
    char *encode_ticket_v = des_encode(K_V, ticket_v, strlen(ticket_v));
    char *M4 = strcatPro(4, K_c_v, ID_v, TS_4, encode_ticket_v);
    char *encode_M4 = des_encode(K_c_tgs, M4, strlen(M4));

    printf("(M4)                :%s \n", M4);
    printf("TGS -> C (encode_M4):%s \n", encode_M4);
    return encode_M4;
}
int main() {
    printf("------------------进程TGS:------------------\n");
    while (1) {
        printf("等待服务请求中...\n");
        srand(time(NULL));
        char str[BUFFSIZE] = {0};
        TCP_Server_Recv_and_Send(str, sizeof(str), TGS_PORT, deal, K_V, K_TGS);
        printf("服务完成！\n");
    }
}