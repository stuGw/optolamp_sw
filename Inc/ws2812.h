#ifndef WS2812_H
#define WS2812_H
/*
Библиотека для работы со светодиодами ws28xx
Каждый диод кодируется тремя цветами максимальное значение для каждого цвета 255
Протокол передачи однопроводной
Единицы и нули кодируются передачей двух уровней 
		 ______
0	- |  Т0H |___T0L_____| - код 0
     __________
1 - |   T1H    |__T1L__|  - код 1

RES - |_______TRES_______|     - код RES (установки значений)

Таблица значений для разных диодов
|Диод  | TOH | TOL | T1H | T1L | RES|
|------|-----|-----|-----|-----|----|
|2812  |0.35 |0.8  |0.7  |0.6  |>50 |uS +/- 150nS
|------|-----|-----|-----|-----|----|
|2812cn|0.22 |0.75 |0.75 |0.22 |>300|uS +/- 150nS(до 0.42 1.6 1.6 0.42)
|------|-----|-----|-----|-----|----|
|2812  |0.35 |0.8  |0.7  |0.6  |>50 |uS +/- 150nS
|------|-----|-----|-----|-----|----|
|2812  |0.35 |0.8  |0.7  |0.6  |>50 |uS +/- 150nS
|------|-----|-----|-----|-----|----|
|2812  |0.35 |0.8  |0.7  |0.6  |>50 |uS +/- 150nS
|------|-----|-----|-----|-----|----|

 __     __     __     __  	 __     __     __     __     __     __     __     __  	 __     __     __     __     __     __     __     __  	 __     __     __     __         
|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___|	 |___|	|___ ________________|
:  7   :  6   :  5   :  4   :  3   :  2   :  1   :  0   :  7   :  6   :  5   :  4   :  3   :  2   :  1   :  0   :  7   :  6   :  5   :  4   :  3   :  2   :  1   :  0   :       RES				:
{									      GREEEN                         }{                        RED                            }{                       BLUE														}{    RES         }

Первый 24 бита поданые на вход записывает во внутреннюю память первый пиксель.
Последующие пакеты битов он пропускает через себя на выход. Второй пиксель повторяет действия первого, 
записывая к себе 24 бита дошедшие до него первыми. И так по всем пикселям.
Чтобы записанные в память значения стали активными(светодиоды загорелись), нужно подать сигнал RES
(подать на вход низкий уровень на время TRES)/
*/

#define RNB_RUE_DIFF 10
#define LED_ONCE 0x01
#define LED_CIRC 0x02
#define LED_RUE_LEFT 0x02
#define LED_RUE_RIGHT 0x01
#define LED_RUE_NO 0x00
#define LED_RNB_OFF 0x00
#define LED_RNB_LR 0x01
#define LED_RNB_RL 0x02
#define LED_RNB_ALL 0x03
#define LED_FIRE_ALL 0x04
#define LED_FIRE_IND 0x05
#define LED_COAL_IND 0x06
#define LED_FIREC_ALL 0x07
#define LED_FIREC_IND 0x08
#define LED_FIRE_TM 200
#define LED_RNB_TM 200
#define LED_COAL_COEF 7

#define WS_TRANSFER_READY 0
#define WS_TRANSFER_WORK 1
//Параметры для ws28xx диодов
//19 - 14chn
//38 - 38chn
//#define WS_H0 19-1
//#define WS_H1 38-1 

#define WS_H0 15
#define WS_H1 30

#define WS_LED_COUNT 32
#define WS_RES_VAL 64
#define WS_BITS_PER_COLOR 8
//#define LED_RGBW
#ifdef LED_RGBW
	#define WS_COLORS_PER_PIX 4
#else
	#define WS_COLORS_PER_PIX 3
#endif
#define WS_COUNT_BITS WS_LED_COUNT*WS_COLORS_PER_PIX*WS_BITS_PER_COLOR+WS_RES_VAL

