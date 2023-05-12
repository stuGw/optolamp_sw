/*
 * ledeffects.cpp
 *
 *  Created on: Mar 10, 2023
 *      Author: vit
 */

#include <ledeffects.h>

constexpr uint8_t LedEffects::ledFArrBBr[21+1][62];
constexpr uint8_t LedEffects::ledFireValueArr[62*2];
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
			case Effects::RAINBOW_ALL: { rainbowAllHSV(); break; }
			case Effects::RAINBOW_EACH: { rainbowAllHSV();  break; }
			case Effects::FIRE_ALL: { fireAll(); break; }
			case Effects::FIRE_EACH: { fireEach(); break; }
			case Effects::RAINBOW_PAIRS: { rainbowAllHSV(); break; }
			case Effects::RUN_PAIRS: { runPairsHSV(); break; }
			case Effects::RUN_PAIRS_SOFT: { runPairsSoftHSV(); break; }
			case Effects::PAIRS_ON_SLOW_DOWN: { pairsOnSlowDown(); break; }
			case Effects::TWO_COLOR_CHANGE: { twoColorChange(false); break;}
			case Effects::TWO_COLOR_CHANGE_HUE: { twoColorChange(true); break; }
		}


}

void LedEffects::setEffect(Effects eff)
{
	effect = eff;
	switch (effect)
		{
			case Effects::NO_EFFECT: break;
			case Effects::RAINBOW_ALL: { setSpeed(5); prepareAllHSV();/*pairsOfLeds->setBright(effBright); leds->setBright(effBright); */break; }
			case Effects::RAINBOW_EACH: {setSpeed(5); prepareEachHSV();  break; }
			case Effects::FIRE_ALL: {setSpeed(5);  break; }
			case Effects::FIRE_EACH: {setSpeed(5);  break; }
			case Effects::RAINBOW_PAIRS: { setSpeed(5);; preparePairHSV(); break; }
			case Effects::RUN_PAIRS: { setSpeed(10); pairsOfLeds->getStripPtr()->setValue(0); break; }
			case Effects::RUN_PAIRS_SOFT: { setSpeed(10); pairsOfLeds->getStripPtr()->setValue(0); pairsOfLeds->getPair(0)->setValue(effBright*5); break; }
			case Effects::PAIRS_ON_SLOW_DOWN: {setSpeed(5); pairsOfLeds->getStripPtr()->setValue(0); break; }
			case Effects::TWO_COLOR_CHANGE: { setSpeed(100); pairsOfLeds->getStripPtr()->setValue(0); break;}
			case Effects::TWO_COLOR_CHANGE_HUE: { setSpeed(100); pairsOfLeds->getStripPtr()->setValue(0); break; }
		}
	//ledSpeed = 5;
}
void LedEffects::twoColorChange(bool hueUse)
{
	static uint16_t hue1 { 0 };
	static uint16_t hue2 { 180 };
	static uint16_t timer { 0 };
	static uint16_t bright { 0 };
	static bool sign { true };
	static uint8_t effBrightSaved { effBright };
	static uint8_t additiveTimer { effBright*5 };
	if(effBrightSaved!=effBright)
	{
		effBrightSaved = effBright;
		timer = 0;
		hue1 = 0;
		hue2 = 180;
		bright = 0;
		sign = true;
		additiveTimer = 0;//effBright*5;
	}
	if(timer == ledSpeed)
	{
		additiveTimer = effBrightSaved*5;
		//timer = 0;

	}

	if(additiveTimer  == effBrightSaved*5)// effBrightSaved*5)
	{
		for(int i = 0; i<pairsOfLeds->getCount(); i++)
		{
			pairsOfLeds->getPair(i)->setValue(bright,effBrightSaved*5-bright);
			pairsOfLeds->getPair(i)->setHue(hue1,hue2);
		}
		pairsOfLeds->refresh();
		if(sign){ bright++;}else { bright--;}

		if(sign)
		{
			if(bright>effBrightSaved*5)
			{
				if(hueUse)
				{
					//if(!hue2)hue2=180;else hue2 = 0;
					if(hue2<=330)hue2+=30;else hue2 = 0;//30-360-hue2;
				}
				//if(!hue2)hue2=240;else hue2 = 0;
				bright = effBrightSaved*5;sign = false;
				//if(hue2<=330)hue2+=30;else hue2 = 0;//30-360-hue2;
				//hue2 = hue1;
			}
		}
		else
		{
			if(bright == 0)
			{
				if(hueUse)
				{
					//if(!hue1)hue1=180;else hue1 = 0;
					if(hue1<=330)hue1+=30;else hue1 = 0;//30-360-hue1;
				}
				//	if(!hue1)hue1=240;else hue1 = 0;
				//if(hue1<=180)hue1+=180;else hue1 = 0;//30-360-hue1;
				sign = true;
			}
		}
		additiveTimer--;
	}
	else
	{
		if(additiveTimer) additiveTimer--;
		else
			timer = 0;
		timer++;
	}


}
void LedEffects::rainbowAllHSV()
{
	static unsigned short int rnbwCntr = 0;
	rnbwCntr++;
	if(rnbwCntr>ledSpeed)
	{
		leds->incrementHUE();
		leds->setValue(effBright*5);
		rnbwCntr = 0;
		leds->refresh();
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
			leds->setBright(effBright);
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

void LedEffects::prepareAllHSV()
{
	for(int i = 0; i < pairsOfLeds->getCount(); i++)
	{
		pairsOfLeds->getPair(i)->setHue(0);
	}
}
void LedEffects::prepareEachHSV()
{
	uint16_t hue = 0;

	for(int i = 0; i < pairsOfLeds->getCount()/2; i++)
	{
		pairsOfLeds->getPair(i)->setHue(hue,hue+184);
		hue+=23;
	}

	hue = 0;
	for(int i = pairsOfLeds->getCount()/2; i < pairsOfLeds->getCount(); i++)
	{
		pairsOfLeds->getPair(i)->setHue(hue + 184, hue);
		hue+=23;
	}

	/*pairsOfLeds->getPair(0)->setHue(0, 184);
	pairsOfLeds->getPair(1)->setHue(23, 207);
	pairsOfLeds->getPair(2)->setHue(46, 230);
	pairsOfLeds->getPair(3)->setHue(69, 253);
	pairsOfLeds->getPair(4)->setHue(92, 276);
	pairsOfLeds->getPair(5)->setHue(115, 300);
	pairsOfLeds->getPair(6)->setHue(138, 323);
	pairsOfLeds->getPair(7)->setHue(161, 346);
	pairsOfLeds->getPair(8)->setHue(184, 0);
	pairsOfLeds->getPair(9)->setHue(207, 23);
	pairsOfLeds->getPair(10)->setHue(230, 46);
	pairsOfLeds->getPair(11)->setHue(253, 69);
	pairsOfLeds->getPair(12)->setHue(276, 92);
	pairsOfLeds->getPair(13)->setHue(300, 115);
	pairsOfLeds->getPair(14)->setHue(323, 138);
	pairsOfLeds->getPair(15)->setHue(346, 161);*/
}

void LedEffects::preparePairHSV()
{
	uint16_t hue = 0;
	for(int i = 0; i < pairsOfLeds->getCount(); i++)
	{
		pairsOfLeds->getPair(i)->setHue(hue);
		hue+=23;
	}
}

void LedEffects::fireAll()
{
	static unsigned short int fireCntr = 0;
		static unsigned short int i = 0;
		uint8_t bright { 0 };
		if(effBright>5) bright = 1; else bright = 6 - effBright;
		if(fireCntr>=ledSpeed)
		{

			for(int j = 0; j< leds->count(); j++)
			{
				leds->getLed(j)->setValue(ledFireValueArr[i]/bright);
				leds->getLed(j)->setHUE(8);//setColor(0xff,0x36,0x05);
			}

			i++;
			if(i>=FR_CF_LED_CNT*2)i = 0;

			fireCntr = 0;
			leds->refresh();
		}
		fireCntr++;
}

void LedEffects::fireEach()
{

	static unsigned short int fireCntr = 0;
	static  uint8_t coefs[32] = { 0, 12, 7, 9, 34, 22, 29, 10, 17, 3, 33, 1, 27, 11, 5, 44, 19,
			31, 35, 13, 24, 6, 18, 4, 19, 33, 30, 20, 40, 50, 29, 8};
	uint8_t bright { 0 };
	if(effBright>5) bright = 1; else bright = 6 - effBright;
	if(fireCntr>=ledSpeed)
	{

		for(int j = 0; j< leds->count(); j++)
		{
			leds->getLed(j)->setValue(ledFireValueArr[coefs[j]]/bright);
			//leds->getLed(j)->setHUE(8 + coefs[j]);
			leds->getLed(j)->setHUE(18 - ledFArrC[coefs[j]]);
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
		leds->getLed(currentLed)->setBright(effBright);
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

void LedEffects::runPairsSoftHSV()
{
	pairsOfLeds->setBright(0);
	volatile static unsigned short int rnbwCntr = 0;
	static uint32_t currentPair { 0 }, previousPair { pairsOfLeds->getCount()-1 };

	static uint8_t brightCurrent = 0;
	static uint8_t effBrightSaved { effBright };

	rnbwCntr++;

	if(effBrightSaved!=effBright)
	{
		rnbwCntr = 0;
		currentPair = 0;
		previousPair = pairsOfLeds->getCount()-1;
		brightCurrent = 0;
		effBrightSaved = effBright;
		pairsOfLeds->getStripPtr()->setValue(0);
		pairsOfLeds->getPair(previousPair)->setValue(effBrightSaved*5);
	}

	if(rnbwCntr>ledSpeed)
	{
		pairsOfLeds->getPair(previousPair)->decrementValue();
		pairsOfLeds->getPair(currentPair)->incrementValue(effBrightSaved*5);

		if(brightCurrent == effBrightSaved*5)
		{
			previousPair = currentPair;
			if(currentPair<pairsOfLeds->getCount()-1) currentPair++; else currentPair = 0;
			brightCurrent = 0;

		}

		brightCurrent++;
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

void LedEffects::runPairsHSV()
{
	static unsigned short int rnbwCntr = 0;
	static uint32_t currentLed { 0 }, previousLed { pairsOfLeds->getCount()-1 };
	rnbwCntr++;
	if(rnbwCntr>ledSpeed)
	{
		pairsOfLeds->getPair(previousLed)->setValue(0);
		pairsOfLeds->getPair(currentLed)->setValue(effBright*5);
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
		unsigned short int colorTmp ;
		rnbwCntr++;

		if(rnbwCntr>ledSpeed)
		{
			pairsOfLeds->setBright(effBright);
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



void LedEffects::pairsOnSlowDown()
{
	static uint8_t currentPair { 0 };
	static uint16_t delay { 10 };
	static uint8_t effBrightSaved { effBright };
	//static bool brightChanged { false };
	static bool up { true };
	static uint8_t bright { effBright*5 };

	if(effBrightSaved!=effBright)
	{
		//brightChanged = true;
		delay = 10;
		bright = effBright*5;
		up = true;
		currentPair = 0;
		effBrightSaved = effBright;
		pairsOfLeds->getStripPtr()->setValue(0);
	}

	if(delay){ delay--; return; }
	if(up)
	{

		currentPair++;
		delay = 10;
		if(currentPair == pairsOfLeds->getCount())
		{
			up = false;
			currentPair = 0;
			delay = 100;
		}
		pairsOfLeds->getPair(currentPair)->setValue(effBrightSaved*5);

	}
	else
	{
		bright--;
		pairsOfLeds->getStripPtr()->decrementValue();//setBright(bright);
		if(bright == 0)
		{
	//		if(brightChanged){ brightChanged = false; effBrightSaved = effBright; }
			bright = effBrightSaved*5;
			up = true;

		}
		delay = 5;
	}

	pairsOfLeds->refresh();

}
