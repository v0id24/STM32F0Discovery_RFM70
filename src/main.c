#include <stddef.h>
#include "stm32f0xx_conf.h"

#define LED_PORT GPIOC
#define LED1 GPIO_Pin_9
#define LED2 GPIO_Pin_8

#define KEY_PORT GPIOA
#define KEY GPIO_Pin_0

// Pin Configuration SPI1
// PA1 CE
// PA4 CSN
// PA5 SCK
// PA6 MISO
// PA7 MOSI
// PC4 IRQ

void GPIO_ToggleBit(GPIO_TypeDef* port, uint16_t pin) {
	if (GPIO_ReadOutputDataBit(port, pin) == Bit_SET) {
		GPIO_ResetBits(port, pin);
	} else {
		GPIO_SetBits(port, pin);
	}
}

void sendUSART(const char *data,const uint8_t length){
	for(uint8_t i = 0;i < length;i++){
		while(!USART_GetFlagStatus(USART2,USART_FLAG_TC));
		USART_ClearFlag(USART2,USART_FLAG_TC);
		USART_SendData(USART2,data[i]);
	}
}

void TIM2_IRQHandler(void) {
	GPIO_ToggleBit(LED_PORT,LED2);
	//sendUSART("Hallo",6);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

void InitSPI(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;

	SPI_Init(SPI1,&SPI_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
}

void InitGPIO(){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	//USART
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PC8 and PC9 in output pushpull mode
	 * PC8 = LD3 Green LED
	 * PC9 = LD4 Blue LED
	 * */
	GPIO_InitStructure.GPIO_Pin = LED1 | LED2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);

	//SPI
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_0);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_0);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_0);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//Button
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void InitUSART(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitTypeDef usart;
	USART_StructInit(&usart);
	usart.USART_BaudRate = 9600;
	USART_Init(USART2, &usart);
	USART_Cmd(USART2, ENABLE);
}

void InitTIM2(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef timerSt;
	TIM_TimeBaseStructInit(&timerSt);
	timerSt.TIM_Prescaler = 10000 - 1;
	timerSt.TIM_Period = (SystemCoreClock / 10000) - 1;
	TIM_TimeBaseInit(TIM2, &timerSt);
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = TIM2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPriority = 3;
	NVIC_Init(&nvic);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

int main(void) {
	SystemCoreClockUpdate();

	InitGPIO();
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	InitUSART();
	InitTIM2();
	InitSPI();

	TIM_Cmd(TIM2,ENABLE);

	while (1) {

		//while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) != RESET);
		//while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) != SET);

		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		SPI_I2S_SendData16(SPI1,0x55);
		while(SPI_GetTransmissionFIFOStatus(SPI1) == SPI_TransmissionFIFOStatus_Empty);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		USART_SendData(USART2,SPI_I2S_ReceiveData16(SPI1));
		GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_SET);

		for(uint16_t i = 0;i<10000;i++);

	}
	
	return 0;
}

void assert_failed(uint8_t* file, uint32_t line) {
	while (1)
		;
}
