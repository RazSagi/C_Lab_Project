.extern EXT
.entry LOOP

DATA: .data 10,-1
TXT:  .string "hi"

LOOP:
    inc DATA
    jmp LOOP
    mov #5, r3
    bne EXT
    stop
