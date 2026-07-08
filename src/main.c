#include "header.h"

static bool flag_timer = false;
static bool flag_input = false;

morse_table_t MORSE_TABLE[] = {
    {'A', ".-"},   {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},  {'E', "."},    {'F', "..-."}, {'G', "--."},
    {'H', "...."}, {'I', ".."},   {'J', ".-"},   {'K', ".-.-"}, {'L', ".-.."}, {'M', "--"},   {'N', "-."},
    {'O', "---"},  {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."},  {'S', "..."},  {'T', "-"},    {'U', "..-"},
    {'V', "...-"}, {'W', ".--"},  {'X', "-..-"}, {'Y', "-.--"}, {'Z', "--.."}, {' ', " / "},
};

const size_t MORSE_TABLE_LEN = sizeof(MORSE_TABLE) / sizeof(MORSE_TABLE[0]);

// encoder variables
uint8_t *input_string   = "";
uint8_t *encoded_string = "";

// decoder variables
uint8_t  current_state = 0;
uint8_t *current_time  = 0;
uint8_t *output_signal = "";

interrupt[TIM1_COMPA] void timer1_compa_isr(void) {
    TCNT1      = 0x00;
    flag_timer = true;
}

interrupt[USART_RXC] void usart_rx_isr(void) {}

int main(void) {
    setup_reg();

    /* application loop */
    while (1) {
        // if button is pressed beep buzzer
        BUZZER_GPIO = BUTTON_GPIO;

        // encode input logic
        if (flag_input) {
            flag_input = false;

            encode_machine();
        }

        // decode input logic
        if (flag_timer) {
            flag_timer = false;

            decode_machine(&current_state);
        }
    }
}