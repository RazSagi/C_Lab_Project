; second_pass_only_errors.as

.extern EXT
.entry  MISSING
.entry  EXT

DATA1: .data 10
CODE1:
    inc r2

MAIN:
    mov labelNotDef, r1     ; undefined symbol (direct)
    jmp DATA1               ; jump to data (should be code-label only)
    cmp #1, #2              ; legal syntax; range ok
    mov #999999, r3         ; immediate out of range (assuming IMM_MAX smaller)
    bne MISSING             ; .entry was declared for MISSING but it’s undefined
    stop
