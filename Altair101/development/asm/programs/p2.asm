    Start:
            mvi a,1     ; Move 1 to A.
    Loop:
            nop
            jmp There   ; 0303, 5, 0,
            mvi a,'a'
            out 3
            mvi a,':'
            out 3
            mvi a,'b'
            out 3
            nop
            nop
    There:
            inr a
            jmp Loop
                        ; End