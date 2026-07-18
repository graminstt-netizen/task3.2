/*
main.c - главный модуль программы. 

Бабурин Дмитрий Сергеевич
МК-101
*/


#include "lib_main.h"

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