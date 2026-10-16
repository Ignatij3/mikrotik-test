#include "clock.hpp"

#include "led.hpp"

void clockInit()
{
    // RM0008 page 130: After a system reset, the HSI oscillator is selected as system clock
    // RM0008 page 132: The microcontroller clock output (MCO) capability allows the clock to be output onto the external MCO pin

    // RCC_CR reset value is 0x0000 XX83, which means HSI is enabled and ready

    // HSI by default is source for SYSCLK, but we need 36MHz for UART
    // SYSCLK can be powered by PLLCLK, which is HSE = 8MHz, then PLLCLK * 9 / 2 = 36MHz

    // enable Flash Prefetch Buffer and set 2 Wait States
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);
    CLEAR_BIT(FLASH->ACR, FLASH_ACR_LATENCY);
    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_2);

    RCC->CR |= RCC_CR_HSEON;
    while (!READ_BIT(RCC->CR, RCC_CR_HSERDY)) {
        __NOP();
    }

    CLEAR_BIT(RCC->CFGR, RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);

    // set PLL source to HSE and multiplier to x9, 8MHz * 9 = 72MHz
    SET_BIT(RCC->CFGR, RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

    // configure Prescalers to output 36MHz
    SET_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV2);
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV1);
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV1);

    // enable PLL and wait until it stabilizes
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY))
        ;

    // set PLL as SYSCLK
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW);
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);

    // now peripherals should receive 36MHz
}