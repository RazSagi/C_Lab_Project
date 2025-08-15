.entry START
.extern EXT1

MAIN:   mov r3, r4
        add r1, r2
START:  stop

.data   5, -3
.string "ok"
