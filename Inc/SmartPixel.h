/*
 * SmartPixel.h
 *
 *  Created on: Mar 2, 2023
 *      Author: vit
 */

#ifndef SMARTPIXEL_H_
#define SMARTPIXEL_H_
#include <stdint.h>


class SmartPixel {

	static constexpr uint16_t maxBrightIndex { 21 };

	uint8_t red;
		uint8_t green;
		uint8_t blue;

		uint8_t inline getHwColorValueRed();
		uint8_t inline getHwColorValueGreen();
		uint8_t inline getHwColorValueBlue();

public:

	SmartPixel(uint8_t* ptr = nullptr, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t br = 0);
	virtual inline void setWhite(uint8_t color){};
	inline void setRed(uint8_t color){ red = color; }
	inline void setGreen(uint8_t color){ green = color; }
	inline void setBlue(uint8_t color){ blue = color; }
	inline void setBright(uint8_t b){ bright = b; }
	inline void setHardwarePtr(uint8_t* ptr){ hwPtr = ptr; }
	inline void hwRefresh();
	virtual ~SmartPixel();

protected:
	static constexpr uint16_t maxBrightValueInArr { 1000 };
	static constexpr uint16_t rgbBright[ maxBrightIndex+1 ] {0, 9, 12, 14, 18, 22, 35, 44, 55, 68,
		85, 107, 134, 167, 209, 262, 327, 410, 512, 640, 800, 1000};

	uint8_t* hwPtr;

	uint8_t bright;
};


class SmartPixelW : public SmartPixel {
	uint8_t white;

	uint8_t inline getHwColorValueWhite() ;
public:

	SmartPixelW(uint8_t* hw = nullptr, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t br = 0, uint8_t w = 0) : SmartPixel(hw, r,g,b,br)
	{
		white = w;
	}
	inline void setWhite(uint8_t color) override{ white = color; }
	void hwRefresh();
	virtual ~SmartPixelW(){};
};
#endif /* SMARTPIXEL_H_ */
