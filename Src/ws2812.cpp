#include <string.h>
#include "ws2812.h"
#include <math.h>
#include <stdint.h>
#include "init.h"
/*Управление светодиодами ws

setLedColor(led,color)
setLedBright(led,bright)
setLedSat(led,sat)

timer3Start();
startWSDMA();

*/
//Буфер сигналов протокола ws
struct LedHwPack hwBuff;
struct LedHwPackw hwBuffw;
char effBrightCoef = 0;
char brightAllLed = 0;
const unsigned char wsHw[2] = {WS_H0,WS_H1};
const unsigned char hsvBright[10] = {0,10,20,30,40,50,60,70,80,90};//Яркость для HSV
extern unsigned char ledEffType;
const unsigned short int rgbBright[WS_MAX_BRIGHT+1] = {0, 9, 12, 14, 18, 22, 35, 44, 55, 68,
85, 107, 134, 167, 209, 262, 327, 410, 512, 640, 800, 1000};
//const float rgbBright[WS_MAX_BRIGHT+1] = {0, 0.009276, 0.011576, 0.014456, 0.018056, 0.022556, 0.035216, 0.044016, 0.055016, 0.068756,
//0.085936, 0.10742, 0.134426, 0.16782, 0.20976, 0.26218, 0.32768, 0.4096, 0.512, 0.64, 0.8, 0.99};

//const float rgbBright[WS_MAX_BRIGHT+1] ={0,0.014456, 0.022556, 0.044016, 0.068756, 0.10742, 0.16782, 0.4096, 0.64, 0.99};// {0,0.01,0.03,0.05,0.1,0.3,0.4,0.5,0.7,0.9};//Яркость для RGB
const unsigned char hsvSat[9+1] = {0,10,20,30,40,50,60,70,80,90};//Насыщенность HSV
struct HsvColor ledHsvColor[WS_LED_COUNT];//Структура HSV
struct RgbColor ledRgbColor[WS_LED_COUNT];//Структура RGB
#ifdef LED_RGBW
const uint32_t* hwLedAdress = (uint32_t*)&hwBuffw;
#else
const uint32_t* hwLedAdress = (uint32_t*)&hwBuff;
#endif

#define FR_CF_LED_CNT 62
			
unsigned short int colors[WS_ENCO_VAL] =
{	
0,4, 8, 12, 17, 21, 25, 29, 34, 38, 42, 46, 51, 55, 59, 63, 68, 72, 76, 81, 85,89, 93, 98, 102, 106, 
110, 115, 119, 123, 127, 132, 136, 140, 145, 149, 153, 157, 162, 166, 170, 174, 179, 183, 187, 191, 196, 200, 204, 209, 213, 
217, 221, 226, 230, 234, 238, 243, 247, 251, 255, 260, 264, 268, 273, 277, 281, 285, 290, 294, 298, 302, 307, 311, 315, 319, 324, 328, 332, 337, 341, 
345, 349, 354, 358, 362, 366, 371, 375, 379, 383, 388, 392, 396, 401, 405, 409, 413, 418, 422, 426, 430, 435, 439, 443, 447, 452, 456, 460, 465, 469, 
473, 477, 482, 486, 490, 494, 499, 503, 507, 511, 516, 520, 524, 529, 533, 537, 541, 546, 550, 554, 558, 563, 567, 571, 575, 580, 584, 588, 593, 597, 
601, 605, 610, 614, 618, 622, 627, 631, 635, 639, 644, 648, 652, 657, 661, 665, 669, 674, 678, 682, 686, 691, 695, 699, 703, 708, 712, 716, 721, 725, 
729, 733, 738, 742, 746, 750, 755, 759, 763, 767, 772, 776, 780, 785, 789, 793, 797, 802, 806, 810, 
814, 819, 823, 827, 831, 836, 840, 844, 849, 853, 857, 861, 866, 870, 874, 878, 883, 887, 891, 895, 
900, 904, 908, 913, 917, 921, 925, 930, 934, 938, 942, 947, 951, 955, 959, 964, 968, 972, 977, 981, 
985, 989, 994, 998, 1002, 1006, 1011, 1015, 1019, 1023, 1028, 1032, 1036, 1041, 1045, 1049, 1053, 1058, 1062, 1066, 
1070, 1075, 1079, 1083, 1087, 1092, 1096, 1100, 1105, 1109, 1113, 1117, 1122, 1126, 1130, 1134, 1139, 1143, 1147, 1151, 
1156, 1160, 1164, 1169, 1173, 1177, 1181, 1186, 1190, 1194, 1198, 1203, 1207, 1211, 1215, 1220, 1224, 1228, 1233, 1237, 
1241, 1245, 1250, 1254, 1258, 1262, 1267, 1271, 1275, 1279, 1284, 1288, 1292, 1297, 1301, 1305, 1309, 1314, 1318, 1322, 
1326, 1331, 1335, 1339, 1343, 1348, 1352, 1356, 1361, 1365, 1369, 1373, 1378, 1382, 1386, 1390, 1395, 1399, 1403, 1407, 1412, 1416, 1420, 1425, 1429, 1433, 1437, 1442, 1446, 1450, 
1454, 1459, 1463, 1467, 1471, 1476, 1480, 1484, 1489, 1493, 1497, 1501, 1506, 1510, 1514, 1518, 1523, 1527, 1531, 
1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543};

char ledFArrC[FR_CF_LED_CNT] = {10,10,11,12,11,10,10,10,10,10,10,10,10,10,10,9,9,9,8,8,7,8,9,10,11,11,10,10,10,
																10,9,10,10,10,10,10,10,10,10,10,10,11,10,11,10,10,10,11,12,12,11,10,10,10,9,10,10,10,
																11,10,10,10};
	
