                        ; --------------------------------------
                        ; Test CALL and RET.
                        ; --------------------------------------
            jmp Test    ; Jump to the test section.
    Error:
            mvi a,'\n'
            out 3
            mvi a,'-'   ; Move the byte value of "-" to register A.
            out 3       ; Output register A content to the serial port (serial monitor).
            mvi a,' '
            out 3
            mvi a,'E'
            out 3
            mvi a,'r'
            out 3
            mvi a,'r'
            out 3
            mvi a,'o'
            out 3
            mvi a,'r'
            out 3
            out 39      ; Print the registers and other system values.
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ;
                        ; --------------------------------------
    Test:
            NOP
            mvi a,1
            out 37
            call Hello1
            mvi a,2
            out 37
            call Hello2
                        ; --------------------------------------
    Success:
            NOP
            mvi a,'\n'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'S'
            out 3
            mvi a,'u'
            out 3
            mvi a,'c'
            out 3
            mvi a,'c'
            out 3
            mvi a,'e'
            out 3
            mvi a,'s'
            out 3
            mvi a,'s'
            out 3
            mvi a,'.'
            out 3
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
                        ;
                        ; --------------------------------------
                        ; Call routines
    Hello1:
            NOP
            mvi a,'\n'
            out 3
            mvi a,'C'   ; Move the byte value of "h" to register A.
            out 3       ; Output register A content to the serial port (serial monitor).
            mvi a,'a'
            out 3
            mvi a,'l'
            out 3
            mvi a,'l'
            out 3
            mvi a,'e'
            out 3
            mvi a,'d'
            out 3
            mvi a,'*'
            out 3
            mvi a,' '
            out 3
            mvi a,'H'
            out 3
            mvi a,'e'
            out 3
            mvi a,'l'
            out 3
            mvi a,'l'
            out 3
            mvi a,'o'
            out 3
            mvi a,' '
            out 3
            mvi a,'#'
            out 3
            mvi a,'1'
            out 3
            ret
            NOP
            jmp Error   ; Failed to return.
    Hello2:
            NOP
            mvi a,'\n'
            out 3
            mvi a,'C'   ; Move the byte value of "h" to register A.
            out 3       ; Output register A content to the serial port (serial monitor).
            mvi a,'a'
            out 3
            mvi a,'l'
            out 3
            mvi a,'l'
            out 3
            mvi a,'e'
            out 3
            mvi a,'d'
            out 3
            mvi a,'*'
            out 3
            mvi a,' '
            out 3
            mvi a,'H'
            out 3
            mvi a,'e'
            out 3
            mvi a,'l'
            out 3
            mvi a,'l'
            out 3
            mvi a,'o'
            out 3
            mvi a,' '
            out 3
            mvi a,'#'
            out 3
            mvi a,'2'
            out 3
            ret
                        ; --------------------------------------
            NOP
            jmp Error   ; Failed to return.
                        ; End.