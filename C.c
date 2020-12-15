#include "Tools/DES.c"
#include "Tools/MD5.c"
#include "Tools/TCP.c"
#include "config.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define K_C "C#######"    // ͨ������HASH�õ���C��AS�������Կ(����ʡ����HASH��һ��)
#define ID_TGS "TGS00001" // ��Ҫ���ʵ�TGS��ID
#define ID_V "SERVICE1"   // ��Ҫ���ʵ�SERVICE��ID
#define AD_C "12700001"   // �ҵ�IP��ַ(ʵ��û�õ�)

char userName[100];     //�����û���
char userPassword[100]; //�����û�����

char *userLogin() {
    printf("�������û�����");
    scanf("%s", userName);
    printf("���������룺");
    scanf("%s", userPassword);
    printf("��½�ɹ���\n\n");
    return substr(strcatPro(3, "C_", userName, "###########"), 0, 8);
}
// TCP�����IP�Ͷ˿ں�
// C(127.0.0.1,���)
// AS(127.0.0.1 ,1111)
// TGS(127.0.0.1,2222)
// V(127.0.0.1,3333)
// ID_c��AS������(AS_ip,AS_port)�����ú�ID_tgsͨ�ŵĻỰ��ԿK_c_tgs��Ʊ��ticket_tgs
void *authenticateToAS(char *ID_c, char *K_c, char *ID_tgsS, char *AS_ip, int AS_port, char **K_c_tgs,
                       char **ticket_tgs) {
    char *TS_1 = timeToChars(time(NULL));

    char *M1 = strcatPro(3, ID_c, ID_tgsS, TS_1); // C -> AS (M1)
    printf("C -> AS (M1): %s \n", M1);
    char *M2 = TPC_Client_Send_and_Recv(M1, strlen(M1), AS_ip, AS_port); // C <- AS (M2)
    printf("C <- AS (M2): %s \n", M2);
    char *decode_M2 = des_decode(K_c, M2, strlen(M2)); //(decode_M2)
    printf("(decode_M2) : %s \n", decode_M2);

    printf("��֤AS��������");
    char *TS_2 = substr(decode_M2, 16, 8);
    if (CharsToTime(TS_2) - CharsToTime(TS_1) > TRUST) {
        printf("ʱ������󣬲����η�����AS�������˳�!\n");
        exit(-1);
    }
    //��֤AS
    printf("��֤AS������ͨ���������TGS�ĻỰ��Կ��Ʊ��!\n");

    *K_c_tgs = substr(decode_M2, 0, 8);      //�����TGSͨ�ŵĻỰ��Կ!
    *ticket_tgs = substr(decode_M2, 32, 56); //�������TGS�ķ����Ʊ��!
    printf("�����TGSͨ�ŵĻỰ��Կ:%s\n", *K_c_tgs);
    printf("�������TGS�ķ����Ʊ��:%s\n\n", *ticket_tgs);
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

    //��֤TGS
    printf("��֤TGS��������");
    char *TS_4 = substr(decode_M4, 16, 8);
    printf("%d %d", CharsToTime(TS_4), CharsToTime(TS_3));
    if (CharsToTime(TS_4) - CharsToTime(TS_3) > TRUST) {

        printf("ʱ������󣬲����η�����TGS�������˳�!\n");
        exit(-1);
    }
    printf("��֤TGS������ͨ���������V�ĻỰ��Կ��Ʊ��!\n");

    *K_c_v = substr(decode_M4, 0, 8);      //�����Vͨ�ŵĻỰ��Կ!
    *ticket_v = substr(decode_M4, 24, 56); //�������V�ķ����Ʊ��!
    printf("�����Vͨ�ŵĻỰ��Կ:%s\n", *K_c_v);
    printf("�������V�ķ����Ʊ��:%s\n\n", *ticket_v);
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

    //��֤V
    printf("��֤V��������");
    char *TS_6 = substr(decode_M6, 0, 8);
    if (CharsToTime(TS_6) - CharsToTime(TS_5) != 1) {
        printf("ʱ������󣬲����η�����V�������˳�!\n");
        exit(-1);
    }
    printf("��֤V������ͨ������֤����!\n\n");
}
int main() {
    printf("------------------����Client:------------------\n");
    char *ID_c = userLogin();

    char *K_c_tgs, *ticket_tgs;
    authenticateToAS(ID_c, K_C, ID_TGS, AS_IP, AS_PORT, &K_c_tgs, &ticket_tgs);
    char *K_c_v, *ticket_v;
    authenticateToTGS(ID_c, K_c_tgs, ticket_tgs, TGS_IP, TGS_PORT, &K_c_v, &ticket_v);
    authenticateToV(ID_c, K_c_v, ticket_v, V_IP, V_PORT);
}
