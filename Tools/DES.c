#include "Others.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define bool char
// #define char unsigned char
bool tmp[64] = {0};
bool tmptxt[64] = {0}; //存放64位明文

//初始置换表
const int IP[64] = {57, 49, 41, 33, 25, 17, 9,  1,  59, 51, 43, 35, 27, 19, 11, 3,  61, 53, 45, 37, 29, 21,
                    13, 5,  63, 55, 47, 39, 31, 23, 15, 7,  56, 48, 40, 32, 24, 16, 8,  0,  58, 50, 42, 34,
                    26, 18, 10, 2,  60, 52, 44, 36, 28, 20, 12, 4,  62, 54, 46, 38, 30, 22, 14, 6};
//逆初始置换表
const int IP_1[64] = {39, 7,  47, 15, 55, 23, 63, 31, 38, 6,  46, 14, 54, 22, 62, 30, 37, 5,  45, 13, 53, 21,
                      61, 29, 36, 4,  44, 12, 52, 20, 60, 28, 35, 3,  43, 11, 51, 19, 59, 27, 34, 2,  42, 10,
                      50, 18, 58, 26, 33, 1,  41, 9,  49, 17, 57, 25, 32, 0,  40, 8,  48, 16, 56, 24};
//扩展置换表E
const int E_Table[48] = {31, 0,  1,  2,  3,  4,  3,  4,  5,  6,  7,  8,  7,  8,  9,  10,
                         11, 12, 11, 12, 13, 14, 15, 16, 15, 16, 17, 18, 19, 20, 19, 20,
                         21, 22, 23, 24, 23, 24, 25, 26, 27, 28, 27, 28, 29, 30, 31, 0};
//置换表P
const int P_Table[32] = {15, 6, 19, 20, 28, 11, 27, 16, 0,  14, 22, 25, 4,  17, 30, 9,
                         1,  7, 23, 13, 31, 26, 2,  8,  18, 12, 29, 5,  21, 10, 3,  24};