//Определение цветового пр-ва для пользователя
//#define HSV_COLOR
#define LED_HW_LENGTH WS_LED_COUNT*24+WS_RES_VAL
//Параметры для HSV
#define WS_HSV_RED    0
#define WS_HSV_GREEN  120
#define WS_HSV_BLUE   240
#define WS_HSV_YELLOW 60
#define WS_HSV_CYAN   180
#define WS_HSV_MAGENT 300
#define WS_HSV_ORANGE 19
#define WS_HSV_MAX_BRIGHT 100
#define WS_HSV_MAX_SAT 100
#define WS_HSV_COLORS 359
#define WS_HSV_WHITE 361
#define COUNT_LED_EFF 8
//Параметры для RGB
#define WS_RGB_MAX_VAL 255
#define WS_RGB_BRIGHT_MASK 0xff000000
#define WS_RGB_RED_MASK 0x00ff0000
#define WS_RGB_GREEN_MASK 0x0000ff00
#define WS_RGB_BLUE_MASK 0x000000ff
#define WS_RGB_RED 768
#define WS_RGB_GREEN 769
#define WS_RGB_BLUE 770
#define WS_RGB_YELLOW 771
#define WS_RGB_CYAN 772
#define WS_RGB_MAGENT 773
#define WS_RGB_WHITE 774
#define WS_MAX_BRIGHT 21
#define WS_RGB_COLORS 1535
#define WS_ENCOCOLOR_VAL 360
#define WS_ENCO_VAL WS_ENCOCOLOR_VAL+COUNT_LED_EFF
//774 for768
//Общие
#ifdef HSV_COLOR
	#define WS_MAX_COLOR_NUM WS_HSV_COLORS
	#define WS_RED WS_HSV_RED 
	#define WS_GREEN WS_HSV_GREEN 
	#define WS_BLUE WS_HSV_BLUE 
	#define WS_WHITE WS_HSV_WHITE 
	#define WS_MAGENT WS_HSV_MAGENT 
	#define WS_CYAN WS_HSV_CYAN 
	#define WS_YELLOW WS_HSV_YELLOW
	#define WS_ORANGE WS_HSV_ORANGE 
	#define LED_COLOR_CIRC WS_MAX_COLOR_NUM
#else
	#define WS_MAX_COLOR_NUM WS_RGB_COLORS
	#define WS_RED WS_RGB_RED 
	#define WS_GREEN WS_RGB_GREEN 
	#define WS_BLUE WS_RGB_BLUE 
	#define WS_WHITE WS_RGB_WHITE 
	#define WS_MAGENT WS_RGB_MAGENT 
	#define WS_CYAN WS_RGB_CYAN 
	#define WS_YELLOW WS_RGB_YELLOW
	#define WS_ORANGE WS_RGB_ORANGE 
	#define LED_COLOR_CIRC 767
#endif

#define MAX_EFF_BRIGHT 8

/*
для sk6812 rgbw
#define WS_COLORS_PER_PIX 4



*/

extern const unsigned char hsvBright[9+1];
struct HwRgbPix
{
	unsigned char Grn[8];
	unsigned char Red[8];
	unsigned char Bl[8];
};

struct RgbColor
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char W;
	unsigned char BR;//Bright when use rgb model,NOT used when use HSV
};

struct HwRgbwPix
{
	unsigned char Grn[8];
	unsigned char Red[8];
	unsigned char Bl[8];
	unsigned char Wht[8];
};

struct RgbwColor
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char W;
	unsigned char BR;//Bright when use rgb model,NOT used when use HSV
};

struct HsvColor
{
	unsigned short int H;
	unsigned char S;
	unsigned char V;
};


struct LedHwPack
{
	//Значения цветов 24байта * WS_LED_COUNT
	struct HwRgbPix ledHwPix[WS_LED_COUNT];
	//Res
	unsigned char res[WS_RES_VAL];
};

struct LedHwPackw
{
	//Значения цветов 24байта * WS_LED_COUNT
	struct HwRgbwPix ledHwPix[WS_LED_COUNT];
	//Res
	unsigned char res[WS_RES_VAL];
};
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//void setRGBColor768All(unsigned short int rgb);
//void setRGBColor768(unsigned char led,unsigned short int rgb);
//void setRGBBright(unsigned char led, char bright);
//void setRGBBrightAll(char bright);
void setLedAutoBright(unsigned short bright);
void setRGBColorHD(unsigned char led,unsigned short int rgb);
void convertToCurrColorHD(unsigned short int val, struct RgbColor *rgb);
void setLedColor(unsigned char led,unsigned short color);
void setLedBright(unsigned char led, unsigned char bright);
void setLedSaturation(unsigned char led, unsigned char sat);
void setAllLedsBright(unsigned char bright);
void setAllLedsColor(unsigned short int color);
void setAllLedsSat(unsigned char sat);
void setRGBColorCh(unsigned char led, unsigned char r,unsigned char g, unsigned char b,unsigned char w);
void setLedMode(unsigned char mode);
void onLedRainbow(unsigned char time,unsigned char rue, unsigned char rueDiff);
void ledRainbow(unsigned short int time,unsigned char rue, unsigned char rueDiff);
void ledFireEff(unsigned short int time);
void ledEff(unsigned char eff);
//void refreshWsLed(unsigned char led, struct RgbColor *rgb);
//void refreshWsLedRgbBr(unsigned char led,struct RgbColor *rgb);
void initWs2812Buff(void);
void setRGBWhiteAll(unsigned char val);
#endif //WS2812_H
