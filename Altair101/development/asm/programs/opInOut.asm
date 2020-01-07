                        ; --------------------------------------
                        ; Test IN and OUT.
                        ; --------------------------------------
            jmp Start    ; Jump to bypass the halt.
    Error:
            out 39      ; Print the registers and other system values.
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
    Start:
            mvi a,73    ; Move # to register A.
                        ; --------------------------------------
            jmp Error   ; The above should have jumped passed this.
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
                        ; End.