/*char ledFArrB[FR_CF_LED_CNT] = {5,6,8,9,8,7,6,5,5,6,5,5,7,6,5,5,4,4,3,4,4,5,5,6,7,7,6,6,5,
																5,4,5,6,5,6,6,5,5,6,6,5,5,5,6,5,5,6,5,9,8,7,6,5,5,4,5,6,7,
																6,6,5,4};*/			

char ledFArrB[FR_CF_LED_CNT] = {5,6,8,9,8,7,6,5,6,4,5,3,7,6,5,4,7,4,5,4,8,6,5,4,7,8,6,7,5,
																7,4,5,6,5,6,8,5,4,9,6,4,5,7,6,7,5,6,5,9,8,7,6,5,9,4,5,6,7,
																6,7,5,4};		

//low -0 middle - 1 high - 2																
/*unsigned char ledFArrBBr[3][FR_CF_LED_CNT] = 
{
	{2,3,4,3,4,2,3,2,3,1,2,1,4,3,2,1,3,1,2,1,4,3,2,1,3,4,3,4,2,3,1,
	 2,3,4,3,4,2,1,4,3,1,2,4,3,4,2,3,2,4,3,2,3,2,4,1,2,3,4,3,2,4,1},
	
	{4,5,6,5,6,4,5,4,5,3,4,3,6,5,4,3,5,3,4,3,6,5,4,3,5,6,5,6,4,5,3,
	 4,5,6,5,6,4,3,6,5,3,4,6,5,6,4,5,4,6,5,4,5,4,6,3,4,5,6,5,4,6,3},
		
		{7,8,9,8,9,7,8,7,8,6,7,6,9,8,7,6,8,6,7,6,9,8,7,6,8,9,8,9,7,8,6,
	 7,8,9,8,9,7,6,9,8,6,7,9,8,9,7,8,7,9,8,7,8,7,9,6,7,8,9,8,7,9,6}
};
*/
unsigned char ledFArrBBr[MAX_EFF_BRIGHT+1][FR_CF_LED_CNT] = 
{
	{4,5,6,5,6,4,5,4,5,3,4,3,6,5,4,3,5,3,4,3,6,5,4,3,5,6,5,6,4,5,3,
	 4,5,6,5,6,4,3,6,5,3,4,6,5,6,4,5,4,6,5,4,5,4,6,3,4,5,6,5,4,6,3},
		
{4,5,6,5,6,4,5,4,5,3,4,3,6,5,4,3,5,3,4,3,6,5,4,3,5,6,5,6,4,5,3,
4,5,6,5,6,4,3,6,5,3,4,6,5,6,4,5,4,6,5,4,5,4,6,3,4,5,6,5,4,6,3,},

{6,7,8,7,8,6,7,6,7,5,6,5,8,7,6,5,7,5,6,5,8,7,6,5,7,8,7,8,6,7,5,
6,7,8,7,8,6,5,8,7,5,6,8,7,8,6,7,6,8,7,6,7,6,8,5,6,7,8,7,6,8,5,},

{8,9,10,9,10,8,9,8,9,7,8,7,10,9,8,7,9,7,8,7,10,9,8,7,9,10,9,10,8,9,7,
8,9,10,9,10,8,7,10,9,7,8,10,9,10,8,9,8,10,9,8,9,8,10,7,8,9,10,9,8,10,7,},

{10,11,12,11,12,10,11,10,11,9,10,9,12,11,10,9,11,9,10,9,12,11,10,9,11,12,11,12,10,11,9,
10,11,12,11,12,10,9,12,11,9,10,12,11,12,10,11,10,12,11,10,11,10,12,9,10,11,12,11,10,12,9,},

{12,13,14,13,14,12,13,12,13,11,12,11,14,13,12,11,13,11,12,11,14,13,12,11,13,14,13,14,12,13,11,
12,13,14,13,14,12,11,14,13,11,12,14,13,14,12,13,12,14,13,12,13,12,14,11,12,13,14,13,12,14,11,},

{14,15,16,15,16,14,15,14,15,13,14,13,16,15,14,13,15,13,14,13,16,15,14,13,15,16,15,16,14,15,13,
14,15,16,15,16,14,13,16,15,13,14,16,15,16,14,15,14,16,15,14,15,14,16,13,14,15,16,15,14,16,13,},

{16,17,18,17,18,16,17,16,17,15,16,15,18,17,16,15,17,15,16,15,18,17,16,15,17,18,17,18,16,17,15,
16,17,18,17,18,16,15,18,17,15,16,18,17,18,16,17,16,18,17,16,17,16,18,15,16,17,18,17,16,18,15,},

{19,20,21,20,21,19,20,19,20,18,19,18,21,20,19,18,20,18,19,18,21,20,19,18,20,21,20,21,19,20,18,
19,20,21,20,21,19,18,21,20,18,19,21,20,21,19,20,19,21,20,19,20,19,21,18,19,20,21,20,19,21,18,}
};																
char ledFArrBL[FR_CF_LED_CNT] = {2,3,4,3,4,2,3,2,3,1,2,1,4,3,2,1,3,1,2,1,4,3,2,1,3,4,3,4,2,
																3,1,2,3,4,3,4,2,1,4,3,1,2,4,3,4,2,3,2,4,3,2,3,2,4,1,2,3,4,
																3,2,4,1};		
											
