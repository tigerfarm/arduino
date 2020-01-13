                        ; --------------------------------------
                        ; Test OUT to the serial monitor.
                        ; --------------------------------------
            jmp Start   ; Jump to bypass the halt.
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
                        ; Output the word, "Hello" to the serial monitor.
    Start:
            mvi a,'H'   ; Move the byte value of "h" to register A.
            out 3       ; Output register A content to the serial port (serial monitor).
            mvi a,'e'   ;
            out 3       ;
            mvi a,'l'   ;
            out 3       ;
            mvi a,'l'   ;
            out 3       ;
            mvi a,'o'   ;
            out 3       ;
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
                        ; End.