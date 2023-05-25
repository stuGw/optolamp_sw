/*
 * ledeffects.cpp
 *
 *  Created on: Mar 10, 2023
 *      Author: vit
 */

#include <ledeffects.h>
#include "logger.h"
constexpr uint8_t LedEffects::ledFArrBBr[21+1][62];
constexpr uint8_t LedEffects::ledFireValueArr[62*2];
constexpr uint8_t LedEffects::ledFArrC[];
constexpr uint16_t LedEffects::colors[];

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

void LedEffects::play(uint32_t time)
{
	//LOG->DEBG("Time = ", time);
	//LOG->DEBG("EffTime = ", effectTime);
	if((time - effectLastTime)>effectTime)
	{
		effectLastTime = time;
	switch (effect)
		{
			case Effects::NO_EFFECT: break;
			case Effects::RAINBOW_ALL: { rainbowAllHSV(); break; }
			case Effects::RAINBOW_EACH: { rainbowAllHSV();  break; }
			case Effects::FIRE_ALL: { fireAll(); break; }
			case Effects::FIRE_EACH: { fireEach(); break; }
			case Effects::RAINBOW_PAIRS: { rainbowAllHSV(); break; }
			case Effects::RUN_PAIRS: { runPairsHSV(); break; }
			case Effects::RUN_PAIRS_SOFT: { runPairsSoftHSV(false); break; }
			case Effects::PAIRS_ON_SLOW_DOWN: { pairsOnSlowDown(); break; }
			case Effects::TWO_COLOR_CHANGE: { twoColorChange(false); break;}
			case Effects::TWO_COLOR_CHANGE_HUE: { twoColorChange(true); break; }
			case Effects::SINGLE_PAIR: { runPairsSoftHSV(true); break; }
		}
	}

}

void LedEffects::setEffect(Effects eff)
{
	effect = eff;
	sign = true;
	effBrightSaved = effBright;
	currentPair = 0;

	switch (effect)
		{
			case Effects::NO_EFFECT: break;
			case Effects::RAINBOW_ALL: { setTime(50); prepareAllHSV();/*pairsOfLeds->setBright(effBright); leds->setBright(effBright); */break; }
			case Effects::RAINBOW_EACH: {setTime(50); prepareEachHSV();  break; }
			case Effects::FIRE_ALL: {setTime(75); pairsOfLeds->getStripPtr()->setHUE(8); break; }
			case Effects::FIRE_EACH: {setTime(75); pairsOfLeds->getStripPtr()->setHUE(12); currentHue = 12; break; }
			case Effects::RAINBOW_PAIRS: { setTime(50);; preparePairHSV(); break; }
			case Effects::RUN_PAIRS: { setTime(200); pairsOfLeds->getStripPtr()->setValue(0); break; }
			case Effects::RUN_PAIRS_SOFT: { setTime(20); pairsOfLeds->getStripPtr()->setValue(0); pairsOfLeds->getPair(0)->setValue(effBright*5); break; }
			case Effects::PAIRS_ON_SLOW_DOWN: { delay = 0; currentBright = effBright*5;  setTime(30); pairsOfLeds->getStripPtr()->setValue(0); break; }
			case Effects::TWO_COLOR_CHANGE: { setTime(100); pairsOfLeds->getStripPtr()->setValue(0); break;}
			case Effects::TWO_COLOR_CHANGE_HUE: { setTime(100); pairsOfLeds->getStripPtr()->setValue(0); break; }
			case Effects::SINGLE_PAIR: { setTime(100); pairsOfLeds->getStripPtr()->setValue(0); pairsOfLeds->getPair(0)->setValue(effBright*5); break;}
		}
	//ledSpeed = 5;
}
void LedEffects::twoColorChange(bool hueUse)
{
	//static uint16_t hue2 { 180 };

	if(effBrightSaved!=effBright)
	{
		effBrightSaved = effBright;
		delay = 0;
		currentHue = 0;


		currentBright = 0;
		sign = true;
	}

	if(delay){ delay--; return; }
		for(int i = 0; i<pairsOfLeds->getCount(); i++)
		{
			pairsOfLeds->getPair(i)->setValue(currentBright,effBrightSaved*5-currentBright);
			pairsOfLeds->getPair(i)->setHue(currentHue, currentHue2);
		}
		pairsOfLeds->refresh();
		if(sign){ currentBright++;}else { currentBright--;}

		if(sign)
		{
			if(currentBright>effBrightSaved*5)
			{
				if(hueUse)
				{
					//if(!hue2)hue2=180;else hue2 = 0;
					if(currentHue2<=330)currentHue2+=30;else currentHue2 = 0;//30-360-hue2;
				}
				//if(!hue2)hue2=240;else hue2 = 0;
				currentBright = effBrightSaved*5;
				sign = false;
				delay = 20 + ( 21 - effBrightSaved );
				//if(hue2<=330)hue2+=30;else hue2 = 0;//30-360-hue2;
				//hue2 = hue1;
			}
		}
		else
		{
			if(currentBright == 0)
			{
				if(hueUse)
				{
					//if(!hue1)hue1=180;else hue1 = 0;
					if(currentHue<=330)currentHue+=30;else currentHue = 0;//30-360-hue1;
				}
				//	if(!hue1)hue1=240;else hue1 = 0;
				//if(hue1<=180)hue1+=180;else hue1 = 0;//30-360-hue1;
				sign = true;
				delay = 20 + ( 21 - effBrightSaved );

			}
		}




}
void LedEffects::rainbowAllHSV()
{
	leds->incrementHUE();
	leds->setValue(effBright*5);
	leds->refresh();
}

