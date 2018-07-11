#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_

typedef struct
{
	float rawTemp;
	int16_t temp;
	float rawRate;
	uint16_t rate;
	float rawHumidity;
	uint8_t humidity;
} environmentData;

#endif