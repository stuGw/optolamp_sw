/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "STM32G030.h"
#include "button.h"
#include "adc.h"
#include "led.h"
#include "serial.h"
#include "rtctime.h"
#include "logger.h"
#include "init.h"
#include "ws2812.h"
#include "SmartPixelStrip.h"
#include "ledeffects.h"
#include "ledpairs.h"
#include "resistanceconverter.h"
#include "lightsensor5528.h"
#include "memory24xx.h"
#include "crchw.h"
#include "version.h"



#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif




Button bLeft;
Button bRight;
uint32_t* hwLA;
AnalogConverter sensorADC;

Serial serial;//serial, using in logger &
Led *led;//just led
RTCTime time;//using to configure and read realtime

CRCHw crc;
uint8_t flgWSTransfer = 0;

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


volatile bool flagButt { false };
volatile bool flagMinute { false };
volatile bool flagSecund { false };



void RTCIrq_Handler()
{
	if((RTC->MISR & 0x00000001) == 0x00000001){ RTC->SCR |= 0x00000001; flagMinute = 1; }
	if((RTC->MISR & 0x00000002) == 0x00000002){ RTC->SCR |= 0x00000002; flagSecund = 1; }
	led->on();
}

void DMA_2_3_Handler()
{
	stopWSDMA();
	//stopTimer3();
		 DMA->IFCR|=0x00000010;//DMA_IFCR_CGIF6;
}

	//Таймер для реализации протокола ws2812
	//После передачи всех данных - остановить таймер
void TIM3_Handler(void)
{
	static int counter = 0;
	if(!(DMA->CCR2&0x0001))stopTimer3();
	if( DMA->IFCR &0x00000010)stopTimer3();//DMA_IFCR_CGIF6;)
	TIM3->SR &= ~0x0001; //Clean UIF Flag
}


void SysTick_Handler()
{

	static uint32_t milliseconds = 0;

	milliseconds++;
	bLeft.timeIncrease();
	bRight.timeIncrease();
	if((milliseconds%20) == 0)
	{
		flagButt = 1;
	}
}

void EXTI0_Handler()
{
	if(EXTI->RPR1 & 0x0002){ EXTI->RPR1 |= 0x0002; bLeft.rise();  /*LOG->DEBG("BEFFECT rise",bLeft.allCounter);*/ }
	if(EXTI->RPR1 & 0x0001){ EXTI->RPR1 |= 0x0001; bRight.rise();/* LOG->DEBG("BBRIGHT rise",bRight.allCounter);*/ }
	if(EXTI->FPR1 & 0x0002){ EXTI->FPR1 |= 0x0002; bLeft.fall();  /*LOG->DEBG("BEFFECT fall", bLeft.allCounter);*/ }
	if(EXTI->FPR1 & 0x0001){ EXTI->FPR1 |= 0x0001; bRight.fall();  /*LOG->DEBG("BBRIGHT fall", bRight.allCounter);*/ }
}


void initializeModules()
{
	led = new Led(&(GPIOC->ODR),GPIOPIN_13, false);
		led->on();

		serial.begin();
		//serial.DEBG("Starting hardware initialization...");
		//serial.DEBG("RTC initialization...");

		RTCTime::TimeNDate tmdt;
		tmdt.hour = 0;
		tmdt.min = 0;
		tmdt.sec = 0;
		time.RTCInit();
		time.setTime(tmdt);

		serial.DEBG("Logger initialization...");
		LOG->setSerial(&serial);
		LOG->setTimeSource(&time);

		LOG->DEBG("Logger is on!");

		LOG->DEBG("Initialize adc...");
		sensorADC.init();
		//PA7 - ADC7 - current, PB0 - ADC_8 - 60v voltage, PA6 - ADC6 - 12v voltage
		sensorADC.initChannels(AnalogConverter::ADC_7);

		sensorADC.getDataFiltered(AnalogConverter::ADC_7);
		LOG->DEBG("Initialize button interface...");
		bLeft.init(&(GPIOA->IDR), GPIOPIN_1);
		bRight.init(&(GPIOA->IDR), GPIOPIN_0);
}


