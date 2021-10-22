/*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Lab 6  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/u7DokuBSN0U
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {START, PB_0, PB_1_to_0, PB_1_to_2, PB_2} state;

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

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
	_avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

enum States prev;
unsigned char pause;
unsigned char tmp_A;

void Tick() {
    unsigned char tmp_B = PORTB;
    
    switch (state) {
	case START:
	    state = PB_0;
	    break;
        case PB_0:
	    if (tmp_A && !pause) {
                pause = 1;
            }
	    else if (tmp_A && pause) {
                pause = 0;
            }
	    if (!pause) {
                state = PB_1_to_2;
	    }
	    break;

	case PB_1_to_0:
	    if (tmp_A && !pause) {
                pause = 1;
            }
	    else if (tmp_A && pause) {
                pause = 0;
            }
	    if (!pause) {
	    state = PB_0;
	    }
	    break;

	case PB_1_to_2:
            if (tmp_A && !pause) {
                pause = 1;
            }
	    else if (tmp_A && pause) {
                pause = 0;
            }
	    if (!pause) {
                state = PB_2;
	    }
            break;

	case PB_2:
	    if (tmp_A && !pause) {
                pause = 1;
            }
	    else if (tmp_A && pause) {
                pause = 0;
            }
	    if (!pause) {
	        state = PB_1_to_0;
	    }
	    break;

	default:
	    break;
    }

    switch (state) {
	case START:
	    break;
        case PB_0:
            tmp_B = 0x01;
            break;
        case PB_1_to_0:
	    tmp_B = 0x02;
            break;
	case PB_1_to_2:
	    tmp_B = 0x02;
	    break;
        case PB_2:
            tmp_B = 0x04;
            break;
        default:
            break;
    }

    PORTB = tmp_B;
}



int main() {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */
    TimerSet(300);
    TimerOn();
    
    PORTB = 0x01;
    state = START;
    pause = 0;

    while (1) {
        tmp_A = (~PINA) & 0x01;

        Tick();
        
        while (!TimerFlag) { };
	TimerFlag = 0;
    }

    return 1;
}
