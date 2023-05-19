/*
 * ledeffects.h
 *
 *  Created on: Mar 10, 2023
 *      Author: vit
 */

#ifndef LEDEFFECTS_H_
#define LEDEFFECTS_H_
#include "SmartPixelStrip.h"
#include "ledpairs.h"

class LedEffects {
	SmartPixelStrip* leds { nullptr };
	LedPairs* pairsOfLeds { nullptr };

	uint32_t effectTime { 0 };
	uint16_t rainbowEachColorDiff { 45 };
	uint8_t effBright { 5 };
	uint8_t effBrightSaved { effBright };
	uint16_t rnbPairColorCoef { 50 };

	uint8_t currentBright { 1 };
	uint16_t currentHue { 0 };
    uint8_t currentPair { 0 };
	uint8_t delay { 0 };
	bool sign { false };

	static constexpr uint8_t FR_CF_LED_CNT { 62 };

	static constexpr uint8_t ledFireValueArr[FR_CF_LED_CNT*2] = {50,55,60,70,80,85,90,85,80,75,70,65,60,55,50,55,60,50,40,45,50,40,30,50,70,65,60,55,50,45,40,55,70,55,40,45,50,45,40,60,80,70,60,55,50,
			45,40,55,70,75,80,70,60,65,70,60,50,60,70,55,40,45,50,55,60,55,50,55,60,70,80,65,50,45,40,65,90,75,60,50,40,45,50,60,70,65,60,65,70,60,50,55,60,55,50,70,90,85,80,75,70,65,60,55,50,70,90,65,
			40,45,50,55,60,65,70,65,60,65,70,60,50,45,40,45};
		//														{25,27,30,35,40,43,45,42,40,37,35,32,30,27,25,27,30,25,20,23,25,20,15,25,35,33,30,27,25,22,20,27,35,23,20,45,50,45,40,60,80,70,60,55,50,
	//		45,40,55,70,75,80,70,60,65,70,60,50,60,70,55,40,45,50,55,60,55,50,55,60,70,80,65,50,45,40,65,90,75,60,50,40,45,50,60,70,65,60,65,70,60,50,55,60,55,50,70,90,85,80,75,70,65,60,55,50,70,90,65,
		//	40,45,50,55,60,65,70,65,60,65,70,60,50,45,40,45};


	static constexpr uint8_t ledFArrBBr[21+1][FR_CF_LED_CNT] =
	{


			{4,5,6,5,6,4,5,4,5,3,4,3,6,5,4,3,5,3,4,3,6,5,4,3,5,6,5,6,4,5,3,
		 4,5,6,5,6,4,3,6,5,3,4,6,5,6,4,5,4,6,5,4,5,4,6,3,4,5,6,5,4,6,3},

	{4,5,6,5,6,4,5,4,5,3,4,3,6,5,4,3,5,3,4,3,6,5,4,3,5,6,5,6,4,5,3,
	4,5,6,5,6,4,3,6,5,3,4,6,5,6,4,5,4,6,5,4,5,4,6,3,4,5,6,5,4,6,3,},

	{6,7,8,7,8,6,7,6,7,5,6,5,8,7,6,5,7,5,6,5,8,7,6,5,7,8,7,8,6,7,5,
	6,7,8,7,8,6,5,8,7,5,6,8,7,8,6,7,6,8,7,6,7,6,8,5,6,7,8,7,6,8,5,},

	{8,9,10,9,10,8,9,8,9,7,8,7,10,9,8,7,9,7,8,7,10,9,8,7,9,10,9,10,8,9,7,
	8,9,10,9,10,8,7,10,9,7,8,10,9,10,8,9,8,10,9,8,9,8,10,7,8,9,10,9,8,10,7,},

	{10,11,12,11,12,10,11,10,11,9,10,9,12,11,10,9,11,9,10,9,12,11,10,9,11,12,11,12,10,11,9,
	10,11,12,11,12,10,9,12,11,9,10,12,11,12,10,11,10,12,11,10,11,10,12,9,10,11,12,11,10,12,9,},

	{12,13,14,13,14,12,13,12,13,11,12,11,14,13,12,11,13,11,12,11,14,13,12,11,13,14,13,14,12,13,11,
	12,13,14,13,14,12,11,14,13,11,12,14,13,14,12,13,12,14,13,12,13,12,14,11,12,13,14,13,12,14,11,},

	{14,15,16,15,16,14,15,14,15,13,14,13,16,15,14,13,15,13,14,13,16,15,14,13,15,16,15,16,14,15,13,
	14,15,16,15,16,14,13,16,15,13,14,16,15,16,14,15,14,16,15,14,15,14,16,13,14,15,16,15,14,16,13,},

	{16,17,18,17,18,16,17,16,17,15,16,15,18,17,16,15,17,15,16,15,18,17,16,15,17,18,17,18,16,17,15,
	16,17,18,17,18,16,15,18,17,15,16,18,17,18,16,17,16,18,17,16,17,16,18,15,16,17,18,17,16,18,15,},

	{19,20,21,20,21,19,20,19,20,18,19,18,21,20,19,18,20,18,19,18,21,20,19,18,20,21,20,21,19,20,18,
	19,20,21,20,21,19,18,21,20,18,19,21,20,21,19,20,19,21,20,19,20,19,21,18,19,20,21,20,19,21,18,}
	};

	static constexpr uint8_t ledFArrC[FR_CF_LED_CNT] = {10,10,11,12,11,10,10,10,10,10,10,10,10,10,10,9,9,9,8,8,7,8,9,10,11,11,10,10,10,
																	10,9,10,10,10,10,10,10,10,10,10,10,11,10,11,10,10,10,11,12,12,11,10,10,10,9,10,10,10,
																	11,10,10,10};

public:
	LedEffects(SmartPixelStrip* strip);
	LedEffects(LedPairs* pairs);
	virtual ~LedEffects();
	inline SmartPixelStrip* getLedsStrip() { return leds; }
	static constexpr uint8_t countEffects{ 11 };
	enum Effects : uint8_t { NO_EFFECT = 0x00, RAINBOW_ALL = 0x01, RAINBOW_EACH = 0x02, FIRE_ALL = 0x03, FIRE_EACH = 0x04, RAINBOW_PAIRS = 0x05,
		 RUN_PAIRS = 0x06,  RUN_PAIRS_SOFT = 0x07, PAIRS_ON_SLOW_DOWN = 0x08, TWO_COLOR_CHANGE_HUE = 0x09, TWO_COLOR_CHANGE = 0x0a, SINGLE_PAIR = 0x0b} effect { NO_EFFECT };

	void setEffect(Effects eff);//{ ledSpeed = 5; effect = eff; }
	void setEffectBright(uint8_t bright){ effBright = bright; }
	uint32_t effectLastTime { 0 };
	void setTime(uint32_t time){ effectTime = time; }

	void setRainbowEachPairCoefficient(uint16_t coef);

	void play(uint32_t time);
	void rainbowAll();
	void rainbowAllHSV();
	void runPairsHSV();

	void rainbowEach();
	void fireAll();
	void fireEach();
	void runPixels();
	void runPixelsSoft();

	void runPairs();
	void runPairsSoft();
	void runPairsSoftHSV(bool flagLong);
	void rainbowPairs();
	void pairsOnSlowDown();

	void twoColorChange(bool hueUse);
	void prepareEachHSV();
	void prepareAllHSV();
	void preparePairHSV();
};

#endif /* LEDEFFECTS_H_ */
