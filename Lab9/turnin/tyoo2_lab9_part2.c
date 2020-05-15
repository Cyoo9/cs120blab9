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


enum State {Start, Init, Toggle, Down, Up, Hold} state;
double scale[8] = {261.33, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char index = 0;
unsigned char toggleVar = 1;

void Tick() {
	switch(state) {
	case Start:
		state = Init;
		break;
	case Init:
		if((~PINA & 0x07) == 1) { 
			state = Up;
		}
		else if((~PINA & 0x07) == 2) {
			state = Down;
		}
		else if((~PINA & 0x07) == 4) {
			state = Toggle; 
		}
		else { state = Init; }
		break;
	case Up:
		if(index < 7) {
			index = index + 1;
		}
		if(toggleVar == 1) {
			set_PWM(scale[index]);
		}
		state = Hold;
		break;
	case Down:
		if(index > 0) {
			index = index - 1;
		}
		if(toggleVar == 1) {
			set_PWM(scale[index]);
		}
		state = Hold;
		break;
	case Toggle:
		if(toggleVar == 1) {
			toggleVar = 0;
			set_PWM(0);
		 //turn it off
		}
		else { 
			set_PWM(scale[index]);
			toggleVar = 1;
		}
		state = Hold;
		break;
	case Hold:
		if((~PINA &  0x07) == 0) {
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
	state = Start;
    while (1) {
	Tick();

    }
    return 0;
}
