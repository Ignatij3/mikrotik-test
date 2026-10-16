#ifndef MIKROTIK_IOT_TEST_DRIVERS_LED_LED_HPP
#define MIKROTIK_IOT_TEST_DRIVERS_LED_LED_HPP

#include "pin.hpp"

/**
 * Led class contains logic to work with an LED.
 *
 * @attention The class cannot check whether an actual LED is physically present on the specified GPIO.
 */
class Led {
public:
    /**
     * Constructor sets up configuration of an LED.
     *
     * @param[in] port GPIO port of the pin.
     * @param[in] pin pin number.
     * @param[in] active_low
     */
    explicit inline Led(GPIO_TypeDef* port, uint32 pin, bool active_low = false) :
        Gpio_(port, pin, Pin::Config::OUTPUT_PP),
        ActiveLow_(active_low)
    {}

    /**
     * On turns on the LED.
     */
    inline void On() const { Gpio_.Write(!ActiveLow_); }

    /**
     * Off turns off the LED.
     */
    inline void Off() const { Gpio_.Write(ActiveLow_); }

    /**
     * Toggle changes state of the LED.
     *
     * On  -> Off
     * Off -> On
     */
    inline void Toggle() const { Gpio_.Write(!Gpio_.Read()); }

private:
    const Pin  Gpio_;       //!< Physical pin of the LED.
    const bool ActiveLow_;  //!< If an LED is active low, this will correct for it.
};

#endif                      // MIKROTIK_IOT_TEST_DRIVERS_LED_LED_HPP