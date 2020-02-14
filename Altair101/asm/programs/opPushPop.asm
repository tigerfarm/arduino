                            ; --------------------------------------
                            ; Test INR and DCR.
                            ; --------------------------------------
            jmp Start       ; Jump to bypass the halt.
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
                            ; Test B:C push and pop.
            mvi a,'\n'
            out 3
            mvi a,'B'
            out 3
            mvi a,':'
            out 3
            mvi a,'C'
            out 3
                            ;
            mvi b,2
            mvi c,3
            push b
            out 30          ; Register B.
            out 31          ; Register C.
            mvi b,0
            mvi c,0
            out 30          ; Register B.
            out 31          ; Register C.
            pop b
            out 30          ; Register B.
            out 31          ; Register C.
                            ;
                            ; --------------------------------------
                            ; Test D:E push and pop.
            mvi a,'\n'
            out 3
            mvi a,'D'
            out 3
            mvi a,':'
            out 3
            mvi a,'E'
            out 3
                            ;
            mvi d,2
            mvi e,3
            push d
            out 32          ; Register D.
            out 33          ; Register E.
            mvi d,0
            mvi e,0
            out 32          ; Register D.
            out 33          ; Register E.
            pop d
            out 32          ; Register D.
            out 33          ; Register E.
                            ;
                            ; --------------------------------------
                            ; Test H:L push and pop.
            mvi a,'\n'
            out 3
            mvi a,'H'
            out 3
            mvi a,':'
            out 3
            mvi a,'L'
            out 3
                            ;
            mvi h,2
            mvi l,3
            push h
            out 34          ; Register H.
            out 35          ; Register L.
            mvi h,0
            mvi l,0
            out 34          ; Register H.
            out 35          ; Register L.
            pop h
            out 34          ; Register H.
            out 35          ; Register L.
                            ;
                        ; --------------------------------------
            jmp Halt    ; Jump back to the early halt command.
            end
