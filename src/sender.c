#include "header.h"

char letter_list[TABLE_SIZE] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

char *morse_list[TABLE_SIZE] = {".-",  "-...", "-.-.", "-..",  ".",   "..-.", "--.",  "....", "..",
                                ".-",  ".-.-", ".-..", "--",   "-.",  "---",  ".--.", "--.-", ".-.",
                                "...", "-",    "..-",  "...-", ".--", "-..-", "-.--", "--..", "/"};
// Máquina de estados
char estado = 0;

// Flags interrupt 
bool flag_timer = 0;
bool flag_botao = 0;

// Timer até TIMER_DESEJADO
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
                    putchar(' ');
                    putchar('\n');

                    delay_ms(500);
                }

                break;

            case COUNT_TIME:
                timer_counter++;

                // Quando o timer bater, verifica o estado do botão 0
                if (timer_counter >= TIMER_DESEJADO) {
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

                //printf(".");
                strcat(morse_input, ".");

                estado = SOLTA_BOTAO;
                break;

            case TRACO:
                flag_botao = 0;

                //printf("-");
                strcat(morse_input, "-");

                estado = SOLTA_BOTAO;

                break;
                
            case SOLTA_BOTAO:
                if (PIND.2 == 1) {
                    flag_botao = 0;
                    
                    estado = AGUARDA_CLIQUE;
                }
                
                break;

            case AGUARDA_CLIQUE:
                timer_counter++;
                
                // Se for o botão 0 for pressionado no intervalo, recomeça o loop de input
                if (flag_botao == 1) {
                    timer_counter = 0;
                
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