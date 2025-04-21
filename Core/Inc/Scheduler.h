#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>

#define START_MENU_EVENT     (1 << 0)
#define POLLING_GAME_EVENT   (1 << 1)
#define SCORE_SCREEN_EVENT   (1 << 2)

uint32_t getScheduledEvents();
//return the scheduled events

void addSchedulerEvent(uint32_t event_to_schedule);
//Adds event to the scheduler


void removeSchedulerEvent(uint32_t event_to_remove);
//Removes event from scheduler


#endif
