#include "header.h"

// USART setup ===================================================================================================

bool flag_usart = 0;

// USART Receiver buffer
char rx_buffer[RX_BUFFER_SIZE];

// USART Transmitter buffer
char tx_buffer[TX_BUFFER_SIZE];

unsigned char rx_wr_index, rx_rd_index, rx_counter;
unsigned char tx_wr_index, tx_rd_index, tx_counter;

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
            flag_usart = 1;
        }

        rx_buffer[rx_wr_index++] = data;
        if (rx_wr_index == RX_BUFFER_SIZE)
            rx_wr_index = 0;

        if (++rx_counter == RX_BUFFER_SIZE) {
            rx_counter = 0;

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

// USART setup ===================================================================================================

void main(void) {
    int idx_io = 0;

    // Bytes recebidos
    char input_string[65];

    // String impressa no LCD
    char lcd_string[33];

    input_string[0] = '\0';
    lcd_string[0]   = '\0';

    // Inicializa os registradores com a configuração desejada
    reg_init();

    lcd_init(16);
    PORTC.3 = 0;

    while (1) {

        // Se o botão 0 for pressionado, limpa o LCD
        if (PINA.0 == 0) {
            strcpy(lcd_string, "");

            // Reseta o LCD
            lcd_gotoxy(0, 0);
            lcd_clear();

            // Atualiza o LCD
            lcd_puts(lcd_string);

            delay_ms(500);
        }

        if (flag_usart == 1) {
            strcpy(input_string, rx_buffer);

            flag_usart  = 0;
            rx_wr_index = 0;
            rx_counter  = 0;

            if (input_string[0] == 'M') {
                for (idx_io = 1; input_string[idx_io] != '\0'; idx_io++) {
                    // Apita tempo de um ponto
                    if (input_string[idx_io] == '.') {
                        delay_ms(BUZZER_TICK);
                        PORTA.0 = 0;
                        delay_ms(BUZZER_TICK);
                        PORTA.0 = 1;
                    }

                    // Apita tempo de um traço
                    if (input_string[idx_io] == '-') {
                        delay_ms(BUZZER_TICK);
                        PORTA.0 = 0;
                        delay_ms(3 * BUZZER_TICK);
                        PORTA.0 = 1;
                    }

                    // Delay entre letras
                    if (input_string[idx_io] == ' ') {
                        delay_ms(3 * BUZZER_TICK);
                    }

                    // Delay entre palavras
                    if (input_string[idx_io] == '/') {
                        delay_ms(7 * BUZZER_TICK);
                    }
                }
            } else if (input_string[0] == 'L') {
                lcd_gotoxy(0, 0);
                lcd_clear();

                // Adiciona a nova letra ao lcd_string e imprime na tela
                if (strlen(lcd_string) < 32) {
                    strcat(lcd_string, &input_string[1]);
                    lcd_puts(lcd_string);
                    printf("%s", lcd_string);
                }
            }
        }
    }
}

void reg_init(void) {
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
    OCR1BH = 0x00;
    OCR1BL = 0x00;

    // Timer/Counter 2 initialization
    ASSR  = 0x00;
    TCCR2 = 0x00;
    TCNT2 = 0x00;
    OCR2  = 0x00;

    // External Interrupt(s) initialization
    MCUCR  = 0b00000010; // INT0 - falling edge
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

// Global enable interrupts
#asm("sei")
}