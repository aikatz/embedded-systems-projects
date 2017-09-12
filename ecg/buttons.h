#ifndef _BUTTONS_H
#define _BUTTONS_H

// Initialize the buttons
void buttons_init(void);

// Interrupt handlers for the two buttons
void PORTA_IRQHandler(void);
void PORTC_IRQHandler(void);

#endif /* _BUTTONS_H */
