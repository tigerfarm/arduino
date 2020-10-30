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
                            ; + regA:   1 = 001 = 00000001
                            ; + regB:   2 = 002 = 00000010  regC:   3 = 003 = 00000011
                            ; + regD:   4 = 004 = 00000100  regE:   5 = 005 = 00000101
                            ; + regH:   6 = 006 = 00000110  regL:   7 = 007 = 00000111
                            ; 
                            ; --------------------------------------
                            ; Test by incrementing the registers up.
            inr a           ; Increment register A.
            inr b
            inr c
            inr d
            inr e
            inr h
            inr l
            out 38          ; Print the register values.
                            ; + regA:   2 = 002 = 00000010
                            ; + regB:   3 = 003 = 00000011  regC:   4 = 004 = 00000100
                            ; + regD:   5 = 005 = 00000101  regE:   6 = 006 = 00000110
                            ; + regH:   7 = 007 = 00000111  regL:   8 = 010 = 00001000
                            ; 
                            ; --------------------------------------
                            ; Test by decrementing the registers down.
            dcr a           ; Decrement register A.
            dcr b
            dcr c
            dcr d
            dcr e
            dcr h
            dcr l
            out 38          ; Print the register values.
                            ; + regA:   1 = 001 = 00000001
                            ; + regB:   2 = 002 = 00000010  regC:   3 = 003 = 00000011
                            ; + regD:   4 = 004 = 00000100  regE:   5 = 005 = 00000101
                            ; + regH:   6 = 006 = 00000110  regL:   7 = 007 = 00000111
                            ; 
                            ; --------------------------------------
            NOP
            jmp Halt        ; Jump back to the early halt command.
            end