char ledFArrCC[FR_CF_LED_CNT] = {210,210,209,208,209,210,210,210,210,210,210,210,210,210,210,211,211,211,212,212,213,
																212,211,210,209,208,210,210,210,
																210,211,210,210,210,210,210,210,210,210,210,210,209,
																210,209,210,210,210,209,208,208,209,210,210,210,211,210,210,210,
																209,210,210,210};		

unsigned char ledMode = LED_ONCE;
//Обновление буфера из формата rgb при использовании rgb
void setHwRgbBr(unsigned char led,struct RgbColor *rgb)
{
	register unsigned char i = 0;
	unsigned char mask = 0x80;
	while(i<8)
	{
		if(((char)((rgb[led].B*rgbBright[rgb[led].BR])/1000)&mask) == mask)
			hwBuff.ledHwPix[led].Bl[i] = WS_H1;
		else hwBuff.ledHwPix[led].Bl[i] = WS_H0;
		
		if(((char)((rgb[led].R*rgbBright[rgb[led].BR])/1000)&mask) == mask) 
			hwBuff.ledHwPix[led].Red[i] = WS_H1;
		else hwBuff.ledHwPix[led].Red[i] = WS_H0;
		
		if(((char)((rgb[led].G*rgbBright[rgb[led].BR])/1000)&mask) == mask)
			hwBuff.ledHwPix[led].Grn[i] = WS_H1;
		else hwBuff.ledHwPix[led].Grn[i] = WS_H0;
		
		mask = mask>>1;
		i++;
	}
}

//Обновление буфера из формата rgb при использовании hsv
void setHwRgb(unsigned char led, struct RgbColor *rgb)
{
	register unsigned char i = 0;
	unsigned char mask = 0x80;
	while(i<8)
	{
		if((rgb[led].B&mask) == mask) hwBuff.ledHwPix[led].Bl[i] = WS_H1;
		else hwBuff.ledHwPix[led].Bl[i] = WS_H0;
		
		if((rgb[led].R&mask) == mask) hwBuff.ledHwPix[led].Red[i] = WS_H1;
		else hwBuff.ledHwPix[led].Red[i] = WS_H0;
		
		if((rgb[led].G&mask) == mask) hwBuff.ledHwPix[led].Grn[i] = WS_H1;
		else hwBuff.ledHwPix[led].Grn[i] = WS_H0;
		mask = mask>>1;
		i++;
	}
}

//Обновление буфера из формата rgb при использовании rgb rgbw
void setHwRgbwBr(unsigned char led,struct RgbColor *rgbw)
{
	register unsigned char i = 0;
	unsigned char mask = 0x80;
	while(i<8)
	{
		if(((char)((rgbw[led].B*rgbBright[rgbw[led].BR])/1000)&mask) == mask)
			hwBuffw.ledHwPix[led].Bl[i] = WS_H1;
		else hwBuffw.ledHwPix[led].Bl[i] = WS_H0;
		
		if(((char)((rgbw[led].R*rgbBright[rgbw[led].BR])/1000)&mask) == mask)
			hwBuffw.ledHwPix[led].Red[i] = WS_H1;
		else hwBuffw.ledHwPix[led].Red[i] = WS_H0;
		
		if(((char)((rgbw[led].G*rgbBright[rgbw[led].BR])/1000)&mask) == mask)
			hwBuffw.ledHwPix[led].Grn[i] = WS_H1;
		else hwBuffw.ledHwPix[led].Grn[i] = WS_H0;
		
		if(((char)((rgbw[led].W*rgbBright[rgbw[led].BR])/1000)&mask) == mask)
			hwBuffw.ledHwPix[led].Wht[i] = WS_H1;
		else hwBuffw.ledHwPix[led].Wht[i] = WS_H0;
		mask = mask>>1;
		i++;
	}
}

//Обновление буфера из формата rgb при использовании hsv rgbw 
void setHwRgbw(unsigned char led, struct RgbColor *rgbw)
{
	register unsigned char i = 0;
	unsigned char mask = 0x80;
	while(i<8)
	{
		if((rgbw[led].B&mask) == mask) hwBuffw.ledHwPix[led].Bl[i] = WS_H1;
		else hwBuffw.ledHwPix[led].Bl[i] = WS_H0;
		
		if((rgbw[led].R&mask) == mask) hwBuffw.ledHwPix[led].Red[i] = WS_H1;
		else hwBuffw.ledHwPix[led].Red[i] = WS_H0;
		
		if((rgbw[led].G&mask) == mask) hwBuffw.ledHwPix[led].Grn[i] = WS_H1;
		else hwBuffw.ledHwPix[led].Grn[i] = WS_H0;
		
		if((rgbw[led].W&mask) == mask) hwBuffw.ledHwPix[led].Wht[i] = WS_H1;
		else hwBuffw.ledHwPix[led].Wht[i] = WS_H0;
		mask = mask>>1;
		i++;
	}
}


//Преобразует целое число 0-770 - в цвет RGB
void convertToCurrColor(unsigned short int val, struct RgbColor *rgb)
{
		if(val<=255){rgb->R = 255 - val;rgb->G = val;rgb->B = 0;}//red to green
		else if((val>255)&&(val<=511)){rgb->G = 255 - (val - 256);rgb->B = val - 256;rgb->R = 0;}//Green to Blue
		else if((val>511)&&(val<=767)){rgb->B = 255 - (val-512); rgb->R = val-512;rgb->G = 0;}//Blue To Red
		else 
		{
			switch(val)
			{
				case WS_RGB_RED:{rgb->R = 255;rgb->G = 0;rgb->B = 0;break;}
				case WS_RGB_GREEN:{rgb->R = 0;rgb->G = 255;rgb->B = 0;break;}
				case WS_RGB_BLUE:{rgb->R = 0;rgb->G = 0;rgb->B = 255;break;}
				case WS_RGB_YELLOW:{rgb->R = 255;rgb->G = 255;rgb->B = 0;break;}
				case WS_RGB_MAGENT:{rgb->R = 255;rgb->G = 0;rgb->B = 255;break;}
				case WS_RGB_CYAN:{rgb->R = 0;rgb->G = 255;rgb->B = 255;break;}
				case WS_RGB_WHITE:{rgb->R = 255;rgb->G = 255;rgb->B = 255;break;}
				default:break;
			}
		}				
}

