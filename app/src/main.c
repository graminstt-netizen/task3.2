/*
Точка входа в программу (интерфейс пользователя)

Бабурин Дмитрий Сергеевич
МК-101
*/

#include "lib_main.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Логи строго на английском
    printf("BigNum Calculator started\n");

    if (argc < 3) {
        printf("Usage: %s <hex_string_1> <hex_string_2>\n", argv[0]);
        return 1;
    }

    // Валидация входных строк на корректность hex-символов
    if (!IsValidHexStr(argv[1]) || !IsValidHexStr(argv[2])) {
        fprintf(stderr, "Error: Input strings contain invalid hexadecimal characters.\n");
        return 1;
    }

    size_t size1, size2;
    BigNum num1 = GetBigNumByStr(argv[1], &size1);
    BigNum num2 = GetBigNumByStr(argv[2], &size2);

    printf("First number:  ");
    PrintBigNum(num1, size1);

    printf("Second number: ");
    PrintBigNum(num2, size2);

    // --- 1. Сложение ---
    size_t addResSize = (size1 > size2 ? size1 : size2) + 1;
    BigNum addRes = AllocBigNum(addResSize);
    AddBigNum(num1, num2, addRes, size1, size2);

    if (addResSize > 1 && BitsArrayGet(addRes, (unsigned int)(addResSize - 1)) == 0) {
        addResSize--;
    }
    printf("Sum:           ");
    PrintBigNum(addRes, addResSize);

    // --- 2. Вычитание ---
    // Проверяем, не будет ли результат отрицательным
    if (CompareBigNum(num1, num2, size1, size2) < 0) {
        printf("Difference:    Error: Result would be negative (first number is smaller than second)\n");
    } else {
        size_t subResSize = size1;
        BigNum subRes = AllocBigNum(subResSize);
        SubBigNum(num1, num2, subRes, size1, size2);

        // Убираем ведущие нули в разности для красивого вывода
        while (subResSize > 1 && BitsArrayGet(subRes, (unsigned int)(subResSize - 1)) == 0) {
            subResSize--;
        }
        printf("Difference:    ");
        PrintBigNum(subRes, subResSize);
        
        // Освобождаем subRes только здесь, так как он создавался внутри ветки else
        free(subRes);
    }

    // Очищаем оставшуюся глобальную выделенную память
    free(num1);
    free(num2);
    free(addRes);

    return 0;
}