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

char letter_list[TABLE_SIZE] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

char *morse_list[TABLE_SIZE] = {".-",  "-...", "-.-.", "-..",  ".",   "..-.", "--.",  "....", "..",
                                ".-",  ".-.-", ".-..", "--",   "-.",  "---",  ".--.", "--.-", ".-.",
                                "...", "-",    "..-",  "...-", ".--", "-..-", "-.--", "--..", "/"};

char estado = 0;

bool flag_timer = 0;
bool flag_botao = 0;

int timer_counter = 0;

// Seta flag_timer a cada 10 ms
interrupt[TIM1_COMPA] void timer1_compa_isr(void) {
    TCNT1      = 0x00;
    flag_timer = 1;
}

// Seta flag_botao na falling edge do botão
interrupt[EXT_INT0] void external_int0_isr(void) { flag_botao = 1; }

void main(void) {
    int idx_io   = 0;
    int idx_list = 0;

    // Encodificação (entra letra -> sai morse)
    char letter_input[17];
    char morse_output[65];

    // Decodificação (entra morse -> sai letra)
    char morse_input[8];
    char letter_output[2];

    morse_input[0] = '\0';

    // Inicializa os registradores com a configuração desejada
    reg_init();

    while (1) {
        // Botão pressionado -> Apita o buzzer
        PORTA.0 = PIND.2;

        // MORSE ENCODER
        if (flag_usart == 1) {
            morse_output[0] = '\0';

            strcpy(letter_input, rx_buffer);
            flag_usart  = 0;
            rx_wr_index = 0;
            rx_counter  = 0;

            // Percorre o input e verifica a lista de letras para encodificar em morse
            for (idx_io = 0; letter_input[idx_io] != '\0'; idx_io++) {
                for (idx_list = 0; idx_list < TABLE_SIZE; idx_list++) {
                    if (letter_input[idx_io] == letter_list[idx_list]) {
                        strcat(morse_output, morse_list[idx_list]);
                        strcat(morse_output, " ");
                        break;
                    }
                }
            }

            // Pacote enviado
            putchar('M');
            printf("%s", morse_output);
            putchar('\n');
        }

        // MORSE DECODER
        if (flag_timer == 1) {
            flag_timer = 0;

            switch (estado) {

            case STANDBY:
                // Se o botão 0 for pressionado, começa a contar o tempo
                if (flag_botao == 1) {
                    flag_botao = 0;

                    estado = COUNT_TIME;
                }

                // Se o botão 1 for pressionado, envia espaço
                if (PIND.3 == 0) {
                    putchar('L');
                    printf(' ');
                    putchar('\n');

                    delay_ms(500);
                }

                break;

            case COUNT_TIME:
                timer_counter++;

                // Quando o timer bater, verifica o estado do botão 0
                if (timer_counter == TIMER_DESEJADO) {
                    timer_counter = 0;

                    estado = VERIFICA_BOTAO;
                }

                break;

            case VERIFICA_BOTAO:
                // Se o botão 0 ainda estiver pressionado, define como traço, caso contrário, define como ponto
                if (PIND.2 == 0) {
                    estado = TRACO;
                } else {
                    estado = PONTO;
                }

                break;

            case PONTO:
                flag_botao = 0;

                printf(".");
                strcat(morse_input, ".");

                estado = AGUARDA_CLIQUE;
                break;

            case TRACO:
                flag_botao = 0;

                printf("-");
                strcat(morse_input, "-");

                estado = AGUARDA_CLIQUE;

                break;

            case AGUARDA_CLIQUE:
                timer_counter++;

                // Se for o botão 0 for pressionado no intervalo, recomeça o loop de input
                if (flag_botao == 1) {
                    estado = STANDBY;
                }

                // Se o botão 0 não for pressionado no intervalo, decodifica o morse
                if (timer_counter >= 3 * TIMER_DESEJADO) {
                    timer_counter = 0;

                    estado = DECODIFICA;
                }

                break;

            case DECODIFICA:
                // Percorre a lista de morse e verifica com o input
                for (idx_list = 0; idx_list < TABLE_SIZE; idx_list++) {
                    if (strcmp(morse_input, morse_list[idx_list]) == 0) {
                        putchar('L');
                        printf("%c", letter_list[idx_list]);
                        putchar('\n');

                        sprintf(letter_output, "%c", letter_list[idx_list]);

                        break;
                    }
                }

                // Limpa o morse_input
                strcpy(morse_input, "");

                estado = STANDBY;

                break;
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