#include <xinu.h>

void park(){
	intmask mask = disable(); 
	if(proctab[currpid].about_to_park == 0){
		restore(mask); 
		return; 
	}
	struct procent *prptr = &proctab[currpid]; 
	prptr->prstate = PR_SUSP; 
	resched(); 
	restore(mask);
}

void unpark(pid32 pid){
	intmask mask = disable(); 
	struct procent *prptr = &proctab[currpid]; 
	prptr->prstate = PR_READY; 
	insert(pid, readylist, prptr->prprio); 
	prptr->about_to_park = 0; 
	restore(mask);
}

