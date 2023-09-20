/*
* File: main.c
* Author: Doan Quoc Phu
* Date: 18/09/2023
* Description: This is a file for receiving data which array by UART protocol.
*/

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stdlib.h"

#define Tx      GPIO_Pin_6
#define Rx      GPIO_Pin_7
#define PORTS   GPIOB

#define LED     GPIO_Pin_13

void delayTime(double time);
void checkled(void);
void configurationGpioUart(void);
void configurationGpioLed(void);

void waitForStartReceive(void);
void waitForEndReceive(void);
uint8_t receiveData(void);

int main(void){
	
	configurationGpioUart();
	configurationGpioLed();
	
	uint8_t * dataReceive = (uint8_t*)malloc(4*sizeof(uint8_t));
	
	uint8_t i = 0;
	
	GPIO_SetBits(PORTS, Rx);
	delayTime(1000);
	
	while(i < 4){
	
		waitForStartReceive();
			
		*(dataReceive + i)= receiveData();
			
		waitForEndReceive();
			
		checkled();                // receive data: 1 byte then turn on Led PC13
			
		i++;
	}
	free(dataReceive);
	return 0;
}

void delayTime(double time){
	
	for(int i = 0; i < time; i++);
}


void configurationGpioUart(){
	
	GPIO_InitTypeDef gpio;
	
	/*ENABLE CLOCK*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/*PB7 RX*/
	gpio.GPIO_Pin   = Rx;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(PORTS, &gpio);
	
	/*PB6 TX*/
	gpio.GPIO_Pin   = Tx;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(PORTS, &gpio);
}

void configurationGpioLed(){

	// Enable clock Port C
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	// Reset Port C
	GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
	// Set output PC13
	GPIOC->CRH |= GPIO_CRH_MODE13_0;
	
}
void waitForStartReceive(){

	while(GPIO_ReadInputDataBit(PORTS, Rx) == 1)
	delayTime(12.5);
}
void waitForEndReceive(){
	
	while(GPIO_ReadInputDataBit(PORTS, Rx) == 0);
	delayTime(20);
}

void checkled(){
	
	GPIOC->BSRR = GPIO_BSRR_BR13;
	delayTime(200);								 // bit time
	GPIOC->BSRR = GPIO_BSRR_BS13;
	delayTime(200);								 // bit time
	
}

uint8_t receiveData(){

	uint8_t data  = 0;
	uint8_t count = 0;
			
	for(uint8_t i = 0 ; i< 9; i++ ){
	
	if(GPIO_ReadInputDataBit(PORTS,Rx) == 1) { 
		
		 if(i < 8 ) data |= 1 << i;
			
		 count++;
	}
	
	delayTime(5);
	
	}
	
	if(count % 2 == 0 ) return data;
	
}
