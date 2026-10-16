#ifndef MIKROTIK_IOT_TEST_DRIVERS_UART_UART_HPP
#define MIKROTIK_IOT_TEST_DRIVERS_UART_UART_HPP

#include "pin.hpp"
#include "stdint.hpp"
#include "stm32f10x.h"

/**
 * This class contains all the functionality needed to create and use a UART connection with a device.
 */
class Uart {
public:
    /**
     * BaudRate enumeration contains available baud rate configuration options.
     *
     * The values themselves represent USARTn_BRR register values, according to
     * documentation in RM0008 page 799, table 192.
     */
    enum class BaudRate : uint16 {
        BAUD_2400    = 0b00111010'10011000,  //!< 937.5 results in 2400 bps.
        BAUD_9600    = 0b00001110'10100110,  //!< 234.375 results in 9600 bps.
        BAUD_19200   = 0b00000111'01010011,  //!< 117.1875 results in 19200 bps.
        BAUD_57600   = 0b00000010'01110001,  //!< 39.0625 results in 57600 bps.
        BAUD_115200  = 0b00000001'00111000,  //!< 19.5 results in 115200 bps.
        BAUD_230400  = 0b00000000'10011100,  //!< 9.75 results in 230400 bps.
        BAUD_460800  = 0b00000000'01001110,  //!< 4.875 results in 460800 bps.
        BAUD_921600  = 0b00000000'00100111,  //!< 2.4375 results in 921600 bps.
        BAUD_2250000 = 0b00000000'00010000   //!< 1 results in 2250000 bps.
    };

    /**
     * Constructor initializes pins for the UART to operate and enables UART with default configuration.
     *
     * @param[in] rx_port GPIO port for RX line.
     * @param[in] rx_pin pin number which RX line uses, consult documentation for available options.
     * @param[in] tx_port GPIO port for TX line.
     * @param[in] tx_pin pin number which TX line uses, consult documentation for available options.
     * @param[in] inst relevant USART instance.
     * @param[in] baud chosen BAUD rate.
     */
    explicit Uart(GPIO_TypeDef* rx_port, uint32 rx_pin, GPIO_TypeDef* tx_port, uint32 tx_pin, USART_TypeDef* inst, BaudRate baud);

    /**
     * Destructor disables transmitter and receiver, and then disables UART peripheral
     * @warning Peripheral clock is also disabled.
     */
    ~Uart();

    /**
     * Send transmits 'data' using the UART peripheral.
     *
     * @param[in] data bytes to be sent over the UART connection.
     * @param[in] size number of bytes to be send.
     */
    void Send(uint8* data, uint16 size);

    /**
     * Receive reads data from the UART peripheral.
     *
     * If idle frame is detected before 'size' bytes was received,
     * the reception stops.
     *
     * @param[out] data array with length "size" in which the received data is stored.
     * @param[in] size number of bytes to be received.
     *
     * @return how many bytes were received before an idle frame.
     */
    uint32 Receive(uint8* data, uint16 size);

    /**
     * Receive_Char reads data from the UART peripheral until chr is found.
     *
     * If idle frame is detected before 'size' bytes was received,
     * the reception stops.
     *
     * @param[out] data array with length "size" in which the received data is stored.
     * @param[in] size maximum number of bytes to be received.
     * @param[in] chr after receiving this character the reception stops.
     *
     * @return how many bytes were received before an idle frame.
     */
    uint32 Receive_Char(uint8* data, uint16 size, char chr);

private:
    /**
     * Enable_Clock_ starts the clock specific for the UART instance in this class.
     */
    void EnableClock_();

    /**
     * Disable_Clock_ stops the clock specific for the UART instance in this class.
     */
    void DisableClock_();

    Pin Tx_;                   //!< UART tx pin.
    Pin Rx_;                   //!< UART rx pin.

    USART_TypeDef* Instance_;  //!< UART instance.
};

#endif                         // MIKROTIK_IOT_TEST_DRIVERS_UART_UART_HPP