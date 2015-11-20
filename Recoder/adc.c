#include "adc.h"

uint16_t sum_adc_value = 0;

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
	for (i = 0; i<8; i++)
	{
		value = GetADCResult(ch);
		sum_adc_value += value;
	}

	temp = (unsigned char)(sum_adc_value >> 3);
	sum_adc_value = 0;
	return temp;
}