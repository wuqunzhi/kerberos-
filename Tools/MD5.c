#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned int uint;
typedef unsigned char uchar;
uint T[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
              0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
              0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
              0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
              0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
              0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
              0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
              0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
int S[4][4] = {{7, 12, 17, 22}, {5, 9, 14, 20}, {4, 11, 16, 23}, {6, 10, 15, 21}};

uint reverse(uint a) {
    uint res = 0;
    res |= (a & 0x000000ff) << 24;
    res |= (a & 0x0000ff00) << 8;
    res |= (a & 0x00ff0000) >> 8;
    res |= (a & 0xff000000) >> 24;
    return res;
}

uint md5_P(int ver, uint b, uint c, uint d) {
    switch (ver) {
    case 1:
        return (b & c) | (~b & d);
    case 2:
        return (b & d) | (c & ~d);
    case 3:
        return b ^ c ^ d;
    case 4:
        return c ^ (b | ~d);
    }
}
// a = b + (a + P(b , c , d) + m + t) << s;
void md5_PP(int ver, uint *a, uint b, uint c, uint d, uint m, uint t, int s) {
    uint tmp = *a + md5_P(ver, b, c, d) + m + t;
    uint tmp2 = (tmp << s) | (tmp >> 32 - s);
    *a = b + tmp2;
}
// 填充明文到指定512bit的倍数
int md5_fill(uchar *m, unsigned long len, uint **num) {
    unsigned long fill = ((512 * 2 - len * 8 % 512 - 64 - 1) % 512 + 1) / 8; //填充的字节数。
    unsigned long n = (len + fill + 8) / 4;                                  //总int数
    *num = (uint *)malloc(len + fill + 8);
    for (unsigned long i = 0; i < n; i++) {
        (*num)[i] = 0;
    }

    for (unsigned long i = 0; i < len; i++) {
        (*num)[i / 4] |= m[i] << ((i % 4) * 8); // 小端序
    }

    (*num)[len / 4] |= 0x80 << (len % 4) * 8; // 尾部添加1

    (*num)[n - 2] = len * 8; //小端序
    (*num)[n - 1] = len * 8 >> 16 >> 16;

    return n;
}
//返回16char数组
uchar *MD5(uchar *message, int len) {
    uint *num;
    unsigned long n = md5_fill(message, len, &num); // int数
    uint A = 0x67452301;
    uint B = 0xEFCDAB89;
    uint C = 0x98BADCFE;
    uint D = 0x10325476;
    unsigned a, b, c, d;
    int st[4] = {0, 1, 5, 0};
    int dt[4] = {1, 5, 3, 7};
    for (unsigned long t = 0; t < n / 16; ++t) {
        a = A, b = B, c = C, d = D;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 16; ++j) {
                md5_PP(i + 1, &a, b, c, d, num[(st[i] + dt[i] * j) % 16 + t * 16], T[i * 16 + j],
                       S[i][j % 4]);
                uint tmp = d;
                d = c, c = b, b = a, a = tmp;
            }
        }
        A += a, B += b, C += c, D += d;
    }
    free(num);
    uint res[4] = {A, B, C, D};
    uchar *resc = (uchar *)malloc(16);
    for (int i = 0; i < 16; ++i) {
        resc[i] = res[i / 4] >> i * 8; //转换小端序
    }
    return resc;
}
// test MD5
/*int main() {
    uchar *resc = MD5((uchar *)"18342098", 8); //第二个参数为字符串大小
    for (int i = 0; i < 16; ++i)
        printf("%02X", resc[i]);
    free(resc);
}*/