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

	uint16_t colorHD { 0 };
	static constexpr uint16_t maxColorValue { 1535 };


public:
    static constexpr uint8_t HW_0 { 22 };
    static constexpr uint8_t HW_1  { 58 };
    static constexpr uint16_t coldWhiteColor { 1536 };
    	static constexpr uint16_t whiteColor { 1537 };
    	static constexpr uint16_t warmWhiteColor { 1538 };
	SmartPixel(uint8_t* ptr = nullptr, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t br = 0);


    virtual  void setWhite(uint8_t color){};
	 void setRed(uint8_t color){ red = color; }
	void setGreen(uint8_t color){ green = color; }
	 void setBlue(uint8_t color){ blue = color; }
	 void setBright(uint8_t b){ bright = b; }
     void setHwPtr(uint8_t* ptr){ hwPtr = ptr; }
    virtual void hwRefresh();
	virtual ~SmartPixel();

	void setColor(uint16_t color)
	{
		colorHD = color;
		convertToCurrColorHD(color);
	}
	void setColor(uint8_t r, uint8_t g, uint8_t b)
	{
		red = r; green = g; blue = b;
	}

	uint16_t getColor(){ return colorHD; }
	uint8_t getBright(){ return bright; }
protected:
	static constexpr uint16_t maxBrightValueInArr { 1000 };
	static constexpr uint16_t rgbBright[ maxBrightIndex + 1 ] { 0, 9, 12, 14, 18, 22, 35, 44, 55, 68,
		85, 107, 134, 167, 209, 262, 327, 410, 512, 640, 800, 1000 };


	static constexpr uint8_t redOffset { 8 };
	static constexpr uint8_t greenOffset { 0 };
	static constexpr uint8_t blueOffset { 16 };


    uint8_t* hwPtr { nullptr };
	void convertToCurrColorHD(unsigned short int val);
	uint8_t bright;
	uint8_t  getHwColorValueRed();
	uint8_t  getHwColorValueGreen();
	uint8_t  getHwColorValueBlue();

};


class SmartPixelW : public SmartPixel {

	uint8_t white;
    static constexpr uint8_t whiteOffset { 24 };
	uint8_t  getHwColorValueWhite() ;
public:

	SmartPixelW(uint8_t* hw = nullptr, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t br = 0, uint8_t w = 0) : SmartPixel(hw, r,g,b,br)
	{
		white = w;
	}
	void setWhite(uint8_t color) override { white = color; }
     void hwRefresh() override;
	virtual ~SmartPixelW(){};
};
#endif /* SMARTPIXEL_H_ */
