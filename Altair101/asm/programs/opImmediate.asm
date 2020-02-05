                                    ; --------------------------------------
                                    ; Test using immediate value in opcodes.
                                    ; --------------------------------------
                                    ; Definition section.
                                    ;
        ; A       equ     'A'         ; Labels need to be case sensitive for this case.
                                    ;
        a       equ     'a'
        NL      equ     '\n'
        p2      equ     3           ; Variable for the out port address.
        Final   equ     42
        addr1   equ     503h        ; Test using an address greater than 255 (byte size max).
                                    ;
                                    ; --------------------------------------
                jmp Test            ; Jump to the test section.
                                    ;
                                    ; --------------------------------------
    Error:
                mvi a,'-'
                out p2
    Halt:
                mvi a,'\n'
                out p2
                hlt                 ; The program will halt at each iteration, after the first.
                                    ;
                                    ; --------------------------------------
    Test:                           ; Immediate sample testing
                adi 3               ; Add immediate number to register A. 
                adi p2
                adi 08h
                ani 248             ; AND # with register A.
                ani p2
                in p2
                sui 08h             ; Subtract immediate number from register A.
                sui 3
                sui Final
                                    ;
                mvi a,p2            ; Move # to register A.
                cpi p2              ; 73 = A. Zero bit flag is true. Carry bit is false.
                                    ;
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
                                    ;
                mvi a,addr1         ; The addr1 value is over 255, which makes it too high for a byte. No error checking for this.
                                    ; 00000011 : 503 > immediate: addr1 : 1283 ... The low byte value.
                out 3
                                    ;
                ; mvi a,Fianl         ; Test an assembler error, "Label not found."
                                    ;
                                    ; --------------------------------------
    Success:
                mvi a,'+'
                out 3
                                    ; --------------------------------------
                jmp Halt            ; Jump back to the early halt command.
                                    ;
                                    ; --------------------------------------
                end