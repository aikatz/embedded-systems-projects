#ifndef _UTILS_H
#define _UTILS_H

#include "stdint.h"

extern volatile uint32_t zoom;
extern volatile uint32_t amplify_scaler;
extern volatile uint32_t screen_pos;

// Processes raw reading from the adc
uint16_t process_raw_value(uint16_t value);

// Delays processor for time_ms milliseconds
void delay_ms(uint32_t time_ms);

// Simple GPIO functions for communicating with the TFT
void set_DC_high(void);
void set_DC_low(void);
void set_CS_high(void);
void set_CS_low(void);

#endif
