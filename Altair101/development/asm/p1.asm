    Start:
            mvi a,1     ; Move 1 to A.
    Loop:
            nop
            jmp There   ; 0303, 5, 0,
            nop
            nop
    There:
            inr a
            jmp Loop
                        ; End