                            ; --------------------------------------
                            ; Set registers to sequential values.
    Start:
            mvi a,1         ; Move an immediate number to each register.
            mvi b,2
            mvi c,3
            mvi d,4
            mvi e,5
            mvi h,6
            mvi l,7
                            ; --------------------------------------
            hlt             ; The program will halt at each iteration, after the first.
            jmp Start
            end