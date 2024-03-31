#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void config_PS2(void);
void config_KEYS(void);
void enableInterrupts(void);
void interrupt_handler(void);
void ps2_ISR(void);
void pushbutton_ISR(void);

#endif