#include "header.h"

// change getchar()
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

// change putchar
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

void setup_reg() {
    // Port A initialization
    PORTA = 0x01;
    DDRA  = 0x01;

    // Port B initialization
    PORTB = 0x00;
    DDRB  = 0x03;

    // Port C initialization
    PORTC = 0x00;
    DDRC  = 0x08;

    // Port D initialization
    PORTD = 0x00;
    DDRD  = 0x00;

    // Timer/Counter 0 initialization
    TCCR0 = 0b00000101;
    TCNT0 = 0x00;
    OCR0  = 143;

    // Timer/Counter 1 initialization
    TCCR1A = 0b00000101; // 1024
    TCCR1B = 0b00000101; // 1024
    TCNT1H = 0x00;
    TCNT1L = 0x00;
    ICR1H  = 0x00;
    ICR1L  = 0x00;
    OCR1A  = 143;

    // OCR1AH=0x00;
    // OCR1AL=0x00;
    // OCR1BH = 0x00;
    // OCR1BL = 0x00;

    // Timer/Counter 2 initialization
    ASSR  = 0x00;
    TCCR2 = 0x00;
    TCNT2 = 0x00;
    OCR2  = 0x00;

    // External Interrupt(s) initialization
    MCUCR  = 0b00000010;
    MCUCSR = 0x00;
    GICR   = 0b01000000;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK = 0b00010000;

    // USART initialization
    UCSRA = 0x00;
    UCSRB = 0xD8;
    UCSRC = 0x86;
    UBRRH = 0x00;
    UBRRL = 0x2F;

    // Analog Comparator initialization
    ACSR  = 0x80;
    SFIOR = 0x00;

    // ADC initialization
    ADCSRA = 0x00;

    // SPI initialization
    SPCR = 0x00;

    // TWI initialization
    TWCR = 0x00;

#asm("sei")
}

void beep(char type) {

    if (type == DOT_CHAR) {
        delay_ms(BUZZER_DELAY);
        BUZZER_GPIO = 0;
        delay_ms(BUZZER_DELAY);
        BUZZER_GPIO = 1;
    }

    if (type == DASH_CHAR) {
        delay_ms(BUZZER_DELAY);
        BUZZER_GPIO = 0;
        delay_ms(3 * BUZZER_DELAY);
        BUZZER_GPIO = 1;
    }

    if (type == SPACE_CHAR) {
        delay_ms(6 * BUZZER_DELAY);
    }
}