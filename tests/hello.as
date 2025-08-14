; pass1_comments.as — should show only the duplicate extern error

MAIN: mov r1, r2      ; ok comment
      add #5, r3      ; ok comment
      prn #7          ; ok comment
      rts             ; ok comment

STR: .string "hello"  ; ok
NUMS: .data 1,2,3     ; ok
MAT1: .mat [1][3] 4,5,6 ; ok

.data 9,8,7           ; ok
.string "ok"          ; ok
.mat [1][2] 10,20     ; ok

.extern EXT1          ; ok
.entry MAIN           ; ok
.entry FORWARD        ; ok

FORWARD: not r3       ; ok

EXTDUP: .extern EXT1  ; label before .extern -> error (intentional)

; Try a pure duplicate extern (no label):
.extern EXT1          ; duplicate extern -> error (intentional)
