#define MAX_USER_INTERRUPT_HANDLERS 5
#define USER_INTERRUPT_DIVISOR ((1<<CS22)|(1<<CS21)|(1<<CS20)) // 1024

#ifdef MCU_atmega644
	#define TIMSK TIMSK1
	#define TCCR2 TCCR2B
#endif

typedef uint8_t (* myISR)(void);


void user_interrupt_hook_in_my_ISR(myISR my_new_ISR);
void user_interrupt_activate(void);

