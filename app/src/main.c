/*
main.c - главный модуль программы. 

Бабурин Дмитрий Сергеевич
МК-101
*/

#include "lib_main.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Вывод логов строго на английском, как требует задание
    printf("BigNum Calculator started\n");

    if (argc < 2) {
        printf("Usage: %s <hex_string>\n", argv[0]);
        return 1;
    }

    size_t size;
    BigNum num = GetBigNumByStr(argv[1], &size);

    printf("Parsed BigNum: ");
    PrintBigNum(num, size);

    // Не забываем освобождать выделенную память
    free(num);

    return 0;
}