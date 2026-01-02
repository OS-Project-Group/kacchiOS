/* process_entry.S - Process entry trampoline */
.text
.globl process_trampoline
.extern user_process_exit

/* 
 * Process entry trampoline
 * Called when a new process starts
 * EAX contains the function pointer to call
 */
process_trampoline:
    /* Function address is already set up by create_process_with_func */
    /* It will be in a register or we call it directly */
    /* For now, the function address is on stack as if we're returning to it */
    
    /* When we get here from ctxsw RET, the process function address */
    /* should be what we return to. But when THAT returns, we need */
    /* to catch it. So we push exit handler as return address first. */
    
    /* This won't work with current setup. Let me use a different approach. */
    /* We'll just make the process function address the return target */
    /* and rely on proper stack setup */
    ret
