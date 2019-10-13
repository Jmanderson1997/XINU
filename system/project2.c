#include <xinu.h>

void burst_execution(uint32 number_bursts, uint32 burst_duration, uint32 sleep_duration){

	uint32 timer, i; 
	sync_printf("args: %d, %d, %d\n", number_bursts, burst_duration, sleep_duration);
	for(i=0; i<number_bursts; i++){
		sync_printf("loop %d\n", i);
		timer = ctr1000; 
		while(ctr1000-timer< burst_duration){}
		sleepms(sleep_duration);
	}
}

uint32 process_tickets[NPROC]= {0}; 

void set_tickets(pid32 pid,uint32 tickets){
	process_tickets[(int)pid] = tickets;
} 


