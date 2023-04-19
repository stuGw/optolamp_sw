/*
 * resistanceconverter.h
 *
 *  Created on: Apr 19, 2023
 *      Author: vit
 */

#ifndef RESISTANCECONVERTER_H_
#define RESISTANCECONVERTER_H_
#include <stdint.h>
class ResistanceConverter {
public:
	uint32_t R1 { 1000000 };
	uint32_t power { 3300 };
	uint16_t resolution { 4095 };
	float voltageDiscrete { 0.8059 };

	ResistanceConverter();//uint32_t R1Value = 1000000, uint32_t powerVoltage_mV = 3300, uint16_t maxResolution = 4095);
	virtual ~ResistanceConverter();
	uint32_t getCurrentResistanceOhm(uint32_t voltageADCDiscretesValue)
	{

		uint32_t ADCVoltage_mV = (float)voltageADCDiscretesValue*voltageDiscrete;
		uint32_t resistance = R1/((float)(3300-ADCVoltage_mV)/(float)ADCVoltage_mV);
		return resistance;
	}
};

#endif /* RESISTANCECONVERTER_H_ */
