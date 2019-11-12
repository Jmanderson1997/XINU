#include <xinu.h>

uint16 active_pilocks = 0; 
pi_lock_t * pi_locks[NPILOCKS];

syscall pi_initlock(pi_lock_t *l){

	if(active_pilocks == NPILOCKS){
		return SYSERR; 
	}
	
	l->flag = 0; 
	l->guard = 0;
	l->queue = newqueue();
	l->highest_prio = 0; 
	l->prio_owner = 0; 
	l->index = active_pilocks; 
	pi_locks[active_pilocks] = l;  
	active_pilocks++; 
	return OK;  
}


syscall pi_lock(pi_lock_t *l){

	while(test_and_set(&l->guard, 1) == 1){
		sleepms(QUANTUM);
	}
	if(l->flag == 0){
		l->flag = 1; 
		l->owner = currpid; 
		l->highest_prio = proctab[currpid].prprio; 
		l->prio_owner = currpid; 
		//kprintf("thread %d aquired lock %d\n", currpid, l->index);
		l->guard = 0; 
	}
	else{
		proctab[currpid].about_to_park = 1; 
		proctab[currpid].lock_causing_wait = l->index;
		if(proctab[currpid].prprio > proctab[l->owner].prprio){
			l->highest_prio = proctab[currpid].prprio;
			l->prio_owner = currpid;
			set_priority(l->owner, l->highest_prio); 
		} 
		//kprintf("thread %d queued lock %d\n", currpid, l->index);
		enqueue(currpid, l->queue); 
		l->guard = 0; 
		park(); 
		while(test_and_set(&l->guard, 1) == 1){sleepms(QUANTUM);}
		l->owner = currpid;
		//kprintf("thread %d aquired lock %d\n", currpid, l->index);
		if(l->highest_prio > proctab[currpid].prprio){
			set_priority(currpid, l->highest_prio);
		}
		l->guard = 0;
	}
	return OK;
}

syscall pi_unlock(pi_lock_t *l){

	if(l->owner != currpid){
		return SYSERR; 
	}


	while(test_and_set(&l->guard, 1) == 1){
		sleepms(QUANTUM);
	} 
	//kprintf("thread %d released lock %d\n", currpid, l->index);
	if(isempty(l->queue)){
		l->flag = 0; 
		l->owner = 0; 
		l->highest_prio = 0;
		l->prio_owner = 0;  
		l->guard = 0;
	}
	else{
		pid32 pid = dequeue(l->queue); 
		proctab[currpid].lock_causing_wait = NOLOCK;  
		if(pid == l->prio_owner){
			set_new_priority_owner(l, pid);
		}
		unpark(pid); 
		l->guard = 0; 
	} 
	if(proctab[currpid].prprio != proctab[currpid].initial_prio){
				restore_priority(currpid); 
	}
	return OK; 
}
