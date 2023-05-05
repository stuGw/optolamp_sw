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
	uint32_t resetBytes { 64 };
	static constexpr uint8_t RGB_COUNT_COLORS { 3 };
	static constexpr uint8_t RGBW_COUNT_COLORS { 4 };
    static constexpr uint8_t HW_BYTES_PER_COLOR { 8 };
    uint8_t ledCountColors { RGB_COUNT_COLORS };//rgb

    uint8_t* packet;


	void setPixelHwColorRed(uint32_t id, uint8_t color);
	void setPixelHwColorGreen(uint32_t id, uint8_t color);
	void setPixelHwColorBlue(uint32_t id, uint8_t color);
	void (*startTransfer)(uint32_t) { nullptr };

public:
    uint32_t* getHwAdress(){ return reinterpret_cast<uint32_t*>(packet); } //return addres to hw array for DMA & timer
	enum PixelStripType : uint8_t { WS2812 = 0x00, SK6812RGBW = 0x01 } ledType { WS2812 };
	enum PixelStripMode : uint8_t { ONCE_MODE = 0x00, CIRC_MODE = 0x01 } transferMode { CIRC_MODE };
	SmartPixelStrip(uint32_t count, PixelStripType type, PixelStripMode mode, void(*transferFunction)(uint32_t), bool HSVMode);

	void initializeWS2812Strip(bool inHSVMode);

	void initializeSK6812RGBWStrip();

    SmartPixel* getLed(uint32_t id){ if(id<pixelCount) return &leds[id]; else return nullptr; }
    void refresh();
    uint32_t count(){ return pixelCount; }
    uint8_t sizePerPixel(){ return ledCountColors * HW_BYTES_PER_COLOR; }
    void setBright(uint8_t bright);
    void setValue(uint8_t bright);
    void setHUE(uint16_t hue);
    void incrementHUE();
    void decrementValue();
    void setColor(uint16_t color);
    uint32_t hwSize(){ return resetBytes + sizePerPixel() * pixelCount; }
	virtual ~SmartPixelStrip();
};

#endif /* SMARTPIXELSTRIP_H_ */
