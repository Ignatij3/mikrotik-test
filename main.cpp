#include "led.hpp"
#include "uart.hpp"

/**
 * Execution entry point.
 */
int main();

int main()
{
    // options GPIOB 6, GPIOB 7, GPIOB 8, GPIOB 9

    Led led1 = Led(GPIOB, 6);
    Led led2 = Led(GPIOB, 7);

    led1.Off();
    led2.Off();

    Uart uart = Uart(GPIOA, 10, GPIOA, 9, USART1, Uart::BaudRate::BAUD_9600);

    // turn on debug led to signal startup complete
    Led ledDebug = Led(GPIOC, 13);
    ledDebug.On();

    while (true) {
    }

    return 0;
}