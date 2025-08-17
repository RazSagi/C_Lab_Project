.entry START
.extern EXT1

MAIN:   mov r1, r2
        add r3, r4
        cmp EXT1, r5
        prn #7
        cmp NOTDEF, r6   ; should print "undefined symbol 'NOTDEF'"
START:  stop

DATA:   .data 1,2
