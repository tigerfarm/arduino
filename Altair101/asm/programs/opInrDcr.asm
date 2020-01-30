                            ; --------------------------------------
                            ; Test INR and DCR.
                            ; --------------------------------------
            jmp Start       ; Jump to bypass the halt.
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
            mvi a,1         ; Initialize the register.
            mvi b,2
            mvi c,3
            mvi d,4
            mvi e,5
            mvi h,6
            mvi l,7
            out 38          ; Print the register values.
                            ; --------------------------------------
                            ; Test by incrementing the registers up.
            inr a           ; Increment register A.
            inr b
            inr c
            inr d
            inr e
            inr h
            inr l
            out 38
                            ; --------------------------------------
                            ; Test by decrementing the registers down.
            dcr a           ; Decrement register A.
            dcr b
            dcr c
            dcr d
            dcr e
            dcr h
            dcr l
            out 38
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
            end
