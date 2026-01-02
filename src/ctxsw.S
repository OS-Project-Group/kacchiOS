/* ctxsw.S - Context Switch Implementation */
.text
.globl ctxsw

/* 
 * void ctxsw(void** old_sp, void** new_sp)
 * 
 * Parameters:
 *   old_sp: pointer to save old stack pointer
 *   new_sp: pointer to load new stack pointer
 *
 * Saves current CPU state and restores new process state
 */
ctxsw:
    /* Get parameters from stack */
    movl 4(%esp), %eax      /* old_sp */
    movl 8(%esp), %edx      /* new_sp */

    /* Save old process state */
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %ebx
    
    /* Save old ESP */
    movl %esp, (%eax)
    
    /* Load new ESP */
    movl (%edx), %esp
    
    /* Restore new process state */
    popl %ebx
    popl %esi
    popl %edi
    popl %ebp
    
    /* Return to new process */
    ret