void LedEffects::prepareAllHSV()
{
	for(int i = 0; i < pairsOfLeds->getCount(); i++)
	{
		pairsOfLeds->getPair(i)->setHue(0);
	}
}
void LedEffects::prepareEachHSV()
{
	for(int i = 0; i < pairsOfLeds->getCount()/2; i++)
	{
		pairsOfLeds->getPair(i)->setHue(currentHue,currentHue+184);
		currentHue+=23;
	}

	currentHue = 0;
	for(int i = pairsOfLeds->getCount()/2; i < pairsOfLeds->getCount(); i++)
	{
		pairsOfLeds->getPair(i)->setHue(currentHue + 184, currentHue);
		currentHue+=23;
	}
}

void LedEffects::preparePairHSV()
{

	currentHue = 0;
	for(int i = 0; i < pairsOfLeds->getCount(); i++)
	{
		pairsOfLeds->getPair(i)->setHue(currentHue);
		currentHue+=23;
	}
	pairsOfLeds->refresh();
}

void LedEffects::fireAll()
{
	static unsigned short int i = 0;
	if(effBright>5) currentBright = 1; else currentBright = 6 - effBright;
	for(int j = 0; j< leds->count(); j++)
	{
		leds->getLed(j)->setValue(ledFireValueArr[i]/currentBright);
		//leds->getLed(j)->setHUE(8);//setColor(0xff,0x36,0x05);
	}

	i++;
	if(i>=FR_CF_LED_CNT*2)i = 0;
	leds->refresh();
}

void LedEffects::fireEach()
{
	static  uint8_t coefs[32] = { 0, 12, 7, 9, 34, 22, 29, 10, 17, 3, 33, 1, 27, 11, 5, 44, 19,
			31, 35, 13, 24, 6, 18, 4, 19, 33, 30, 20, 40, 50, 29, 8};

	if(effBright>5) currentBright = 1; else currentBright = 6 - effBright;

	for(int j = 0; j< leds->count(); j++)
	{
		leds->getLed(j)->setValue(ledFireValueArr[coefs[j]]/currentBright);
		//leds->getLed(j)->setHUE(8 + coefs[j]);
		if(currentHue<12) currentHue = 12;
		leds->getLed(j)->setHUE(currentHue - ledFArrC[coefs[j]]);
		coefs[j]++;
		if(coefs[j]>=FR_CF_LED_CNT) coefs[j] = 0;
	}

	leds->refresh();
}

void LedEffects::runPairsSoftHSV(bool flagLong)
{
	pairsOfLeds->setBright(0);

	static uint8_t previousPair { pairsOfLeds->getCount()-1 };

	if(effBrightSaved!=effBright)
	{

		currentPair = 0;
		previousPair = pairsOfLeds->getCount()-1;
		currentBright = 0;
		effBrightSaved = effBright;
		delay = 0;//21 - effBrightSaved;
		pairsOfLeds->getStripPtr()->setValue(0);
		pairsOfLeds->getPair(previousPair)->setValue(effBrightSaved*5);
	}

		if(delay){ delay--; return; }
		pairsOfLeds->getPair(previousPair)->decrementValue();
		pairsOfLeds->getPair(currentPair)->incrementValue(effBrightSaved*5);

		if(currentBright == effBrightSaved*5)
		{
			previousPair = currentPair;
			if(currentPair<pairsOfLeds->getCount()-1) currentPair++; else currentPair = 0;
			currentBright = 0;
			delay = (21-effBrightSaved) * 2;
			if(flagLong) delay+= 200;
		}

		currentBright++;
		pairsOfLeds->refresh();


}


void LedEffects::runPairsHSV()
{
	static uint8_t previousPair { pairsOfLeds->getCount()-1 };
	pairsOfLeds->getPair(previousPair)->setValue(0);
	pairsOfLeds->getPair(currentPair)->setValue(effBright*5);
	pairsOfLeds->refresh();
	previousPair = currentPair;
	if(currentPair<pairsOfLeds->getCount()-1) currentPair++; else currentPair = 0;
}


void LedEffects::pairsOnSlowDown()
{
	if(effBrightSaved!=effBright)
	{
		//brightChanged = true;
		delay = 10;
		currentBright = effBright*5;
		sign = true;
		currentPair = 0;
		effBrightSaved = effBright;
		pairsOfLeds->getStripPtr()->setValue(0);
	}

	if(delay){ delay--; return; }
	if(sign)
	{

		currentPair++;
		delay = 10;
		if(currentPair == pairsOfLeds->getCount())
		{
			sign = false;
			currentPair = 0;
			delay = 21-effBrightSaved;
		}
		pairsOfLeds->getPair(currentPair)->setValue(effBrightSaved*5);

	}
	else
	{
		currentBright--;
		pairsOfLeds->getStripPtr()->decrementValue();//setBright(bright);
		if(currentBright == 0)
		{
	//		if(brightChanged){ brightChanged = false; effBrightSaved = effBright; }
			currentBright = effBrightSaved*5;
			sign = true;

		}
		delay = 0;//5-(effBrightSaved/5);
	}

	pairsOfLeds->refresh();

}
