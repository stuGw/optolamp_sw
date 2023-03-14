/*
 * ledpairs.h
 *
 *  Created on: Mar 10, 2023
 *      Author: vit
 */

#ifndef LEDPAIRS_H_
#define LEDPAIRS_H_
#include "SmartPixelStrip.h"
#include "ledpair.h"

class LedPairs {
	uint16_t count { 0 };
	SmartPixelStrip* ledStrip { nullptr };
	LedPair* pairs { nullptr };
public:
	LedPairs(SmartPixelStrip* strip);
	virtual ~LedPairs();

	void setPair(uint16_t id, uint16_t idLeft, uint16_t idRight);
	LedPair* getPair(uint16_t id);
	void refresh();
	uint16_t getCount(){ return count; }
	void setBright(uint8_t bright){ ledStrip->setBright(bright); }
	SmartPixelStrip* getStripPtr(){ return ledStrip; }
};

#endif /* LEDPAIRS_H_ */
