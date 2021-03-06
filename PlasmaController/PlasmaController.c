/*
 * PlasmaController.cpp
 *
 * Created: 30.10.2016 18:03:13
 *  Author: Serhg
 */ 

#include "conf.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include "lcd-library.h"
#include "encoder.h"
#include "custom_symbols.h"
#include "millis.h"

unsigned long warm_delay;
float set_delay;

bool emerg_stop = 0;
volatile bool torch_on = 0;
volatile bool arc_on_out = 0;
volatile bool rele_out = 0;
volatile bool manual_mode = 0;

volatile unsigned long delay_start_time;
volatile short delay_start_ena = 0; //0 - ?????????, 1- ???????, 2 - ??????

char stringOne[17];
char stringTwo[17];
short display_mutex;
volatile short display_changed;

//??????? ?????????? ???????? ?????????? INT0- ARC_ON-????
ISR( INT0_vect )
{
	if (PIND & (1 << PIND2))
	{
		manual_mode = 1;
		display_changed|=4;
	}
	else
	{
		manual_mode = 0;
		display_changed|=4;
	}
}
//??????? ?????????? ???????? ?????????? INT1- torch_ON-????
ISR( INT1_vect )
{
	if (PIND & (1 << PIND3))
	{
		torch_on = 1;
		delay_start_ena = 1;
		millis_reset();
		display_changed|=4;
	}
	else
	{
		torch_on = 0;
		delay_start_ena = 0;
		arc_on_out=0;
		rele_out = 0;
		display_changed|=4;
	}
}

//int analogRead(int An_pin)
//{
	//ADMUX=An_pin;   
	//_delay_us(10);	  
	//ADCSRA=B11000110;	//B11000111-125kHz B11000110-250kHz 
	//while (ADCSRA & (1 << ADSC));
	//An_pin = ADCL;
	//int An = ADCH; 
	//return (An<<8) + An_pin;
//}
void displayRefrash()
{
	//???? ??? ?? ??????????, ???? ???? ?????????? ???????? ??????
	if ((display_changed)||(delay_start_ena>0))
	if (!display_mutex)
	{
		//lcdClear();
		if ((delay_start_ena==1)&&(manual_mode==0))
		{
			int d1=warm_delay-(float)millis()/1000;

			//float f1 = warm_delay-millis()/1000;
			//int d1 = f1;            // Get the integer part (678).
			//float f2 = f1 - d1;     // Get fractional part (678.0123 - 678 = 0.0123).
			//int d2 = trunc(f2 * 10);   // Turn into integer (123).
			sprintf(stringOne, " PO PEB:%3d %3d ", (int)warm_delay, d1);
		}
		else if (delay_start_ena==2)
		{
			//int d1=set_delay-(float)millis()/1000;

			float f1 = set_delay-(float)millis()/1000;
			int d1 = f1;            // Get the integer part (678).
			float f2 = f1 - d1;     // Get fractional part (678.0123 - 678 = 0.0123).
			int d2 = trunc(f2 * 10);   // Turn into integer (123).
			sprintf(stringOne, " PO PEB:%3d  %1d.%1d", (int)warm_delay, d1, d2);
		}
		else
			sprintf(stringOne, " PO PEB:%3d     ", (int)warm_delay);
		stringOne[0]=P_CHAR_CODE;
		stringOne[3]=G_CHAR_CODE;
		//lcdPuts(stringOne);
		//lcdGotoXY(1,0);
		int d1 = set_delay;            // Get the integer part (678).
		float f2 = set_delay - d1;     // Get fractional part (678.0123 - 678 = 0.0123).
		int d2 = trunc(f2 * 10);   // Turn into integer (123).
		sprintf(stringTwo, " PO   :%1d.%01d      ", d1, d2);
		stringTwo[0]=P_CHAR_CODE;
		stringTwo[3]=J_CHAR_CODE;
		stringTwo[4]=I_CHAR_CODE;
		stringTwo[5]=G_CHAR_CODE;
		//lcdPuts(stringOne);
		if (arc_on_out)
		{
			//lcdGotoXY(1,11);
			stringTwo[15]=CHAR_THUNDER_CODE;
			//stringOne[1]=0;
			//lcdPuts(stringOne);
		}
		if (torch_on)
		{
			//lcdGotoXY(1,14);
			stringTwo[13]=TORCH_CHAR_CODE;
			//stringOne[1]=0;
			//lcdPuts(stringOne);
		}
		if (delay_start_ena==1)
		{
			//lcdGotoXY(0,11);
			stringTwo[11]=WARM_CHAR_CODE;
			//stringOne[1]=0;
			//lcdPuts(stringOne);
		}
		if (delay_start_ena==2)
		{
			//lcdGotoXY(0,11);
			stringTwo[11]=BURN_CHAR_CODE;
			//stringOne[1]=0;
			//lcdPuts(stringOne);
		}
		if((emerg_stop)&&(delay_start_ena!=2))
		{
			//lcdGotoXY(0,14);
			stringOne[13]='E';
			//stringOne[1]=0;
			//lcdPuts(stringOne);
		}
		if((manual_mode)&&(delay_start_ena!=2))
		{
			//lcdGotoXY(0,14);
			stringOne[15]='P';
			//stringOne[1]=0;
			//lcdPuts(stringOne);
		}
		stringOne[16]=0;
		stringTwo[16]=0;
		if (!lcdGotoXY(0,0))
		{
			lcdInit();
			display_changed = 1;
		}
		lcdPuts(stringOne);
		if (!lcdGotoXY(1,0))
		{
			lcdInit();
			display_changed = 1;
		}
		lcdPuts(stringTwo);
		display_changed=0;
	}
}


