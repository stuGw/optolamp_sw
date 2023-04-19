/*
 * memory24xx.h
 *
 *  Created on: Jun 16, 2022
 *      Author: vit
 */

#ifndef MEMORY24XX_H_
#define MEMORY24XX_H_
#include <stdint.h>

class Memory24xx {
	uint32_t countBytes { 0x200 };//512 bytes
	uint8_t icAddress { 0xa0 };
	bool twoBytesAddress { false };
	void (*i2cRead)(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress);
	void (*i2cWrite)(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress);
public:
	Memory24xx();
	virtual ~Memory24xx();
	void setMemoryParams(uint32_t inBytes, uint8_t address);
	void setInterfaceRead(void (*i2cfr)(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress));
	void setInterfaceWrite(void (*i2cfw)(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress));
	uint8_t readByte(uint32_t address);
	void readArray(uint8_t* buffer, uint32_t count, uint32_t address);
	void writeByte(uint32_t address, uint8_t byte);
	void writeArray(uint32_t address, uint8_t* buffer, uint32_t count);
};

#endif /* MEMORY24XX_H_ */
