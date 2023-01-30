.section .ivt, "ax"
ivt:
    b handler_rst // reset -> nothing to do here
    b handler_und // undefined instruction -> und
    b handler_swi // software interrupt -> svc
    b handler_pfabt // prefetch abort -> abt
    b handler_dabt // data abort -> abt
    nop // nothing here
    b handler_irq // interrupt -> irq
    b handler_fiq // fast interrupt -> fiq

.section .handlers, "ax"
handler_rst:
    STMFD sp!,{r0-r12,lr}
    mov r0, lr
    bl chandler_rst
    str r0, [sp, #13*4]
    LDMFD sp!,{r0-r12,pc}^

handler_und:
    STMFD sp!,{r0-r12,lr}
    mov r0, lr
    bl chandler_und
    str r0, [sp, #13*4]
    LDMFD sp!,{r0-r12,pc}^

handler_swi:
    STMFD sp!,{lr}
    PUSH {r0, r1} // dummy data to be replaced by user mode r13, r14
    STMFD sp!,{r0-r12}
    ADD sp, #(13*4)
    STMIA sp, {r13,r14}^
    SUB sp, #(13*4)
    mov r0, lr
    mov r1, sp
    bl __chandler_swi_veneer
    str r0, [sp, #15*4]
    LDMIA sp, {r0-r14}^
    ADD sp, #(14*4)
    POP {lr} // what the f*** is this?! we need to write to the LR of the interrupt mode and not of the user mode????
    LDMFD sp!,{pc}^

handler_pfabt:
    STMFD sp!,{r0-r12,lr}
    mov r0, lr
    bl chandler_pfabt
    str r0, [sp, #13*4]
    LDMFD sp!,{r0-r12,pc}^

handler_dabt:
    STMFD sp!,{r0-r12,lr}
    mov r0, lr
    bl chandler_dabt
    str r0, [sp, #13*4]
    LDMFD sp!,{r0-r12,pc}^

handler_irq:
    STMFD sp!,{lr}
    PUSH {r0, r1} // dummy data to be replaced by user mode r13, r14
    STMFD sp!,{r0-r12}
    ADD sp, #(13*4)
    STMIA sp, {r13,r14}^
    SUB sp, #(13*4)
    mov r0, lr
    mov r1, sp
    bl __chandler_irq_veneer
    str r0, [sp, #15*4]
    LDMIA sp, {r0-r14}^
    ADD sp, #(14*4)
    POP {lr} // what the f*** is this?! we need to write to the LR of the interrupt mode and not of the user mode????
    LDMFD sp!,{pc}^

handler_fiq:
    STMFD sp!,{r0-r12,lr}
    mov r0, lr
    bl chandler_fiq
    str r0, [sp, #13*4]
    LDMFD sp!,{r0-r12,pc}^