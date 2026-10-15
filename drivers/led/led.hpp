#ifndef MIKROTIK_IOT_TEST_DRIVERS_LED_LED_HPP
#define MIKROTIK_IOT_TEST_DRIVERS_LED_LED_HPP

#include "pin.hpp"

/**
 * Led class contains logic to work with an LED.
 *
 * @attention The class cannot check whether an actual LED is physically present on the specified GPIO.
 */
class Led final {
public:
    /**
     * Constructor sets up configuration of an LED.
     */
    explicit inline Led(GPIO_TypeDef* port, uint32 pin) : Gpio_(port, pin, Pin::Config::OUTPUT_PP) {}

    /**
     * On turns on the LED.
     */
    inline void On() const { Gpio_.Write(true); }

    /**
     * Off turns off the LED.
     */
    inline void Off() const { Gpio_.Write(false); }

    /**
     * Toggle changes state of the LED.
     *
     * On  -> Off
     * Off -> On
     */
    inline void Toggle() const { Gpio_.Write(!Gpio_.Read()); }

private:
    const Pin Gpio_;  //!< Physical pin of the LED.
};

#endif                // MIKROTIK_IOT_TEST_DRIVERS_LED_LED_HPP