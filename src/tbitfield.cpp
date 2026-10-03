// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"
#include <cstring>

#define BITS_IN_ONE_MEM (sizeof(TELEM) * 8)

TBitField::TBitField(int len)
{
	if (len < 0)
		throw 1;
	BitLen = len;
	MemLen = (len + BITS_IN_ONE_MEM - 1) / BITS_IN_ONE_MEM;
	if (MemLen <= 0) MemLen = 1;
	pMem = new TELEM[MemLen];
	for (int i = 0; i < MemLen; i++)
		pMem[i] = 0;
}

TBitField::TBitField(const TBitField& bf) // конструктор копирования
{
	BitLen = bf.BitLen;
	MemLen = bf.MemLen;
	pMem = new TELEM[MemLen];
	for (int i = 0; i < MemLen; i++)
		pMem[i] = bf.pMem[i];
}

TBitField::~TBitField()
{
	delete[] pMem;
	pMem = nullptr;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
	return n / BITS_IN_ONE_MEM;
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{

	return (TELEM)1 << (n % BITS_IN_ONE_MEM);
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
	return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
	if ((n < 0) || (n >= BitLen))
		throw 2;
	pMem[GetMemIndex(n)] |= GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
	if ((n < 0) || (n >= BitLen))
		throw 2;
	pMem[GetMemIndex(n)] &= ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
	if ((n < 0) || (n >= BitLen))
		throw 2;
	return (pMem[GetMemIndex(n)] & GetMemMask(n)) ? 1 : 0;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField & bf) // присваивание
{
	if (this == &bf)
		return *this;

	// Если размер памяти отличается — перевыделяем
	if (MemLen != bf.MemLen)
	{
		delete[] pMem;
		MemLen = bf.MemLen;
		pMem = new TELEM[MemLen];
	}
	BitLen = bf.BitLen;
	for (int i = 0; i < MemLen; i++)
		pMem[i] = bf.pMem[i];
	return *this;
}

int TBitField::operator==(const TBitField & bf) const // сравнение
{
	if (BitLen != bf.BitLen)
		return 0;
	for (int i = 0; i < MemLen; i++)
		if (pMem[i] != bf.pMem[i])
			return 0;
	return 1;
}

int TBitField::operator!=(const TBitField & bf) const // сравнение
{
	return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField & bf) // операция "или"
{
	int maxLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
	TBitField res(maxLen);

	// Копируем "свой" кусок
	for (int i = 0; i < MemLen; i++)
		res.pMem[i] = pMem[i];
	// OR-им с чужим
	for (int i = 0; i < bf.MemLen; i++)
		res.pMem[i] |= bf.pMem[i];

	return res;
}

TBitField TBitField::operator&(const TBitField & bf) // операция "и"
{
	int maxLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
	TBitField res(maxLen);

	// AND по общим элементам
	int minMemLen = (MemLen < bf.MemLen) ? MemLen : bf.MemLen;
	for (int i = 0; i < minMemLen; i++)
		res.pMem[i] = pMem[i] & bf.pMem[i];
	// Остальные элементы res уже нули (инициализированы в конструкторе)

	return res;
}

TBitField TBitField::operator~(void) // отрицание
{
	TBitField res(*this);
	for (int i = 0; i < res.MemLen; i++)
		res.pMem[i] = ~res.pMem[i];
	// Обнуляем "лишние" биты в последнем элементе
	int tail = BitLen % BITS_IN_ONE_MEM;
	if (tail != 0)
	{
		TELEM mask = ((TELEM)1 << tail) - 1;
		res.pMem[res.MemLen - 1] &= mask;
	}
	return res;
}

// ввод/вывод

istream& operator>>(istream & istr, TBitField & bf) // ввод
{
	char ch;
	for (int i = 0; i < bf.BitLen; i++)
	{
		istr >> ch;
		if (ch == '1')      bf.SetBit(i);
		else if (ch == '0') bf.ClrBit(i);
		else { istr.setstate(ios::failbit); return istr; }
	}
	return istr;
}

ostream& operator<<(ostream & ostr, const TBitField & bf) // вывод
{
	for (int i = 0; i < bf.BitLen; i++)
		ostr << (bf.GetBit(i) ? '1' : '0');
	return ostr;
}
