/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
*      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

void TimerOff() { TCCR1B = 0x00; }
void TimerISR() { TimerFlag = 1; }

ISR(TIMER1_COMPA_vect) { 
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency;
	if(frequency != current_frequency) {
		if(!frequency) { TCCR3B &= 0x08;}
		else {TCCR3B |= 0x03; }
		if(frequency < 0.954) { OCR3A = 0xFFFF; }
		else if(frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000/(128*frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


enum State {Start, Init, Press, Hold} state;
double scale[10] = {261.33, 293.66, 349.23, 523.25, 261.33, 440.00, 
493.88, 293.66, 493.88, 0};
unsigned char i = 0;
unsigned char counter = 0;

void Tick() {
	switch(state) {
		case Start:
			state = Init;
			break;
		case Init:
			i = 0;
			counter = 0;
			if((~PINA & 0x07) == 4) {
				state = Press;
			}
			else { state = Init; }
			break;
		case Press:
			counter++;
			if(counter <= 250) {
				set_PWM(scale[i]);
				if((counter % 25) == 0) {
					i++;
				}
				state = Press;
			}
			if(((~PINA & 0X07) == 4) && (counter == 251)) {
				state = Hold;
			}
			else if(counter == 251) {
				state = Init;
			}
			break;
		case Hold:
			if((~PINA & 0x07) == 0) {
				state = Init;
			}
			else {
				state = Hold;
			}
			break;
		default:
			break;
	}	
}
		


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0x40; PORTB = 0xBF;
	DDRA = 0x00; PORTA = 0xFF; 
    /* Insert your solution below */
	PWM_on();
	TimerSet(20);
	TimerOn();
    while (1) {
	Tick();
	while(!TimerFlag);
	TimerFlag = 0;

    }
    return 0;
}
