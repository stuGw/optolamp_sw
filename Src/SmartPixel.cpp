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






SmartPixel::SmartPixel(uint8_t* hw, uint8_t r, uint8_t g, uint8_t b, uint8_t br) {
	// TODO Auto-generated constructor stub
	red = r;
	green = g;
	blue = b;
	bright = br;
	hwPtr = hw;
}

void SmartPixel::convertToCurrColorHD(uint16_t val)
{
	if(val<=255){red = 255; green = val; blue = 0;}
	if((val>255)&&(val<512)){red = 511-val; green = 255; blue = 0;}
	if((val>=512)&&(val<768)){red = 0; green = 255; blue = val-512;}
	if((val>=768)&&(val<1024)){red =0; green = 1023-val; blue = 255;}
	if((val>=1024)&&(val<1280)){red =val-1024; green = 0; blue = 255;}
	if((val>=1280)&&(val<1536)){red = 255; green = 0; blue = 1535-val;}
	if(val == coldWhiteColor){ red = 230; green = 230; blue = 255;};
	if(val == whiteColor){ red = 255; green = 240; blue = 150;};
	if(val == warmWhiteColor){ red = 255; green = 200; blue = 80;};

}
/*if(((char)((rgb[led].R*rgbBright[rgb[led].BR])/1000)&mask) == mask)
			hwBuff.ledHwPix[led].Red[i] = WS_H1;
		else hwBuff.ledHwPix[led].Red[i] = WS_H0;*/
uint8_t SmartPixel::getHwColorValueRed()
{
	return (uint8_t)(((uint16_t)red*rgbBright[bright])/maxBrightValueInArr);
}

uint8_t SmartPixel::getHwColorValueGreen()
{
    return (uint8_t)(((uint16_t)green*rgbBright[bright])/maxBrightValueInArr);
}

uint8_t SmartPixel::getHwColorValueBlue()
{
    return (uint8_t)(((uint16_t)blue*rgbBright[bright])/maxBrightValueInArr);
}

void SmartPixel::hwRefresh()
{
	uint8_t r, g, b, i { 0 };
	r = getHwColorValueRed();
	b = getHwColorValueBlue();
	g = getHwColorValueGreen();
	for( uint8_t mask = 0x80; mask > 0; mask = mask >> 1)
	{
		if(r&mask) hwPtr[redOffset + i] = HW_1; else hwPtr[redOffset + i] = HW_0;
		if(g&mask) hwPtr[i] = HW_1; else hwPtr[i] = HW_0;
		if(b&mask) hwPtr[blueOffset + i] = HW_1; else hwPtr[blueOffset + i] = HW_0;
		i++;
	}
}

uint8_t SmartPixelW::getHwColorValueWhite()
{
	return (uint8_t)(white*rgbBright[bright]/maxBrightValueInArr);
}


void SmartPixelW::hwRefresh()
{


	uint8_t r, g, b, w, i { 0 };
	r = getHwColorValueRed();
	b = getHwColorValueBlue();
	g = getHwColorValueGreen();
	w = getHwColorValueWhite();
	for( uint8_t mask = 1; mask > 0; mask = mask << 1)
	{
		if(r&mask) hwPtr[redOffset + i] = HW_1; else hwPtr[redOffset + i] = HW_0;
		if(g&mask) hwPtr[i] = HW_1; else hwPtr[i] = HW_0;
		if(b&mask) hwPtr[blueOffset + i] = HW_1; else hwPtr[blueOffset + i] = HW_0;
		if(w&mask) hwPtr[whiteOffset + i] = HW_1; else hwPtr[whiteOffset + i] = HW_0;
		i++;
	}
}

SmartPixel::~SmartPixel() {
	// TODO Auto-generated destructor stub
}

