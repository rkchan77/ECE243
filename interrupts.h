#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void enableInterrupts(void);
void interrupt_handler(void);
void config_PS2(void);
void ps2_ISR(void);

#endif