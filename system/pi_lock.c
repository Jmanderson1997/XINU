#include <xinu.h>

pi_lock_t pi_locks[NPILOCKS]; 
uint16 active_pilocks =0; 

syscall pi_initlock(pi_lock_t *l){

	if(active_pilocks == NPILOCKS){
		return SYSERR;
	}

	l->flag = 0; 
	l->guard = 0;
	l->queue = newqueue(); 
	active_pilocks++; 
	return OK;  
}


syscall pi_lock(pi_lock_t *l){

	while(test_and_set(&l->guard, 1) == 1);
	if(l->flag == 0){
		l->flag = 1; 
		l->guard = 0; 
		l->owner = currpid; 
	}
	else{
		insert(currpid, l->queue, 1); 
		proctab[currpid].about_to_park = 1;
		l->guard = 0; 
		priopark(); 
	}
	return OK;
}

syscall pi_unlock(pi_lock_t *l){

	if(l->owner != currpid){
		return SYSERR; 
	}

	while(test_and_set(&l->guard, 1) == 1); 
	if(isempty(l->queue)){
		l->guard = 0; 
		l->flag = 0; 
		l->owner = 0; 
	}
	else{
		l->guard = 0; 
		pid32 pid = dequeue(l->queue); 
		l->owner = pid; 
		priounpark(pid); 
	} 
	return OK; 
}
