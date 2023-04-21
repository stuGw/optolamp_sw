/*
 * lightsensor5528.cpp
 *
 *  Created on: Apr 19, 2023
 *      Author: vit
 */

#include "lightsensor5528.h"
#include "logger.h"
//LightSensor5528::LightSensor5528() {
//	// TODO Auto-generated constructor stub
//
//}

LightSensor5528::~LightSensor5528() {
	// TODO Auto-generated destructor stub
}

LightSensor5528::LightSensor5528(uint32_t darkResistance, AnalogConverter* source, AnalogConverter::Channels channel)
{
	dataSource = source;
	ADCChannel = channel;
	Rconverter = new ResistanceConverter();
	sensorDarkResistance = darkResistance;
	lightDiscrete = darkResistance / maxValue;
}

int32_t LightSensor5528::getSensorData()
{
	uint32_t sensorValue { 0 };
	int32_t sensorResistance { 0 };
	sensorValue = dataSource->getDataFiltered(ADCChannel);
	sensorResistance = Rconverter->getCurrentResistanceOhm(sensorValue);
	//LOG->DEBG("Sensor resistance = ",sensorResistance);
	if(sensorResistance>sensorDarkResistance) sensorResistance = sensorDarkResistance;
	return sensorResistance;

}
