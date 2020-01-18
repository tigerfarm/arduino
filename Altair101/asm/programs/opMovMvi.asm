                        ; --------------------------------------
                        ; Test MVI and MOV to the serial monitor.
                        ; --------------------------------------
            jmp Start   ; Jump to bypass the halt.
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
    Start:
            mvi a,6     ; Move # to registers.
            mvi b,0
            mvi c,1
            mvi d,2
            mvi e,3
            mvi h,4
            mvi l,5
            out 38      ; Print the register values.
                        ; --------------------------------------
            mvi b,7     ; Move # to register B.
            mov a,b
            inr b
            mov c,b
            inr b
            mov d,b
            inr b
            mov e,b
            inr b
            mov h,b
            inr b
            mov l,b
            inr b
            out 38
                        ; --------------------------------------
            mvi c,7     ; Move # to register C.
            mov a,c
            inr c
            mov b,c
            inr c
            mov d,c
            inr c
            mov e,c
            inr c
            mov h,c
            inr c
            mov l,c
            inr c
            out 38
                        ; --------------------------------------
            mvi d,7     ; Move # to register D.
            mov a,d
            inr d
            mov b,d
            inr d
            mov c,d
            inr d
            mov e,d
            inr d
            mov h,d
            inr d
            mov l,d
            inr d
            out 38
                        ; --------------------------------------
            mvi e,7     ; Move # to register E.
            mov a,e
            inr e
            mov b,e
            inr e
            mov c,e
            inr e
            mov d,e
            inr e
            mov h,e
            inr e
            mov l,e
            inr e
            out 38
                        ; --------------------------------------
            mvi h,7     ; Move # to register H.
            mov a,h
            inr h
            mov b,h
            inr h
            mov c,h
            inr h
            mov d,h
            inr h
            mov e,h
            inr h
            mov l,h
            inr h
            out 38
                        ; --------------------------------------
            mvi l,7     ; Move # to register L.
            mov a,l
            inr l
            mov b,l
            inr l
            mov c,l
            inr l
            mov d,l
            inr l
            mov e,l
            inr l
            mov h,l
            inr l
            out 38
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
            end