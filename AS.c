#include "Tools/DES.c"
#include "Tools/MD5.c"
#include "Tools/TCP.c"
#include "config.c"
#include <stdio.h>
#define K_C "C#######"       // Cͨ������HASH�õ���C��AS�������Կ
#define K_TGS "TGS#####"     // AS��TGS����ļ�����Կ
#define LIFETIME2 "00086400" // Ticket_tgsƱ����Ч�ڣ�1day

// AS�յ�M1,����󷵻�M2
char *deal(char *M1, char *K_c, char *K_tgs) {

    printf("AS <- C (M1)       :%s \n", M1);
    // M1 = ID_c || ID_tgs || TS_1

    char *ID_c = substr(M1, 0, 8);
    char *ID_tgs = substr(M1, 8, 8);
    char *TS_2 = timeToChars(time(NULL));

    // M2 = Encode(K_c, K_C_tgs || ID_tgs || TS_2 || Lifetime2 || Ticket_tgs);
    // Ticket_tgs = Encode(K_tgs, ID_c || AD_c || ID_tgs || TS_2 || Lifetime2);
    char *K_c_tgs = timeToChars(rand() * rand() % 100000000);
    char *ticket_tgs = strcatPro(6, K_c_tgs, ID_c, "12700001", ID_tgs, TS_2, LIFETIME2);
    char *decode_ticket_tgs = des_encode(K_tgs, ticket_tgs, strlen(ticket_tgs));
    char *M2 = strcatPro(5, K_c_tgs, ID_tgs, TS_2, LIFETIME2, decode_ticket_tgs);
    char *encode_M2 = des_encode(K_c, M2, strlen(M2));

    printf("��֤ͨ����\n");
    printf("(M2)               :%s\n", M2);
    printf("AS -> C (encode_M2):%s\n", encode_M2);
    return encode_M2;
}
int main() {
    printf("------------------����AS:------------------\n");
    while (1) {
        printf("�ȴ�����������...\n");
        srand(time(NULL));
        char str[BUFFSIZE] = {0};
        TCP_Server_Recv_and_Send(str, BUFFSIZE, AS_PORT, deal, K_C, K_TGS);
        printf("������ɣ�\n");
    }
}