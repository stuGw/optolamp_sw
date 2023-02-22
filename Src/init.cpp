/*
 * init.cpp
 *
 *  Created on: Feb 21, 2023
 *      Author: vit
 */
#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "STM32G030.h"
#include "adc.h"
#include "ws2812.h"

void init(void);
void Default_Handler(void);
void USART2_Handler(void);
void USART1_Handler(void);
void HardFault_Handler(void);
void EXTI0_Handler(void);
void SysTick_Handler(void);
void DMA_2_3_Handler(void);
void RTCIrq_Handler(void);
void TIM3_Handler();
int main(void);
// The following are 'declared' in the linker script
extern unsigned char  INIT_DATA_VALUES;
extern unsigned char  INIT_DATA_START;
extern unsigned char  INIT_DATA_END;
extern unsigned char  BSS_START;
extern unsigned char  BSS_END;

extern unsigned char  _sidata;//INIT_DATA_VALUES;
extern unsigned char  _sdata;//INIT_DATA_START;
extern unsigned char  _edata;//INIT_DATA_END;
extern unsigned char  _sbss;//BSS_START;
extern unsigned char  _ebss;//BSS_END;
//#define __bss_start__ BSS_START
//#define __bss_end__ BSS_END

extern uint32_t hwLedAdress;
extern uint8_t flgWSTransfer;
extern void (*__preinit_array_start []) (void) __attribute__((weak));
extern void (*__preinit_array_end []) (void) __attribute__((weak));
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));

typedef void (*fptr)(void);
// the section "vectors" is placed at the beginning of flash
// by the linker script
#pragma GCC push_options
#pragma GCC optimize ("O0")

const fptr Vectors[] __attribute__((section(".vectors"))) ={
	(fptr)0x20002000, 	/* Top of stack (8k) */
	init,   		/* Reset Handler */
	Default_Handler,	/* NMI */
	HardFault_Handler,	/* Hard Fault */
	Default_Handler,	/* MemManage */
	Default_Handler,	/* Reserved  */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* SVCall */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* Reserved */
	Default_Handler,	/* PendSV */
	SysTick_Handler,	/* SysTick */
/* External interrupt handlers follow */
	Default_Handler, 	/* 0: WWDG */
	Default_Handler, 	/* 1: PVD */
	RTCIrq_Handler, 	/* 2: RTC */
	Default_Handler, 	/* 3: FLASH */
	Default_Handler, 	/* 4: RCC */
	EXTI0_Handler, 	/* 5: EXTI0_1 */
	Default_Handler, 	/* 6: EXTI2_3 */
	Default_Handler,    /* 7: EXTI4_15 */
	Default_Handler, 	/* 8: Reserved */
	AnalogConverter::DMA_Handler, 	/* 9: DMA_CH1 */
	DMA_2_3_Handler, 	/* 10: DMA_CH2_3 */
	Default_Handler, 	/* 11: DMA_CH4_5_6_7 */
	Default_Handler, 	/* 12: ADC/COMP */
	Default_Handler, 	/* 13: TIM1_BRK_UP_TRG_COM */
	Default_Handler, 	/* 14: TIM1_CC */
	Default_Handler, 	/* 15: TIM2 */
	TIM3_Handler, 	/* 16: TIM3 */
	Default_Handler, 	/* 17: Reserved */
	Default_Handler, 	/* 18: Reserved */
	Default_Handler, 	/* 19: TIM14 */
	Default_Handler, 	/* 20: Reserved */
	Default_Handler, 	/* 21: TIM16 */
	Default_Handler, 	/* 22: TIM17 */
	Default_Handler, 	/* 23: I2C1 */
	Default_Handler, 	/* 24: I2C2 */
	Default_Handler, 	/* 25: SPI1 */
	Default_Handler, 	/* 26: SPI2 */
	Default_Handler, 	/* 27: USART1 */
	USART2_Handler, 	/* 28: USART2 */
	Default_Handler, 	/* 29: USART3/USART4/LPUART1 */
	Default_Handler, 	/* 30: CEC */
	Default_Handler 	/* 31: AES/RNG */

};
void initClock()
{
     //return;
    // After reset, CPU clock is set to HSISYS = 16MHz

// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1<<24);

        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared

        // Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
        // inserted into Flash memory interface
        // If the chip is run at 64MHz then 2 wait states are required.
        // SysClock is taken from output R of the PLL.  The R output is VCO/2 so need to aim for
        // twice the desired value i.e. 128MHz
        // 128 = 16 * 16 / 2 so N = 16; M = 2
        FLASH->ACR &= 0xfffffff0;
        FLASH->ACR |= 2;
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 8);

        // Set PLL input clock to 16MHz HSI clock
        RCC->PLLSYSCFGR |= (1<<1);
        RCC->PLLSYSCFGR &= 0xffff80ff; // clear N bits
        RCC->PLLSYSCFGR |= (16 << 8);  // set N = 8;
        RCC->PLLSYSCFGR &= 0xffffff0f; // clear M bits
        RCC->PLLSYSCFGR |= (1 << 4);  // set M = 1 (divide by 2)
        RCC->PLLSYSCFGR |= (1 << 28) + (1 << 29); // enable R output of 128/2
        // and turn the PLL back on again
        RCC->CR |= (1<<24);
        // set PLL as system clock source
        RCC->CFGR |= (1<<1);
}
void init_array()
{
    // This function calls constructors for global and static objects
    uint32_t count;
    uint32_t i;

    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++)
        __preinit_array_start[i] ();
    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++)
        __init_array_start[i] ();
}


