                                        ; --------------------------------------
                                        ; Test the various LCD OUT options.
                                        ; --------------------------------------
                                        ;
            jmp Start                   ; Jump to bypass the halt.
                                        ;
                                        ; --------------------------------------
                                        ;
    LCD         equ     1
    StartMsg    db      'Start...'      ; Strings to print out.
    Again       db      'Again.'        ; Strings to print out.
    TERMB       equ     0ffh            ; String terminator.
    NL          equ     10              ; 10 is new line, '\n'.
    Halt:
            hlt                         ; The program will halt at each iteration, after the first.
                                        ; --------------------------------------
    Start:
                                        ; --------------------------------------
                                        ; Turn back light off.
            mvi a,0
            out LCD
            hlt
                                        ; Turn back light on.
            mvi a,1
            out LCD
            hlt
                                        ;
                                        ; --------------------------------------
                                        ; Print "Hello" to the Arduino IDE serial monitor.
                                        ; Clear screen.
            mvi a,2
            out LCD
            hlt
                                        ;
                                        ; --------------------------------------
                                        ; Print "Hello" to the Arduino IDE serial monitor.
            mvi a,'\n'
            out LCD
            mvi a,'H'                   ; Move the byte value of "h" to register A.
            out LCD                       ; Output register A content to the serial port (serial monitor).
            mvi a,'e'
            out LCD
            mvi a,'l'
            out LCD
            mvi a,'l'
            out LCD
            mvi a,'o'
            out LCD
            mvi a,'\n'
            out LCD
            hlt
                                        ; --------------------------------------
            mvi a,NL
            out LCD
            lxi h,Again
            mvi a,NL
            out LCD
            call sPrint
            out 42                      ; Flash the LED light success sequence.
                                        ; --------------------------------------
            jmp Halt                    ; Jump back to the early halt command.
                                        ;
                                        ; --------------------------------------
        sPrint:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz sPrintDone
                out LCD                   ; Out register A to the serial terminal port.
                inr m                   ; Increment H:L register pair.
                jmp sPrint
        sPrintDone:
                ret
                                        ;
                                        ; --------------------------------------
                end