#pragma pack(1)
struct LampParams
{
	uint16_t color { 0 };//2
	uint8_t bright{ 0 };//3
	uint8_t autoBright { 0 };//4
	uint8_t effectNo { 0 };//5
	uint8_t effectMode { 0 };//6
	uint8_t dummy2 { 0 };//7
	uint8_t crc { 0x00 };//8
};

struct CONFIG
{
	LampParams parameters;
	bool brightChanged{ 0 };
	bool colorChanged { 0 };
	bool effectChanged { 0 };
	bool autoBrightSetted { 0 };

};

#pragma pack()


bool getConfiguration(CONFIG* conf, Button *buttonR, Button *buttonL)
{
	bool changed = false;
	Button::State bRState = buttonR->getState();
	Button::State bLState = buttonL->getState();
	if(bRState == Button::DOUBLE)
	{
		conf->parameters.autoBright = !conf->parameters.autoBright;
		if(conf->parameters.autoBright)
		{

			conf->autoBrightSetted = true;

			LOG->DEBG("Autobright on!");
		}
		else LOG->DEBG("Autobright off");
		changed = true;
	}

	if(bLState == Button::DOUBLE)
	{
		conf->parameters.effectMode = !conf->parameters.effectMode;
		if(conf->parameters.effectMode) LOG->DEBG("Effect mode on!"); else LOG->DEBG("Effect mode off");
		changed = true;
	}

	if(bRState == Button::SINGLE)
	{
		if(!conf->parameters.autoBright)
		{
			if((conf->parameters.effectNo == LedEffects::FIRE_ALL) || (conf->parameters.effectNo == LedEffects::FIRE_EACH))
			{
				conf->parameters.bright+=1;
				if(conf->parameters.bright > 5) conf->parameters.bright = 1;
			}
			else
			{
				conf->parameters.bright+=2;

				if(conf->parameters.bright > 21) conf->parameters.bright = 1;
			}
			conf->brightChanged = true;
			changed = true;
		}
	}

	if(bLState == Button::SINGLE)
	{
		if(conf->parameters.effectMode)
		{
			conf->parameters.effectNo++;
			if(conf->parameters.effectNo>LedEffects::countEffects)conf->parameters.effectNo = 1;
			conf->effectChanged = true;
		}
		else
		{

			conf->parameters.color+=10;
			if(conf->parameters.color>360)
			{

				conf->parameters.color = 0;
			}

			conf->colorChanged = true;
		}
		changed = true;
	}
	return changed || conf->brightChanged;
}

void configureLamp(CONFIG* lampConfiguration, LedEffects* leds)
{
	if(lampConfiguration->brightChanged)
		{
			if(lampConfiguration->parameters.effectMode)leds->setEffectBright(lampConfiguration->parameters.bright); else { leds->getLedsStrip()->setValue(lampConfiguration->parameters.bright*5); };
			lampConfiguration->brightChanged = false;
			LOG->DEBG("Bright changed! - ", lampConfiguration->parameters.bright);

		}


		//color/effect
		if(lampConfiguration->colorChanged)
		{
			leds->getLedsStrip()->setHUE(lampConfiguration->parameters.color);
			//leds->getLedsStrip()->setColor(lampConfiguration->parameters.color);
			lampConfiguration->colorChanged = false;
			LOG->DEBG("Color changed! - ", lampConfiguration->parameters.color);

		}

		if(lampConfiguration->effectChanged)
		{
			leds->setEffect((LedEffects::Effects)lampConfiguration->parameters.effectNo);
			lampConfiguration->effectChanged = false;
			LOG->DEBG("Effect changed! - ", lampConfiguration->parameters.effectNo);

		}



		if(!lampConfiguration->parameters.effectMode)
		{
			leds->getLedsStrip()->refresh();
		}
}

