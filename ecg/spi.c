#include "fsl_device_registers.h"
#include "utils.h"
#include "spi.h"


/*
 SPI_init

 Initializes the SPI, took a lot of trial and error to get right.
 
 Most of the code is taken from https://community.nxp.com/thread/372146
	*Some pieces were added/deleted and some settings changed
	*Some comments were added
 
 Pin assignment notes:
	PTD1 -> SCK
	PTD2 -> MISO
	PDT3 -> MOSI
	
	PTD4 -> CS (controlled in software)
	PTC12 -> DC (controlled in software)
	
	SPI0 Settings:
		* Master mode
		* FIFO enabled
		* TX size is 8 bits
		* TX happens on falling edge of SCK
		* data is TX'd MSB first
		* SCK speed is 4.5872MHz (measured on scope,
		  matched exactly to the speed on the Arduino TFT driver
*/
void SPI_init() 
{
	/* Enable clock for PORTD and SPI0 */
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; 								// Enable clock to PORTD
	SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;  								// Enable clock to SPI0
	
	/* Configure IO in PORTD register */
	PORTD_PCR(1) = PORT_PCR_MUX(2); 								// SCK
	PORTD_PCR(2) = PORT_PCR_MUX(2); 								// SOUT
	PORTD_PCR(3) = PORT_PCR_MUX(2); 								// SIN
	
	PORTD->PCR[0] = (1 << 8);										// Configure CS (PTD0) for GPIO
	PTD->PDOR = (1 << 0);         									// Turn off PTD0
	PTD->PDDR = (1 << 0);											// Enable PTD0 as output
	
	/* Clear all registers */
	
	// Clear the status bits (write-1-to-clear)
	SPI0_SR = (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | 
			   SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK);
	SPI0_TCR = 0;													// Clear out the transfer count register
	SPI0_RSER = 0;  												// Clear out the DMA/interrupt request select and enable register
	SPI0_PUSHR = 0; 												// Clear out PUSHR register. Since DSPI is halted, nothing should be transmitted
	SPI0_CTAR0 = 0;													// Clear out the clock and transfer attributes register
	 
	/* Configure registers */
	SPI0_MCR |= SPI_MCR_MSTR_MASK | SPI_MCR_PCSIS_MASK;				// Enable master mode, set inactive state of the PCS							
	SPI0_MCR &= (~SPI_MCR_DIS_RXF_MASK & ~SPI_MCR_DIS_TXF_MASK); 	// Enable FIFOs
	SPI0_MCR &=  (~SPI_MCR_MDIS_MASK & ~SPI_MCR_HALT_MASK); 		// Enable SPI
	SPI0_CTAR0 |=  SPI_CTAR_FMSZ_8BIT | 							// Set the frame size to 8 bits (tx size)
				   SPI_CTAR_BR(2); 									// clock will be 4.5872MHz
		   
	set_CS_high();
}


/*
 SPI_write

 Transmits 8 bits of data over the MOSI pin

 Most of the code is taken from https://community.nxp.com/thread/372146
	Some pieces were added/deleted and some settings changed
	Some comments were added

 This function doesn't affect CS, since it is software controlled.
 CS needs to be set low before calling this and set back high when it's finished.
*/
void SPI_write(unsigned char tx_data) 
{
	NVIC_DisableIRQ(PORTA_IRQn);
	NVIC_DisableIRQ(PORTC_IRQn);
	
	SPI0_MCR |=  SPI_MCR_HALT_MASK;											// Set HALT bit, stops any active transfers
	SPI0_MCR |= (SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK); 				// Flush the RX and TX fifos
	SPI0_SR |= (SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | 
				SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK | SPI_SR_RFDF_MASK);	// Clear the status bits (write-1-to-clear)
	SPI0_TCR |= SPI_TCR_SPI_TCNT_MASK;										// Set SPI_TCNT to 0xFFFF (will wrap to 0x0)
	SPI0_MCR &=  ~SPI_MCR_HALT_MASK;										// Clear the HALT bit, starts transfer
	SPI0_PUSHR =  tx_data;													// load the tx data into PUSHR

	while(!(SPI0_SR & SPI_SR_TCF_MASK));								
	SPI0_SR |= SPI_SR_TFFF_MASK; //clear the status bits (write-1-to-clear)
	
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
}
