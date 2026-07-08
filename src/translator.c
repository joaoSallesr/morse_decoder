#include "header.h"

uint8_t idx = 0;

void encode_machine() {
    strcpy(encoded_signal, EMPTY);
    uint8_t pos = 0;

    printf("Input: %s\r\n", input_string);

    // encoder logic
    for (pos = 0; input_string[pos] != '\0'; pos++) {
        for (idx = 0; idx < MORSE_TABLE_LEN; idx++) {
            if (input_string[pos] == MORSE_TABLE[idx].letter) {
                printf("Morse: %s", MORSE_TABLE[idx].morse);

                strcat(encoded_signal, MORSE_TABLE[idx].morse);

                break;
            }
        }
    }

    printf("Encoded: %s\r\n", encoded_signal);

    // buzzer logic
    for (pos = 0; encoded_signal[pos] != '\0'; pos++) {
        if (encoded_signal[pos] == DOT_CHAR)
            beep(DOT_CHAR);

        if (encoded_signal[pos] == DASH_CHAR)
            beep(DASH_CHAR);

        if (encoded_signal[pos] == SPACE_CHAR)
            beep(SPACE_CHAR);
    }
}

void decode_machine(uint8_t *state) {
    switch (*state) {

    // if btn pressed, start counting
    case DEFAULT:
        if (BUTTON1_GPIO == 0)
            *state = COUNTING;
        if (BUTTON2_GPIO == 0)
            *state = PRINT_WORD;
        break;

    // after x ms check if the btn is still pressed
    case COUNTING:
        current_time++;

        if (current_time == INPUT_DURATION) {
            *state = CHECK_BTN;

            current_time = 0;
        }
        break;

    // apply decode logic depending on signal duration
    case CHECK_BTN:
        if (BUTTON1_GPIO == 0)
            *state = DASH_INPUT;
        else
            *state = DOT_INPUT;
        break;

    case DOT_INPUT:
        printf(".");
        if (strlen(output_signal) < sizeof(output_signal) - 1)
            strcat(output_signal, DOT_STR);
        *state = WAIT_INPUT;
        break;

    case DASH_INPUT:
        printf("-");
        if (strlen(output_signal) < sizeof(output_signal) - 1)
            strcat(output_signal, DASH_STR);
        *state = WAIT_INPUT;
        break;

    // wait for a new input before decoding signal
    case WAIT_INPUT:
        current_time++;

        // new input - check signal
        if (BUTTON1_GPIO == 0) {
            *state = COUNTING;

            current_time = 0;
            break;
        }

        // no new input - end signal
        if (current_time == INPUT_DELAY) {
            *state = DECODE_SIGNAL;

            current_time = 0;
        }
        break;

    // decode entire signal and return a letter
    case DECODE_SIGNAL:
        bool letter_found = false;
        printf("Input: %s\r\n", output_signal);
        for (idx = 0; idx < MORSE_TABLE_LEN; idx++) {
            if (strcmp(output_signal, MORSE_TABLE[idx].morse) == 0) {
                printf("Letter: %c", MORSE_TABLE[idx].letter);

                char buf[2] = {MORSE_TABLE[idx].letter, '\0'};
                if (strlen(decoded_string) + 1 < sizeof(decoded_string))
                    strcat(decoded_string, buf);

                letter_found = true;
                break;
            }
        }

        if (!letter_found)
            printf("Decoder failed!");

        strcpy(output_signal, EMPTY);
        *state = DEFAULT;
        break;

    // print the decoded word
    case PRINT_WORD:
        printf("%s\r\n", decoded_string);

        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_puts(decoded_string);

        strcpy(decoded_string, EMPTY);

        *state = DEFAULT;
        break;

    default:
        break;
    }
}