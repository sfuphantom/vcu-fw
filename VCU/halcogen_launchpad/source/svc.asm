;-------------------------------------------------------------------------------
; svc.asm
;
; (c) Texas Instruments 2009-2013, All rights reserved.
;

    .text
    .arm


;-------------------------------------------------------------------------------

		.def   _svc
        .asmfunc
_svc
		stmfd   sp!, {r12,lr}
		ldrb	r12, [lr, #-1]
		ldr		r14,  table
		ldr     r12, [r14, r12, lsl #2]
		blx     r12
		ldmfd   sp!, {r12,pc}^

table
		.word	jumpTable

jumpTable
		.word	swiGetCurrentMode		; 0
		.word  	swiSwitchToMode			; 1

       .endasmfunc

;-------------------------------------------------------------------------------

        .asmfunc

swiGetCurrentMode
		mrs		r12, spsr
		ands	r0, r12, #0x1F		; return value
		bx		r14

		.endasmfunc

;-------------------------------------------------------------------------------

       .asmfunc

swiSwitchToMode
		mrs		r12, spsr
		bic		r12, r12, #0x1F
		orr		r12, r12, r0
		msr		spsr_c, r12
		bx		r14

		.endasmfunc

;-------------------------------------------------------------------------------

