.entry START
.entry LEN
.extern EXT

STR:  .string "abc"
LEN:  .data 3
ARR:  .data 1,2,3
M:    .mat [2][2] 4,5,6,7

START:
    mov M[r1][r0], r2
    add r2, ARR
    cmp r2, #3
    bne EXT
    prn STR
    stop
