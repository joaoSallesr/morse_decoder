#include "header.h"

void encode_machine() {
    bool morse_found = false;

    uint8_t pos = 0;

    printf("Input: %s\r\n", input_string);

    // encoder logic
    for (pos = 0; encoded_signal[pos] != '\0'; pos++) {
        for (idx = 0; idx < MORSE_TABLE_LEN; idx++) {
            if (input_string[pos] == MORSE_TABLE[idx].letter) {
                printf("Morse: %c", MORSE_TABLE[idx].morse);
                strcat(*encoded_signal, MORSE_TABLE[idx].morse);
                morse_found = true;
                break;
            }
        }
    }

    if (!morse_found) {
        printf("Encoder failed!");
        return;
    }

    printf("Encoded: %s\r\n", encoded_signal);

    // buzzer logic
    for (pos = 0; encoded_signal[pos] != '\0'; pos++) {
        if (encoded_signal[pos] == DOT)
            beep(DOT);

        if (encoded_signal[pos] == DASH)
            beep(DASH);

        if (encoded_signal[pos] == SPACE)
            beep(SPACE);
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
        strcat(*output_signal, DOT);
        *state = WAIT_INPUT;
        break;

    case DASH_INPUT:
        printf("-");
        strcat(*output_signal, DASH);
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
        if (current_time == INPUT_DURATION) {
            *state = DECODE_SIGNAL;

            current_time = 0;
        }
        break;

    // decode entire signal and return a letter
    case DECODE_SIGNAL:
        bool letter_found = false;
        printf("Input: %s\r\n", output_signal);
        for (idx = 0; idx < MORSE_TABLE_LEN; idx++) {
            if (output_signal == MORSE_TABLE[idx].morse) {
                printf("Letter: %c", MORSE_TABLE[idx].letter);
                strcat(*decoded_string, MORSE_TABLE[idx].letter);
                letter_found = true;
                break;
            }
        }

        if (!letter_found)
            printf("Decoder failed!");

        *state = DEFAULT;
        break;

    // print the decoded word
    case PRINT_WORD:
        printf("%s\r\n", decoded_string);
        strcpy(*decoded_string, EMPTY);

        *state = DEFAULT;
        break;

    default:
        break;
    }
}