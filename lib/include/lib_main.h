/*
Заголовочный файл библиотеки битовых массивов и длинной арифметики

Бабурин Дмитрий Сергеевич
МК-101
*/

#ifndef LIB_MAIN_H
#define LIB_MAIN_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

// ===================================================================
// НАСТРОЙКА БИТОВОГО МАССИВА ПРИ КОМПИЛЯЦИИ
// По заданию поддерживается любой беззнаковый целочисленный тип.
// Вы можете раскомментировать любой из вариантов ниже для тестирования:
// ===================================================================

typedef unsigned int BitsArrayElementType;
// typedef unsigned char      BitsArrayElementType;
// typedef unsigned short     BitsArrayElementType;
// typedef unsigned long      BitsArrayElementType;
// typedef unsigned long long BitsArrayElementType;
// typedef uint8_t            BitsArrayElementType;
// typedef uint16_t           BitsArrayElementType;
// typedef uint32_t           BitsArrayElementType;
// typedef uint64_t           BitsArrayElementType;
// typedef size_t             BitsArrayElementType;
// typedef uintptr_t          BitsArrayElementType;

// ===================================================================

// Битовый массив представляется массивом слов
typedef BitsArrayElementType *BitsArray;

// Тип, в который поместится любой N-битный кусочек (до 64 бит включительно)
typedef unsigned long long BitsArrayMaxType;

// Большое число — это по сути тот же битовый массив
typedef BitsArray BigNum;

// Размер элементов большого числа (поддерживаются 4, 8, 16, 32, 64)
#define N   32

#define IN
#define OUT

// --- Прототипы функций работы с битовым массивом ---

// Вычисляет размер одного слова (ячейки) в битах
size_t GetWordBitCount(void);

// Выделяет память для хранения большого числа, состоящего из bigNumSize элементов по N бит
BigNum AllocBigNum(size_t bigNumSize);

// Возвращает i-ый N-битный элемент из битового массива
BitsArrayMaxType BitsArrayGet(BitsArray bitsArray, unsigned int i);

// Помещает i-ый N-битный элемент в битовый массив
void BitsArraySet(BitsArray bitsArray, unsigned int i, BitsArrayMaxType value);

// Инициализирует большое число из строки шестнадцатеричных символов
BigNum GetBigNumByStr(IN const char *str, OUT size_t *bigNumSize);

// Выводит большое число в консоль в шестнадцатеричном формате
void PrintBigNum(IN BigNum bigNum, size_t bigNumSize);

// Сложение двух больших чисел
void AddBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size);

// Вычитание двух больших чисел
void SubBigNum(IN BigNum bigNum1, IN BigNum bigNum2, OUT BigNum res, size_t bigNum1Size, size_t bigNum2Size);

// Сравнивает два больших числа
int CompareBigNum(IN BigNum num1, IN BigNum num2, size_t size1, size_t size2);

// Проверяет, является ли строка корректным шестнадцатеричным числом
int IsValidHexStr(const char *str);

#endif // LIB_MAIN_H