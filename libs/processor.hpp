#ifndef MIKROTIK_IOT_TEST_LIBS_PROCESSOR_HPP
#define MIKROTIK_IOT_TEST_LIBS_PROCESSOR_HPP

#include "led.hpp"
#include "stdint.hpp"
#include "uart.hpp"

/**
 * This class implements command processing and execution.
 */
class Processor {
public:
    /**
     * Constructor initializes LEDs and UART.
     *
     * LEDs are turned off.
     */
    Processor();

    /**
     * PromptAndExecute will get a command from user via UART and execute it.
     *
     * @retval true entered command is correct.
     * @retval false entered command has errors or is incomplete.
     */
    bool PromptAndExecute();

private:
    /**
     * SetLED turns on a specified LED for a specified time duration.
     *
     * The function then reports led-off on UART when time has passed.
     *
     * @param[in] led_id ID of LED to turn on. Possible values: 0, 1.
     * @param[in] ms time in milliseconds for which to turn LEDs on, range of allowed values: [1; 5000).
     *
     * @attention I have not written the function yet, it's 16-10-2026 21:36 at the moment of writing this message.
     * I am already aware that this function will be blocking, and I have unfortunately no more time to give to this
     * task, as tomorrow at 13:00 I have a plane for a couple days vacation. I am aware that to avoid blocking we
     * can implement external interrupt (EXTI), but as was already voiced, I do not have time resources for this
     * implementation. Please know that I am aware of this problem and am aware of the (one) of the solutions for it.
     */
    void SetLED_(uint8 led_id, uint16 ms);

    /**
     * Echo_ repeats data with prefix.
     *
     * Data is taken from the internal buffer.
     *
     * @param[in] data buffer from which data is taken, it must at least be
     * @param[in] len length of data block, range of allowed values: [0; 300].
     */
    void Echo_(char* data, uint32 len);

    Led  Led1_;         //!< First active LED.
    Led  Led2_;         //!< Second active LED.
    Uart Uart_;         //!< UART peripheral.
    char Buffer_[512];  //!< Buffer for UART reception.
};

#endif                  // MIKROTIK_IOT_TEST_LIBS_PROCESSOR_HPP