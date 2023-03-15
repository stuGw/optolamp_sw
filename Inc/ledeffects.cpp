/*
 * ledeffects.cpp
 *
 *  Created on: Mar 10, 2023
 *      Author: vit
 */

#include <ledeffects.h>

constexpr uint8_t LedEffects::ledFArrBBr[21+1][62];
constexpr uint8_t LedEffects::ledFArrC[];


LedEffects::LedEffects(SmartPixelStrip* strip) {
	// TODO Auto-generated constructor stub
	leds = strip;
}
LedEffects::LedEffects(LedPairs* pairs) {
	// TODO Auto-generated constructor stub
	pairsOfLeds = pairs;
	leds = pairsOfLeds->getStripPtr();
}
LedEffects::~LedEffects() {
	// TODO Auto-generated destructor stub
}

void LedEffects::play()
{
	switch (effect)
		{
			case Effects::NO_EFFECT: break;
			case Effects::RAINBOW_ALL: { rainbowAll(); break; }
			case Effects::RAINBOW_EACH: { rainbowEach();  break; }
			case Effects::FIRE_ALL: { fireAll(); break; }
			case Effects::FIRE_EACH: { fireEach(); break; }
			case Effects::RAINBOW_PAIRS: { rainbowPairs(); break; }
			case Effects::RAINBOW_EACH_PAIR: { rainbowEachPair(); break; }
			case Effects::RUN_PIXELS: { runPixels(); break; }
			case Effects::RUN_PAIRS: { runPairs(); break; }
			case Effects::RUN_PIXELS_SOFT: { runPixelsSoft(); break; }
			case Effects::RUN_PAIRS_SOFT: { runPairsSoft(); break; }
		}
}

void LedEffects::rainbowAll()
{
		static unsigned short int rnbwCntr = 0;
		static unsigned short int color = 0;
		unsigned short int maxColor = 1535;
		rnbwCntr++;
		if(rnbwCntr>ledSpeed)
		{
			leds->setColor(color);
	     	color++;
			if(color>=maxColor)color = 0;
			rnbwCntr = 0;
			leds->refresh();
		}

}

void LedEffects::rainbowEach()
{
	//time - скорость изменения цвета
	//hsv 0 -360
	//rgb 0 - 767
	static unsigned short int rnbwCntr = 0;
	static unsigned short int color = 0;
	unsigned short int maxColor = 1535;
	unsigned short int colorTmp;
	rnbwCntr++;
	if(rnbwCntr>ledSpeed)
	{

		for(int i = 0; i<leds->count(); i++)
		{
			colorTmp = color + rainbowEachColorDiff * i;
			if(colorTmp>maxColor)colorTmp = colorTmp-maxColor;
			leds->getLed(i)->setColor(colorTmp);
		}


		color++;
		if(color>=maxColor)color = 0;
		rnbwCntr = 0;

		leds->refresh();
	}
}

void LedEffects::fireAll()
{
	static unsigned short int fireCntr = 0;
		static unsigned short int i = 0;
		if(fireCntr>=ledSpeed)
		{

			for(int j = 0; j< leds->count(); j++)
			{
				leds->getLed(j)->setBright(ledFArrBBr[effBright][i]);
				leds->getLed(j)->setColor(0xff,0x36,0x05);
			}

			i++;
			if(i>=FR_CF_LED_CNT)i = 0;

			fireCntr = 0;
			leds->refresh();
		}
		fireCntr++;
}

void LedEffects::fireEach()
{

	static unsigned short int fireCntr = 0;
	static  uint8_t coefs[32] = { 0, 12, 7, 9, 34, 22, 45, 10, 17, 3, 55, 1, 27, 11, 5, 44, 19,
			31, 35, 13, 24, 6, 18, 4, 49, 33, 30, 20, 40, 50, 29, 8};

	if(fireCntr>=ledSpeed)
	{

		for(int j = 0; j< leds->count(); j++)
		{
			leds->getLed(j)->setBright(ledFArrBBr[effBright][coefs[j]]);
			leds->getLed(j)->setColor(0xff-ledFArrC[coefs[j]],0x22,0x03-ledFArrC[coefs[j]]/10);
			coefs[j]++;
			if(coefs[j]>=FR_CF_LED_CNT) coefs[j] = 0;
		}

		fireCntr = 0;
		leds->refresh();
	}
	fireCntr++;

}


void LedEffects::runPixels()
{
	leds->setBright(0);
	static unsigned short int rnbwCntr = 0;
	static uint32_t currentLed { 0 }, previousLed { leds->count()-1 };
	rnbwCntr++;
	if(rnbwCntr>ledSpeed)
	{
		leds->getLed(previousLed)->setBright(0);
		leds->getLed(currentLed)->setBright(21);
		leds->refresh();
		previousLed = currentLed;
		if(currentLed<leds->count()-1) currentLed++; else currentLed = 0;
		rnbwCntr = 0;
	}
}

