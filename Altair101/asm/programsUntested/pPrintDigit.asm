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
    PrintDigit:
            mvi b,'0'
            add b
            out 3
            ret
                            ; 
                            ; --------------------------------------
            end
