.section .ivt, "ax"
ivt:
    nop // reset -> nothing to do here
    b handler_und // undefined instruction -> und
    b handler_swi // software interrupt -> svc
    b handler_pfabt // prefetch abort -> abt
    b handler_dabt // data abort -> abt
    nop // nothing here
    b handler_irq // interrupt -> irq
    b handler_fiq // fast interrupt -> fiq