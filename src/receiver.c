#include "header.h"

void main(void) {
    int idx_io = 0;
    int idx_lcd = 0;

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
        if (PIND.2 == 0) {
            strcpy(lcd_string, "");
            idx_lcd = 0;

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

            if (input_string[0] == '.' || input_string[0] == '-') {
                for (idx_io = 0; input_string[idx_io] != '\0'; idx_io++) {
                    printf("%c", input_string[idx_io]);
                    
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
                        delay_ms(2 * BUZZER_TICK);
                    }

                    // Delay entre palavras
                    if (input_string[idx_io] == '/') {
                        delay_ms(2 * BUZZER_TICK);
                    }
                }
            } else {
                printf("%c", input_string[1]);

                // Adiciona a nova letra ao lcd_string e imprime na tela
                if (strlen(lcd_string) < 32) {
                    lcd_gotoxy(0, 0);
                    lcd_clear();
                
                    lcd_string[idx_lcd] = input_string[0];
                    lcd_string[idx_lcd + 1] = '\0';
                    idx_lcd += 1;    
                    
                    for (idx_io = 0; lcd_string[idx_io] != '\0'; idx_io++) {
                        lcd_putchar(lcd_string[idx_io]);    
                    }
                    
                    //printf("String Total: %s", lcd_string);
                }
            }
        }
    }
}