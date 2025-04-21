#include "Scheduler.h"

static uint32_t scheduledEvents;

uint32_t getScheduledEvents(){
	return scheduledEvents;
}

void addSchedulerEvent(uint32_t event_to_schedule){
	scheduledEvents |= event_to_schedule;
}


void removeSchedulerEvent(uint32_t event_to_remove){
	scheduledEvents &= !event_to_remove;//this might be wrong
}
