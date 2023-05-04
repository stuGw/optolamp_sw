/*
 * SmartPixel.h
 *
 *  Created on: Mar 2, 2023
 *      Author: vit
 */

#ifndef SMARTPIXEL_H_
#define SMARTPIXEL_H_
#include <stdint.h>
#include <math.h>

class SmartPixel {

	static constexpr uint16_t maxBrightIndex { 21 };

	bool HSVMode { false };

	uint8_t red;
	uint8_t green;
	uint8_t blue;

	uint16_t hue;
	uint8_t saturation;
	uint8_t value;

	uint16_t colorHD { 0 };
	static constexpr uint16_t maxColorValue { 1535 };


public:
    static constexpr uint8_t HW_0 { 22 };
    static constexpr uint8_t HW_1  { 58 };
    static constexpr uint16_t coldWhiteColor { 1536 };
    	static constexpr uint16_t whiteColor { 1537 };
    	static constexpr uint16_t warmWhiteColor { 1538 };
	SmartPixel(uint8_t* ptr = nullptr, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t br = 0, bool hsv = false);

	void setHSVOn(bool on ){ HSVMode = on; }
    virtual  void setWhite(uint8_t color){};
	 void setRed(uint8_t color){ red = color; }
	void setGreen(uint8_t color){ green = color; }
	void setBlue(uint8_t color){ blue = color; }
	void setBright(uint8_t b){ bright = b; }
    void setHwPtr(uint8_t* ptr){ hwPtr = ptr; }
    virtual void hwRefresh();
    void hwRefreshHSV();
    void hwRefreshRGB();
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

	void incrementHUE()
	{
		if(hue==359) hue = 0; else hue++;
	}
	void setHUE(uint16_t h)
	{
		if(h>=360) hue = 359; else hue = h;
	}

	void setSaturation(uint8_t s)
	{
		if(s>=100)saturation = 99; else saturation = s;
	}

	void incrementValue()
	{
		if(value<100) value++;
	}
	void incrementValue(uint8_t maxValue)
		{
			if((value<100) && (value<maxValue)) value++;
		}
	void decrementValue()
	{
		if(value>0) value--;
	}

	void setValue(uint8_t v)
	{
		if(v>=100) value = 99; else value = v;
	}

	void setHSV(uint16_t h, uint8_t s, uint8_t v)
	{
		if(h>=360) hue = 359; else hue = h;
		if(s>=100)saturation = 99; else saturation = s;
		if(v>=100) value = 99; else value = v;
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
	void refreshHSVFF()
	    {


	            int64_t bhue = hue*10000;
	            int64_t bsaturation = saturation*10000;
	            int64_t bvalue = value*10000;

	            int64_t h = bhue/600000;

	            int64_t hd = (bhue%600000)/60;//fmod(bhue,60)/60;

	            int64_t vm = ((1000000-bsaturation)*bvalue)/1000000;

	            int64_t a = ((bvalue-vm)*(hd))/10000;

	            int64_t vi = vm+a;
	            int64_t vd = bvalue-a;
	            int64_t r,g,b;
	            switch(h)
	            {
	                case 0:{r = bvalue;g = vi;b = vm;break;}
	                case 1:{r = vd;g = bvalue;b = vm;break;}
	                case 2:{r = vm;g =bvalue;b = vi;break;}
	                case 3:{r = vm;g = vd;b = bvalue;break;}
	                case 4:{r = vi;g = vm;b = bvalue;break;}
	                case 5:{r = bvalue;g = vm;b = vd;break;}
	            }
	            red = ((r/100)*255)/10000;
	            green =( (g/100)*255)/10000;
	            blue = ((b/100)*255)/10000;

	            //hwRefreshHSV();
	    }

	void refreshHSV()
	{
	        int h = hue/60;
	        float hd = fmod(hue,60)/60.0;
	        float vm = ((100.0-saturation)*value)/100.0;
	        float a = (value-vm)*(hd);
	        float vi = vm+a;
	        float vd = value-a;
	        float r,g,b;
	        switch(h)
	        {
	            case 0:{r = value;g = vi;b = vm;break;}
	            case 1:{r = vd;g = value;b = vm;break;}
	            case 2:{r = vm;g =value;b = vi;break;}
	            case 3:{r = vm;g = vd;b = value;break;}
	            case 4:{r = vi;g = vm;b = value;break;}
	            case 5:{r = value;g = vm;b = vd;break;}
	        }
	        red = (r/100)*255;
	        green = (g/100)*255;
	        blue = (b/100)*255;

	        //hwRefreshHSV();
	}
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
