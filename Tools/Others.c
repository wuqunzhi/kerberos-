#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// ��count���ַ�������
char *strcatPro(int Count, ...) {
    char *res = (char *)malloc(1000);
    memset(res, 0, 1000);
    va_list vl;
    va_start(vl, Count);
    for (int i = 0; i < Count; i++) {
        strcat(res, va_arg(vl, char *));
    }
    return res;
}
// time_t(����)תchar[8]
char *timeToChars(time_t t) {
    char *res = (char *)malloc(9);
    for (int i = 0; i < 8; ++i) {
        res[7 - i] = t % 10 + '0';
        t /= 10;
    }
    res[8] = '\0';
    return res;
}
// char[8]תtime_t(����)
time_t CharsToTime(char *str) {
    time_t res = 0;
    for (int i = 0; i < 8; ++i) {
        res *= 10;
        res += (str[i] - '0');
    }
    return res;
}
// ���ش�str[start]��ʼ����Ϊn�����ַ���
char *substr(char *str, int start, int n) {
    char *res = (char *)malloc(n + 1);
    strncpy(res, str + start, n);
    res[n] = 0;
    return res;
}