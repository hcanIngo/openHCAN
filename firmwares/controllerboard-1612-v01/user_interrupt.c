#include <avr/interrupt.h>

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <user_interrupt.h>



//MAX_USER_INTERRUPT_HANDLERS

volatile uint8_t counter[MAX_USER_INTERRUPT_HANDLERS];
//uint8_t counter[MAX_USER_INTERRUPT_HANDLERS];
uint8_t   number_of_hooked_in_ISRs = 0;
myISR ISR_functions[MAX_USER_INTERRUPT_HANDLERS];

void user_interrupt_hook_in_my_ISR(myISR my_new_ISR)
{

  // is there still space, if not return 0L
  if (number_of_hooked_in_ISRs == MAX_USER_INTERRUPT_HANDLERS)
    return;


  // if ok, register the new ISR and return a pointer the TIMER counter
  ISR_functions[number_of_hooked_in_ISRs] =  my_new_ISR;
  //  counter[number_of_hooked_in_ISRs-1] = (number_of_hooked_in_ISRs-1) * 30;


  // a new ISR enters the scene
  number_of_hooked_in_ISRs ++;
}



void user_interrupt_activate(void)
{

  // Start the user interrupt only, if there are consumers..
  if (number_of_hooked_in_ISRs > 0) {
    
    // Erst Zaehltakt waehlen:
    TCCR2 = USER_INTERRUPT_DIVISOR;
    
    // Jetzt enablen:
    TIMSK |= (1<<TOIE2);
  }
}




// The dispatcher for all user interrupt handlers:
ISR(TIMER2_OVF_vect) 
{
  uint8_t i;
  uint8_t max_counter;

  for (i=0;i<number_of_hooked_in_ISRs;i++) {
    // If the counter is 0, the corresponding function gets called
    if (counter[i] == 0)
      counter[i] = (*ISR_functions[i])();
  }

  // Now we to find out when to wakeup again:
  max_counter = 0;
  for (i=0;i<number_of_hooked_in_ISRs;i++)
    if (counter[i] >  max_counter)
      max_counter =  counter[i];
  
   TCNT2 = max_counter;
   //TCNT2 = -1;


  // Finally, all counters need to be adjusted: 
  //counter[i] + (256-max_counter)   == counter[i] - max_counter
  for (i=0;i<number_of_hooked_in_ISRs;i++)
    counter[i] -= max_counter;

  //  counter[0] = 0;
}
