/*
 * init.h
 *
 *  Created on: Jun 23, 2022
 *      Author: vit
 */

#ifndef INIT_H_
#define INIT_H_

#define GPIOPIN_0  0x00000001
#define GPIOPIN_1  0x00000002
#define GPIOPIN_2  0x00000004
#define GPIOPIN_3  0x00000008
#define GPIOPIN_4  0x00000010
#define GPIOPIN_5  0x00000020
#define GPIOPIN_6  0x00000040
#define GPIOPIN_7  0x00000080
#define GPIOPIN_8  0x00000100
#define GPIOPIN_9  0x00000200
#define GPIOPIN_10 0x00000400
#define GPIOPIN_11 0x00000800
#define GPIOPIN_12 0x00001000
#define GPIOPIN_13 0x00002000
#define GPIOPIN_14 0x00004000
#define GPIOPIN_15 0x00008000

void initI2C1GPIO();
void offI2C1();
void initI2C1();
void writeI2C1(uint8_t addrMode, uint16_t deviceAddress, uint16_t memAddress, uint8_t count, uint8_t* data, bool twoByteAddress);
void readI2C1(uint8_t addrMode, uint16_t deviceAddress, uint16_t addrFrom, uint8_t count, uint8_t* data, bool twoByteAddress);
void initLedPin();
void initBeeperPin();
void initHD44780GPIOMode8();
void initSysTick();
void initSPI2Pins();
void initSPI2();
uint16_t spi2GetData();
void initEXTIPins();
void initializeHw();

void i2cFindDevices();

void initWSTimer();

void startTimer3();
void stopTimer3();

void initializeDMA(uint32_t* address, uint32_t count);
void startWSDMA(uint32_t count);
void stopWSDMA(void);
//Постоянная передача на все диоды
void startWSDMACircle();
void startWsTransfer(uint32_t count);

void wsDmaTmCirc(void);
//Режим одиночной передачи для ws
void wsDmaTmOnce(void);
#endif /* INIT_H_ */