ISR (TIMER1_COMPA_vect)
{
	displayRefrash();
}

void init()
{
	wdt_enable(WDTO_500MS);
	
	lcdInit();
	
	warm_delay=((int)eeprom_read_word(0));
	set_delay=((float)eeprom_read_word(3))/10;
	if ((warm_delay>MAX_WARM_DELAY)||(warm_delay<MIN_WARM_DELAY))
	{
		warm_delay = 140;
		eeprom_write_word(0, (uint16_t)(warm_delay));
	}
	if ((set_delay>MAX_DELAY)||(set_delay<MIN_DELAY))
	{
		set_delay = 1.0;
		eeprom_write_word(3, (uint16_t)(set_delay*10));
	}
	
	//????????????? ??????? ???????
	millis_init();
	millis_resume();
	
	//?????? ?????????? ???????
    OCR1A = 0xFA0;
    TCCR1B |= (1 << WGM12);
    // Mode 4, CTC on OCR1A
    TIMSK1 |= (1 << OCIE1A);
    //Set interrupt on compare match
    TCCR1B |= (1 << CS12) | (1 << CS10);
    // set prescaler to 1024 and start the timer
	
	//????? ?????????? ????????
	SetBit(TORCH_ON_OUT_DDR, TORCH_ON_OUT);
	ClearBit(TORCH_ON_OUT_PORT, TORCH_ON_OUT);
	//????? ???-??
	SetBit(ARC_ON_OUT_DDR, ARC_ON_OUT);
	SetBit(ARC_ON_OUT_PORT, ARC_ON_OUT);
	//????? E_STOP
	SetBit(E_STOP_OUT_DDR, E_STOP_OUT);
	SetBit(E_STOP_OUT_PORT, E_STOP_OUT);
	//????? UP
	SetBit(UP_OUT_DDR, UP_OUT);
	SetBit(UP_OUT_PORT, UP_OUT);
	//????? DOWN
	SetBit(DOWN_OUT_DDR, UP_OUT);
	SetBit(DOWN_OUT_PORT, UP_OUT);
	
	//????????????? ????????? ????????
	if (PIND & (1 << PIND3))
		torch_on = 1;
	else
		torch_on = 0;
	if (PIND & (1 << PIND2))
		manual_mode = 1;
	else
		manual_mode = 0;
	
	//???? E_STOP
	ClearBit(E_STOP_IN_DDR, E_STOP_IN);
	SetBit(E_STOP_IN_PORT, E_STOP_IN);
	
	//???? INT0
	ClearBit(DDRD, 2);
	SetBit(PORTD, 2);
	//???? INT1
	ClearBit(DDRD, 3);
	SetBit(PORTD, 3);
	//?????????? ??? ???? ISCxx
	EICRA &= ~( (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00) );
	//??????????? ?? ???????????? ?? ?????? ?????????
	EICRA |= (1<<ISC00)|(1<<ISC10);
	//????????? ??????? ??????????
	EIMSK |= (1<<INT0)|(1<<INT1);;
	
	ENC_InitEncoder();
	
	lcdLoadCharacterf(CHAR_THUNDER_CODE, char_thunder);
	lcdLoadCharacterf(TORCH_CHAR_CODE, torch_char);
	lcdLoadCharacterf(WARM_CHAR_CODE, warm_char);
	lcdLoadCharacterf(BURN_CHAR_CODE, burn_char);
	lcdLoadCharacterf(P_CHAR_CODE, p_char);
	lcdLoadCharacterf(G_CHAR_CODE, g_char);
	lcdLoadCharacterf(J_CHAR_CODE, j_char);
	lcdLoadCharacterf(I_CHAR_CODE, i_char);
	
	display_mutex = 0;
	display_changed = 3;
	sei();
}


