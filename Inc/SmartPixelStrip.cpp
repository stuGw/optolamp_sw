/*
 * SmartPixelStrip.cpp
 *
 *  Created on: Mar 2, 2023
 *      Author: vit
 */

#include <SmartPixelStrip.h>

SmartPixelStrip::SmartPixelStrip(uint32_t count, PixelStripType type)
{
	if(!count) return;
	pixelCount = count;
	ledType = type;
	switch (type)
	{
	case WS2812:
	{
		initializeWS2812Strip();
		leds = new SmartPixel[pixelCount];
		break;
	}
	case SK6812RGBW:
	{
		initializeSK6812RGBWStrip();
		leds = new SmartPixelW[pixelCount];
		leds->setWhite(255);
	}
	}





}

void SmartPixelStrip::initializeWS2812Strip()
{
	ledCountColors = RGB_COUNT_COLORS;
	packet.ledHwPix = new uint8_t[pixelCount*ledCountColors];
	packet.res = new uint8_t[resetTiming];
}

void SmartPixelStrip::initializeSK6812RGBWStrip()
{
	ledCountColors = RGBW_COUNT_COLORS;
	packet.ledHwPix = new uint8_t[pixelCount*ledCountColors];
	packet.res = new uint8_t[resetTiming];
}



void SmartPixelStrip::setPixelHwColorRed(uint32_t id, uint8_t color)
{
	for(uint8_t mask = 1; mask>0; mask = mask<<1)
	{
		packet.ledHwPix[id].setRed(color&mask, mask);
	}
}

void SmartPixelStrip::setPixelHwColorGreen(uint32_t id, uint8_t color)
{
	for(uint8_t mask = 1; mask>0; mask = mask<<1)
	{
		packet.ledHwPix[id].setGreen(color&mask, mask);
	}
}

void SmartPixelStrip::setPixelHwColorBlue(uint32_t id, uint8_t color)
{
	for(uint8_t mask = 1; mask>0; mask = mask<<1)
	{
		packet.ledHwPix[id].setBlue(color&mask, mask);
	}
}

SmartPixelStrip::~SmartPixelStrip() {
	// TODO Auto-generated destructor stub
}