//rgb by int (xxRRGGBB)
void setRGBColor(unsigned char led,int rgb)
{
	char r = (rgb&WS_RGB_RED_MASK)>>16;
	char g = (rgb&WS_RGB_GREEN_MASK)>>8;
	char b = (rgb&WS_RGB_BLUE_MASK);
	ledRgbColor[led].R = r;
	ledRgbColor[led].G = g;
	ledRgbColor[led].B = b;
#ifdef LED_RGBW
	setHwRgbwBr(led,ledRgbColor);
#else
	setHwRgbBr(led,ledRgbColor);
#endif
}

//rgb by short int(768 type)
void setRGBColor768(unsigned char led,unsigned short int rgb)
{
	convertToCurrColor(rgb,&ledRgbColor[led]);
#ifdef LED_RGBW
	setHwRgbwBr(led,ledRgbColor);
#else
	setHwRgbBr(led,ledRgbColor);
#endif
}
//rgb by short int(768 type)
void setRGBColorHD(unsigned char led,unsigned short int rgb)
{
	convertToCurrColorHD(rgb,&ledRgbColor[led]);
#ifdef LED_RGBW
	setHwRgbwBr(led,ledRgbColor);
#else
	setHwRgbBr(led,ledRgbColor);
#endif
}
void setRGBColor768All(unsigned short int rgb)
{
	int i = 0;
	while(i<WS_LED_COUNT)
	{
		setRGBColor768(i,rgb);
		i++;
	}
}
void setRGBColorCh(unsigned char led, unsigned char r,unsigned char g, unsigned char b,unsigned char w)
{
	ledRgbColor[led].R = r;
	ledRgbColor[led].G = g;
	ledRgbColor[led].B = b;
#ifdef LED_RGBW
	ledRgbColor[led].W = w;
	setHwRgbwBr(led,ledRgbColor);
#else
	setHwRgbBr(led,ledRgbColor);
#endif
}
//Яркость задается индексом от 0 до 10 путем умножения rgb на соотв. коэф. в массиве rgbBright
void setRGBBright(unsigned char led, unsigned char bright)
{
	ledRgbColor[led].BR = bright;
	#ifdef LED_RGBW
	setHwRgbwBr(led,ledRgbColor);
	#else
	setHwRgbBr(led,ledRgbColor);
	#endif
}
void setRGBWhite(unsigned char led,unsigned char val)
{
	ledRgbColor[led].W = val;
	#ifdef LED_RGBW
	setHwRgbwBr(led,ledRgbColor);
	#endif
}
void setRGBWhiteAll(unsigned char val)
{
	int i = 0;
	while(i<WS_LED_COUNT)
	{
		setRGBWhite(i,val);
		i++;
	}
}
void setRGBBrightAll(unsigned char bright)
{
	int i = 0;
	while(i<WS_LED_COUNT)
	{
		setRGBBright(i,bright);
		i++;
	}
		
}
//Начальная инициализация буфера
void initWs2812Buff()
{
	register unsigned char i = 0;
	memset(hwBuff.res,0,WS_RES_VAL);

	i = 0;
	while(i<WS_LED_COUNT)
	{
		register unsigned char j = 0;
		while(j<8)
		{
			hwBuff.ledHwPix[i].Bl[j] = WS_H0;
			hwBuff.ledHwPix[i].Grn[j] = WS_H0;
			hwBuff.ledHwPix[i].Red[j] = WS_H0;
			j++;
		}
		i++;
	}
}



//Конвертация hsv rgb
void hsvToRgb(struct  RgbColor* rgb, struct HsvColor*  hsv)
{
        int h = hsv->H/60;
        float hd = fmod(hsv->H,60)/60.0;
        float vm = ((100.0-hsv->S)*hsv->V)/100.0;
        float a = (hsv->V-vm)*(hd);
        float vi = vm+a;
        float vd = hsv->V-a;
        float r,g,b;
        switch(h)
        {
            case 0:{r = hsv->V;g = vi;b = vm;break;}
            case 1:{r = vd;g = hsv->V;b = vm;break;}
            case 2:{r = vm;g =hsv->V;b = vi;break;}
            case 3:{r = vm;g = vd;b = hsv->V;break;}
            case 4:{r = vi;g = vm;b = hsv->V;break;}
            case 5:{r = hsv->V;g = vm;b = vd;break;}
        }
        rgb->R = (r/100)*255;
        rgb->G = (g/100)*255;
        rgb->B = (b/100)*255;
}

//Настройка яркости hsv
void setHsvLedBright(unsigned char led,unsigned char bright)
{
	//if(bright<=WS_HSV_MAX_BRIGHT)
		ledHsvColor[led].V = hsvBright[bright];
	//else
		//ledHsvColor[led].V = WS_HSV_MAX_BRIGHT;
}
//Настройка цвета hsv
void setHsvLedHue(unsigned char led,unsigned short int hue)
{
	if(hue<=WS_HSV_COLORS)
		ledHsvColor[led].H = hue;
	else
		ledHsvColor[led].H = WS_HSV_COLORS;
}
//Настройка насыщенности hsv
void setHsvLedSat(unsigned char led,unsigned char sat)
{
	if(sat<=WS_HSV_MAX_SAT)
		ledHsvColor[led].S = sat;
	else
		ledHsvColor[led].S = WS_HSV_MAX_SAT;
}

