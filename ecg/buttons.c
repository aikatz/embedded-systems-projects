#include "fsl_device_registers.h"

#include "utils.h"
#include "HX8357.h"
#include "buttons.h"

void buttons_init()
{
	// Configure both buttons as GPIO, interrupt on rising-edge
	PORTA->PCR[4] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(9); 
	PORTC->PCR[6] = PORT_PCR_MUX(1) | PORT_PCR_IRQC(9); 
	
	// SW2 -> PTC6, SW3 -> PTA4
	PTC->PDDR |= (0 << 6);
	PTA->PDDR |= (0 << 4);
	
	// Enable interrupts
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
}

void PORTC_IRQHandler(void) 
{
	screen_pos = 1;
	fillScreen(BLACK);
	
	zoom = zoom ? 0 : 1;
	
	PORTC->PCR[6] |= (1 << PORT_PCR_ISF_SHIFT);
}

void PORTA_IRQHandler(void) 
{
	screen_pos = 1;
	fillScreen(BLACK);
	
	amplify_scaler *= 2;
	if (amplify_scaler > 4)
		amplify_scaler = 1;

	PORTA->PCR[4] |= (1 << PORT_PCR_ISF_SHIFT);
}
