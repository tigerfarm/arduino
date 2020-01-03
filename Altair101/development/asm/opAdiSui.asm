                        ; --------------------------------------
                        ; Test ADI and SUI.
                        ; Add immediate number to register A.
                        ; Subtract immediate number from register A.
                        ; --------------------------------------
            jmp Test    ; Jump to bypass the halt.
    Error:
            out 39      ; Print the registers and other system values.
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
    Test:
            mvi a,73    ; Move # to register A.
            out 37
            mvi b,76    ; Move # to register B.
            mvi c,73    ; Move # to register C.
                        ; --------------------------------------
            adi 3       ; Add immediate number to register A. Answer should be 76.
            out 37
            cmp b       ; B = A. Zero bit flag is true.
            jz okayb1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okayb1:
                        ; --------------------------------------
            sui 3       ; Subtract immediate number from register A.
            out 37
            cmp c       ; C = A. Zero bit flag is true.
            jz okayc1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okayc1:
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt start command.
                        ; End.