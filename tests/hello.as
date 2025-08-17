; sanity mix
.entry START
.extern EXT1

START:  mov  #5, r1
        add  r1, r2
        lea  START, r3
        cmp  #127, r0
        cmp  #128, r0      ; should error: immediate out of bounds
        prn  #-1
        red  r2
        red  r9            ; should error: invalid addressing (expects register)
        jmp  EXT1
        stop

DATA1:  .data 10, -3, 127
TXT:    .string "ok"
