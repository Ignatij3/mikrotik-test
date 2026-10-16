#include "pin.hpp"

Pin::Pin(GPIO_TypeDef* port, uint32 pin, Config cfg) :
    Port_(port),
    PinN_(pin),
    Conf_(cfg)
{
    EnableClock_();

    // 4 bits are used to set pin configuration
    // to move those bits to a correct place, we use pin number (starting with 0), and multiply by 4 (to move 4 bits)
    // but first clear all relevant bits
    if (PinN_ < 8) {

        Port_->CRL &= ~(0b1111UL << (PinN_ * 4));
        Port_->CRL |= Conf_ << (PinN_ * 4);

    } else {

        // correct pin number to address register correctly
        Port_->CRH &= ~(0b1111UL << ((PinN_ - 8) * 4));
        Port_->CRH |= Conf_ << ((PinN_ - 8) * 4);
    }
}

Pin::~Pin()
{
    DisableClock_();
}

void Pin::Write(bool state) const
{
    if (!(Conf_ == Config::OUTPUT_OD || Conf_ == Config::OUTPUT_PP)) {
        // log error
        return;
    }

    SET_BIT(Port_->BRR, 1 << PinN_);
    SET_BIT(Port_->ODR, (uint32)state << PinN_);
}

bool Pin::Read() const
{
    if (!(Conf_ == Config::INPUT_ANALOG || Conf_ == Config::INPUT_FLOATING || Conf_ == Config::INPUT_PULL)) {
        // log error
        return false;
    }

    return READ_BIT(Port_->IDR, 1 << PinN_);
}

void Pin::EnableClock_()
{
    // so basically we just enable clock o alternate function and then the peripheral enables clock for itself and it should work
    // https://community.st.com/stm32-mcus-products-25/stm32f103-alternate-function-selection-for-a-specific-pin-60316
    if (Conf_ == Config::ALTERNATE_OD || Conf_ == Config::ALTERNATE_PP) {
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
    }

    if (Port_ == GPIOA) {
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);

    } else if (Port_ == GPIOB) {
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);

    } else if (Port_ == GPIOC) {
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);
    }
}

void Pin::DisableClock_()
{
    // don't clear anything to not mess with other pins
    // it is not planned anyway that this function is called

    // // it shouldn't be done like this, because it disables every AF pin that's active
    // if (Conf_ == Config::ALTERNATE_OD || Conf_ == Config::ALTERNATE_PP) {
    //  CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
    // }

    // if (Port_ == GPIOA) {
    //  CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);

    // } else if (Port_ == GPIOB) {
    //  CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);

    // } else if (Port_ == GPIOC) {
    //  CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);
    // }
}