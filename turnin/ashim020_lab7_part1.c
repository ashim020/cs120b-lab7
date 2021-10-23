/*	Author: Andrew Shim
 *  	Partner(s) Name: 
 *	Lab Section: 21
 *	Assignment: Lab # 7 Exercise # 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {Start, STATE} state;

unsigned char tmpA;
unsigned char count;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void Tick(){
    switch(state){ //State Transitions
        case Start:
            state = STATE;
            count = 0x00;
            break;

        case STATE:
            if ((tmpA & 0x03) == 0x02) {
                if (count < 9) {
                    count++;
                }
            }
            else if ((tmpA & 0x03) == 0x01) {
                if (count > 0) {
                    count--;
                }
            }
            else if ((tmpA & 0x03) == 0x00) {
                count = 0;
            }
            break;

        default:
            state = Start;
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;  
    DDRC = 0xFF; PORTC = 0x00; // LCD data lines
    DDRD = 0xFF; PORTD = 0x00; // LCD control lines

    TimerSet(1000);
    TimerOn();

    // Initializes the LCD display
    LCD_init();

    /* Insert your solution below */
    while (1) {
	tmpA = PINA;
        Tick();
	LCD_Cursor(1);
        LCD_WriteData(count + '0');
        while(!TimerFlag) {}; //Wait 1 second
        TimerFlag = 0;
    }
}
