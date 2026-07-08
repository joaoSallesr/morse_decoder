#include "header.h"

void setup_reg() {
    // Input/Output Ports initialization
    // Port A initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
    PORTA = 0x01;
    DDRA  = 0x01;

    // Port B initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
    PORTB = 0x00;
    DDRB  = 0x03;

    // Port C initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
    PORTC = 0x00;
    DDRC  = 0x00;

    // Port D initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
    PORTD = 0x00;
    DDRD  = 0x00;

    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: Timer 0 Stopped
    // Mode: Normal top=0xFF
    // OC0 output: Disconnected
    TCCR0 = 0b00000101;
    TCNT0 = 0x00;
    OCR0  = 143;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: Timer1 Stopped
    // Mode: Normal top=0xFFFF
    // OC1A output: Discon.
    // OC1B output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
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
    // Clock source: System Clock
    // Clock value: Timer2 Stopped
    // Mode: Normal top=0xFF
    // OC2 output: Disconnected
    ASSR  = 0x00;
    TCCR2 = 0x00;
    TCNT2 = 0x00;
    OCR2  = 0x00;

    // External Interrupt(s) initialization
    // INT0: On
    // INT1: Off
    // INT2: Off
    MCUCR  = 0b00000010;
    MCUCSR = 0x00;
    GICR   = 0b01000000;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK = 0b00010000;

    // USART initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART Receiver: On
    // USART Transmitter: On
    // USART Mode: Asynchronous
    // USART Baud Rate: 19200
    UCSRA = 0x00;
    UCSRB = 0xD8;
    UCSRC = 0x86;
    UBRRH = 0x00;
    UBRRL = 0x2F;

    // Analog Comparator initialization
    // Analog Comparator: Off
    // Analog Comparator Input Capture by Timer/Counter 1: Off
    ACSR  = 0x80;
    SFIOR = 0x00;

    // ADC initialization
    // ADC disabled
    ADCSRA = 0x00;

    // SPI initialization
    // SPI disabled
    SPCR = 0x00;

    // TWI initialization
    // TWI disabled
    TWCR = 0x00;

#asm("sei") /* Global enable interrupts */
}

void beep(uint8_t type) {

    if (type == DOT) {
        delay_ms(BUZZER_DELAY);
        BUZZER_GPIO = 0;
        delay_ms(BUZZER_DELAY);
        BUZZER_GPIO = 1;
    }

    if (type == DASH) {
        delay_ms(BUZZER_DELAY);
        BUZZER_GPIO = 0;
        delay_ms(3 * BUZZER_DELAY);
        BUZZER_GPIO = 1;
    }

    if (type == SPACE) {
        delay_ms(6 * BUZZER_DELAY);
    }
}