void autoBrightLamp(CONFIG* lampConfiguration, LedEffects* leds, LightSensor5528* sensor)
{
	if(lampConfiguration->parameters.autoBright)
	{
		int32_t autoBrightValue = sensor->getValue();
		if(lampConfiguration->autoBrightSetted)
		{
			lampConfiguration->autoBrightSetted = false;
			leds->getLedsStrip()->setBright(sensor->getValue(false));
			LOG->DEBG("Bright value = ", sensor->getValue(false));
			leds->getLedsStrip()->refresh();
		}
		else if(autoBrightValue!= LightSensor5528::VALUE_NOT_CHANGED)
		{
			LOG->DEBG("AutoBright value = ", autoBrightValue);
			if(lampConfiguration->parameters.effectMode)leds->setEffectBright(autoBrightValue); else { leds->getLedsStrip()->setBright(autoBrightValue); leds->getLedsStrip()->refresh(); };
		}
	}
}
//0-15
//1-20
//2-12
//3-17
//4-19
//5-21
//6-25
//7-29
//8-28
//9-23
//10-18
//11-27
////////12-2
//13-30
//14-24
////////15-0
//16-26
////////17-3
//22-31
inline void configureLedPairs(LedPairs* pairs)
{
	pairs->setPair(0, 0, 15);
			pairs->setPair(1, 1, 20);
			pairs->setPair(2, 2, 12);
			pairs->setPair(3, 3, 17);

			pairs->setPair(4, 4, 19);
			pairs->setPair(5, 5, 21);
			pairs->setPair(6, 6, 25);
			pairs->setPair(7, 7, 29);

			pairs->setPair(8, 8, 28);
			pairs->setPair(9, 9, 23);
			pairs->setPair(10, 10, 18);
			pairs->setPair(11, 11, 27);

			pairs->setPair(12, 13, 30);
			pairs->setPair(13, 14, 24);
			pairs->setPair(14, 16, 26);
			pairs->setPair(15, 22, 31);


			//pairs->getPair(0)->setBright(21);
			//pairs->getPair(0)->setColor(255);

			//pairs->getPair(1)->setBright(21);
			//pairs->getPair(1)->setColor(2);

			//pairs->refresh();

			for(int i = 0; i<pairs->getCount(); i++)
			{
				pairs->getPair(i)->setHSV(0, 100, 100);
			}




			pairs->refresh();
}


uint32_t paramsEEPROMAddress { 0x00 };
uint16_t sizeOfEEPROM { 1024 };
uint8_t addressOfEEPROM { 0xA0 };