void setLedColor(unsigned char led,unsigned short color)
{
#ifdef HSV_COLOR
	setHsvLedHue(led,color);
	hsvToRgb(&ledRgbColor[led],&ledHsvColor[led]);
	#ifdef LED_RGBW
	setHwRgbw(led,ledRgbColor);
	#else
	setHwRgb(led,ledRgbColor);
	#endif
#else
	setRGBColorHD(led,color);
#endif
}
void setLedAutoBright(unsigned short int bright)
{
	static unsigned short prevBrStd = 0, prevBrEff = 0;
	unsigned short int brEff, brStd;
		if((ledEffType == LED_FIRE_IND)||(ledEffType == LED_FIREC_ALL)||(ledEffType == LED_FIRE_ALL)||(ledEffType == LED_COAL_IND))
		{
			brEff = bright/7; 
			if(brEff != prevBrEff)
			{
				effBrightCoef = brEff;
				prevBrEff = brEff;
				if(effBrightCoef == 0) effBrightCoef = 1;
			}
		}
		else
		{
			brStd = bright/3;
			if(brStd<1)brStd = 1; if(brStd>21)brStd = 21;
			if(brStd!=prevBrStd){ prevBrStd = brStd; setAllLedsBright(brStd); if(ledMode == LED_ONCE)startWsTransfer(WS_COUNT_BITS);}
		}
}
void setLedBright(unsigned char led, unsigned char bright)
{
#ifdef HSV_COLOR
	setHsvLedBright(led,bright);
	hsvToRgb(&ledRgbColor[led],&ledHsvColor[led]);
	#ifdef LED_RGBW
		setHwRgbw(led,ledRgbColor);
	#else
		setHwRgb(led,ledRgbColor);
	#endif
#else
	setRGBBright(led,bright);
#endif
}

void setLedSaturation(unsigned char led, unsigned char sat)
{
	#ifdef HSV_COLOR
	setHsvLedSat(led,sat);
	hsvToRgb(&ledRgbColor[led],&ledHsvColor[led]);
	#ifdef LED_RGBW
	setHwRgbw(led,ledRgbColor);
	#else
	setHwRgb(led,ledRgbColor);
	#endif
	#endif
}

void setAllLedsBright(unsigned char bright)
{
	int i = 0;
	brightAllLed = bright;
	while(i<WS_LED_COUNT){setLedBright(i,bright);i++;}
	if(ledMode == LED_ONCE) startWsTransfer(WS_COUNT_BITS);
}

void setAllLedsColor(unsigned short int color)
{
	int i = 0;
	while(i<WS_LED_COUNT){setLedColor(i,color);i++;}
	if(ledMode == LED_ONCE) startWsTransfer(WS_COUNT_BITS);
}

void setAllLedsSat(unsigned char sat)
{
	int i = 0;
	while(i<WS_LED_COUNT){setLedSaturation(i,sat);i++;}
}


void setLedMode(unsigned char mode)
{
	ledMode = mode;
	if(ledMode == LED_CIRC)
	{
		wsDmaTmCirc();
	}
	else
	{
		wsDmaTmOnce();
	}
}
void onLedRainbow(unsigned char time,unsigned char rue, unsigned char rueDiff)
{
}

