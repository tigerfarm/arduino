                                    ; --------------------------------------
                                    ; Test CALL and RET.
                                    ; --------------------------------------
                                    ; Definition section.
        A       equ     'a'
        NL      equ     '\n'
        Final   equ     42
        addr1   equ     500h        ; Test using an address greater than 255 (byte size max).
                                    ;
                                    ; --------------------------------------
                jmp Test            ; Jump to the test section.
                                    ;
                                    ; --------------------------------------
    Error:
                mvi a,'-'
                out 3
    Halt:
                mvi a,'\n'
                out 3
                hlt                 ; The program will halt at each iteration, after the first.
                                    ;
                                    ; --------------------------------------
    Test:
                                    ; Immediate sample testing
                mvi a,080h
                out 3
                mvi a,80h
                out 3
                mvi a,NL
                out 3
                mvi a,a             ; The second "a", is an EQU name.
                out 3
                mvi a,'b'
                out 3
                mvi a,'c'
                out 3
                mvi a,Final
                out 3
                mvi a,addr1         ; The addr1 value is over 255, which makes it too high for a byte. No error checking for this.
                ; mvi a,Fianl         ; Test an assembler error, "Label not found."
                ; out 3
                                    ; --------------------------------------
    Success:
                mvi a,'+'
                out 3
                                    ; --------------------------------------
                jmp Halt            ; Jump back to the early halt command.
                                    ;
                                    ; --------------------------------------
                end.