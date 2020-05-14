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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

enum States {Start, Off, cPressed, dPressed, ePressed} state;


void Tick() {
	switch(state) {
		case Start:
			state = Off;
			break;
		case Off:
			if((~PINA & 0x01) == 1) {
                                state = cPressed;
                        }
                        else if((~PINA & 0x02) == 2) {
                                state = dPressed;
                        }
                        else if((~PINA & 0x04) == 4) {
                                state = ePressed;
                        }

			else {
				state = Off;
			}
			break;
		case cPressed:
			if((~PINA & 0x01) == 1) {
				state = cPressed;
			}
			else {
				state = Off;
			}
			break;
		
		case dPressed:
			if((~PINA & 0x02) == 2) {
				state = dPressed;
			}
			else {
				state = Off;
			}
			break;
		case ePressed:
			if((~PINA & 0x04) == 4) {
				state = ePressed;
			}
			else {
				state = Off;
			}
			break;
		default:
			break;
	}
	switch(state) {
		case Off:
			set_PWM(0);
			break;
		case cPressed:
			set_PWM(261.63);
			break;
		case dPressed:
			set_PWM(293.66);
			break;
		case ePressed:
			set_PWM(329.63);
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
	state = Start;
	
    while (1) {
	Tick();
	
    }
    return 0;
}
