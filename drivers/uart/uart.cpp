#include "uart.hpp"

Uart::Uart(GPIO_TypeDef* rx_port, uint32 rx_pin, GPIO_TypeDef* tx_port, uint32 tx_pin, USART_TypeDef* inst, BaudRate baud) :
    // RX/TX pins configured according to RM0008 page 166, table 24
    Rx_(rx_port, rx_pin, Pin::Config::ALTERNATE_PP),
    Tx_(rx_port, rx_pin, Pin::Config::ALTERNATE_OD),
    Instance_(inst)
{
    EnableClock_();

    /**
     * default configuration is:
     * - 8bits data
     * - 1 stop bit
     * - no parity
     * - full duplex
     * - asynchronous mode
     */

    // baud rate is calculated with:
    // baud = f_CK / (16 * USARTDIV)
    //
    // USARTDIV comprises of DIV_Mantissa (decimal number coded binary)
    // and DIV_Fraction (decimal number coded binary), it is then divided by 16 to get fractional number
    // USARTDIV = DIV_Mantissa + DIV_Fraction/16

    // program baud rate
    WRITE_REG(Instance_->BRR, (uint16)baud);

    // set word length to 8 bits
    SET_BIT(Instance_->CR1, USART_CR1_M);

    // disable parity control
    CLEAR_BIT(Instance_->CR1, USART_CR1_PCE);

    // set stop bits to 1
    CLEAR_BIT(Instance_->CR2, USART_CR2_STOP);

    // disable all flow controls
    CLEAR_BIT(Instance_->CR3, USART_CR3_CTSE);
    CLEAR_BIT(Instance_->CR3, USART_CR3_RTSE);

    // disable half duplex communication (presumably keeps it full duplex)
    CLEAR_BIT(Instance_->CR3, USART_CR3_HDSEL);

    // disable transmitter and enable receiver, by default we listen to incoming data
    CLEAR_BIT(Instance_->CR1, USART_CR1_TE);
    SET_BIT(Instance_->CR1, USART_CR1_RE);

    // last, enable USART
    SET_BIT(Instance_->CR1, USART_CR1_UE);
}

Uart::~Uart()
{
    // disable transmitter and receiver
    CLEAR_BIT(Instance_->CR1, USART_CR1_TE);
    CLEAR_BIT(Instance_->CR1, USART_CR1_RE);

    // disable USART
    CLEAR_BIT(Instance_->CR1, USART_CR1_UE);

    DisableClock_();
}

void Uart::Send(uint8* data, uint16 size)
{
    // prodecure described at RM0008 page 792

    // disable receiver and enable transmitter
    CLEAR_BIT(Instance_->CR1, USART_CR1_RE);
    SET_BIT(Instance_->CR1, USART_CR1_TE);

    for (uint32 i = 0; i < size; i++) {
        // push data to transmit register
        Instance_->DR = data[i];

        // wait until data is in shift register
        while (!READ_BIT(Instance_->SR, USART_SR_TXE)) {
            __NOP();  // POTENTIAL INFINITE LOOP?
        }
    }

    // wait until transmission is complete
    while (!READ_BIT(Instance_->SR, USART_SR_TC)) {
        __NOP();  // POTENTIAL INFINITE LOOP?
    }

    // disable transmitter and enable receiver
    CLEAR_BIT(Instance_->CR1, USART_CR1_TE);
    SET_BIT(Instance_->CR1, USART_CR1_RE);
}

uint32 Uart::Receive(uint8* data, uint16 size)
{
    // prodecure described at RM0008 page 795

    // receiver is always enabled, no need to do anything

    uint32 recv;

    for (recv = 0; recv < size; recv++) {
        // wait for data to be transferred to RDR
        while (!READ_BIT(Instance_->SR, USART_SR_RXNE)) {
            if (READ_BIT(Instance_->SR, USART_SR_IDLE)) {
                // if we receive IDLE line when waiting for data, return
                return recv;
            }
        }

        data[recv] = Instance_->DR;
    }

    return recv;
}

uint32 Uart::Receive_Char(uint8* data, uint16 size, char chr)
{
    uint32 recv;

    for (recv = 0; recv < size; recv++) {
        // wait for data to be transferred to RDR
        while (!READ_BIT(Instance_->SR, USART_SR_RXNE)) {
            if (READ_BIT(Instance_->SR, USART_SR_IDLE)) {
                // if we receive IDLE line when waiting for data, return
                return recv;
            }
        }

        data[recv] = Instance_->DR;

        if (data[recv] == chr) {
            return recv;
        }
    }

    return recv;
}

void Uart::EnableClock_()
{
    if (Instance_ == USART1) {
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);

    } else if (Instance_ == USART2) {
        SET_BIT(RCC->APB2ENR, RCC_APB1ENR_USART2EN);

    } else if (Instance_ == USART3) {
        SET_BIT(RCC->APB2ENR, RCC_APB1ENR_USART3EN);

    } else {
        // log error
        return;
    }
}

void Uart::DisableClock_()
{
    if (Instance_ == USART1) {
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);

    } else if (Instance_ == USART2) {
        CLEAR_BIT(RCC->APB2ENR, RCC_APB1ENR_USART2EN);

    } else if (Instance_ == USART3) {
        CLEAR_BIT(RCC->APB2ENR, RCC_APB1ENR_USART3EN);

    } else {
        // log error
        return;
    }
}