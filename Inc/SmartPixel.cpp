/*
 * SmartPixel.cpp
 *
 *  Created on: Mar 2, 2023
 *      Author: vit
 */

#include <SmartPixel.h>

uint16_t constexpr SmartPixel::rgbBright[];
//{0, 9, 12, 14, 18, 22, 35, 44, 55, 68,
  //  85, 107, 134, 167, 209, 262, 327, 410, 512, 640, 800, 1000};


uint8_t HwColor::HW_0 { 22 };
uint8_t HwColor::HW_1 { 58 };



SmartPixel::SmartPixel(uint8_t* hw, uint8_t r, uint8_t g, uint8_t b, uint8_t br) {
	// TODO Auto-generated constructor stub
	red = r;
	green = g;
	blue = b;
	bright = br;
	hwPtr = hw;
}

uint8_t SmartPixel::getHwColorValueRed()
{
	return (uint8_t)(red*rgbBright[bright]/maxBrightValueInArr);
}

uint8_t SmartPixel::getHwColorValueGreen()
{
	return (uint8_t)(red*rgbBright[bright]/maxBrightValueInArr);
}

uint8_t SmartPixel::getHwColorValueBlue()
{
	return (uint8_t)(red*rgbBright[bright]/maxBrightValueInArr);
}

void SmartPixel::hwRefresh()
{
	uint8_t r, g, b;
	r = getHwColorValueRed();
	b = getHwColorValueBlue();
	g = getHwColorValueGreen();
	for( uint8_t mask = 1; mask > 0; mask = mask << 1)
	{
		hwPtr->setRed(r&mask, mask);
		hwPtr->setGreen(g&mask, mask);
		hwPtr->setBlue(b&mask,  mask);
	}
}

uint8_t SmartPixelW::getHwColorValueWhite()
{
	return (uint8_t)(white*rgbBright[bright]/maxBrightValueInArr);
}


void SmartPixelW::hwRefresh()
{
	SmartPixel::hwRefresh();
	uint8_t w = getHwColorValueWhite();
	for( uint8_t mask = 1; mask > 0; mask = mask << 1)
	{
		hwPtr->setWhite(w&mask, mask);
	}
}

SmartPixel::~SmartPixel() {
	// TODO Auto-generated destructor stub
}

