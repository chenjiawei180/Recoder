#include "adc.h"
#include "usart.h"
#include "wch372.h"
#include "pcm1801.h"
uint16_t sum_adc_value = 0;
uint8_t  tel_state = 2;
unsigned char pre_adc_temp= 0;
uint8_t  pre_tel_state = 2;
unsigned char RingDetectFlag = 0;
unsigned char PreRingDetectFlag = 0;
unsigned char RingUpFlag = 0;
unsigned char tel_status_count = 0;
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
	unsigned char RingupLowlevel = 0;
	unsigned char RingUpFlagTime = 0;
	for (i = 0; i<16; i++)
	{
		value = GetADCResult(ch);
#ifdef DEBUG1
		uart_printf("value=%d \r\n", (unsigned int)value);
#endif
//sjz start
		if (value < 5)
		{
			RingupLowlevel = 1;
		}
		else if ((RingupLowlevel == 1) && (value >50))
		{
			RingUpFlagTime++;
			RingupLowlevel = 0;
		}
		if (RingUpFlagTime >= 2)
		{
			RingUpFlag = 1;
		}
		else
		{
			RingUpFlag = 0;
		}
//sjz end
		sum_adc_value += value;
	}
#ifdef DEBUG1
	uart_printf("RingUpFlag=%d \r\n", (unsigned int)RingUpFlag);
#endif
	temp = (unsigned char)(sum_adc_value >> 4);
	sum_adc_value = 0;
	return temp;
}


void tel_state_process(void) //1 挂机   2无线路  3摘机  
{
	unsigned char adc_temp = 0;
	adc_temp = GetADCResult_av(0);
	//uart_printf("pre_adc_temp =%d \r\n", (unsigned int)pre_adc_temp);
	//uart_printf("adc_temp =%d \r\n", (unsigned int)adc_temp);
//if (tel_state != pre_tel_state)
	//	if (((pre_adc_temp > 200) && (adc_temp<40) && (pre_tel_state == 1) && (tel_state == 1)) || 
	//		((pre_adc_temp <40) && (adc_temp>200) && (pre_tel_state == 3) && (tel_state == 3)))
		if (((pre_adc_temp > 200) && (adc_temp<40)) || 
	((pre_adc_temp <40) && (adc_temp>200)))
	{
		RingDetectFlag = 1;
	}
	//uart_printf("RingDetectFlag =%d \r\n", (unsigned int)RingDetectFlag);
	//if(RingDetectFlag == 1){
	if(P41 == 1){
	if((adc_temp > 200)&&(pre_adc_temp>180))
	{
		tel_state = 1;
		//pre_adc_temp = adc_temp;
		tel_status_count++;
	}
	else if (adc_temp < 3)
	{
		if ((pre_tel_state !=1) && (pre_tel_state != 3))
		{
			tel_state = 2;
		}
		else
		{
			RingDetectFlag = 1;
		}
		//pre_adc_temp = adc_temp;
		tel_status_count++;
	}
	else if ((adc_temp >20 && adc_temp < 40)&&(pre_adc_temp>5))
	{
		tel_state = 3;
		
		tel_status_count++;
	}
	pre_adc_temp = adc_temp;
//}
}
	if(P41 ==0)
	{
		RingDetectFlag = 1;
	}
	if((RingDetectFlag == 1))
	{
		wch372_send2byte(0xA1, 0x01, 0x04);
		if(P41 ==1)
		  RingDetectFlag = 0;
#ifdef DEBUG
		uart_printf("the telephone is Ring \r\n");
#endif
	}
	else
	{

		if(P41 == 1)
		{
		//uart_printf("pre_tel_state =%d \r\n", (unsigned int)pre_tel_state);
		//uart_printf("tel_state =%d \r\n", (unsigned int)tel_state);
		if ((tel_state != pre_tel_state)||((tel_state ==2) &&(pre_tel_state ==2))||(RingDetectFlag==0))
		{
			//if(tel_status_count >=3)
			{
			wch372_send2byte(0xA1, 0x01, tel_state);
			
#ifdef DEBUG
			if(tel_state ==1)
			   uart_printf("the telephone is offline \r\n");
			else if (tel_state == 2)
				uart_printf("no telephone \r\n");
			else if (tel_state == 3)
				uart_printf("the telephone is online \r\n");
#endif
		 }
     
		}
	 }
		pre_tel_state = tel_state;
	}
#if 0
	else if (adc_temp > 200 && tel_state != 1 && P41 == 1 && RingUpFlag !=1)
	{
		wch372_send2byte(0xA1, 0x01, 0x01);
		tel_state = 1;
		pre_tel_state = tel_state;
#ifdef DEBUG
		uart_printf("the telephone is offline \r\n");
#endif
	}
	else if (adc_temp < 10 && tel_state != 2 && P41 == 1 && RingUpFlag != 1)
	{
		wch372_send2byte(0xA1, 0x01, 0x02);
		tel_state = 2;
		pre_tel_state = tel_state;
#ifdef DEBUG
	   uart_printf("no telephone \r\n");
#endif
	}
	else if (adc_temp >20 && adc_temp < 40 && tel_state != 3 && P41 == 1 && RingUpFlag != 1)
	{
		wch372_send2byte(0xA1, 0x01, 0x03);
		tel_state = 3;
		pre_tel_state = tel_state;
#ifdef DEBUG
		uart_printf("the telephone is online \r\n");
#endif
	}
#endif
}