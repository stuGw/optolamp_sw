/*
 * crchw.cpp
 *
 *  Created on: Jun 1, 2022
 *      Author: vit
 */

#include <crchw.h>

CRCHw::CRCHw() {
	// TODO Auto-generated constructor stub

}

CRCHw::~CRCHw() {
	// TODO Auto-generated destructor stub
}

void CRCHw::init(uint32_t initial, PolySizeMask psize, uint32_t polynom, bool reverseOut, ReverseInMask reverseIn)
{
	clockOn();
	initialValue = initial;
	polynomValue = polynom;

	outReverseMask = reverseOut;

	polSize = psize;
	rinMask = reverseIn;
}

void CRCHw::initCRCHw()
{

	CRC->INIT = initialValue;
	CRC->CR &= ~POLYSIZE_7;
	CRC->CR |= static_cast<uint32_t>(polSize);

	//CRC->CR |= 0x00000010;//8bit polysiz
	CRC->CR &= ~BY_WORD;
	CRC->CR |= static_cast<uint32_t>(rinMask);

	//CRC->CR |= 0x00000060;//reverse world

	if(outReverseMask) CRC->CR |= REVERSE_OUT_MASK; else CRC->CR &= ~REVERSE_OUT_MASK;

	//CRC->CR |= 0x00000080;//reverse out
	CRC->POL = polynomValue;

	CRC->CR |= 0x00000001;
}

uint8_t CRCHw::calcCRC(uint8_t *Buffer, uint32_t BufferSize)
{

	register uint32_t data = 0;
	register uint32_t index = 0;
	CRC->INIT = 0x00000000;
	CRC->CR |= 0x00000010;//8bit polysize
	CRC->CR |= 0x00000060;//reverse world
	CRC->CR |= 0x00000080;//reverse out
	CRC->POL = 0x31;
	CRC->CR |= 0x00000001;
	/* Compute the CRC of Data Buffer array*/

	for(index=0; index < (BufferSize / 4); index++)
	{
		data = (uint32_t)((Buffer[4 * index + 3] << 24) | (Buffer[4 * index + 2] << 16) | (Buffer[4 * index + 1] << 8) | Buffer[4 * index]);
		CRC->DR = data;
	}

	/* Last bytes specific handling */
	if ((BufferSize % 4) != 0)
	{
		if (BufferSize % 4 == 1)
		{
			*(uint8_t __IO *)(&CRC->DR)= Buffer[4*index];
		}

		if (BufferSize % 4 == 2)
		{
			*(uint16_t __IO *)(&CRC->DR) = (uint16_t)((Buffer[4 * index + 1]<<8) | Buffer[4 * index]);
		}

		if (BufferSize % 4 == 3)
		{
			*(uint16_t __IO *)(&CRC->DR) = (uint16_t)((Buffer[4 * index + 1]<<8) | Buffer[4 * index]);
			*(uint8_t __IO *)(&CRC->DR) = Buffer[4 * index + 2];
		}
	}

	/* Return computed CRC value */


	return CRC->DR;


}

uint8_t CRCHw::getCRC(uint8_t* buffer, uint32_t size)
{
	initCRCHw();
	return calcCRC(buffer,size);
}
