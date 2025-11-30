.entry START
.extern EXT1
.extern EXT2

STR:    .string "hi!"
ARR:    .data 1,2,-3,4
MAT:    .mat [2][2] 5,6,7,8
K:      .data 0

START:  mov #4, r3
        add r3, K
        cmp r3, #4
        bne EXT1
        mov MAT[r1][r2], r5
        jmp NEXT
EXTLBL: prn #-1
NEXT:   mov r3, ARR
        prn STR
        stop

