#pragma once

#include <alcd.h>
#include <delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FRAMING_ERROR       (1 << FE)
#define PARITY_ERROR        (1 << UPE)
#define DATA_OVERRUN        (1 << DOR)
#define DATA_REGISTER_EMPTY (1 << UDRE)
#define RX_COMPLETE         (1 << RXC)

#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define RX_BUFFER_SIZE 32
#define TX_BUFFER_SIZE 32

#define BUZZER_GPIO  PORTA.0
#define BUTTON1_GPIO PIND.2
#define BUTTON2_GPIO PIND.3

#define DEFAULT       0
#define COUNTING      1
#define CHECK_BTN     2
#define DOT_INPUT     3
#define DASH_INPUT    4
#define WAIT_INPUT    5
#define DECODE_SIGNAL 6
#define PRINT_WORD    7

#define INPUT_DURATION 30 // input duration - dot x dash
#define INPUT_DELAY    90 // end letter after INPUT_DELAY

#define BUZZER_DELAY 100 // 100 ms

#define DOT_CHAR '.'
#define DOT_STR  "."

#define DASH_CHAR '-'
#define DASH_STR  "-"

#define SPACE_CHAR '/'
#define SPACE_STR  "/"

#define EMPTY ""

/* structures */
typedef struct {
    char  letter;
    char *morse;
} morse_table_t;

/* AVR setup variables */
extern bit rx_buffer_overflow;

extern char rx_buffer[RX_BUFFER_SIZE];
extern char tx_buffer[TX_BUFFER_SIZE];

extern unsigned char rx_wr_index, rx_rd_index, rx_counter;
extern unsigned char tx_wr_index, tx_rd_index, tx_counter;

/* global variables */
extern uint8_t idx;

extern morse_table_t MORSE_TABLE[];
extern const size_t  MORSE_TABLE_LEN;

extern char input_string[256];
extern char encoded_signal[256];

extern uint8_t current_time;
extern char    output_signal[8];
extern char    decoded_string[64];

/* global functions */
void setup_reg();
void beep(char type);

void encode_machine();
void decode_machine(uint8_t *current_state);