int main(void)
{
	Memory24xx mem;
	LightSensor5528 lightSensor(1000000, &sensorADC, AnalogConverter::ADC_7);
	CONFIG lampConfiguration;
	ResistanceConverter RConverter;
	uint32_t* address;

	initializeHw();
	initializeModules();

	SmartPixelStrip* ledstrip = new SmartPixelStrip(32,SmartPixelStrip::WS2812, SmartPixelStrip::ONCE_MODE, startWsTransfer, true);
	hwLA = ledstrip->getHwAdress();
	LedPairs pairs(ledstrip);
	initializeDMA(address, ledstrip->hwSize());
	initWSTimer();

	LOG->DEBG("Initialize memory iface, size = ",sizeOfEEPROM);
	mem.setInterfaceRead(readI2C1);
	mem.setInterfaceWrite(writeI2C1);


	mem.setMemoryParams(sizeOfEEPROM, addressOfEEPROM);

	LOG->DEBG("Initialize crc module...");
	crc.init(0x00000000, CRCHw::POLYSIZE_8, 0x31, true, CRCHw::BY_WORD);

	LOG->DEBG("Initialize ws DMA & Timer");




	NVIC->ISER |= (1 << 10); // enable interrupt # 28 (USART2)
	NVIC->ISER |= (1 << 16); // enable interrupt # 28 (USART2)



	setLedMode(LED_ONCE);

	LOG->DEBG("Read lamp configuration from EEPROM...");
		mem.readArray(reinterpret_cast<uint8_t*>(&lampConfiguration.parameters), sizeof(LampParams), paramsEEPROMAddress);


		LOG->DEBG("Ready!");
			uint8_t calculatedCrc = crc.calcCRC(reinterpret_cast<uint8_t*>(&lampConfiguration.parameters), sizeof(LampParams)-1);
			if(lampConfiguration.parameters.crc!= calculatedCrc)
			{

				LOG->DEBG("Error reading from mem! set 0!");
				lampConfiguration.parameters.autoBright = true;
				lampConfiguration.parameters.bright =  1;
				lampConfiguration.parameters.color  = 0;
				lampConfiguration.parameters.effectNo = 1;
				lampConfiguration.parameters.effectMode =  0;


			}
			else
			{
				LOG->DEBG("Configuration:");
				LOG->DEBG("AutoBright: ", lampConfiguration.parameters.autoBright);
				LOG->DEBG("Bright value: ",lampConfiguration.parameters.bright);
				LOG->DEBG("Color: ", lampConfiguration.parameters.color);
				LOG->DEBG("Effect on: ", lampConfiguration.parameters.effectMode);
				LOG->DEBG("EffetNo: ", lampConfiguration.parameters.effectNo);
			}

		configureLedPairs(&pairs);

		LedEffects ledEffect(&pairs);
		ledEffect.setSpeed(2500);


		if(lampConfiguration.parameters.effectMode)
		{
			ledEffect.setEffectBright(lampConfiguration.parameters.bright);
			ledEffect.setEffect(static_cast<LedEffects::Effects>(lampConfiguration.parameters.effectNo));

		}
		else
		{
			ledstrip->setBright(lampConfiguration.parameters.bright);
			ledstrip->setColor(lampConfiguration.parameters.color);
		}




		LOG->DEBG(version);

    /* Loop forever */
/*test*/
	//	for(int i = 0; i<pairs.getCount(); i++)
	//							{
		//							pairs.getPair(i)->setHSV(0, 100, 0);
			//					}

				//				pairs.getPair(0)->setValue(0,100);
					//			pairs.refresh();



	/*endtest*/

						//		uint32_t timer = 0;
							//	uint16_t bright = 0;
								//uint16_t hue1  {0 };
								//uint16_t hue2 { 240 };
								//bool side {false};
								//bool sign { true };
		for(;;)
	{

		/*	if(timer == 150000)
			{
				for(int i = 0; i<pairs.getCount(); i++)
					{
					pairs.getPair(i)->setValue(bright,10-bright);
					pairs.getPair(i)->setHue(hue1,hue2);
					}
												pairs.refresh();
												if(sign){ bright++;}else { bright--;}

												if(sign)
												{

													if(bright>10){
													//	if(!hue2)hue2=240;else hue2 = 0;

													bright = 10;sign = false;}
												}
												else
												{
													if(bright == 0){
													//	if(!hue1)hue1=240;else hue1 = 0;
														sign = true;}
												}
				timer = 0;
			}
			timer++;
		*/




		if(getConfiguration(&lampConfiguration, &bRight, &bLeft))
		{
			configureLamp(&lampConfiguration, &ledEffect);
			lampConfiguration.parameters.crc = crc.calcCRC(reinterpret_cast<uint8_t*>(&lampConfiguration.parameters), sizeof(LampParams)-1);
		    mem.writeArray(paramsEEPROMAddress, reinterpret_cast<uint8_t*>(&lampConfiguration.parameters), sizeof(LampParams));
		}

		autoBrightLamp(&lampConfiguration, &ledEffect, &lightSensor);

		if(flagButt)
		{
			flagSecund = 0;
			if(lampConfiguration.parameters.effectMode)ledEffect.play();
			flagButt = 0;
		}
	}
}
