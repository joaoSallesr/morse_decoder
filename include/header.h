#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

#define DOT   "."
#define DASH  "-"
#define SPACE "/"
#define EMPTY ""

/* structures */
typedef struct {
    uint8_t  letter;
    uint8_t *morse;
} morse_table_t;

/* global variables */
extern uint8_t idx;

extern morse_table_t MORSE_TABLE[];
extern const size_t  MORSE_TABLE_LEN;

extern uint8_t *input_string;
extern uint8_t *encoded_signal;

extern uint8_t  current_time;
extern uint8_t *output_signal;
extern uint8_t *decoded_string;

/* global functions */
void setup_reg();
void buzzer();

void encode_machine();
void decode_machine(uint8_t *current_state);