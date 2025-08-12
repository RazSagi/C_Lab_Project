; good data
GOOD1:   .data  +3,  -2,  0

; bad comma in data (double comma)
BADCOMMA: .data 1,, 3

; duplicate symbol
DUP: .data 5
DUP: .string "x"

; string missing closing quote
STRMISS: .string "abc

; extraneous text after .string
STRJUNK: .string "hi"   trailing

; .mat too many initializers for 2x2
MTOO: .mat [2][2] 1,2,3,4,5

; .mat zero-fill ok
MZERO: .mat [2][3]

; .mat invalid comma usage (trailing comma)
MBAD: .mat [1][3] 9, 8,

; label before .entry (invalid per booklet)
LBL1: .entry MAIN

; unknown directive
.UNKNOWN 123

; missing args after .data
MISSING: .data

; fine string
OKSTR: .string "xyz"
