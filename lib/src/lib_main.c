/*
main.c - главный модуль программы. 

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
    size_t wordBits = GetWordBitCount();
    size_t startBit = (size_t)i * N; // Глобальный индекс начального бита
    
    BitsArrayMaxType result = 0;
    size_t bitsCollected = 0; // Сколько бит мы уже прочитали
    
    while (bitsCollected < N) {
        size_t currentBit = startBit + bitsCollected;
        size_t wordIdx = currentBit / wordBits;       // Индекс слова в массиве
        size_t bitOffset = currentBit % wordBits;     // Смещение внутри этого слова
        
        // Сколько бит осталось прочитать для этого элемента
        size_t bitsLeftToRead = N - bitsCollected;
        // Сколько бит доступно в текущем слове начиная со смещения bitOffset
        size_t bitsAvailableInWord = wordBits - bitOffset;
        
        // Берем минимум из того, что нужно, и того, что есть в слове
        size_t bitsToTake = (bitsLeftToRead < bitsAvailableInWord) ? bitsLeftToRead : bitsAvailableInWord;
        
        // Создаем маску для копирования нужного количества бит
        BitsArrayMaxType mask = (bitsToTake == 64) ? ~0ULL : ((1ULL << bitsToTake) - 1);
        
        // Извлекаем кусочек бит из текущего слова
        BitsArrayMaxType chunk = (bitsArray[wordIdx] >> bitOffset) & mask;
        
        // Сдвигаем кусочек на его законное место в результате и склеиваем через ИЛИ
        result |= (chunk << bitsCollected);
        
        bitsCollected += bitsToTake;
    }
    
    return result;
}

// Помещает i-ый N-битный элемент в упакованный массив
void BitsArraySet(BitsArray bitsArray, unsigned int i, BitsArrayMaxType value) {
    size_t wordBits = GetWordBitCount();
    size_t startBit = (size_t)i * N; // Глобальный индекс начального бита
    
    size_t bitsWritten = 0; // Сколько бит мы уже записали
    
    // Обрезаем входящее значение до N бит на всякий случай
    BitsArrayMaxType valueMask = (N == 64) ? ~0ULL : ((1ULL << N) - 1);
    value &= valueMask;
    
    while (bitsWritten < N) {
        size_t currentBit = startBit + bitsWritten;
        size_t wordIdx = currentBit / wordBits;       // Индекс слова в массиве
        size_t bitOffset = currentBit % wordBits;     // Смещение внутри этого слова
        
        size_t bitsLeftToWrite = N - bitsWritten;
        size_t bitsAvailableInWord = wordBits - bitOffset;
        
        // Сколько бит мы запишем за эту итерацию
        size_t bitsToOverwrite = (bitsLeftToWrite < bitsAvailableInWord) ? bitsLeftToWrite : bitsAvailableInWord;
        
        // Маска для выделения записываемой части из value
        BitsArrayMaxType mask = (bitsToOverwrite == 64) ? ~0ULL : ((1ULL << bitsToOverwrite) - 1);
        
        // Вырезаем нужный кусочек бит из value
        BitsArrayMaxType chunk = (value >> bitsWritten) & mask;
        
        // Очищаем старые биты в слове на этой позиции
        BitsArrayElementType clearMask = ~((BitsArrayElementType)(mask << bitOffset));
        bitsArray[wordIdx] &= clearMask;
        
        // Записываем новые биты
        bitsArray[wordIdx] |= ((BitsArrayElementType)(chunk << bitOffset));
        
        bitsWritten += bitsToOverwrite;
    }
}

// Вспомогательная функция перевода hex-символа в число
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

    // Каждый hex-символ — это 4 бита. Вычисляем количество N-битных элементов
    size_t totalBits = len * 4;
    *bigNumSize = (totalBits + N - 1) / N;

    BigNum res = AllocBigNum(*bigNumSize);

    // Читаем строку с конца (справа налево — от младших разрядов к старшим)
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
        // N / 4 дает нам количество hex-символов в одном элементе
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