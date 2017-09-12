#include "system_MK64F12.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

#include "utils.h"
#include "HX8357.h"

volatile uint32_t zoom = 0;
volatile uint32_t amplify_scaler = 1;
volatile uint32_t screen_pos = 0;

/*
 process_raw_value

 Takes a raw value from the ADC, normalizes it to fit
 on the screen and then amplifies it. 
*/
uint16_t process_raw_value(uint16_t value) {
	
	// Normalization
	value = (320 * value)/65536;
	
	// Amplification
	uint32_t mid = HX8357_TFTWIDTH/2;
	if(value > mid)
		value = value + amplify_scaler * (value - mid);
	else
		value = value - amplify_scaler * (mid - value);
	
	// Trimming
	if(value >= HX8357_TFTWIDTH)
		value = HX8357_TFTWIDTH-1;
	
	return value;
}

/*
 delay_ms

 Makes the processor wait for time_ms. The n_cycles calculation is kind of
 messy, and the factor of 100/70 was purely from trial and error - no idea
 why it works, but it is extremely accurate
*/
void delay_ms(unsigned time_ms)
{
	unsigned n_cycles = (unsigned)((float)time_ms * (float)(DEFAULT_SYSTEM_CLOCK/1000) * (float)(100.0/70.0));
	for(int i = 0; i < n_cycles; i++);
}

/*
 set_DC_high

 Sets the pin used for switching between Data and Command mode high
*/
void set_DC_high()
{
	PTC->PSOR = 1 << 12;
}

/*
 set_DC_low

 Sets the pin used for switching between Data and Command mode low
*/
void set_DC_low()
{
	PTC->PCOR = 1 << 12;
}

/*
 set_CS_high

 Sets the chip select pin high
*/
void set_CS_high()
{
	PTD->PSOR = 1;
}

/*
 set_CS_low

 Sets the chip select pin low
*/
void set_CS_low()
{
	PTD->PCOR = 1;
}
