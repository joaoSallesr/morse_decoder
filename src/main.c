#include "header.h"

static bool flag_timer = false;
static bool flag_input = false;

interrupt[TIM1_COMPA] void timer1_compa_isr(void) {
    TCNT1      = 0x00;
    flag_timer = true;
}

char rx_buffer[RX_BUFFER_SIZE];

unsigned char rx_wr_index, rx_rd_index, rx_counter;

// This flag is set on USART Receiver buffer overflow
bit rx_buffer_overflow;

// USART Receiver interrupt service routine
interrupt[USART_RXC] void usart_rx_isr(void) {
    char status, data;
    status = UCSRA;
    data   = UDR;
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0) {
        if (data == '\n') {
            data       = '\0';
            flag_input = 1;
        }
        rx_buffer[rx_wr_index++] = data;
        if (rx_wr_index == RX_BUFFER_SIZE)
            rx_wr_index = 0;

        if (++rx_counter == RX_BUFFER_SIZE) {
            rx_counter         = 0;
            rx_buffer_overflow = 1;
        }
    }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used +
char getchar(void) {
    char data;
    while (rx_counter == 0)
        continue;
    data = rx_buffer[rx_rd_index++];
    if (rx_rd_index == RX_BUFFER_SIZE)
        rx_rd_index = 0;

#asm("cli")
    --rx_counter;
#asm("sei")
    return data;
}
#pragma used -
#endif

// USART Transmitter buffer
char tx_buffer[TX_BUFFER_SIZE];

unsigned char tx_wr_index, tx_rd_index, tx_counter;

// USART Transmitter interrupt service routine
interrupt[USART_TXC] void usart_tx_isr(void) {
    if (tx_counter) {
        --tx_counter;
        UDR = tx_buffer[tx_rd_index++];
        if (tx_rd_index == TX_BUFFER_SIZE)
            tx_rd_index = 0;
    }
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used +
void putchar(char c) {
    while (tx_counter == TX_BUFFER_SIZE)
        continue;
#asm("cli")
    if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY) == 0)) {
        tx_buffer[tx_wr_index++] = c;

        if (tx_wr_index == TX_BUFFER_SIZE)
            tx_wr_index = 0;

        ++tx_counter;
    } else
        UDR = c;
#asm("sei")
}
#pragma used -
#endif

// morse
morse_table_t MORSE_TABLE[] = {
    {'A', ".-"},   {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},  {'E', "."},    {'F', "..-."}, {'G', "--."},
    {'H', "...."}, {'I', ".."},   {'J', ".---"}, {'K', "-.-"},  {'L', ".-.."}, {'M', "--"},   {'N', "-."},
    {'O', "---"},  {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."},  {'S', "..."},  {'T', "-"},    {'U', "..-"},
    {'V', "...-"}, {'W', ".--"},  {'X', "-..-"}, {'Y', "-.--"}, {'Z', "--.."}, {' ', "/"},
};
const size_t MORSE_TABLE_LEN = sizeof(MORSE_TABLE) / sizeof(MORSE_TABLE[0]);

// encoder variables
char input_string[256];
char encoded_signal[256];

// decoder variables
uint8_t current_time;
uint8_t current_state = DEFAULT;
char    output_signal[8];
char    decoded_string[64];

int main(void) {
    setup_reg();
    lcd_init(16);

    /* application loop */
    while (1) {
        // if button is pressed beep buzzer
        BUZZER_GPIO = BUTTON1_GPIO;

        // encode input logic
        if (flag_input) {
#asm("cli")
            strcpy(input_string, rx_buffer);
            flag_input  = false;
            rx_wr_index = 0;
            rx_counter  = 0;
#asm("sei")
            encode_machine();
        }

        // decode input logic
        if (flag_timer) {
            flag_timer = false;

            decode_machine(&current_state);
        }
    }
}