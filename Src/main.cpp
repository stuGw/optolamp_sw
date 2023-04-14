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

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif




Button bLeft;
Button bRight;
uint32_t* hwLA;
AnalogConverter lightSensor;
Serial serial;//serial, using in logger &
Led *led;//just led
RTCTime time;//using to configure and read realtime

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
/*	if(counter>=1*24+5)
	{
		stopTimer3();
		//startLightSensDMA();//Запускаем канал LightSens
		flgWSTransfer = WS_TRANSFER_READY;//Передача закончена - можно передавать заново
		counter = 0;
	}
	else
		counter++;*/
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
	if(EXTI->RPR1 & 0x0002){ EXTI->RPR1 |= 0x0002; bLeft.rise();  /*LOG->DEBG("BEFFECT rise",bLeft.buttonTime);*/ }
	if(EXTI->RPR1 & 0x0001){ EXTI->RPR1 |= 0x0001; bRight.rise(); /*LOG->DEBG("BBRIGHT rise",bRight.buttonTime);*/ }
	if(EXTI->FPR1 & 0x0002){ EXTI->FPR1 |= 0x0002; bLeft.fall();  /*LOG->DEBG("BEFFECT fall");*/ }
	if(EXTI->FPR1 & 0x0001){ EXTI->FPR1 |= 0x0001; bRight.fall();  /*LOG->DEBG("BBRIGHT fall");*/ }
}


void initializeModules()
{
	led = new Led(&(GPIOC->ODR),GPIOPIN_13, false);
		led->on();

		serial.begin();
		serial.DEBG("Starting hardware initialization...");
		serial.DEBG("RTC initialization...");

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
		lightSensor.init();
		//PA7 - ADC7 - current, PB0 - ADC_8 - 60v voltage, PA6 - ADC6 - 12v voltage
		lightSensor.initChannels(AnalogConverter::ADC_7);

		lightSensor.getDataFiltered(AnalogConverter::ADC_7);
		LOG->DEBG("Initialize button interface...");
		bLeft.init(&(GPIOA->IDR), GPIOPIN_1, 1000, 300);
		bRight.init(&(GPIOA->IDR), GPIOPIN_0, 1000, 300);
}


struct CONFIG
{
	bool brightChanged{ 0 };
	bool colorChanged { 0 };
	bool effectChanged { 0 };
	uint8_t bright { 1 };
	bool autoBright { true };
	uint16_t colorValue { 0 };
	uint8_t currentEffect { 0 };
	bool effectMode { false };
};

bool getConfiguration(CONFIG* conf, uint8_t bR, uint8_t bL)
{
	bool changed = false;
	if(bR == Button::DOUBLE)
	{
		conf->autoBright = !conf->autoBright;
		if(conf->autoBright) LOG->DEBG("Autobright on!"); else LOG->DEBG("Autobright off");
		changed = true;
	}

	if(bL == Button::DOUBLE)
	{
		conf->effectMode = !conf->effectMode;
		if(conf->effectMode) LOG->DEBG("Effect mode on!"); else LOG->DEBG("Effect mode off");
		changed = true;
	}

	if(bR == Button::SINGLE)
	{
		if(!conf->autoBright)
		{
			conf->bright+=2;
			if(conf->bright > 21) conf->bright = 1;
			conf->brightChanged = true;
			changed = true;
		}
	}

	if(bL == Button::SINGLE)
	{
		if(conf->effectMode)
		{
			conf->currentEffect++;
			if(conf->currentEffect>10)conf->currentEffect = 1;
			conf->effectChanged = true;
		}
		else
		{
			conf->colorValue+=50;
			if(conf->colorValue>1530)conf->colorValue = 0;
			conf->colorChanged = true;
		}
		changed = true;
	}
	return changed;
}

