/*
lib_main.c - главный модуль программы. 

Бабурин Дмитрий Сергеевич
МК-101
*/


#include "lib_main.h"
#include <string.h>
#include <stdio.h>

// Функция возвращает размер одного слова BitsArrayElementType в битах (обычно 8, 16, 32 или 64)
size_t GetWordBitCount(void) {
    return sizeof(BitsArrayElementType) * 8;
}

// Выделяет память под большое число и заполняет её нулями
BigNum AllocBigNum(size_t bigNumSize) {
    size_t wordBits = GetWordBitCount();
    size_t totalBits = bigNumSize * N;
    
    // Округляем количество необходимых слов в большую сторону
    size_t wordsNeeded = (totalBits + wordBits - 1) / wordBits;
    
    // Выделяем память через calloc, чтобы все биты изначально были нулевыми
    return (BigNum)calloc(wordsNeeded, sizeof(BitsArrayElementType));
}

// Возвращает i-ый N-битный элемент из упакованного массива
BitsArrayMaxType BitsArrayGet(BitsArray bitsArray, unsigned int i) {
    // Проверка безопасности на случай передачи пустого указателя
    if (bitsArray == NULL) {
        return 0;
    }

    size_t wordBits = GetWordBitCount();
    size_t startBit = (size_t)i * N;
    
    BitsArrayMaxType result = 0;
    size_t bitsCollected = 0;
    
    while (bitsCollected < N) {
        size_t currentBit = startBit + bitsCollected;
        size_t wordIdx = currentBit / wordBits;
        size_t bitOffset = currentBit % wordBits;
        
        size_t bitsLeftToRead = N - bitsCollected;
        size_t bitsAvailableInWord = wordBits - bitOffset;
        
        size_t bitsToTake = (bitsLeftToRead < bitsAvailableInWord) ? bitsLeftToRead : bitsAvailableInWord;
        
        BitsArrayMaxType mask = (bitsToTake == 64) ? ~0ULL : ((1ULL << bitsToTake) - 1);
        BitsArrayMaxType chunk = (bitsArray[wordIdx] >> bitOffset) & mask;
        
        result |= (chunk << bitsCollected);
        bitsCollected += bitsToTake;
    }
    
    return result;
}
// Помещает i-ый N-битный элемент в упакованный массив
void BitsArraySet(BitsArray bitsArray, unsigned int i, BitsArrayMaxType value) {
    // Проверка безопасности на случай передачи пустого указателя
    if (bitsArray == NULL) {
        return;
    }

    size_t wordBits = GetWordBitCount();
    size_t startBit = (size_t)i * N;
    
    size_t bitsWritten = 0;
    
    BitsArrayMaxType valueMask = (N == 64) ? ~0ULL : ((1ULL << N) - 1);
    value &= valueMask;
    
    while (bitsWritten < N) {
        size_t currentBit = startBit + bitsWritten;
        size_t wordIdx = currentBit / wordBits;
        size_t bitOffset = currentBit % wordBits;
        
        size_t bitsLeftToWrite = N - bitsWritten;
        size_t bitsAvailableInWord = wordBits - bitOffset;
        
        size_t bitsToOverwrite = (bitsLeftToWrite < bitsAvailableInWord) ? bitsLeftToWrite : bitsAvailableInWord;
        
        BitsArrayMaxType mask = (bitsToOverwrite == 64) ? ~0ULL : ((1ULL << bitsToOverwrite) - 1);
        BitsArrayMaxType chunk = (value >> bitsWritten) & mask;
        
        BitsArrayElementType clearMask = ~((BitsArrayElementType)(mask << bitOffset));
        bitsArray[wordIdx] &= clearMask;
        
        bitsArray[wordIdx] |= ((BitsArrayElementType)(chunk << bitOffset));
        bitsWritten += bitsToOverwrite;
    }
}

// Вспомогательная функция перевода хекс символа в число
static unsigned char HexCharToVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