void convertToCurrColorHD(unsigned short int val, struct RgbColor *rgb)
{
	
	if(val<=255){rgb->R = 255; rgb->G = val; rgb->B = 0;}
	if((val>255)&&(val<512)){rgb->R = 511-val; rgb->G = 255; rgb->B = 0;}
	if((val>=512)&&(val<768)){rgb->R = 0; rgb->G = 255; rgb->B = val-512;}
	if((val>=768)&&(val<1024)){rgb->R =0; rgb->G = 1023-val; rgb->B = 255;}
	if((val>=1024)&&(val<1280)){rgb->R =val-1024; rgb->G = 0; rgb->B = 255;}
	if((val>=1280)&&(val<1536)){rgb->R = 255; rgb->G = 0; rgb->B = 1535-val;}
}
void ledFireEff(unsigned short int time)
{
	static unsigned short int fireCntr = 0;
	static unsigned short int i = 0,j= 12,k = 7,l = 9,m = 34, n = 22;
	if(fireCntr>=time)
	{
		//setAllLedsColor(ledFArrC[i]);
		//setAllLedsBright(ledFArrB[i]);
	/*	setLedColor(0,ledFArrC[i]-2);
		setLedBright(0,ledFArrB[i]-2);
		setLedColor(5,ledFArrC[i]-2);
		setLedBright(5,ledFArrB[i]-2);
		
		setLedColor(1,ledFArrC[i]-1);
		setLedBright(1,ledFArrB[i]-1);
		setLedColor(4,ledFArrC[i]-1);
		setLedBright(4,ledFArrB[i]-1);
		
		setLedColor(2,ledFArrC[i]);
		setLedBright(2,ledFArrB[i]);
		setLedColor(3,ledFArrC[i]);
		setLedBright(3,ledFArrB[i]);
		*/
		
		setLedBright(0,ledFArrBBr[effBrightCoef][i]);setLedColor(0,ledFArrC[i]-LED_COAL_COEF);
		setLedBright(1,ledFArrBBr[effBrightCoef][j]);setLedColor(1,ledFArrC[j]-LED_COAL_COEF);
		setLedBright(2,ledFArrBBr[effBrightCoef][k]);setLedColor(2,ledFArrC[k]-LED_COAL_COEF);
		setLedBright(3,ledFArrBBr[effBrightCoef][l]);setLedColor(3,ledFArrC[l]-LED_COAL_COEF);
		setLedBright(4,ledFArrBBr[effBrightCoef][m]);setLedColor(4,ledFArrC[m]-LED_COAL_COEF);
		setLedBright(5,ledFArrBBr[effBrightCoef][n]);setLedColor(5,ledFArrC[n]-LED_COAL_COEF);
		
		i++;j++;k++;l++;m++;n++;
		if(i>=FR_CF_LED_CNT)i = 0;
		if(j>=FR_CF_LED_CNT)j = 0;
		if(k>=FR_CF_LED_CNT)k = 0;
		if(l>=FR_CF_LED_CNT)l = 0;
		if(m>=FR_CF_LED_CNT)m = 0;
		if(n>=FR_CF_LED_CNT)n = 0;
		
		
		
		fireCntr = 0;
	}
	fireCntr++;
}
void ledFireAll()
{
	static unsigned short int fireCntr = 0;
	static unsigned short int i = 0;
	if(fireCntr>=LED_FIRE_TM)
	{	
		
		unsigned char c1 = 1,c2 = 2;
		if(ledFArrBBr[effBrightCoef][i] == 1){c1 = 0; c2 = 0;}else {c1 = 1;c2 = 2;}
		
		setLedBright(0,ledFArrBBr[effBrightCoef][i]-c2); setRGBColorCh(0,0xff,0x1e,0x00,0);//setLedColor(0,ledFArrC[i]-8+100);
		setLedBright(1,ledFArrBBr[effBrightCoef][i]-c1);setRGBColorCh(1,0xff,0x22,0x03,0);//setLedColor(1,ledFArrC[i]-4+100);
		setLedBright(2,ledFArrBBr[effBrightCoef][i]); setRGBColorCh(2,0xff,0x36,0x05,0);//setLedColor(2,ledFArrC[i]+100);
		setLedBright(3,ledFArrBBr[effBrightCoef][i]);setRGBColorCh(3,0xff,0x36,0x05,0);//setLedColor(3,ledFArrC[i]+100);
		setLedBright(4,ledFArrBBr[effBrightCoef][i]-c1);setRGBColorCh(4,0xff,0x22,0x03,0);//setLedColor(4,ledFArrC[i]-4+100);
		setLedBright(5,ledFArrBBr[effBrightCoef][i]-c2);setRGBColorCh(5,0xff,0x1e,0x00,0);//setLedColor(5,ledFArrC[i]-8+100);
		
		i++;
		if(i>=FR_CF_LED_CNT)i = 0;

		fireCntr = 0;
	}
	fireCntr++;
}
void ledFireInd()
{
	static unsigned short int fireCntr = 0;
	static unsigned short int i = 0,j= 12,k = 7,l = 9,m = 34, n = 22;
	if(fireCntr>=LED_FIRE_TM)
	{	
		setLedBright(0,ledFArrBBr[effBrightCoef][i]); setRGBColorCh(0,0xff-ledFArrC[i],0x22,0x03-ledFArrC[i]/10,0);//setLedColor(4,ledFArrC[i]-4+100);
		//setLedColor(0,ledFArrC[i]+50);
		setLedBright(1,ledFArrBBr[effBrightCoef][j]); setRGBColorCh(1,0xff-ledFArrC[j],0x22,0x03-ledFArrC[j]/10,0);
		//setLedColor(1,ledFArrC[j]+50);
		setLedBright(2,ledFArrBBr[effBrightCoef][k]); setRGBColorCh(2,0xff-ledFArrC[k],0x22,0x03-ledFArrC[k]/10,0);
		//setLedColor(2,ledFArrC[k]+50);
		setLedBright(3,ledFArrBBr[effBrightCoef][l]); setRGBColorCh(3,0xff-ledFArrC[l],0x22,0x03-ledFArrC[l]/10,0);
		//setLedColor(3,ledFArrC[l]+50);
		setLedBright(4,ledFArrBBr[effBrightCoef][m]); setRGBColorCh(4,0xff-ledFArrC[m],0x22,0x03-ledFArrC[m]/10,0);
		//setLedColor(4,ledFArrC[m]+50);
		setLedBright(5,ledFArrBBr[effBrightCoef][n]); setRGBColorCh(5,0xff-ledFArrC[n],0x22,0x03-ledFArrC[n]/10,0);
		//setLedColor(5,ledFArrC[n]+50);
		
		i++;j++;k++;l++;m++;n++;
		if(i>=FR_CF_LED_CNT)i = 0;
		if(j>=FR_CF_LED_CNT)j = 0;
		if(k>=FR_CF_LED_CNT)k = 0;
		if(l>=FR_CF_LED_CNT)l = 0;
		if(m>=FR_CF_LED_CNT)m = 0;
		if(n>=FR_CF_LED_CNT)n = 0;
		
		fireCntr = 0;
	}
	fireCntr++;
}

