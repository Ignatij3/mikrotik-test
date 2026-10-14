#ifndef MIKROTIK_IOT_TEST_DRIVERS_CORE_CLOCK_HPP
#define MIKROTIK_IOT_TEST_DRIVERS_CORE_CLOCK_HPP

#include "stm32f10x.h"

/**
 * clockInit sets up clock source for MCU and peripherals.
 *
 * Clock tree diagram from RM0008 page 126 was provided to make
 * clock setup more clear. It is to be found at 'docs/clock-tree.png'.
 * 
 * This function only sets up system clocks. Clock setup and division
 * for peripherals is a responsibility, of peripherals. There is
 * no security against peripherals messing with clocks at the same
 * time, because there is no such use case for this project.
 */
void clockInit();

#endif  // MIKROTIK_IOT_TEST_DRIVERS_CORE_CLOCK_HPP