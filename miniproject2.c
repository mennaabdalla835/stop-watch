/*
 * miniproject2.c
 *
 *  Created on: Aug 16, 2023
 *      Author: Menna abdalla
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
/************************************************************************************************
 *                                       global variables
 ************************************************************************************************/
unsigned char sec =0;
unsigned char min =0;
unsigned char hour=0;
/************************************************************************************************
 *                                      ISR_functions
 ***********************************************************************************************/
ISR (INT0_vect)
{
	if(!(PIND & (1<<PD2)))
	{
		_delay_ms(30);
		if (!(PIND & (1<<PD2)))
		{
	       sec = min =hour=0; // reset the stop watch
	  	}
	}
}
ISR (INT1_vect)
{
	if (PIND & (1<<PD3))
	{
		_delay_ms(30);
		if(PIND & (1<<PD3))
		{
			 TCCR1B &=~(1<<CS10) &~(1<<CS11) &~(1<<CS12); // stop timer
		}
	}
}
ISR (INT2_vect)
{
	if (!(PINB &(1<<PB2)))
	{
		_delay_ms(30);
		if (!(PINB &(1<<PB2)))
		{
			TCCR1B = (1<<WGM12) |(1<<CS10) |(1<<CS12); // resume timer
		}
	}
}
ISR(TIMER1_COMPA_vect)
{
	sec++;
	if (sec == 60)
	{
		sec=0;
		min++;
	}
	if (min == 60)
	{
		min = 0;
		hour ++;
	}
	if (hour == 24)
	{
		sec = min = hour =0;
	}

}
/************************************************************************************************
 *                                      Interrupts
 ***********************************************************************************************/
void INT0_Init()
{
	DDRD &=~(1<<PD2); // PD2 as input pin
	PORTD |=(1<<PD2); // activate internal pull up
	GICR |=(1<<INT0); //enable external interrupt0
	MCUCR = (1<<ISC01); //falling edge of INT0 generates interrupt request ISC01=1 ISC00 =0
}
void INT1_Init()
{
	DDRD &=~(1<<PD3); //PD3 as input pin
	GICR |=(1<<INT1); // enable external interrupt1
	MCUCR |=(1<<ISC11) |(1<<ISC10); //rising edge of INT1 generates interrupt request ISC11=1 ISC10=1
}
void INT2_Init()
{
	DDRB &=~(1<<PB2); // PB2 as input pin
	PORTB |=(1<<PB2); // activate internal pull up
	GICR  |=(1<<INT2);// enable external interrupt1
	MCUCSR &=~(1<<ISC2); // falling edge of INT2 generates interrupt request ISC2=0
}
/************************************************************************************************
 *                                       TIMER
 ************************************************************************************************/
void TIMER1_Init()
{
	TCNT1  = 0; // timer initial value
	OCR1A  = 1000; // timer compare value to make one sec
	/* configure timer1 control reg
	 * non PWM mode FOC1A =1 FOC1B=1
	 * No need for OC1A,OC1B so COM1A1=0 COM1A0 =0 COM1B1=0 COM1B1=0
	 * CTC mode WGM10=0 WGM11=0 WGM12 =1 WGM13=0
	 */
	TCCR1A = (1<<FOC1A) |(1<<FOC1B);
	/*
	 * Prescaler =1024  CS10=1 CS11=0 CS12=1
	 */
	TCCR1B = (1<<WGM12) |(1<<CS10) |(1<<CS12);
	/*
	 * enable interrupt mode for channel A
	 */
	TIMSK  |=(1<<OCIE1A);
}

/*************************************************************************************************
 *                                       main function
 ************************************************************************************************/

int main ()
{


	DDRA |=0x3F; // first 6 pins as output
	PORTA|=0x3F; // enable all 7-segments
	DDRC |=0x0F; // first 4 pins as output
	PORTC&=0xF0; // all of them as 0 value
	SREG |=(1<<7); // enable global interrupt
	INT0_Init(); // reset interrupt
	INT1_Init(); // pause interrupt
	INT2_Init(); // resume interrupt
	TIMER1_Init(); // start timer
	while (1)
	{
		PORTA  = (1<<0);
		PORTC  = sec %10;
		_delay_ms(5);
		PORTA  = (1<<1);
		PORTC  = sec /10;
		_delay_ms(5);
		PORTA  = (1<<2);
		PORTC  = min % 10;
		_delay_ms(5);
		PORTA  = (1<<3);
		PORTC  = min /10;
		_delay_ms(5);
		PORTA  = (1<<4);
		PORTC  = hour % 10;
		_delay_ms(5);
		PORTA  = (1<<5);
		PORTC  = hour /10;
		_delay_ms(5);

	}

}
