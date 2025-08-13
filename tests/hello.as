; issues.as — lines ≤80 chars, all with labels

ONLYLBL:

LBLENTRY: .entry MAIN
LBLEXTERN: .extern EXT

BADDIR: .foobar 1

BADOP1: foo r1, r2
MISS1: add
MISS2: add r1
MISS3: add r1,
MISS4: add , r2

JUNK1: mov r2, r3 junk

DATA1: .data 1,,2
DATA2: .data 1 2

STR1: .string abc

MAT1: .mat [0][3]
MAT2: .mat [2][2] 1,2,3,4,5
MAT3: .mat 2][2] 1,2
