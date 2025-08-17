; bad_branch.as — should flag jumping to a data label

.extern EXT1
.entry START

DATA:    .data 1, 2, 3
MSG:     .string "ok"

START:   mov  #5, r1
         jsr  EXT1        ; allowed: extern target
         jmp  DATA        ; should error: data label used as branch target
         bne  LOOP        ; ok: code label

LOOP:    dec  r1
         bne  LOOP
         stop
