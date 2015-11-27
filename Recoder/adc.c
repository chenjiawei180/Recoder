#include "adc.h"
#include "usart.h"
#include "wch372.h"

uint16_t sum_adc_value = 0;
uint8_t  tel_state = 2;

void InitADC()
{
	P1ASF = 0x03;                   //Open 8 channels ADC function
	ADC_RES = 0;                    //Clear previous result
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
	Delay(2);                       //ADC power-on and delay
}

unsigned char GetADCResult(BYTE ch)
{
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
	_nop_();                        //Must wait before inquiry
	_nop_();
	_nop_();
	_nop_();
	while (!(ADC_CONTR & ADC_FLAG));//Wait complete flag
	ADC_CONTR &= ~ADC_FLAG;         //Close ADC

	return ADC_RES;                 //Return ADC result
}

void Delay(WORD n)
{
	WORD x;

	while (n--)
	{
		x = 5000;
		while (x--);
	}
}

unsigned char GetADCResult_av(BYTE ch)
{
	unsigned char temp = 0, value = 0, i = 0;
	for (i = 0; i<16; i++)
	{
		value = GetADCResult(ch);
		sum_adc_value += value;
	}

	temp = (unsigned char)(sum_adc_value >> 4);
	sum_adc_value = 0;
	return temp;
}

void tel_state_process(void) //1 挂机   2无线路  3摘机  
{
	unsigned char adc_temp = 0;
	adc_temp = GetADCResult_av(0);
	if (adc_temp > 200 && tel_state != 1 && P41 == 1)
	{
		wch372_send2byte(0xA1, 0x01, 0x01);
		tel_state = 1;
#ifdef DEBUG
		uart_printf("the telephone is offline \r\n");
#endif
	}
	else if (adc_temp < 10 && tel_state != 2 && P41 == 1)
	{
		wch372_send2byte(0xA1, 0x01, 0x02);
		tel_state = 2;
#ifdef DEBUG
	   uart_printf("no telephone \r\n");
#endif
	}
	else if (adc_temp >20 && adc_temp < 40 && tel_state != 3 && P41 == 1)
	{
		wch372_send2byte(0xA1, 0x01, 0x03);
		tel_state = 3;
#ifdef DEBUG
		uart_printf("the telephone is online \r\n");
#endif
	}
}