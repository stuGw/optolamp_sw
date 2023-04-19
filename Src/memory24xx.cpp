/*
 * memory24xx.cpp
 *
 *  Created on: Jun 16, 2022
 *      Author: vit
 */

#include "memory24xx.h"

Memory24xx::Memory24xx() {
	// TODO Auto-generated constructor stub

}

Memory24xx::~Memory24xx() {
	// TODO Auto-generated destructor stub
}

void Memory24xx::setMemoryParams(uint32_t inBytes, uint8_t address)
{
	countBytes = inBytes;
	icAddress = address;
	if(inBytes>=4096) twoBytesAddress = true; else twoBytesAddress = false;//memory since 32Kbit has two byte address
}

void Memory24xx::setInterfaceRead(void (*i2cfr)(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress))
{
	i2cRead = i2cfr;
}
void Memory24xx::setInterfaceWrite(void (*i2cfw)(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress))
{
	i2cWrite = i2cfw;
}
uint8_t Memory24xx::readByte(uint32_t address)
{
	uint8_t byte { 0 };
	i2cRead(0,icAddress,address,1,&byte,twoBytesAddress);
	return byte;
}
void Memory24xx::readArray(uint8_t* buffer, uint32_t count, uint32_t address)
{
	if((address+count)<countBytes) i2cRead(0,icAddress,address,count,buffer,twoBytesAddress);
}
void Memory24xx::writeByte(uint32_t address, uint8_t byte)
{
	i2cWrite(0,icAddress,address,1,&byte, twoBytesAddress);
}
void Memory24xx::writeArray(uint32_t address, uint8_t* buffer, uint32_t count)
{
	if((address+count)<countBytes) i2cWrite(0,icAddress,address,count,buffer, twoBytesAddress);
}

