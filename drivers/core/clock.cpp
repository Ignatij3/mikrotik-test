#include "clock.hpp"

void clockInit()
{
    // RM0008 page 130: After a system reset, the HSI oscillator is selected as system clock
    // RM0008 page 132: The microcontroller clock output (MCO) capability allows the clock to be output onto the external MCO pin

    // RCC_CR reset value is 0x0000 XX83, which means HSI is enabled and ready

    // HSI by default is source for SYSCLK, but we need 36MHz for UART
    // SYSCLK can be powered by PLLCLK, which is HSI/2 = 4MHz, then PLLCLK * 9 = 36MHz

    // choose HSI just in case as SYSCLK to be able to turn off PLL
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW);

    // disable PLL to set multiplier
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);

    // set PLL entry source to HSI
    SET_BIT(RCC->CFGR, RCC_CFGR_PLLSRC_HSI_Div2);

    // set PLL multiplier as x9
    SET_BIT(RCC->CFGR, RCC_CFGR_PLLMULL9);

    // enable PLL
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);

    // wait for PLL to turn ON (possible infinite loop?)
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY)) {
        __NOP();
    }

    // choose PLL as SYSCLK
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);

    // set all prescalers to 1
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE);
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_PPRE1);
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_PPRE2);

    // now peripherals should receive 36MHz
}