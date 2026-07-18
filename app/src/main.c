/*
main.c - главный модуль программы. 

Бабурин Дмитрий Сергеевич
МК-101
*/

#include "lib_main.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Вывод логов строго на английском
    printf("BigNum Calculator started\n");

    if (argc < 3) {
        printf("Usage: %s <hex_string_1> <hex_string_2>\n", argv[0]);
        return 1;
    }

    size_t size1, size2;
    BigNum num1 = GetBigNumByStr(argv[1], &size1);
    BigNum num2 = GetBigNumByStr(argv[2], &size2);

    // Размер результата при сложении может быть на 1 элемент больше максимального
    size_t resSize = (size1 > size2 ? size1 : size2) + 1;
    BigNum res = AllocBigNum(resSize);

    // Выполняем сложение
    AddBigNum(num1, num2, res, size1, size2);

    printf("First number:  ");
    PrintBigNum(num1, size1);

    printf("Second number: ");
    PrintBigNum(num2, size2);

    // Если самый старший разряд остался нулевым (переноса не было),
    // уменьшаем размер для красивого вывода без ведущего нуля
    if (resSize > 1 && BitsArrayGet(res, (unsigned int)(resSize - 1)) == 0) {
        resSize--;
    }

    printf("Sum:           ");
    PrintBigNum(res, resSize);

    // Очищаем память
    free(num1);
    free(num2);
    free(res);

    return 0;
}