/*
 * memory24xx.cpp
 *
 *  Created on: Jun 16, 2022
 *      Author: vit
 */

#include "memory24xx.h"
#include "logger.h"
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

void Memory24xx::setInterfaceRead(int8_t (*i2cfr)(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress))
{
	i2cRead = i2cfr;
}
void Memory24xx::setInterfaceWrite(int8_t (*i2cfw)(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress))
{
	i2cWrite = i2cfw;
}
uint8_t Memory24xx::readByte(uint32_t address)
{
	uint8_t byte { 0 };
	int8_t err = i2cRead(0,icAddress,address,1,&byte,twoBytesAddress);
	LOG->DEBG("I2c Read error = ", err);
	return byte;
}
void Memory24xx::readArray(uint8_t* buffer, uint32_t count, uint32_t address)
{
	int8_t err { 0 } ;
	if((address+count)<countBytes)err = i2cRead(0,icAddress,address,count,buffer,twoBytesAddress);

	LOG->DEBG("I2c Read error = ", err);
}
void Memory24xx::writeByte(uint32_t address, uint8_t byte)
{
	int8_t err = i2cWrite(0,icAddress,address,1,&byte, twoBytesAddress);
	LOG->DEBG("I2c write error = ", err);
}
void Memory24xx::writeArray(uint32_t address, uint8_t* buffer, uint32_t count)
{
	int8_t err { 0 };
	if((address+count)<countBytes) err = i2cWrite(0,icAddress,address,count,buffer, twoBytesAddress);
	if(err<0)LOG->DEBG("I2c write error = ", err);
}

