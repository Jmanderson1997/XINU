/*testandset.S - test_and_set (for x86) */
	.text
	.globl	test_and_set

/*------------------------------------------------------------------------
 * ctxsw -  X86 test and set call test_and_set uint32 *ptr, uint32 newvalue
 *------------------------------------------------------------------------
 */

test_and_set:
	movl	8(%esp),%eax  //load new value
	movl	4(%esp),%ecx   // load address of old 
	XCHG    (%ecx), %eax	
	ret
	
