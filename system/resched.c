/* resched.c - resched, resched_cntl */

#include <xinu.h>
#include <stdlib.h>
//#define DEBUG_CTXSW 1

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	uint16 old = currpid; //save old pid
	currpid = dequeue(readylist); 
	if(currpid ==0  && !isempty(readylist)){
		int16	curr;			/* Runs through items in a queue*/
		curr = firstid(readylist);
		uint32 num_tickets = 0; 
		while (1) {
			if(curr == -1){break;}
			num_tickets += process_tickets[(int)curr]; 
			//kprintf("process:%d, tickets %d\n",curr, process_tickets[(int)curr]);
			//kprintf("num_tickets %d\n", num_tickets); 
			if(curr == lastid(readylist)){break;}
			//kprintf("last id %d\n", lastid(readylist));
			curr = queuetab[curr].qnext;
		}
		if (num_tickets == 0){goto SKIPTOEND;}
		int winner = rand() % num_tickets;
		//kprintf("rand %d\n", winner);
		curr = firstid(readylist);
		while (1) {
			if(curr == -1){break;}
			winner -= process_tickets[(int)curr]; 
			if(winner<=0){
				//kprintf("Winner process %d\n", curr);	
				currpid = (pid32)curr; 
				break;	
			}
			if(curr == lastid(readylist)){break;}
		}
		SKIPTOEND:
			winner=0; //Empty statement

	} 
	uint16 new = currpid; //new pid
	if(old == new){return;}
	ptnew = &proctab[currpid];
	/*if(new == 6){
		kprintf("name: %s\n", ptnew->prname);
		kprintf("pri: %d\n", ptnew->prprio);
		kprintf("time: %d\n", ptnew->time_created);
		kprintf("stk: %d\n", ptnew->prstkptr);
		kprintf("base: %d\n", ptnew->prstkbase);
	}*/
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	#ifdef DEBUG_CTXSW 
		sync_printf("ctsw::%d-%d\n",old, new);
		//sync_printf("name of new %s\n", ptnew->prname);
	#endif
	ptnew->num_ctxsw++;
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
	//kprintf("switch successful\n");
	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
