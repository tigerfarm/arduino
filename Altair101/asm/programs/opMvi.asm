                            ; --------------------------------------
                            ; Test MVI using serial monitor output.
                            ; The various immediate types, tests the assembler.
                            ; --------------------------------------
            jmp Start       ; Jump to bypass the halt.
                            ;
    iValue  equ     39      ; Label immediate to test with.
                            ; iValue = 39
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
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
                            ; --------------------------------------
            mvi a,0Ah       ; Move a hex value immediate to each register.
            mvi b,00Ah      ; 0Ah = 10 decimal
            mvi c,0Bh
            mvi d,0Ch
            mvi e,0Dh
            mvi h,0Eh
            mvi l,0Fh
            out 38          ; Print the register values.
                            ; --------------------------------------
            mvi a,'a'       ; Move a character value immediate number to each register.
            mvi b,'b'
            mvi c,'c'
            mvi d,'d'
            mvi e,'e'
            mvi h,'f'
            mvi l,'g'
            out 38
                            ; --------------------------------------
            mvi a,iValue    ; Move a label immediate number to each register.
            mvi b,iValue
            mvi c,iValue
            mvi d,iValue
            mvi e,iValue
            mvi h,iValue
            mvi l,iValue
            out 38
                            ; --------------------------------------
            NOP
            jmp Halt        ; Jump back to the early halt command.
            end