; tests/step_regs_imm_final.as
.entry START

mov   r3, r4      ; reg+reg → 1 reg word
cmp   #5, r2      ; imm+reg → reg word then imm
cmp   r2, #5      ; reg+imm → reg word then imm
add   r1, #-3     ; reg+imm
clr   r7          ; single reg → 1 reg word
prn   r5          ; single reg → 1 reg word

START: stop
