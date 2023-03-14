/*
 * ledpairs.cpp
 *
 *  Created on: Mar 10, 2023
 *      Author: vit
 */

#include <ledpairs.h>
#include "SmartPixelStrip.h"

LedPairs::LedPairs(SmartPixelStrip* strip) {
	// TODO Auto-generated constructor stub
	ledStrip = strip;
	count = ledStrip->count()/2;

	pairs = new LedPair[count];

}

void LedPairs::setPair(uint16_t id, uint16_t idLeft, uint16_t idRight)
{
	pairs[id].createPair(ledStrip->getLed(idLeft), ledStrip->getLed(idRight));
}

LedPair* LedPairs::getPair(uint16_t id)
{
	if(id<count) { return &pairs[id]; }
}



void LedPairs::refresh()
{
	ledStrip->refresh();
}
LedPairs::~LedPairs() {
	// TODO Auto-generated destructor stub
}

