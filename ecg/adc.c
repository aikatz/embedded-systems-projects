#include "fsl_device_registers.h"

#include "adc.h"

/*
 ADC_init

 Initializes ADC0
 
 From: https://developer.mbed.org/media/uploads/defrost/frdm-k64_adc_configuration.pdf
*/
void ADC_init(void)
{
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK; 	// Enable clock to the ADC
	ADC0_CFG1 |= ADC_CFG1_MODE(3); 		// Set ADC mode to 16bit
	ADC0_SC1A |= ADC_SC1_ADCH(31); 		// Disable the module, ADCH = 1111
}

/*
 ADC_init

 Puts 16 bits through the ADC from PTB2, returns the quantized value
 
 From: https://developer.mbed.org/media/uploads/defrost/frdm-k64_adc_configuration.pdf
*/
uint16_t ADC_read16b(void)
{
	NVIC_DisableIRQ(PORTA_IRQn);
	NVIC_DisableIRQ(PORTC_IRQn);
	
	ADC0_SC1A = ADC_INPUT_PIN & ADC_SC1_ADCH_MASK; 	 //Write to SC1A to start conversion
	while(ADC0_SC2 & ADC_SC2_ADACT_MASK); 	 		 //Conversion in progress
	while(!(ADC0_SC1A & ADC_SC1_COCO_MASK)); 		 //Wait until conversion complete
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
	return ADC0_RA;
}