int main(void)
{

	CONFIG lampConfiguration;

	SmartPixelStrip* ledstrip = new SmartPixelStrip(32,SmartPixelStrip::WS2812, SmartPixelStrip::ONCE_MODE, startWsTransfer);
	hwLA = ledstrip->getHwAdress();
	initializeHw();
	initializeModules();
	uint32_t* address;

	initializeDMA(address, ledstrip->hwSize());

	initWSTimer();
	NVIC->ISER |= (1 << 10); // enable interrupt # 28 (USART2)
	NVIC->ISER |= (1 << 16); // enable interrupt # 28 (USART2)

//	initWs2812Buff();
		//setAllLedsBright(10);
		setLedMode(LED_ONCE);
	//	setAllLedsColor(500);


		ledstrip->getLed(0)->setGreen(1);
		ledstrip->getLed(0)->setBright(21);



		ledstrip->getLed(1)->setRed(2);
		ledstrip->getLed(1)->setBright(21);


		ledstrip->getLed(2)->setBlue(3);
		ledstrip->getLed(2)->setBright(21);


		ledstrip->getLed(3)->setGreen(4);
		ledstrip->getLed(3)->setRed(255);
		ledstrip->getLed(3)->setBright(21);


		ledstrip->getLed(4)->setGreen(5);
		ledstrip->getLed(4)->setBlue(255);
		ledstrip->getLed(4)->setBright(21);



		LedPairs pairs(ledstrip);

		pairs.setPair(0, 0, 10);
		pairs.setPair(1, 1, 15);
		pairs.setPair(2, 2, 17);
		pairs.setPair(3, 3, 19);

		pairs.setPair(4, 4, 23);
		pairs.setPair(5, 5, 27);
		pairs.setPair(6, 6, 26);
		pairs.setPair(7, 7, 21);

		pairs.setPair(8, 8, 16);
		pairs.setPair(9, 9, 25);
		pairs.setPair(10, 11, 28);
		pairs.setPair(11, 12, 22);

		pairs.setPair(12, 13, 31);
		pairs.setPair(13, 14, 24);
		pairs.setPair(14, 18, 30);
		pairs.setPair(15, 20, 29);

		pairs.getPair(0)->setBright(21);
		pairs.getPair(0)->setColor(255,0,0);
		pairs.getPair(0)->refresh();

		pairs.getPair(1)->setBright(21);
		pairs.getPair(1)->setColor(0,255,0);
		pairs.getPair(1)->refresh();

		pairs.getPair(2)->setBright(21);
		pairs.getPair(2)->setColor(0,0,255);
		pairs.getPair(2)->refresh();

		pairs.getPair(3)->setBright(21);
		pairs.getPair(3)->setColor(255,255,0);
		pairs.getPair(3)->refresh();

		pairs.setBright(17);

		pairs.getPair(0)->setColor(255, 0, 0,  0, 128, 0);
		pairs.getPair(1)->setColor(0, 255, 0,  0, 128, 0);
		pairs.getPair(2)->setColor(0, 0, 255,  0, 128, 0);
		pairs.getPair(3)->setColor(0, 0, 255,  0, 255, 0);
		pairs.getPair(4)->setColor(255, 0, 0,  255, 0, 0);
		pairs.getPair(5)->setColor(0, 0, 255,  255, 0, 0);
		pairs.getPair(6)->setColor(0, 255, 0,  255, 0, 0);
		pairs.getPair(7)->setColor(0, 0, 255,  0, 0, 255);
		pairs.getPair(8)->setColor(255, 0, 0,  0, 0, 255);
		pairs.getPair(9)->setColor(0, 255, 0,  0, 0, 255);
		pairs.getPair(10)->setColor(255, 0, 0,  255, 0, 0);
		pairs.getPair(11)->setColor(0, 0, 255,  0, 255, 255);
		pairs.getPair(12)->setColor(255, 0, 0,  255, 255, 0);
		pairs.getPair(13)->setColor(0, 0, 255,  0, 0, 255);
		pairs.getPair(14)->setColor(255, 0, 0,  0, 128, 233);
		pairs.getPair(15)->setColor(0, 255, 0,  0, 128, 0);



		ledstrip->refresh();

		//LedEffects ledEffect(ledstrip);
		LedEffects ledEffect(&pairs);
		//LedEffects ledEffect(&pairs);
		ledEffect.setSpeed(2500);

		//ledEffect.setRainbowEachPairCoefficient(350);
		ledEffect.setEffect(LedEffects::FIRE_ALL);

	//	startWsTransfer();
		LOG->DEBG("ALl ready!");
    /* Loop forever */
	for(;;)
	{
		static uint16_t ledColor { 0 };
		static uint8_t ledBright { 1 };
		static uint16_t sensorValue { 0 };
		static int16_t autoBrightValue { 0 };
		static int16_t lastAutoBrightValue { 0 };
		bool lightChanged { false };
		uint8_t buttonLeftState { 0 };
		uint8_t buttonRightState { 0 };

		if(flagButt)
		{

			sensorValue = lightSensor.getDataFiltered(AnalogConverter::ADC_7);
			//LOG->DEBG("Light = ", sensorValue);
			autoBrightValue = WS_MAX_BRIGHT - (sensorValue/100);
			if(autoBrightValue<1)autoBrightValue = 1;
			if(autoBrightValue>WS_MAX_BRIGHT) autoBrightValue = WS_MAX_BRIGHT;
			if(autoBrightValue!= lastAutoBrightValue){  lastAutoBrightValue = autoBrightValue; lightChanged = true; };

			flagSecund = 0;
			if(lampConfiguration.effectMode)ledEffect.play();
		//	else
		//	{
		//		ledstrip->setColor(lampConfiguration.colorValue);
		//	}
			flagButt = 0;
		}

		//if(flagButt)
	//	{
			buttonRightState = bRight.getState();
			buttonLeftState = bLeft.getState();

	//		flagButt = 0;
	//	}


		if(lampConfiguration.autoBright && lightChanged)
		{
			LOG->DEBG("AutoBright = ", autoBrightValue);
			LOG->DEBG("SensorValue = ", sensorValue);
			if(lampConfiguration.effectMode)ledEffect.setEffectBright(autoBrightValue);
			else
			{
				ledstrip->setBright(autoBrightValue);
				ledstrip->refresh();
			}
			lightChanged = false;
		}

		if(getConfiguration(&lampConfiguration,buttonRightState, buttonLeftState))
		{

			//Bright
			if(lampConfiguration.autoBright)
			{
			/*	if(lightChanged)
				{
					LOG->DEBG("AutoBright = ", autoBrightValue);
					LOG->DEBG("SensorValue = ", sensorValue);
					if(lampConfiguration.effectMode)ledEffect.setEffectBright(autoBrightValue);
					else
					{
						ledstrip->setBright(autoBrightValue);
						ledstrip->refresh();
					}
					lightChanged = false;
				}*/
			}
			else if(lampConfiguration.brightChanged)
			{
				if(lampConfiguration.effectMode)ledEffect.setEffectBright(lampConfiguration.bright); else { ledstrip->setBright(lampConfiguration.bright); ledstrip->refresh(); };
				lampConfiguration.brightChanged = false;
				LOG->DEBG("Bright changed! - ", lampConfiguration.bright);
			}


			//color/effect
			if(lampConfiguration.colorChanged)
			{
				ledstrip->setColor(lampConfiguration.colorValue);
				ledstrip->refresh();
				lampConfiguration.colorChanged = false;
				LOG->DEBG("Color changed! - ", lampConfiguration.colorValue);
			}

			if(lampConfiguration.effectChanged)
			{
				ledEffect.setEffect((LedEffects::Effects)lampConfiguration.currentEffect);
				lampConfiguration.effectChanged = false;
				LOG->DEBG("Effect changed! - ", lampConfiguration.currentEffect);
			}
		}
	}
}