void ledFireColdInd()
{
	static unsigned short int fireCntr = 0;
	static unsigned short int i = 0,j= 12,k = 7,l = 9,m = 34, n = 22;
	if(fireCntr>=LED_FIRE_TM)
	{	
		setLedBright(0,ledFArrBBr[effBrightCoef][i]);setLedColor(0,ledFArrCC[i]+650);
		setLedBright(1,ledFArrBBr[effBrightCoef][j]);setLedColor(1,ledFArrCC[j]+650);
		setLedBright(2,ledFArrBBr[effBrightCoef][k]);setLedColor(2,ledFArrCC[k]+650);
		setLedBright(3,ledFArrBBr[effBrightCoef][l]);setLedColor(3,ledFArrCC[l]+650);
		setLedBright(4,ledFArrBBr[effBrightCoef][m]);setLedColor(4,ledFArrCC[m]+650);
		setLedBright(5,ledFArrBBr[effBrightCoef][n]);setLedColor(5,ledFArrCC[n]+650);
		
		i++;j++;k++;l++;m++;n++;
		if(i>=FR_CF_LED_CNT)i = 0;
		if(j>=FR_CF_LED_CNT)j = 0;
		if(k>=FR_CF_LED_CNT)k = 0;
		if(l>=FR_CF_LED_CNT)l = 0;
		if(m>=FR_CF_LED_CNT)m = 0;
		if(n>=FR_CF_LED_CNT)n = 0;
		
		fireCntr = 0;
	}
	fireCntr++;
}

void ledFireColdAll()
{
	static unsigned short int fireCntr = 0;
	static unsigned short int i = 0;
	if(fireCntr>=LED_FIRE_TM)
	{
		unsigned char c1 = 1,c2 = 2;
		if(ledFArrBBr[effBrightCoef][i] == 1){c1 = 0; c2 = 1;}else {c1 = 1;c2 = 2;}
		
		setLedBright(0,ledFArrBBr[effBrightCoef][i]-c2);setLedColor(0,ledFArrCC[i]+20+650);
		setLedBright(1,ledFArrBBr[effBrightCoef][i]-c1);setLedColor(1,ledFArrCC[i]+10+650);
		setLedBright(2,ledFArrBBr[effBrightCoef][i]);setLedColor(2,ledFArrCC[i]-20+650);
		setLedBright(3,ledFArrBBr[effBrightCoef][i]);setLedColor(3,ledFArrCC[i]-20+650);
		setLedBright(4,ledFArrBBr[effBrightCoef][i]-c1);setLedColor(4,ledFArrCC[i]+10+650);
		setLedBright(5,ledFArrBBr[effBrightCoef][i]-c2);setLedColor(5,ledFArrCC[i]+20+650);
		
		i++;
		if(i>=FR_CF_LED_CNT)i = 0;
		fireCntr = 0;
	}
	fireCntr++;
}

void ledCoalInd()
{
	static unsigned short int fireCntr = 0;
	static unsigned short int i = 0,j= 12,k = 7,l = 9,m = 34, n = 22;
	if(fireCntr>=LED_FIRE_TM)
	{	
		setLedBright(0,ledFArrBBr[effBrightCoef][i]);setLedColor(0,ledFArrC[i]-LED_COAL_COEF);
		setLedBright(1,ledFArrBBr[effBrightCoef][j]);setLedColor(1,ledFArrC[j]-LED_COAL_COEF);
		setLedBright(2,ledFArrBBr[effBrightCoef][k]);setLedColor(2,ledFArrC[k]-LED_COAL_COEF);
		setLedBright(3,ledFArrBBr[effBrightCoef][l]);setLedColor(3,ledFArrC[l]-LED_COAL_COEF);
		setLedBright(4,ledFArrBBr[effBrightCoef][m]);setLedColor(4,ledFArrC[m]-LED_COAL_COEF);
		setLedBright(5,ledFArrBBr[effBrightCoef][n]);setLedColor(5,ledFArrC[n]-LED_COAL_COEF);
		
		i++;j++;k++;l++;m++;n++;
		if(i>=FR_CF_LED_CNT)i = 0;
		if(j>=FR_CF_LED_CNT)j = 0;
		if(k>=FR_CF_LED_CNT)k = 0;
		if(l>=FR_CF_LED_CNT)l = 0;
		if(m>=FR_CF_LED_CNT)m = 0;
		if(n>=FR_CF_LED_CNT)n = 0;
		
		fireCntr = 0;
	}
	fireCntr++;
}

void ledRnbL()
{
	//time - скорость изменения цвета
	//hsv 0 -360
	//rgb 0 - 767
	static unsigned short int rnbwCntr = 0;
	static unsigned short int color = 0;
	unsigned short int maxColor = 1535;
	unsigned short int colors[6];
	rnbwCntr++;
	if(rnbwCntr>LED_RNB_TM)
	{
					colors[0] = color+RNB_RUE_DIFF;
					if(colors[0]>maxColor)colors[0] = colors[0]-maxColor;
					setLedColor(0,colors[0]);
					
					colors[1] = color+RNB_RUE_DIFF*2;
					if(colors[1]>maxColor)colors[1] = colors[1]-maxColor;
					setLedColor(1,colors[1]);
					
					colors[2] = color+RNB_RUE_DIFF*3;
					if(colors[2]>maxColor)colors[2] = colors[2]-maxColor;
					setLedColor(2,colors[2]);
					
					colors[3] = color+RNB_RUE_DIFF*4;
					if(colors[3]>maxColor)colors[3] = colors[3]-maxColor;
					setLedColor(3,colors[3]);
					
					colors[4] = color+RNB_RUE_DIFF*5;
					if(colors[4]>maxColor)colors[4] = colors[4]-maxColor;
					setLedColor(4,colors[4]);
					
					colors[5] = color+RNB_RUE_DIFF*6;
					if(colors[5]>maxColor)colors[5] = colors[5]-maxColor;
					setLedColor(5,colors[5]);	
			
	color++;
		if(color>=maxColor)color = 0;
		rnbwCntr = 0;


	}
		
	
}
void ledRnbR()
{
		//time - скорость изменения цвета
	//hsv 0 -360
	//rgb 0 - 767
	static unsigned short int rnbwCntr = 0;
	static unsigned short int color = 0;
	unsigned short int maxColor = 1535;
	unsigned short int colors[6];
	rnbwCntr++;
	if(rnbwCntr>LED_RNB_TM)
	{
					colors[0] = color+RNB_RUE_DIFF;
					if(colors[0]>maxColor)colors[0] = colors[0]-maxColor;
					setLedColor(5,colors[0]);
					
					colors[1] = color+RNB_RUE_DIFF*2;
					if(colors[1]>maxColor)colors[1] = colors[1]-maxColor;
					setLedColor(4,colors[1]);
					
					colors[2] = color+RNB_RUE_DIFF*3;
					if(colors[2]>maxColor)colors[2] = colors[2]-maxColor;
					setLedColor(3,colors[2]);
					
					colors[3] = color+RNB_RUE_DIFF*4;
					if(colors[3]>maxColor)colors[3] = colors[3]-maxColor;
					setLedColor(2,colors[3]);
					
					colors[4] = color+RNB_RUE_DIFF*5;
					if(colors[4]>maxColor)colors[4] = colors[4]-maxColor;
					setLedColor(1,colors[4]);
					
					colors[5] = color+RNB_RUE_DIFF*6;
					if(colors[5]>maxColor)colors[5] = colors[5]-maxColor;
					setLedColor(0,colors[5]);	
			
color++;
		if(color>=maxColor)color = 0;
		rnbwCntr = 0;

	}
		
		
}