void initSysTick()
{
	STK->CSR |= ( 7 ); // enable systick, source = cpu clock, enable interrupt
	// SysTick clock source = 64MHz.  Divide this down to create 1 millisecond period
	STK->RVR = 64000-1;
	STK->CVR = 10; // don't want long wait for counter to count down from initial high unknown value
	enable_interrupts();
}


void initEXTIPins()
{
	RCC->IOPENR |=  0x00000001;

	GPIOA->MODER &=~0x00000003;
	GPIOA->PUPDR |= 0x00000001;

	GPIOA->MODER &=~0x0000000c;
	GPIOA->PUPDR |= 0x00000004;

	EXTI->RTSR1 |= 0x00000003;
	EXTI->FTSR1 |= 0x00000003;
	EXTI->IMR1 |= 0x00000003;
	//EXTI->SWIER1 |= 0x00000001;
	NVIC->ISER |= (1<<5);
}

void initLedPin()
{
	//debug
	RCC->IOPENR |= (1 << 1); // enable Port B
	GPIOB->MODER &= ~0x0000008;//~0x0280aaaa;//reset MODE11[1] & MODE12[1] MODE4[1] MODE5[1] MODE6[1] MODE7[1]
	GPIOB->MODER |= 0x00000004;//0x01405555; //set MODE11[0] & MODE12[0] MODE4[0] MODE5[0] MODE6[0] MODE7[0]
}

void initializeHw()
{
	initSysTick();
	initLedPin();
	initEXTIPins();
}

void init()
{
// do global/static data initialization
	unsigned char *src;
	unsigned char *dest;
	unsigned len;
    initClock();
	src= &_sidata;//INIT_DATA_VALUES;
	dest= &_sdata;//INIT_DATA_START;
	len= &_edata-&_sdata;
	while (len--)
		*dest++ = *src++;
// zero out the uninitialized global/static variables
	dest = &_sbss;
	len = &_ebss - &_sbss;
	while (len--)
		*dest++=0;
    init_array();

	main();
}

void Default_Handler()
{
	while(1);
}
void HardFault_Handler()
{
	while(1);
}


