#ifndef _ADC_H
#define _ADC_H

#define ADC_INPUT_PIN				(12U)

// Initialize ADC0
void ADC_init(void);

// Read 16 bits from ADC0
uint16_t ADC_read16b(void);

#endif /* _ADC_H */
