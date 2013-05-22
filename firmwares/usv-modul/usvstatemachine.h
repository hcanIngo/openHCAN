#ifndef USVSTATEMACHINE_H
#define USVSTATEMACHINE_H

void usv_state_handler(void);
void usv_timer_handler(void);
void usv_can_callback(const canix_frame *frame);
void usv_reset_U_stats(void);

#endif
