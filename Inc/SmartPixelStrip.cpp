/*
 * SmartPixelStrip.cpp
 *
 *  Created on: Mar 2, 2023
 *      Author: vit
 */

#include <SmartPixelStrip.h>

SmartPixelStrip::SmartPixelStrip(uint32_t count, PixelStripType type, PixelStripMode mode, void (*transferFunction)(uint32_t))
{
	if(!count) return;
	pixelCount = count;
	ledType = type;
	transferMode = mode;
	startTransfer = transferFunction;
	switch (type)
	{
		case WS2812:
		{
			leds = new SmartPixel[pixelCount];
			initializeWS2812Strip();

			break;
		}
		case SK6812RGBW:
		{
			leds = new SmartPixelW[pixelCount];
			initializeSK6812RGBWStrip();


		}
	}

}

void SmartPixelStrip::initializeWS2812Strip()
{
	ledCountColors = RGB_COUNT_COLORS;
    packet = new uint8_t[pixelCount*ledCountColors*HW_BYTES_PER_COLOR + resetBytes];
    //packet.res = new uint8_t[resetTiming];
    for(int i = 0; i < pixelCount; i++)
    {
        leds[i].setHwPtr(&(packet[resetBytes + i*ledCountColors*HW_BYTES_PER_COLOR]));
    }

    for(int i = 0; i<(pixelCount*ledCountColors*HW_BYTES_PER_COLOR + resetBytes); i++)
    {

    	if(i<resetBytes)packet[i] = 0;
    	else
        packet[i] = SmartPixel::HW_0;

    }
}

void SmartPixelStrip::initializeSK6812RGBWStrip()
{
    ledCountColors = RGBW_COUNT_COLORS;
    packet = new uint8_t[pixelCount*ledCountColors*HW_BYTES_PER_COLOR + resetBytes];
    //packet.res = new uint8_t[resetTiming];
    for(int i = 0; i < pixelCount; i++)
    {
        leds[i].setHwPtr(&(packet[i*ledCountColors*HW_BYTES_PER_COLOR]));
    }

    for(int i = 0; i<(pixelCount*ledCountColors*HW_BYTES_PER_COLOR + resetBytes); i++)
    {
        packet[i] = SmartPixel::HW_0;

    }

}

void SmartPixelStrip::setBright(uint8_t bright)
{
	for(int i = 0; i<pixelCount; i++)
	{
			leds[i].setBright(bright);
	}

}

void SmartPixelStrip::setColor(uint16_t color)
{
	for(int i = 0; i<pixelCount; i++)
	{
			leds[i].setColor(color);
	}

}

void SmartPixelStrip::refresh()
{
	for(int i = 0; i<pixelCount; i++)
	{
		leds[i].hwRefresh();
	}
	if(transferMode == ONCE_MODE)startTransfer(hwSize());
}

SmartPixelStrip::~SmartPixelStrip() {
	// TODO Auto-generated destructor stub
}

