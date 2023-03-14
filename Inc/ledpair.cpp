/*
 * ledpair.cpp
 *
 *  Created on: Mar 10, 2023
 *      Author: vit
 */

#include <ledpair.h>
#include "SmartPixel.h"
LedPair::LedPair(SmartPixel* left, SmartPixel* right)
{
	ledLeft = left;
	ledRight = right;
}
LedPair::LedPair()
{
	ledLeft = nullptr;//left;
	ledRight = nullptr;//right;
}
LedPair::~LedPair()
{

}
void LedPair::createPair( SmartPixel* left, SmartPixel* right)
{
	ledLeft = left;
	ledRight = right;
}
void LedPair::setColor(uint8_t r, uint8_t g, uint8_t b)
{
	ledLeft->setColor(r, g, b);
	ledRight->setColor(r, g, b);
}

void LedPair::setColor(uint8_t rl, uint8_t gl, uint8_t bl, uint8_t rr, uint8_t gr, uint8_t br)
{
	ledLeft->setColor(rl, gl, bl);
	ledRight->setColor(rr, gr, br);
}

void LedPair::setBright(uint8_t bright)
{
	ledLeft->setBright(bright);
	ledRight->setBright(bright);
}

void LedPair::setBright(uint8_t brightL, uint8_t brightR)
{
	ledLeft->setBright(brightL);
	ledRight->setBright(brightR);
}


void LedPair::setColor(uint16_t colorl, uint16_t colorr)
{
	ledLeft->setColor(colorl);
	ledRight->setColor(colorr);
}

void LedPair::setColor(uint16_t color)
{
	ledLeft->setColor(color);
	ledRight->setColor(color);
}

void LedPair::refresh()
{
	ledLeft->hwRefresh();
	ledRight->hwRefresh();
}