void ledRnbAll()
{
	//time - скорость изменения цвета
	//hsv 0 -360
	//rgb 0 - 767
	static unsigned short int rnbwCntr = 0;
	static unsigned short int color = 0;
	unsigned short int maxColor = 1535;
	rnbwCntr++;
	if(rnbwCntr>LED_FIRE_TM)
	{
		setAllLedsColor(color);
		color++;
		if(color>=maxColor)color = 0;
		rnbwCntr = 0;
	}
}

void ledEff(unsigned char eff)
{
	switch (eff)
	{
		case LED_RNB_LR:{ledRnbL();break;}
		case LED_RNB_RL:{ledRnbR();break;}
		case LED_RNB_ALL:{ledRnbAll();break;}
		case LED_FIRE_ALL:{ledFireAll();break;}
		case LED_FIRE_IND:{ledFireInd();break;}
		case LED_COAL_IND:{ledCoalInd();break;}
		case LED_FIREC_ALL:{ledFireColdAll();break;}
		case LED_FIREC_IND:{ledFireColdInd();break;}
		default:break;
	}
	//if((eff>LED_RNB_ALL)&&(effBrightCoef))startWsTransfer();
	//if((eff<LED_FIRE_ALL)&&(brightAllLed>0))startWsTransfer();
}

/*	
void ledRainbow(unsigned short int time,unsigned char rue, unsigned char rueDiff)
{
	//time - скорость изменения цвета
	//hsv 0 -360
	//rgb 0 - 767
	static unsigned short int rnbwCntr = 0;
	static unsigned short int color = 0;
	unsigned short int maxColor = 359;
	unsigned short int colors[6];
	rnbwCntr++;
	if(rnbwCntr>time)
	{
		switch (rue)
		{
			case LED_RNB_ALL: { setAllLedsColor(color); break;}
			case LED_RNB_LR: 
				{
					colors[0] = color+rueDiff;
					if(colors[0]>maxColor)colors[0] = colors[0]-maxColor;
					setLedColor(0,colors[0]);
					
					colors[1] = color+rueDiff*2;
					if(colors[1]>maxColor)colors[1] = colors[1]-maxColor;
					setLedColor(1,colors[1]);
					
					colors[2] = color+rueDiff*3;
					if(colors[2]>maxColor)colors[2] = colors[2]-maxColor;
					setLedColor(2,colors[2]);
					
					colors[3] = color+rueDiff*4;
					if(colors[3]>maxColor)colors[3] = colors[3]-maxColor;
					setLedColor(3,colors[3]);
					
					colors[4] = color+rueDiff*5;
					if(colors[4]>maxColor)colors[4] = colors[4]-maxColor;
					setLedColor(4,colors[4]);
					
					colors[5] = color+rueDiff*6;
					if(colors[5]>maxColor)colors[5] = colors[5]-maxColor;
					setLedColor(5,colors[5]);
					break;
				}
			case LED_RNB_RL:
			{
				colors[0] = color+rueDiff;
					if(colors[0]>maxColor)colors[0] = colors[0]-maxColor;
					setLedColor(5,colors[0]);
					
					colors[1] = color+rueDiff*2;
					if(colors[1]>maxColor)colors[1] = colors[1]-maxColor;
					setLedColor(4,colors[1]);
					
					colors[2] = color+rueDiff*3;
					if(colors[2]>maxColor)colors[2] = colors[2]-maxColor;
					setLedColor(3,colors[2]);
					
					colors[3] = color+rueDiff*4;
					if(colors[3]>maxColor)colors[3] = colors[3]-maxColor;
					setLedColor(2,colors[3]);
					
					colors[4] = color+rueDiff*5;
					if(colors[4]>maxColor)colors[4] = colors[4]-maxColor;
					setLedColor(1,colors[4]);
					
					colors[5] = color+rueDiff*6;
					if(colors[5]>maxColor)colors[5] = colors[5]-maxColor;
					setLedColor(0,colors[5]);
				break;
			}
		}
		color++;
		if(color>=maxColor)color = 0;
		rnbwCntr = 0;
	}
}
*/






