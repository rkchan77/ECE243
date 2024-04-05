#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void config_PS2(void);
void config_KEYS(void);
void enableInterrupts(int);
void disableInterrupts();
void ps2_ISR(void);
void pushbutton_ISR(void);
void the_reset(void) __attribute__((section(".reset")));
void the_exception(void) __attribute__((section(".exceptions")));
void interrupt_handler(void);

#endif