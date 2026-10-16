#include "clock.hpp"
#include "led.hpp"
#include "uart.hpp"

// this is imlpemented to avoid big switch to select baud rate
#define CONCAT_INNER(x, y) x##y
#define CONCAT(x, y)       CONCAT_INNER(x, y)

/// ---------- default values ----------

#ifndef BAUDRATE
#  define BAUDRATE 9600
#endif  // BAUDRATE

#ifndef LED1_PIN
#  define LED1_PIN B6
#endif  // LED1_PIN

#ifndef LED2_PIN
#  define LED2_PIN B8
#endif  // LED2_PIN

/// ---------- default values ----------

/// ---------- compile-time input sanitization ----------

#if BAUDRATE != 2400 && BAUDRATE != 9600 && BAUDRATE != 19200 && BAUDRATE != 57600 && BAUDRATE != 115200 && BAUDRATE != 230400 && BAUDRATE != 460800 && BAUDRATE != 921600 && BAUDRATE != 2250000
#  error "Incorrect BAUD rate chosen, options are: 2400 / 9600 / 19200 / 57600 / 115200 / 230400 / 460800 / 921600 / 2250000"
#endif  // BAUDRATE != 2400 ...

#if (LED1_PIN != B6 && LED1_PIN != B7) && (LED2_PIN != B8 && LED2_PIN != B9)
#  error "Incorrect LED pin selection. For LED1 B6 and B7 are available, and for LED2 B8 and B9 are available"
#endif  // LED1_PIN != B6 ...

/// ---------- compile-time input sanitization ----------

extern "C"
{

    int main()
    {
        clockInit();

#if LED1_PIN == B6
        Led led1 = Led(GPIOB, 6);
#else
        Led led1 = Led(GPIOB, 7);
#endif  // LED1_PIN == B6

#if LED2_PIN == B8
        Led led2 = Led(GPIOB, 8);
#else
        Led led2 = Led(GPIOB, 9);
#endif  // LED2_PIN == B8

        led1.Off();
        led2.Off();

        Uart uart = Uart(GPIOA, 10, GPIOA, 9, USART1, CONCAT(Uart::BaudRate::BAUD_, BAUDRATE));

        // turn on debug led to signal startup complete
        Led ledDebug = Led(GPIOC, 13, true);
        ledDebug.On();

        char hello[] = "hello,oooo ward!\r\n";

        while (true) {
            ledDebug.Off();

            uart.Send((uint8*)hello, sizeof(hello));

            ledDebug.On();
        }
    }
}