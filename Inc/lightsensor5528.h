/*
 * lightsensor5528.h
 *
 *  Created on: Apr 19, 2023
 *      Author: vit
 */

#ifndef LIGHTSENSOR5528_H_
#define LIGHTSENSOR5528_H_
#include "adc.h"
#include "resistanceconverter.h"

class LightSensor5528 {

	AnalogConverter* dataSource;
	ResistanceConverter* Rconverter { nullptr };
	AnalogConverter::Channels ADCChannel { AnalogConverter::ADC_0 };
	int32_t maxValue { 21 };
	int32_t lastValue { 0 };
	int32_t sensorDarkResistance { 1000000 };
	int32_t lightDiscrete { 1 };

public:
	LightSensor5528(uint32_t darkResistance  = 1000000, AnalogConverter* source = nullptr, AnalogConverter::Channels channel = AnalogConverter::ADC_0);
	enum sensorStates : int8_t  { VALUE_NOT_CHANGED = -1, SENSOR_ERROR = -2 };
	int32_t getSensorData();
	int32_t getValue(bool checkChanging = true)
	{
		int32_t value = getSensorData()/lightDiscrete;
		if( checkChanging)
		{
			if(lastValue!=value)
			{
				lastValue = value;
				return maxValue - value;
			}
			else return VALUE_NOT_CHANGED;
		}
		else
		{
			lastValue = value;
			return maxValue - value;
		}
	}
	virtual ~LightSensor5528();
};

#endif /* LIGHTSENSOR5528_H_ */
