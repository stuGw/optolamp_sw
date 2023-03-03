/*
 * SmartPixelStrip.h
 *
 *  Created on: Mar 2, 2023
 *      Author: vit
 */

#ifndef SMARTPIXELSTRIP_H_
#define SMARTPIXELSTRIP_H_
#include "SmartPixel.h"
#include <stdint.h>





class SmartPixelStrip {


	SmartPixel* leds;
	uint32_t pixelCount { 0 };
	uint32_t resetTiming { 64 };
	static constexpr uint8_t RGB_COUNT_COLORS { 3 };
	static constexpr uint8_t RGBW_COUNT_COLORS { 4 };
	uint8_t ledCountColors { RGB_COUNT_COLORS };//rgb


	struct LedHwPack
	{
		//Значения цветов 24байта * WS_LED_COUNT
		uint8_t* ledHwPix { nullptr };//[WS_LED_COUNT];
		//Res
		uint8_t* res { nullptr };
	}packet;


	void setPixelHwColorRed(uint32_t id, uint8_t color);
	void setPixelHwColorGreen(uint32_t id, uint8_t color);
	void setPixelHwColorBlue(uint32_t id, uint8_t color);

	uint32_t* getHwAdress(){ return (uint32_t*)&packet; } //return addres to hw array for DMA & timer
public:

	enum PixelStripType : uint8_t { WS2812 = 0x00, SK6812RGBW = 0x01 } ledType { WS2812 };

	SmartPixelStrip(uint32_t count, PixelStripType type);

	void initializeWS2812Strip();

	void initializeSK6812RGBWStrip();



	virtual ~SmartPixelStrip();
};

#endif /* SMARTPIXELSTRIP_H_ */