void LedEffects::runPixelsSoft()
{
	leds->setBright(0);
	volatile static unsigned short int rnbwCntr = 0;
	static uint32_t currentLed { 0 }, previousLed { leds->count()-1 };
	static bool sign { true };
	static uint8_t brightCurrent = 0;
	static uint8_t brightPrevious = effBright;
	rnbwCntr++;

	static uint8_t brightAllSaved = effBright;




	if(rnbwCntr>(ledSpeed))
	{

		leds->getLed(currentLed)->setBright(brightCurrent);
		leds->getLed(previousLed)->setBright(brightPrevious);

		brightCurrent++;
		brightPrevious--;

		if(brightCurrent == brightAllSaved)
		{

			if(brightAllSaved!=effBright)
					{
						brightCurrent = 0;
						brightPrevious = effBright;
						brightAllSaved = effBright;
					}

			previousLed = currentLed;
			if(currentLed<leds->count()-1) currentLed++; else currentLed = 0;
			brightCurrent = 0;//brightPrevious;
			brightPrevious = brightAllSaved;

		}

		leds->refresh();
		rnbwCntr = 0;
	}

}

void LedEffects::runPairsSoft()
{
	pairsOfLeds->setBright(0);
	volatile static unsigned short int rnbwCntr = 0;
	static uint32_t currentPair { 0 }, previousPair { pairsOfLeds->getCount()-1 };
	static bool sign { true };
	static uint8_t brightCurrent = 0;
	static uint8_t brightPrevious = effBright;
	static uint8_t brightAllSaved = effBright;


	if(brightAllSaved!=effBright)
	{
		brightCurrent = 0;
		brightPrevious = effBright;
		brightAllSaved = effBright;
	}
	rnbwCntr++;

	if(rnbwCntr>ledSpeed)
	{

		pairsOfLeds->getPair(currentPair)->setBright(brightCurrent);
		pairsOfLeds->getPair(previousPair)->setBright(brightPrevious);

		brightCurrent++;
		brightPrevious--;

		if(brightCurrent == effBright)
		{
			previousPair = currentPair;
			if(currentPair<pairsOfLeds->getCount()-1) currentPair++; else currentPair = 0;
			brightCurrent = brightPrevious;
			brightPrevious = effBright;

		}

		pairsOfLeds->refresh();
		rnbwCntr = 0;
	}

}

void LedEffects::runPairs()
{
	pairsOfLeds->setBright(0);
	static unsigned short int rnbwCntr = 0;
	static uint32_t currentLed { 0 }, previousLed { pairsOfLeds->getCount()-1 };
	rnbwCntr++;
	if(rnbwCntr>ledSpeed)
	{
		pairsOfLeds->getPair(previousLed)->setBright(0);
		pairsOfLeds->getPair(currentLed)->setBright(effBright);
		pairsOfLeds->refresh();
		previousLed = currentLed;
		if(currentLed<pairsOfLeds->getCount()-1) currentLed++; else currentLed = 0;
		rnbwCntr = 0;
	}
}


void LedEffects::rainbowPairs()
{
	//time - скорость изменения цвета
		//hsv 0 -360
		//rgb 0 - 767
		static unsigned short int rnbwCntr = 0;
		static unsigned short int color = 0;
		unsigned short int maxColor = 1535;
		unsigned short int colorTmp;
		rnbwCntr++;
		if(rnbwCntr>ledSpeed)
		{

			for(int i = 0; i<pairsOfLeds->getCount(); i++)
			{
				colorTmp = color + rainbowEachColorDiff * i;
				if(colorTmp>maxColor)colorTmp = colorTmp-maxColor;
				pairsOfLeds->getPair(i)->setColor(colorTmp);
			}


			color++;
			if(color>=maxColor)color = 0;
			rnbwCntr = 0;

			pairsOfLeds->refresh();
		}
}

void LedEffects::setRainbowEachPairCoefficient(uint16_t coef)
{
	rnbPairColorCoef = coef;
	for(int i = 0; i<pairsOfLeds->getCount(); i++)
	{
		pairsOfLeds->getPair(i)->setColor(coef*i);
	}
}

void LedEffects::rainbowEachPair()
{
	//time - скорость изменения цвета
		//hsv 0 -360
		//rgb 0 - 767
		static unsigned short int rnbwCntr = 0;

		unsigned short int maxColor = 1535;

		rnbwCntr++;
		if(rnbwCntr>ledSpeed)
		{

			for(int i = 0; i<pairsOfLeds->getCount(); i++)
			{
				uint16_t color = pairsOfLeds->getPair(i)->getColor();
				if(color>=maxColor)color = 0;
				pairsOfLeds->getPair(i)->setColor(color);

			}



			rnbwCntr = 0;

			pairsOfLeds->refresh();
		}
}

