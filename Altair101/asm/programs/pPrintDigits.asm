                            ; --------------------------------------
                            ; Print digits from 0 to 9.
                            ;
                            ; Successful run:
                            ; ++ D:0123456789
                            ;
                            ; --------------------------------------
            jmp Start       ; Jump to bypass the halt.
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
            call NewTest
            mvi a,'D'
            out 3
            mvi a,':'
            out 3
                            ; --------------------------------------
            mvi a,0
            call PrintDigit
            mvi a,1
            call PrintDigit
            mvi a,2
            call PrintDigit
            mvi a,3
            call PrintDigit
            mvi a,4
            call PrintDigit
            mvi a,5
            call PrintDigit
            mvi a,6
            call PrintDigit
            mvi a,7
            call PrintDigit
            mvi a,8
            call PrintDigit
            mvi a,9
            call PrintDigit
                            ; --------------------------------------
            mvi a,12
            call PrintDigits
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ; 
                            ; --------------------------------------
                            ; Routines
    NewTest:
            mvi a,'\n'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            ret
                            ; 
                            ; --------------------------------------
                            ; Print from 0-99.
    PrintDigits:
                            ; 
                            ;  09 = 0000 1001 =  0 + 8 + 0 + 0 + 1
                            ;  06 = 0000 0110 =  0 + 0 + 4 + 2 + 0
                            ;  02 = 0000 0010 =  0 + 0 + 0 + 2 + 0
                            ;  10 = 0000 1010 =  0 + 8 + 0 + 2 + 0
                            ;  12 = 0000 1100 =  0 + 8 + 4 + 0 + 0
                            ;  16 = 0001 0000 = 16 + 0 + 0 + 0 + 0
                            ;  04 = 0000 0100 =  0 + 0 + 4 + 0 + 0
                            ;  20 = 0001 0100 = 16 + 0 + 4 + 0 + 0
                            ;  26 = 0001 0100 = 16 + 8 + 0 + 2 + 0
                            ;  30 = 0001 1110 = 16 + 8 + 4 + 2 + 0
                            ;  96 = 0110 0000 = 000 + 64 + 32 + 00 + 0 + 0 + 0 + 0
                            ; 100 = 0110 0100 = 000 + 64 + 32 + 00 + 0 + 4 + 0 + 0
                            ;
                            ;  01 = 0000 0001 = 000 + 00 + 00 + 00 + 0 + 0 + 0 + 1
                            ;  02 = 0000 0010 = 000 + 00 + 00 + 00 + 0 + 0 + 2 + 0
                            ;  04 = 0000 0100 = 000 + 00 + 00 + 00 + 0 + 4 + 0 + 0
                            ;  08 = 0000 1000 = 000 + 00 + 00 + 00 + 8 + 0 + 0 + 0
                            ;  16 = 0001 0000 = 000 + 00 + 00 + 16 + 0 + 0 + 0 + 0
                            ;  32 = 0010 0000 = 000 + 00 + 32 + 00 + 0 + 0 + 0 + 0
                            ;  64 = 0100 0000 = 000 + 64 + 00 + 00 + 0 + 0 + 0 + 0
                            ; 128 = 1000 0000 = 128 + 00 + 00 + 00 + 0 + 0 + 0 + 0
                            ; 
                            ;  16 = 0001 0000 = 16 + 0 + 0 + 0 + 0
                            ;  09 = 0000 1001 =  0 + 8 + 0 + 0 + 1
                            ;  06 = 0000 0110 =  0 + 0 + 4 + 2 + 0
                            ;  15 = 0000 1111
                            ;  05 = 0000 0101
                            ;  03 = 0000 0011
                            ;  10 = 0000 1010 =  0 + 8 + 0 + 2 + 0
                            ;  26 = 0001 0100 = 16 + 8 + 0 + 2 + 0
                            ; 
                            ; Print 10's. 09 = 0000 1001
                            ; 
            cpi 10          ; Print the 1's.
            jz Greater09    ; If A == 10, Zero bit flag is set.
            jnc PrintOnes   ; If A == 10, Carry bit flag not set.
    Greater09:
            mov b,a
            mvi a,'*'
            out 3
            mov a,b
            ret
                            ; 
    PrintOnes:
            call PrintDigit
            ret
                            ; --------------------------------------
                            ; Print from 0-9.
    PrintDigit:
            mvi b,'0'
            add b
            out 3
            ret
                            ; 
                            ; --------------------------------------
            end