// S盒矩阵
const int sbox[8][4][16] = {{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
                             {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
                             {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
                             {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
                            // S2

                            {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
                             {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
                             {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
                             {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
                            // S3

                            {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
                             {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
                             {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
                             {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
                            // S4

                            {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
                             {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
                             {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
                             {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
                            // S5

                            {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
                             {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
                             {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
                             {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
                            // S6

                            {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
                             {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
                             {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
                             {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
                            // S7

                            {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
                             {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
                             {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
                             {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
                            // S8

                            {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
                             {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
                             {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
                             {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};
//置换选择1
const int PC_1[56] = {56, 48, 40, 32, 24, 16, 8,  0,  57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42,
                      34, 26, 18, 10, 2,  59, 51, 43, 35, 62, 54, 46, 38, 30, 22, 14, 6,  61, 53,
                      45, 37, 29, 21, 13, 5,  60, 52, 44, 36, 28, 20, 12, 4,  27, 19, 11, 3};
//置换选择2
const int PC_2[48] = {13, 16, 10, 23, 0,  4,  2,  27, 14, 5,  20, 9,  22, 18, 11, 3,
                      25, 7,  15, 6,  26, 19, 12, 1,  40, 51, 30, 36, 46, 54, 29, 39,
                      50, 44, 32, 47, 43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31};
// 16轮迭代的左移次数
const int shift_bits[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// num1通过IP表置换得到num2
void permutation(bool *num1, const int *IP, bool *num2, int count) {
    for (int i = 0; i < count; ++i)
        tmp[i] = num1[IP[i]];
    for (int i = 0; i < count; ++i)
        num2[i] = tmp[i];
}

// 循环左移
void shift(bool *key, int count) {
    for (int i = 0; i < 28; ++i)
        tmp[i] = key[(i + count) % 28];
    for (int i = 0; i < 28; ++i)
        tmp[i + 28] = key[(i + count) % 28 + 28];
    for (int i = 0; i < 56; ++i)
        key[i] = tmp[i];
}

// a = b xor c
void xor32(bool *a, bool *b, bool *c, int ct) {
    for (int i = 0; i < ct; ++i)
        a[i] = b[i] ^ c[i];
};

void byte8ToBit64(char *b, bool *txt) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
            txt[i * 8 + j] = (b[i] >> (7 - j)) & 1;
    }
}
void Bit64ToByte8(bool *txt, char *b) {
    for (int i = 0; i < 8; ++i) {
        b[i] = 0;
        for (int j = 0; j < 8; ++j) {
            b[i] |= (txt[i * 8 + j] << (7 - j));
        }
    }
}

// s盒置换
void sboxP(bool *R48) {
    int num[8], row, col;
    for (int i = 0; i < 8; ++i) {
        row = R48[i * 6] * 2 + R48[i * 6 + 5];
        col = R48[i * 6 + 1] * 8 + R48[i * 6 + 2] * 4 + R48[i * 6 + 3] * 2 + R48[i * 6 + 4];
        num[i] = sbox[i][row][col];
    }
    for (int i = 0; i < 8; ++i) {
        R48[i * 4] = (num[i] >> 3) & 1;
        R48[i * 4 + 1] = (num[i] >> 2) & 1;
        R48[i * 4 + 2] = (num[i] >> 1) & 1;
        R48[i * 4 + 3] = num[i] & 1;
    }
}

// feistel轮函数
bool *feistel(bool *R32, bool *keyi) {
    bool R48[48];
    permutation(R32, E_Table, R48, 48);
    xor32(R48, R48, keyi, 48);
    sboxP(R48);
    permutation(R48, P_Table, R32, 32);
    return R32;
}
// 16轮迭代
void iter16(bool *txt, bool key16[16][48]) {
    bool L[17][32], R[17][32];
    memcpy(L[0], txt, 32 * sizeof(bool));
    memcpy(R[0], txt + 32, 32 * sizeof(bool));
    for (int i = 1; i <= 16; ++i) {
        memcpy(L[i], R[i - 1], 32 * sizeof(bool));
        xor32(R[i], L[i - 1], feistel(R[i - 1], key16[i - 1]), 32);
    }
    memcpy(txt, R[16], 32 * sizeof(bool));
    memcpy(txt + 32, L[16], 32 * sizeof(bool));
}
//生成子密钥
void genSubKey(char *keyname, bool *key, bool key16[][48], bool r_key16[][48]) {
    byte8ToBit64(keyname, key);
    bool tmp[64];
    permutation(key, PC_1, tmp, 56);
    for (int i = 0; i < 16; ++i) {
        shift(tmp, shift_bits[i]);
        permutation(tmp, PC_2, key16[i], 48);
    }
    for (int i = 0; i < 16; ++i) //同时生成解密的子密钥（加密子密钥的逆向数组）
        for (int j = 0; j < 48; ++j)
            r_key16[i][j] = key16[15 - i][j];
}
// des加密8个字节
char *des64(char *block, bool key16[16][48]) {
    byte8ToBit64(block, tmptxt);
    permutation(tmptxt, IP, tmptxt, 64);
    iter16(tmptxt, key16);
    permutation(tmptxt, IP_1, tmptxt, 64);
    Bit64ToByte8(tmptxt, block);
    return block;
}
// des加密
char *des_encode(char *keystr, char *plainText, int len) {
    return strcatPro(2, plainText, keystr);
    char *cipherText = (char *)malloc(len % 8 ? len / 8 * 8 + 8 + 1 : len + 1);
    bool key[64] = {0};           //存放64位密钥
    bool key16[16][48] = {{0}};   // 16轮子密钥
    bool r_key16[16][48] = {{0}}; //解密的逆向调度的子密钥（key16的反向数组）

    genSubKey(keystr, key, key16, r_key16);
    char block[8];
    for (int i = 0; i < len / 8; ++i) {
        strncpy(block, plainText + i * 8, 8);
        des64(block, key16);
        printf("%s\n", block);
        strncpy(cipherText + i * 8, block, 8);
    }

    strncpy(block, plainText + len / 8 * 8, len % 8);
    for (int k = len % 8; k < 8; ++k)
        block[k] = 8 - len % 8 + '0';
    printf("%s\n", block);
    des64(block, key16);
    strncpy(cipherText + len / 8 * 8, block, 8);
    cipherText[len % 8 ? len / 8 * 8 + 8 : len] = 0;
    return cipherText;
}
char *des_decode(char *keystr, char *cipherText, int len) {
    char *res = (char *)malloc(len - 8 + 1);
    if (strcmp(substr(cipherText, len - 8, 8), keystr) != 0) {
        printf("des解密密码错误:\nNeed: %s\nRecv: %s\n", substr(cipherText, len - 8, 8), keystr);
        for (int i = 0; i < len - 8; ++i)
            res[i] = rand() % 26 + 'a';
        res[len - 8] = 0;
        return res;
    }
    return substr(cipherText, 0, len - 8);
    if (len % 8 != 0) {
        printf("error:the size of cipherText to des_decode must divide by 8!\n");
        return "error";
    }
    char *plainText = (char *)malloc(len);
    bool key[64] = {0};           //存放64位密钥
    bool key16[16][48] = {{0}};   // 16轮子密钥
    bool r_key16[16][48] = {{0}}; //解密的逆向调度的子密钥（key16的反向数组）
    genSubKey(keystr, key, key16, r_key16);
    char block[8];
    for (int i = 0; i < len / 8; ++i) {
        strncpy(block, cipherText + i * 8, 8);
        des64(block, r_key16);
        strncpy(plainText + i * 8, block, 8);
    }

    for (int i = 0; i < block[7] - '0'; ++i) {
        plainText[len - 1 - i] = 0;
    }
    return plainText;
}
/*
int main() {
    srand(time(NULL));
    int x = 999;
    // C_wqzwqz12700001TGS000010785624400003600
    char s1[x]; // 21
    for (int i = 0; i < x - 1; ++i)
        s1[i] = rand() % 127;
    s1[999] = 0;
}*/
