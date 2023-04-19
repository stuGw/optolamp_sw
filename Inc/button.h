/*
 * button.h
 *
 *  Created on: Jun 14, 2022
 *      Author: vit
 */

#ifndef BUTTON_H_
#define BUTTON_H_
#include <stdint.h>

class Button {
	volatile uint32_t* ioPort { nullptr };
	uint32_t ioMask { 0x00000001 };
	//uint8_t state { FREE };
	static constexpr uint16_t doublePressDelay { 250 };
public:
	enum State : uint8_t { FREE = 0x00, WAIT_FALL_ACK = 0x01, FALLING = 0x02, WAIT_RISE_ACK = 0x03, RISING = 0x04, SINGLE = 0x08, LONG = 0x10, DOUBLE = 0x20 } state { FREE };

	uint8_t countRise { 0 };
	uint8_t countFall { 0 };

	uint16_t buttonTime { 0 };
	uint32_t buttonStateDelay { 0 };
	uint32_t allCounter { 0 };
	uint16_t waitDouble { 0 };
	uint16_t longTime { 1500 };

	//uint32_t riseTime { 0 };
	//uint32_t fallTime { 0 };

	void timeIncrease();
	void fall();
	void rise();
	void init(volatile uint32_t* port, uint32_t mask, uint16_t longPressTime = 1500, uint16_t delay = 30);
	void reset();
	bool isDown(){ if((*ioPort & ioMask) == 0) return true; else return false;}
	Button::State getState();
	Button();
	virtual ~Button();
};

#endif /* BUTTON_H_ */
