/*
 * ledpair.h
 *
 *  Created on: Mar 10, 2023
 *      Author: vit
 */

#ifndef LEDPAIR_H_
#define LEDPAIR_H_
#include "SmartPixel.h"

class LedPair {
	SmartPixel* ledLeft { nullptr };
	SmartPixel* ledRight { nullptr };
public:
	LedPair(SmartPixel* left, SmartPixel* right);
	LedPair();
	void createPair( SmartPixel* left, SmartPixel* right);
	void setColor(uint8_t r, uint8_t g, uint8_t b);
	void setColor(uint8_t rl, uint8_t gl, uint8_t bl, uint8_t rr, uint8_t gr, uint8_t br);
	void setColor(uint16_t colorl, uint16_t colorr);
	void setColor(uint16_t color);
	void setBright(uint8_t bright);
	void setBright(uint8_t brightL, uint8_t brightR);
	inline uint16_t getColor(){ return ledLeft->getColor(); }
	uint16_t getColorL(){ return ledLeft->getColor(); }
	uint16_t getColorR(){ return ledRight->getColor(); }
	void refresh();
	virtual ~LedPair();
};

#endif /* LEDPAIR_H_ */
