#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "fsl_adc16_driver.h"
#include "board.h"

#include "utils.h"
#include "AD8232.h"
#include "HX8357.h"
#include "buttons.h"
#include "spi.h"
#include "adc.h"

int main() 
{
	// Do a bunch of initializations
	hardware_init();
	ADC_init();
	SPI_init();
	AD8232_init();
	HX8357_init();
	
	buttons_init();
	
	// Used for the moving average filter
	uint32_t buf_sum = 0;
	uint32_t mov_avg_idx = 0;
	const uint32_t mov_avg_len = 10;
	uint32_t buf[mov_avg_len];

	// Various variables for storing/drawing on screen
	uint16_t value =  0;
	uint32_t filtered_output = 0;
	uint32_t previous_output = 0;
	
	// Initialize the moving average buffer
	for(int i = 0; i < mov_avg_len; i++) { buf[i] = 0; }
	
	// Make the screen black
	fillScreen(BLACK);
	
	while (1) {		
		// Get the value from the ADC, normalize, and amplify it
		value = process_raw_value(ADC_read16b());

		// Update the moving average filter
		buf_sum -= buf[mov_avg_idx];
		buf[mov_avg_idx] = value;
		buf_sum += buf[mov_avg_idx];
		mov_avg_idx++;
		if (mov_avg_idx >= mov_avg_len)	
			mov_avg_idx = 0;
		
		// Update previous and curruent outputs
		previous_output = filtered_output;
		filtered_output = buf_sum/mov_avg_len;
				
		// Wipe out previous pixels in this column
		drawFastHLine(0, screen_pos, HX8357_TFTWIDTH, BLACK);
		
		// Connect the current output to the previous output with a line
		drawLine(filtered_output, screen_pos, previous_output, screen_pos-1, YELLOW);
		
		// Move to the next column on the screen
		screen_pos++;
		
		// Wrap around if necessary
		if (screen_pos >= 480)
			screen_pos = 0;
		
		// This scales the display out so there are more readings on the screen at one time
		// I.e. it decreases the sampling rate
		if(!zoom)
			delay_ms(10);
	}
}
