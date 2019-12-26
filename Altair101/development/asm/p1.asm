; Sample test program.

org 0
        ; Program start
Start:
    nop
    jmp Next
    mvi a,1     ; Move 1 to A.
    mvi b,24
    mvi c,255
Next:
mvi c, 3
mvi d , 5
mvi e,7

Errors:
mvi f
mvi e,
mvi e,7 1
