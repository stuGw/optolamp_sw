/*
 * button.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: vit
 */

#include <button.h>
#include "logger.h"
Button::Button() {
	// TODO Auto-generated constructor stub

}

Button::~Button() {
	// TODO Auto-generated destructor stub
}

void Button::reset()
{
	buttonTime = 0;
	buttonStateDelay = 0;
	countFall = 0;
	countRise = 0;
	state = FREE;
}

void Button::timeIncrease()
{
	allCounter++;
	if((state ==  FREE) || (state ==  LONG) || (state ==  SINGLE) || (state ==  DOUBLE)) return;

	switch(state)
	{
	case WAIT_FALL_ACK:
	{
		buttonStateDelay++;
		if(buttonStateDelay>30)//wait timeout
		{
			if(!(*ioPort&ioMask))//check if button in low
			{
				countFall++;
				if((countFall == 2)&&(countRise)) state = DOUBLE;
				else
					state = FALLING;
				//fallTime = allCounter;
				//LOG->DEBG("set FALLED ", fallTime);
				//if(countRise)LOG->DEBG("diff = ", riseTime - fallTime);
			} else reset();
			buttonStateDelay = 0;
		}
		break;
	}

	case WAIT_RISE_ACK:
	{
		buttonStateDelay++;
		if(buttonStateDelay>30)//wait timeout
		{
			if((*ioPort&ioMask))//check if button in low
			{
				countRise++;
				state = RISING;
				//riseTime = allCounter;
				//LOG->DEBG("set RAISED ", riseTime);
				//if(countFall)LOG->DEBG("diff = ", fallTime - riseTime);
			} else reset();
			buttonStateDelay = 0;
		}
		break;
	}

	case RISING:
	{
		waitDouble++;
		if(waitDouble>doublePressDelay)
		{
			state = SINGLE;
			waitDouble = 0;
		}
		break;
	}

	case FALLING:
	{
		buttonTime++;
		if(buttonTime>longTime)
		{
			if(!(*ioPort&ioMask))
			{
				state = LONG;
			}
		}
		break;
	}
	default: break;
	}
}

void Button::fall()
{
	if(state>RISING)return;
	if(!(*ioPort&ioMask))
	{
		waitDouble = 0;
		state = WAIT_FALL_ACK;
	}
}

void Button::rise()
{
	if( (*ioPort & ioMask) && (state == FALLING))
	{
		state = WAIT_RISE_ACK;
	}
}

void Button::init(volatile uint32_t* port, uint32_t mask, uint16_t longPressTime, uint16_t delay)
{
	ioPort = port;
	ioMask = mask;
	longTime = longPressTime;
	buttonStateDelay = delay;
}

Button::State Button::getState()
{
	Button::State lst { Button::FREE };
	if(state >= SINGLE)
	{
		lst = state;
		reset();
	}
	return lst;
}