void initWSTimer()
{
	GPIOA->MODER &= ~0x00001000;
	GPIOA->MODER |=  0x00002000;
	GPIOA->OTYPER |= 0x00000040;


		RCC->APBENR1 |= 0x00000002;
//		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;  // Enable TIM2 Periph clock

		GPIOA->AFRL |= 0x01000000;//PA6 yto tim3_ch1;


		//remap ch1 to pb4

	//	AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP;
	//	AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_1;

		TIM3->ARR = 49;//old 59 for 48MHz
		TIM3->CCER |= 0x0001;

		//0x00000060 OC1M_1 & OC1M_2
		//0x00000008 OC1PE
		TIM3->CCMR1_Output |= 0x00000068;// TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1PE ;
		TIM3->CCR1 = 0x0013;
		//TIM3->DIER   |= TIM_DIER_CC1DE | TIM_DIER_UIE;
		TIM3->DIER |= 0x0201;
}


void startTimer3()
{
	TIM3->CR1 |= 0x0081;//TIM_CR1_CEN| TIM_CR1_ARPE;

}
void stopTimer3()
{
	TIM3->CR1&= ~0x0001;//~TIM_CR1_CEN;
}


/*void initWSDMA(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	//ws2812DMA
	DMA1_Channel6->CPAR = (uint32_t) &TIM3->CCR1;
	DMA1_Channel6->CMAR = hwLedAdress;
	DMA1_Channel6->CNDTR = WS_COUNT_BITS;
	DMA1_Channel6->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_PSIZE_0|DMA_CCR_TCIE;//| DMA_CCR6_EN ;

	DMA1_Channel6->CCR|= DMA_CCR_PL_1;//priority high(very high used for ws2812)

	DMA1_Channel6->CCR|=DMA_CCR_PL_0;
}
*/


void initializeDMA()
{
	RCC->AHBENR |= 0x00000001;//enable DMA clock
	DMA->CMAR2 = reinterpret_cast<uint32_t>(hwLedAdress);
	DMA->CPAR2 = reinterpret_cast<uint32_t>(&TIM3->CCR1);

		 DMA->CNDTR2 = WS_COUNT_BITS;
		 //MINC - 				  0x00000080
		 //DIR(read from mem ) -  0x00000010
		 //PSIZE_0(16 bit) - 	  0x00000100
		 //TCIE - 				  0x00000002

		 DMA->CCR2 |= 0x00000192;
			//	 0x000005a0;//configure MSIZE 16bit, PSIZE 16bit, CIRC mode, MEmory increment

		 DMAMUX->C1CR |= 0x00000005;//ADC Request input
		 DMA->CCR2 |= 0x00000002;//transfer complete ipt enable
		 NVIC->ISER |= (1<<10);//enable ipt dma channel1
		 DMA->CCR2 |= 0x00003000;
		 //DMA->CCR2 |= 0x00000001;//enable
}
void startWSDMA(void)
{
	DMA->CNDTR2 = WS_COUNT_BITS;
	DMA->CCR2 = 0x00003190;//DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_EN | DMA_CCR_PSIZE_0|DMA_CCR_TCIE|DMA_CCR_PL_0|DMA_CCR_PL_1;
}

void stopWSDMA(void)
{
	DMA->CCR2&=~0x0001;
}
//Постоянная передача на все диоды
void startWSDMACircle()
{
	DMA->CNDTR2 = WS_COUNT_BITS;
	DMA->CCR2 = 0x00003192;//DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_EN | DMA_CCR_PSIZE_0|DMA_CCR_CIRC|DMA_CCR_PL_0|DMA_CCR_PL_1;
}


void startWsTransfer(void)
{

	if(flgWSTransfer)return;//Ждем когда закончится предыдущая передача
	flgWSTransfer = 1;//Флаг начала передачи
//	stopLightSensDMA();//Выкл. канал датчика освещения т.к. он прерывает канал WS
	startWSDMA();
	startTimer3();
}
void wsDmaTmCirc(void)
{
	startWSDMACircle();
	TIM3->DIER&= ~0x0001;
	startTimer3();
}
//Режим одиночной передачи для ws
void wsDmaTmOnce(void)
{
	DMA->CCR2&= ~0x0020;
	DMA->CCR2|=0x0002;
	TIM3->DIER|=0x01;
}

#pragma GCC pop_options
#endif

