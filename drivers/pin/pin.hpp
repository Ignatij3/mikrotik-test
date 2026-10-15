#ifndef MIKROTIK_IOT_TEST_DRIVERS_PIN_PIN_HPP
#define MIKROTIK_IOT_TEST_DRIVERS_PIN_PIN_HPP

#include "stdint.hpp"
#include "stm32f10x.h"

/**
 * This class represents a single GPIO on an MCU.
 */
class Pin {
public:
    /**
     * Config is an enumeration which specifies pin configuration,
     * whether it's input or output and some sub-configurations of them.
     *
     * Alternate functions are listed in 'docs/STM32F103C6T6-datasheet.pdf, page 28 table 5.
     *
     * @attention It is not possible to choose a specific alternate function
     * for a pin in this MCU. Instead, disable clocks of every other undesired
     * peripheral to ensure only desired alternate is working (not implemented here).
     *
     * 2 higher bits are CNF bits, 2 lower bits are MODE bits.
     * They are set at GPIOx_CRL or GPIOx_CRH registers.
     */
    enum Config : uint32 {
        INPUT_ANALOG   = 0b0000,  //!< Configure GPIO as analog input.
        INPUT_FLOATING = 0b0100,  //!< Configure GPIO as floating input.
        /**
         * @warning Actual pull up or pull down activation is not imlpemented.
         */
        INPUT_PULL     = 0b1000,  //!< Configure GPIO as input with either pull up or pull down.
        OUTPUT_PP      = 0b0001,  //!< Configure GPIO as output push pull.
        OUTPUT_OD      = 0b0101,  //!< Configure GPIO as output open drain.
        /**
         * @note The peripheral which is thought to use this pin should enable its own clock.
         */
        ALTERNATE_PP   = 0b1001,  //!< Configure GPIO as alternate function, push pull.
        /**
         * @note The peripheral which is thought to use this pin should enable its own clock.
         */
        ALTERNATE_OD   = 0b1101  //!< Configure GPIO as alternate function, open drain.
    };

    /**
     * Constructor creates a Pin instance which can be used to manage underlying GPIO.
     *
     * It enables clock to an underlying port and alternate function if used.
     *
     * @param[in] port GPIO port.
     * @param[in] pin GPIO number.
     * @param[in] cfg pin configuration.
     */
    explicit Pin(GPIO_TypeDef* port, uint32 pin, Config cfg);

    /**
     * Destructor disables clock to an underlying port.
     *
     * @warning This function is disabled to not accidentally turn off used port.
     */
    ~Pin();

    /**
     * Write will set either logic HIGH or LOW on the gpio.
     *
     * You can only write to a pin that's configured as OUTPUT pin.
     *
     * @param[in] state high (true) or low (false) pin logic.
     */
    void Write(bool state) const;

    /**
     * Read will report either HIGH or LOW logic on the gpio.
     *
     * If read is used on an OUTPUT pin or ALTERNATE function pin,
     * it always returns false.
     *
     * @retval true pin logic "high".
     * @retval false pin logic "low".
     */
    bool Read() const;

private:
    GPIO_TypeDef* Port_;  //!< GPIO port.
    const uint32  PinN_;  //!< GPIO number.
    const Config  Conf_;  //!< Pin configuration.

    /**
     * EnableClock_ supplies clock to the port on which the pin sits.
     */
    void EnableClock_();

    /**
     * DisableClock_ supplies clock to the port on which the pin sits.
     *
     * It is implicitly called during object destruction.
     *
     * @warning Calling this function prevents from using GPIOs on the same port!
     */
    void DisableClock_();
};

#endif  // MIKROTIK_IOT_TEST_DRIVERS_PIN_PIN_HPP