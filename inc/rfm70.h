#ifndef RFM70_H
#define RFM70_H

#include "spi.h"

typedef struct {
	SPI_Def spi;
	uint16_t CE_Pin;
	GPIO_TypeDef* CE_Port;
} RFM70_Def;

void Init_RFM70(RFM70_Def* rfm);

void toogleBank(RFM70_Def* rfm);
void toogleSpecFunc(RFM70_Def* rfm);

#endif
