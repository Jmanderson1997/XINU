/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
	resched();

	return OK;
}

syscall print_ready_list(){
	kprintf("calls\n");

	int16	curr;			/* Runs through items in a queue*/

	curr = firstid(readylist);
	while (1) {
		kprintf("%d\n", curr);
		if(curr == lastid(readylist)){
			break; 
		}
		curr = queuetab[curr].qnext;
	}

}
