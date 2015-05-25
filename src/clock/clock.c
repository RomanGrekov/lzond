#include "clock.h"


void init_bad_clock_inter(void)
{
	/* Включаем защиту HSE от сбоев - CSS */
	RCC->CR |= RCC_CR_CSSON;
}

uint32_t InitRCC( void) {
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /* Конфигурацяи  SYSCLK, HCLK, PCLK2 и PCLK1 ---------------------------*/
  /* Включаем HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

  /* Ждем пока HSE не выставит бит готовности либо не выйдет таймаут*/
  do {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while( (HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));

  if ( (RCC->CR & RCC_CR_HSERDY) != RESET) {
    HSEStatus = (uint32_t)0x01;
  }
  else {
    HSEStatus = (uint32_t)0x00;
  }

  /* Если HSE запустился нормально */
  if ( HSEStatus == (uint32_t)0x01) {
    /* Включаем буфер предвыборки FLASH */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Конфигурируем Flash на 2 цикла ожидания */
    /* Это нужно потому, что Flash не может работать на высокой частоте */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;


    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

    /* Конфигурируем множитель PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
    /* При условии, что кварц на 8МГц! */
    /* RCC_CFGR_PLLMULL9 - множитель на 9. Если нужна другая частота, не 72МГц */
    /* то выбираем другой множитель. */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL6);

    /* Включаем PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Ожидаем, пока PLL выставит бит готовности */
    while((RCC->CR & RCC_CR_PLLRDY) == 0) {
    }

    /* Выбираем PLL как источник системной частоты */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

    /* Ожидаем, пока PLL выберется как источник системной частоты */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08) {
    }
  }
  else {
   /* Все плохо... HSE не завелся... Чего-то с кварцем или еще что...
      Надо бы както обработать эту ошибку... Если мы здесь, то мы работаем
      от HSI! */
  }

  return HSEStatus;
}
 
/* Обработчик прерывания NMI Возникает если кварц сбойнул. Отключить его нельзя*/
void NMI_Handler( void) 
{
/*Очищаем флаг прерывания CSS иначе прерывание будет генерироваться бесконечно*/
RCC->CIR |= RCC_CIR_CSSC;
 
//Some code to handle situation
}