// Инициализирует большое число из строки
BigNum GetBigNumByStr(IN const char *str, OUT size_t *bigNumSize) {
    if (str == NULL || bigNumSize == NULL) {
        return NULL;
    }

    size_t len = strlen(str);
    if (len == 0) {
        *bigNumSize = 1;
        return AllocBigNum(1);
    }

    // Каждый хекс символ это 4 бита. Вычисляем количество N-битных элементов
    size_t totalBits = len * 4;
    *bigNumSize = (totalBits + N - 1) / N;

    BigNum res = AllocBigNum(*bigNumSize);

    // Читаем строку с конца (справа налево: от младших разрядов к старшим)
    for (size_t i = 0; i < len; i++) {
        char c = str[len - 1 - i];
        unsigned char val = HexCharToVal(c);

        size_t bitPos = i * 4; // Смещение в битах от начала числа
        size_t elemIdx = bitPos / N;
        size_t shift = bitPos % N;

        // Читаем то, что уже успели записать в этот элемент, и подмешиваем новые 4 бита
        BitsArrayMaxType currentElem = BitsArrayGet(res, (unsigned int)elemIdx);
        currentElem |= ((BitsArrayMaxType)val << shift);
        
        BitsArraySet(res, (unsigned int)elemIdx, currentElem);
    }

    return res;
}

// Выводит число на экран
void PrintBigNum(IN BigNum bigNum, size_t bigNumSize) {
    if (bigNum == NULL || bigNumSize == 0) {
        printf("0\n");
        return;
    }

    // Печатаем элементы со старшего (последнего в массиве) к младшему (нулевому)
    size_t printedAnything = 0;

    for (size_t i = bigNumSize; i > 0; i--) {
        size_t idx = i - 1;
        BitsArrayMaxType val = BitsArrayGet(bigNum, (unsigned int)idx);

        // В зависимости от N выбираем ширину вывода (количество символов на один элемент)
        // N / 4 дает нам количество хекс символов в одном элементе
        unsigned int hexCharsInElem = N / 4;

        if (idx == bigNumSize - 1) {
            // Для самого первого (старшего) элемента убираем лишние ведущие нули при выводе
            printf("%llX", val);
            printedAnything = 1;
        } else {
            // Для остальных элементов обязательно сохраняем ведущие нули внутри числа
            switch (hexCharsInElem) {
                case 1:  printf("%01llX", val); break;
                case 2:  printf("%02llX", val); break;
                case 4:  printf("%04llX", val); break;
                case 8:  printf("%08llX", val); break;
                case 16: printf("%016llX", val); break;
                default: printf("%llX", val); break;
            }
        }
    }
    printf("\n");
}

void AddBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size) {
    if (bigNum1 == NULL || bigNum2 == NULL || res == NULL) {
        return;
    }

    size_t maxSize = (bigNum1Size > bigNum2Size) ? bigNum1Size : bigNum2Size;
    BitsArrayMaxType carry = 0; // Перенос в следующий разряд

    for (size_t i = 0; i < maxSize; i++) {
        // Если одно из чисел короче другого, недостающие разряды считаем нулями
        BitsArrayMaxType val1 = (i < bigNum1Size) ? BitsArrayGet(bigNum1, (unsigned int)i) : 0;
        BitsArrayMaxType val2 = (i < bigNum2Size) ? BitsArrayGet(bigNum2, (unsigned int)i) : 0;
        
        BitsArrayMaxType sum = 0;
        
        if (N == 64) {
            // Особый случай для N=64, чтобы избежать переполнения unsigned long long в C
            sum = val1 + val2;
            BitsArrayMaxType next_carry = 0;
            
            if (sum < val1) {
                next_carry = 1; // Произошло переполнение при сложении val1 и val2
            }
            
            sum += carry;
            if (sum < carry) {
                next_carry = 1; // Произошло переполнение при добавлении предыдущего переноса
            }
            
            carry = next_carry;
        } else {
            // Для N < 64 всё проще: мы можем безопасно складывать в более широком типе
            BitsArrayMaxType mask = ((BitsArrayMaxType)1 << N) - 1;
            sum = val1 + val2 + carry;
            carry = sum >> N; // Всё, что вышло за пределы N бит, уходит в перенос
            sum &= mask;      // Обрезаем результат до N бит
        }
        
        BitsArraySet(res, (unsigned int)i, sum);
    }
    
    // Если после сложения всех разрядов остался перенос, записываем его в самый старший разряд
    if (carry > 0) {
        BitsArraySet(res, (unsigned int)maxSize, carry);
    }
}

void SubBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size) {
    if (bigNum1 == NULL || bigNum2 == NULL || res == NULL) {
        return;
    }

    BitsArrayMaxType borrow = 0; // Заем из старшего разряда
    size_t maxSize = bigNum1Size; // Размер результата равен размеру уменьшаемого

    for (size_t i = 0; i < maxSize; i++) {
        BitsArrayMaxType val1 = BitsArrayGet(bigNum1, (unsigned int)i);
        BitsArrayMaxType val2 = (i < bigNum2Size) ? BitsArrayGet(bigNum2, (unsigned int)i) : 0;
        
        BitsArrayMaxType diff = 0;
        BitsArrayMaxType next_borrow = 0;

        if (N == 64) {
            // Для N = 64 используем особенности беззнакового переполнения в C
            BitsArrayMaxType term2 = val2 + borrow;
            
            // Заем происходит, если произошло переполнение при сложении val2 + borrow
            // или если уменьшаемое меньше того, что мы вычитаем
            if (term2 < val2 || val1 < term2) {
                next_borrow = 1;
            }
            diff = val1 - term2;
            borrow = next_borrow;
        } else {
            // Для N < 64 вычисляем заем явно
            BitsArrayMaxType mask = ((BitsArrayMaxType)1 << N) - 1;
            BitsArrayMaxType term2 = val2 + borrow;
            
            if (val1 < term2) {
                next_borrow = 1;
                // Прибавляем основание системы счисления (2^N) к уменьшаемому
                diff = (val1 + ((BitsArrayMaxType)1 << N)) - term2;
            } else {
                next_borrow = 0;
                diff = val1 - term2;
            }
            diff &= mask; // Обрезаем до N бит
            borrow = next_borrow;
        }

        BitsArraySet(res, (unsigned int)i, diff);
    }
}

int IsValidHexStr(const char *str) {
    if (str == NULL || strlen(str) == 0) {
        return 0;
    }
    for (size_t i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (!((c >= '0' && c <= '9') || 
              (c >= 'A' && c <= 'F') || 
              (c >= 'a' && c <= 'f'))) {
            return 0;
        }
    }
    return 1;
}

int CompareBigNum(IN BigNum num1, IN BigNum num2, size_t size1, size_t size2) {
    if (num1 == NULL || num2 == NULL) {
        return 0;
    }

    // Определяем эффективную длину первого числа (без ведущих нулей)
    size_t eff1 = size1;
    while (eff1 > 1 && BitsArrayGet(num1, (unsigned int)(eff1 - 1)) == 0) {
        eff1--;
    }

    // Определяем эффективную длину второго числа
    size_t eff2 = size2;
    while (eff2 > 1 && BitsArrayGet(num2, (unsigned int)(eff2 - 1)) == 0) {
        eff2--;
    }

    // Сравниваем по длине
    if (eff1 > eff2) return 1;
    if (eff1 < eff2) return -1;

    // Если эффективные длины равны, сравниваем элементы поразрядно от старших к младшим
    for (size_t i = eff1; i > 0; i--) {
        size_t idx = i - 1;
        BitsArrayMaxType val1 = BitsArrayGet(num1, (unsigned int)idx);
        BitsArrayMaxType val2 = BitsArrayGet(num2, (unsigned int)idx);
        
        if (val1 > val2) return 1;
        if (val1 < val2) return -1;
    }

    return 0; // Числа полностью равны
}