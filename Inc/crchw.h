/*
 * crchw.h
 *
 *  Created on: Jun 1, 2022
 *      Author: vit
 */

#ifndef CRCHW_H_
#define CRCHW_H_
#include "STM32G030.h"


class CRCHw {
public:
		//static constexpr uint32_t POLYSIZE_7 { 0x00000010 };
		//static constexpr uint32_t POLYSIZE_8 { 0x00000010 };
		//static constexpr uint32_t POLYSIZE_16 { 0x00000010 };
		static constexpr uint32_t REVERSE_OUT_MASK { 0x00000080 };

		enum   PolySizeMask  { POLYSIZE_7 = 0x00000018, POLYSIZE_8 = 0x00000010, POLYSIZE_16 = 0x00000008, POLYSIZE_32 = 0x00000000 } polSize;
		enum ReverseInMask  { NO = 0x00000000, BY_BYTE = 0x00000020, BY_HWORD = 0x00000040, BY_WORD = 0x00000060 } rinMask;

		uint32_t initialValue { 0xffffffff };
		uint32_t polynomValue { 0x04c11db7 };

		bool outReverseMask { false };
		void clockOn(){ RCC->AHBENR |= 0x00001000; }
		void clockOff() { RCC->AHBENR &= ~0x00001000; }

		void init(uint32_t initial, PolySizeMask psize, uint32_t polynom, bool reverseOut, ReverseInMask reverseIn);

		void initCRCHw();


		uint8_t calcCRC(uint8_t *Buffer, uint32_t BufferSize);


		uint8_t getCRC(uint8_t* buffer, uint32_t size);


	CRCHw();
	virtual ~CRCHw();
};

#endif /* CRCHW_H_ */
