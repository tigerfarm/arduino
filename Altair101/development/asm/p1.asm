    Start:
            mvi a,1     ; Move 1 to A.
    Loop:
            nop
            jmp There
            nop
            nop
    There:
            inr a
            jmp Loop
