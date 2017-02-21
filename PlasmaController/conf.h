/*
 * IncFile1.h
 *
 * Created: 01.11.2016 0:27:42
 *  Author: Serhg
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_


#define F_CPU 16000000UL

#define MAX_WARM_DELAY 300
#define MIN_WARM_DELAY 0
#define MAX_DELAY 5
#define MIN_DELAY 0

#define TORCH_ON_OUT_DDR DDRB
#define TORCH_ON_OUT_PORT PORTB
#define TORCH_ON_OUT 5
#define ARC_ON_OUT_DDR DDRB
#define ARC_ON_OUT_PORT PORTB
#define ARC_ON_OUT 4
#define E_STOP_OUT_DDR DDRB
#define E_STOP_OUT_PORT PORTB
#define E_STOP_OUT 3
#define UP_OUT_DDR DDRD
#define UP_OUT_PORT PORTD
#define UP_OUT 0
#define DOWN_OUT_DDR DDRD
#define DOWN_OUT_PORT PORTD
#define DOWN_OUT 1

#define E_STOP_IN_DDR DDRC
#define E_STOP_IN_PORT PORTC
#define E_STOP_IN_PIN PINC
#define E_STOP_IN 1

//���� � ������ � ������� ��������� ������� ��������� ����������
#define PORT_Enc	PORTD
#define PIN_Enc 	PIND
#define DDR_Enc 	DDRD
#define Pin1_Enc 	6
#define Pin2_Enc 	7
//���� � ������ � ������� ��������� ������� ��������� ��������
#define PORT_EncT 	PORTD
#define PIN_EncT 	PIND
#define DDR_EncT 	DDRD
#define Pin1_EncT 	4
#define Pin2_EncT 	5

/*
	����� ������������ ������ �����������, ������������ � LCD. ������ ������
	������ �������� ���� ����. ������� ������� �����. ����� �� ����� �����
	������ ������������� ����������� ������. ���� � ����� �������, �� �����-
	������ ������.
*/ 

#define LCDDATAPORT			PORTC					// ���� � ����,
#define LCDDATADDR			DDRC					// � ������� ����������
#define LCDDATAPIN			PINC					// ������� D4-D7.
#define LCD_D4				2
#define LCD_D5				3
#define LCD_D6				4
#define LCD_D7				5

#define LCDCONTROLPORT		PORTB					// ���� � ����,
#define LCDCONTROLDDR		DDRB					// � ������� ����������
#define LCD_RS				0						// ������� RS, RW � E.
#define LCD_RW				1
#define LCD_E				2




#endif /* INCFILE1_H_ */