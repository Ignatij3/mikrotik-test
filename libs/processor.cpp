#include "processor.hpp"

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

#if LED1_PIN == B6
#  define LED1_CONF GPIOB, 6
#else
#  define LED1_CONF GPIOB, 7
#endif  // LED1_PIN == B6

#if LED2_PIN == B8
#  define LED2_CONF GPIOB, 8
#else
#  define LED2_CONF GPIOB, 9
#endif                                   // LED2_PIN == B8

static char RESP_OK[]    = "OK\r\n";     //!< Response OK.
static char RESP_ERROR[] = "ERROR\r\n";  //!< Response ERROR.

#define RESP_OK_LEN    4
#define RESP_ERROR_LEN 7

static uint32 atoi(char* str)
{
    uint32 i = 0;
    while ((*str >= '0') && (*str <= '9')) {
        i = i * 10 + (uint32)(*str) - '0';
        str++;
    }
    return i;
}

Processor::Processor() :
    Led1_(LED1_CONF),
    Led2_(LED2_CONF),
    Uart_(GPIOA, 10, GPIOA, 9, USART1, CONCAT(Uart::BaudRate::BAUD_, BAUDRATE)),
    Buffer_({ 0 })
{
    // off by default
    Led1_.Off();
    Led2_.Off();

    // initialize pin for timer
    static Pin TIMER_PIN = Pin(GPIOC, 13, Pin::Config::OUTPUT_PP);

    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

    // set prescaler to divide 36MHz down to 1kHz (36000 ticks per second -> 1ms per tick)
    TIM2->PSC = 36000 - 1;
}

bool Processor::PromptAndExecute()
{
    static char msg[] = "Send your command: ";
    Uart_.Send(msg, sizeof(msg) - 1);

    Led ledDebug = Led(GPIOC, 13, true);
    ledDebug.On();

    uint32 recvd = Uart_.ReceiveChar(Buffer_, sizeof(Buffer_), '\r');
    ledDebug.Off();

    // recvd now points to (presumably) \r
    recvd--;

    // if len is less than shortest command: "echo 0,0\r" (9 chars), then it's definitely error
    if (recvd < 9) {
        Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
        return false;
    }

    // parse command
    // find first space
    uint32 char_pos = 0;
    while (Buffer_[char_pos] != ' ' && char_pos < recvd) {
        char_pos++;
    }

    // ends with space or space not found
    if (char_pos == recvd) {
        Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
        return false;
    }

    // space found, check command
    // no strcmp, because stdlib is disabled
    bool isSetLedComm =
        Buffer_[0] == 's' &&
        Buffer_[1] == 'e' &&
        Buffer_[2] == 't' &&
        Buffer_[3] == '-' &&
        Buffer_[4] == 'l' &&
        Buffer_[5] == 'e' &&
        Buffer_[6] == 'd' &&
        Buffer_[7] == ' ';

    bool isEchoComm =
        Buffer_[0] == 'e' &&
        Buffer_[1] == 'c' &&
        Buffer_[2] == 'h' &&
        Buffer_[3] == 'o' &&
        Buffer_[4] == ' ';

    // command incorrect
    if (!isEchoComm && !isSetLedComm) {
        Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
        return false;
    }

    if (isSetLedComm) {
        // led number is index 8, either 0 or 1
        uint32 ledIndex = Buffer_[8] - '0';

        // index incorrect
        if (ledIndex < 0 || ledIndex > 1) {
            Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
            return false;
        }

        // check comma on index 9
        if (Buffer_[9] != ',') {
            Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
            return false;
        }

        // convert number and check
        uint32 timeMS = atoi(&Buffer_[10]);
        Uart_.Send(&Buffer_[10], 5);

        // incorrect time or malformed query
        if (timeMS < 1 || timeMS >= 5000) {
            Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
            return false;
        }

        // all correct
        Uart_.Send(RESP_OK, RESP_OK_LEN);
        SetLED_(ledIndex, timeMS);

    } else {

        // convert length and check, starts at index 5
        uint32 dataLen = atoi(&Buffer_[5]);

        // incorrect len or malformed query
        if (dataLen > 300) {
            Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
            return false;
        }

        if (dataLen == 0 && Buffer_[6] != ',') {
            // someone decided to cheat
            Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
            return false;

        } else if (dataLen == 0) {
            // all ok, buffer not needed
            Echo_(Buffer_, 0);
            return true;
        }

        // find coma
        char_pos = 6;
        while (Buffer_[char_pos] != ',' && char_pos < recvd) {
            char_pos++;
        }

        // no coma found or query ends with coma (must end with \r, internal issue of UART or buffer then)
        if (char_pos == recvd) {
            Uart_.Send(RESP_ERROR, RESP_ERROR_LEN);
            return false;
        }

        // move to the first buffered char
        char_pos++;

        Echo_(&Buffer_[char_pos], dataLen);
    }

    return true;
}

void Processor::SetLED_(uint8 led_id, uint16 ms)
{
    static char             ledOffMsg[] = "led-off: 0\r\n";
    static constexpr uint32 ledIDPos    = 9;

    // will be either 0 or 1
    ledOffMsg[ledIDPos] = led_id + '0';

    if (ms == 0) {
        Uart_.Send(ledOffMsg, sizeof(ledOffMsg) - 1);
        return;
    }

    // set timer cycle
    TIM2->ARR = ms - 1;

    // reset timer
    TIM2->CNT = 0;

    if (led_id == 0) {
        Led1_.On();
    } else {
        Led2_.On();
    }

    // clear the interrupt flag (UIF)
    CLEAR_BIT(TIM2->SR, TIM_SR_UIF);

    // start counting
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);

    // wait until the interrupt flag is set
    while (!READ_BIT(TIM2->SR, TIM_SR_UIF)) {
        __NOP();
    }

    // stop the timer and clear the flag for the next cycle
    CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);
    CLEAR_BIT(TIM2->SR, TIM_SR_UIF);

    if (led_id == 0) {
        Led1_.Off();
    } else {
        Led2_.Off();
    }

    Uart_.Send(ledOffMsg, sizeof(ledOffMsg) - 1);
}

void Processor::Echo_(char* data, uint32 len)
{
    static char ECHO_PREFIX[] = "data: ";

    Uart_.Send(ECHO_PREFIX, sizeof(ECHO_PREFIX) - 1);
    if (len > 0) {
        Uart_.Send(data, len);
    }

    char buf[3] = "\r\n";
    Uart_.Send(buf, 2);

    Uart_.Send(RESP_OK, RESP_OK_LEN);
}