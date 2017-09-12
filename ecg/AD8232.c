#include "fsl_device_registers.h"

#include "AD8232.h"

/*
 AD8232_init
 
 Does some basic setup for the AD8232 board
 
 Pin Assignment Notes:
	* Vin -> 3.3V
	* GND -> GND
	* OUTPUT -> PTB2
	* LO- -> PTB18
	* LO+ -> PTB19
*/
void AD8232_init(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK; 	// Enable clock to IO pins
	PORTB->PCR[19] = (1 <<  8) ;        // Configure PTB19 for GPIO
	PORTB->PCR[18] = (1 <<  8);         // Configure PTB18 for GPIO
	
	PTB->PDOR = 0;						// Set pins to input
}

/*
 AD8232_LeadsOn

 Reads LO- and LO+, which are outputs from the AD8232 used
 used for detecting whether the leads are on or off a person
*/
int AD8232_LeadsOn(void) {
	return PTB->PDIR & ((1 << 18) | (1 << 19));
}
