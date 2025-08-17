; secondpass_strict.as — short lines, second-pass checks only
.extern OUT
.entry START
.entry OUT

DATA: .data 5,6,7
STR: .string "hi"

START: mov #5, r1
add r1, r2
lea DATA, r3

jsr DATA
jmp DATA
bne DATA

cmp #5000, r0
cmp #-5000, r1

mov UNDECL, r2
stop
