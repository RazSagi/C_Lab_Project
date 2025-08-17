.entry START
.extern EXT1

DATA1:   .data 5, -3, 10
TXT:     .string "OK"
M1:      .mat [2][2] 1,2,3,4

START:   mov #5, r1
         add r1, DATA1
         cmp r1, #7
         lea DATA1, r2
         clr r3
         not DATA1
         inc r4
         dec r4
         prn #1
         red r5
         jsr EXT1
         jmp START
         bne EXT1
         rts
         stop

DATA1B:  .data 1, 2