void encoderProcess()
{
	unsigned char rotation = ENC_PollEncoder();
	if (rotation==RIGHT_SPIN)
	if (warm_delay>MIN_WARM_DELAY) 
	{
		warm_delay--;
		display_changed|=1;
		eeprom_write_word(0, warm_delay);
	}
	if (rotation==LEFT_SPIN)
	if (warm_delay<MAX_WARM_DELAY) 
	{
		warm_delay++;
		display_changed|=1;
		eeprom_write_word(0, warm_delay);
	}
		
	rotation = ENC_PollEncoderT();
	if (rotation==RIGHT_SPIN)
	if (set_delay>MIN_DELAY) 
	{
		set_delay-=0.1;
		display_changed|=2;
		eeprom_write_word(3, (uint16_t)(set_delay*10));
	}
	if (rotation==LEFT_SPIN)
	if (set_delay<MAX_DELAY) 
	{
		set_delay+=0.1;
		display_changed|=2;
		eeprom_write_word(3, (uint16_t)(set_delay*10));
	}
}

void logicProcess()
{
	if (torch_on)
	{
		if (!delay_start_ena&&!arc_on_out)
		{
			delay_start_ena=1;
			millis_reset();
		}
		//???? ?????? ?????? ???????? ???????? - ?????????? ?????? ?????? ? ????????? ?? ???????
		if(emerg_stop)
		{
			if (delay_start_ena==1)
			{
				delay_start_ena=2;
				millis_reset();
				rele_out=1;
				display_changed|=4;
			}
		}
		//????? ???????? ?? ??????? - ???????? ???? ? ???????? ?? ?????? ?????? ?? ? ?????? ??????
		if ((delay_start_ena==1)&&(manual_mode==0)&&(millis()>warm_delay*1000))
		{
			delay_start_ena=2;
			millis_reset();
			rele_out=1;
			display_changed|=4;
		}
		//????? ???????? ?? ?????? - ?????????? ????? ???-??
		if ((delay_start_ena==2)&&(millis()>set_delay*1000))
		{
			delay_start_ena=0;
			arc_on_out=1;
			display_changed|=4;
		}
	}
	else
	{
		rele_out=0;
		arc_on_out=0;
		delay_start_ena=0;
	}
}

void setStates()
{
	if (rele_out)	SetBit(TORCH_ON_OUT_PORT, TORCH_ON_OUT);
	else 				ClearBit(TORCH_ON_OUT_PORT, TORCH_ON_OUT);
	if (arc_on_out)			ClearBit(ARC_ON_OUT_PORT, ARC_ON_OUT);
	else 				SetBit(ARC_ON_OUT_PORT, ARC_ON_OUT);
	if (!emerg_stop)		ClearBit(E_STOP_OUT_PORT, E_STOP_OUT);
	else 				SetBit(E_STOP_OUT_PORT, E_STOP_OUT);
}

void readStates()
{
	if (E_STOP_IN_PIN & (1 << E_STOP_IN))
	{
		if (emerg_stop) display_changed|=32;
		emerg_stop = 0;
	}
	else
	{
		if (!emerg_stop) display_changed|=32;
		emerg_stop = 1;
	}
}

int main(void)
{
	init();
	
	while(1)
	{
		wdt_reset();
		encoderProcess();
		readStates();
		logicProcess();
		setStates();
		//displayRefrash();
	}
	

}