                            ; --------------------------------------
                            ; Test MVI using serial monitor output.
                            ; 
                            ; Move various immediate types to various registers.
                            ; 
                            ; --------------------------------------
                            ;
    iValue  equ     39      ; Label immediate to test with.
                            ; iValue = 39
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
            mvi a,6         ; Move an immediate number to each register.
            mvi b,0
            mvi c,1
            mvi d,2
            mvi e,3
            mvi h,4
            mvi l,5
            out 38          ; Print the register values.
                            ; + regA:   6 = 006 = 00000110
                            ; + regB:   0 = 000 = 00000000  regC:   1 = 001 = 00000001
                            ; + regD:   2 = 002 = 00000010  regE:   3 = 003 = 00000011
                            ; + regH:   4 = 004 = 00000100  regL:   5 = 005 = 00000101
                            ; 
                            ; --------------------------------------
            mvi a,0Ah       ; Move a hex value immediate to each register.
            mvi b,00Ah      ; 0Ah = 10 decimal
            mvi c,0Bh
            mvi d,0Ch
            mvi e,0Dh
            mvi h,0Eh
            mvi l,0Fh
            out 38          ; Print the register values.
                            ; + regA:  10 = 012 = 00001010
                            ; + regB:  10 = 012 = 00001010  regC:  11 = 013 = 00001011
                            ; + regD:  12 = 014 = 00001100  regE:  13 = 015 = 00001101
                            ; + regH:  14 = 016 = 00001110  regL:  15 = 017 = 00001111
                            ; 
                            ; --------------------------------------
            mvi a,'a'       ; Move a character value immediate number to each register.
            mvi b,'b'
            mvi c,'c'
            mvi d,'d'
            mvi e,'e'
            mvi h,'f'
            mvi l,'g'
            out 38          ; Print the register values.
                            ; + regA:  97 = 141 = 01100001
                            ; + regB:  98 = 142 = 01100010  regC:  99 = 143 = 01100011
                            ; + regD: 100 = 144 = 01100100  regE: 101 = 145 = 01100101
                            ; + regH: 102 = 146 = 01100110  regL: 103 = 147 = 01100111
                            ;
                            ; --------------------------------------
            mvi a,iValue    ; Move a label immediate number to each register.
            mvi b,iValue
            mvi c,iValue
            mvi d,iValue
            mvi e,iValue
            mvi h,iValue
            mvi l,iValue
            out 38          ; Print the register values.
                            ; + regA:  39 = 047 = 00100111
                            ; + regB:  39 = 047 = 00100111  regC:  39 = 047 = 00100111
                            ; + regD:  39 = 047 = 00100111  regE:  39 = 047 = 00100111
                            ; + regH:  39 = 047 = 00100111  regL:  39 = 047 = 00100111
                            ;
                            ; --------------------------------------
            hlt             ; The program will halt at each iteration, after the first.
            jmp Start
            end