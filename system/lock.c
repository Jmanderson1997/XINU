#include <xinu.h>

uint16 active_locks = 0; 

syscall initlock(lock_t *l){

	if(active_locks == NLOCKS){
		return SYSERR; 
	}
	
	l->flag = 0; 
	l->guard = 0;
	l->queue = newqueue(); 
	active_locks++; 
	return OK;  
}


syscall lock(lock_t *l){

	while(test_and_set(&l->guard, 1) == 1){
		sleepms(QUANTUM);
	}
	if(l->flag == 0){
		l->flag = 1; 
		l->owner = currpid; 
		l->guard = 0; 
	}
	else{
		proctab[currpid].about_to_park = 1; 
		enqueue(currpid, l->queue); 
		l->guard = 0; 
		park(); 
	}
	return OK;
}

syscall unlock(lock_t *l){

	if(l->owner != currpid){
		return SYSERR; 
	}

	while(test_and_set(&l->guard, 1) == 1){
		sleepms(QUANTUM);
	} 
	if(isempty(l->queue)){
		l->flag = 0; 
		l->owner = 0; 
		l->guard = 0; 
	}
	else{
		pid32 pid = dequeue(l->queue); 
		l->owner = pid; 
		unpark(pid); 
		l->guard = 0; 
	} 
	return OK; 
}


