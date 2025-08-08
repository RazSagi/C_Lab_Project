; ---- test file for first pass ----

; comment line should be skipped
MAIN:   mov r3, r5      ; label + opcode
        add r2, r7      ; no label
LOOP:   cmp r1, r2
        bne END
        stop

; directive with label
DATA1:  .data 5, -3, 15

; directive without label
        .string "Hello"

; reserved name as label (should error)
mov:    add r1, r2

END:    rts