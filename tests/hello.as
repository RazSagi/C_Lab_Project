; A) duplicate .entry and .entry on extern
.extern OUT
.entry START
.entry START          ; duplicate entry -> error
.entry OUT            ; entry on extern -> error

; B) code-only target check (jmp/bne/jsr to DATA -> error)
DATA:   .data 1,2,3
START:  jmp  DATA     ; must be code label -> error
        bne  DATA     ; must be code label -> error
        jsr  DATA     ; must be code label -> error

; C) immediate range (assuming 10-bit payload -> range -512..511)
        add  #512, r1 ; out of bounds -> error
        sub  #-513, r2 ; out of bounds -> error

        stop
