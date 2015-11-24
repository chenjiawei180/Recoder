#include "STC12C5A60S2.H"
#include "usart.h"
#include "adc.h"
#include "wch372.h"

void main(void)
{
	unsigned char adc_value = 0;

	usart_init();
	InitADC();
	ch372_init();
	
	while (1)
	{
		adc_value = GetADCResult(0);
		uart_printf("ADC value is %d!\r\n", (unsigned int)adc_value);
	}
}