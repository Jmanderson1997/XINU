#include <xinu.h>


void set_priority(pid32 pid, pri16 prio){
	struct procent* prptr = &proctab[pid];
	//kprintf("set prio called by thread %d prio %d\n", currpid, prio);
	if(prptr->lock_causing_wait != NOLOCK){
		//kprintf("Transitive clause called\n");
		pi_lock_t *lock = pi_locks[prptr->lock_causing_wait]; 
		if(lock->highest_prio < prio){
			lock->highest_prio = prio; 
			lock->prio_owner = pid; 
			set_priority(lock->owner, prio);
		}
	}
	intmask mask = disable();
	proctab[pid].prprio = prio; 
	resched();
	restore(mask);
}

void set_new_priority_owner(pi_lock_t *l, pid32 pid){

	if(isempty(l->queue)){return;}
	pri16 prio = proctab[pid].prprio; 
	pid32 owner = pid;
	pid32 curr = firstid(l->queue);
	while(1){
		//kprintf("stuck thread %d\n", currpid);
		if(proctab[(int)curr].prprio > prio){
			prio = proctab[(int)curr].prprio;
			owner = curr; 
		}
		if(curr == lastid(l->queue)){break;}
		curr = queuetab[curr].qnext;
	}
	l->highest_prio = prio; 
	l->prio_owner = owner;
}

void restore_priority(pid32 pid){
	intmask mask = disable(); 
	proctab[currpid].prprio = proctab[currpid].initial_prio; 
	restore(mask);
}
