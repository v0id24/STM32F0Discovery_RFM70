#ifndef SPI_H
#define SPI_H

#include "stm32f0xx_gpio.h"

typedef struct {
	uint16_t MISO_Pin;
	uint16_t MOSI_Pin;
	uint16_t SCK_Pin;
	uint16_t CS_Pin;
	GPIO_TypeDef* MISO_Port;
	GPIO_TypeDef* MOSI_Port;
	GPIO_TypeDef* SCK_Port;
	GPIO_TypeDef* CS_Port;
} SPI_Def;

void Init_SPI(SPI_Def* spi);
void RW_SPI_LtoM(SPI_Def* spi, const char* t, uint8_t tCount, char* r);

uint32_t getRccFromPort(GPIO_TypeDef* port);
void TIM3_IRQHandler(void);

//#define RFM_RUN GPIO_SetBits(GPIOA,CE);for(int i=0;i<10;i++){SPI_WAIT}GPIO_ResetBits(GPIOA,CE);

#define SPI_WAIT while(bitCount);
#define SPI_START bitCount = 2; TIM_Cmd(TIM3,ENABLE); SPI_WAIT

extern volatile char bitCount;

#endif
