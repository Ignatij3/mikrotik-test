#include "clock.hpp"

void clockInit()
{
    // RM0008 page 130: After a system reset, the HSI oscillator is selected as system clock
    // RM0008 page 132: The microcontroller clock output (MCO) capability allows the clock to be output onto the external MCO pin

    // RCC_CR reset value is 0x0000 XX83, which means HSI is enabled and ready

    // HSI by default is source for SYSCLK, prescaler is 1 by default

    /// MOVE IT TO UART SETUP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // USART1 is powered by ABP2, we need to enable clock to it
    // SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);

    // USART RM0008 page 167 INPUT CONFIG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}