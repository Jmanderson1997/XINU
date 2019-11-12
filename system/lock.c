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
	sync_printf("creation value: %d, empty:%d, firstid:%d\n", l->queue, isempty(l->queue), firstid(l->queue));
	return OK;  
}


syscall lock(lock_t *l){

	while(test_and_set(&l->guard, 1) == 1){
		sleepms(QUANTUM);
	}
	if(l->flag == 0){
		sync_printf("process %d aquired lock\n", currpid);
		sync_printf("value:%d, first: %d\n",l->queue, firstid(l->queue) );
		l->flag = 1; 
		l->guard = 0; 
		l->owner = currpid; 
	}
	else{
		proctab[currpid].about_to_park = 1; 
		sync_printf("process %d inserted in queue\n", currpid);
		sync_printf("value:%d, first: %d\n",l->queue, firstid(l->queue) );
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
	sync_printf("process %d released lock\n", currpid);
	sync_printf("value:%d, first: %d\n",l->queue, firstid(l->queue) );
	if(isempty(l->queue)){
		sync_printf("queue empty\n");
		l->guard = 0; 
		l->flag = 0; 
		l->owner = 0; 
	}
	else{
		l->guard = 0; 
		pid32 pid = dequeue(l->queue); 
		sync_printf("process %d next\n", pid);
		l->owner = pid; 
		unpark(pid); 
	} 
	return OK; 
}


