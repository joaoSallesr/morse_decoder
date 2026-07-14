#ifndef HEADER_H
#define HEADER_H

#include <alcd.h>
#include <delay.h>
#include <mega16.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define STANDBY        0
#define COUNT_TIME     1
#define VERIFICA_BOTAO 2
#define PONTO          3
#define TRACO          4
#define SOLTA_BOTAO    5
#define AGUARDA_CLIQUE 6
#define DECODIFICA     7

#define TABLE_SIZE     27
#define TIMER_DESEJADO 30
#define BUZZER_TICK    100 // in ms

// USART setup ===================================================================================================

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

#define FRAMING_ERROR       (1 << FE)
#define PARITY_ERROR        (1 << UPE)
#define DATA_OVERRUN        (1 << DOR)
#define DATA_REGISTER_EMPTY (1 << UDRE)
#define RX_COMPLETE         (1 << RXC)

#define RX_BUFFER_SIZE 32
#define TX_BUFFER_SIZE 32

// USART setup ===================================================================================================

// Global variables
extern bool flag_usart;
extern char rx_buffer[RX_BUFFER_SIZE];
extern char tx_buffer[TX_BUFFER_SIZE];
extern unsigned char rx_wr_index, rx_rd_index, rx_counter;
extern unsigned char tx_wr_index, tx_rd_index, tx_counter;
extern bit rx_buffer_overflow;

void reg_init(void);

#endif