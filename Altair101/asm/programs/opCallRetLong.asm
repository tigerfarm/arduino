                        ; --------------------------------------
                        ; Test CALL and RET.
                        ; 
                        ; --------------------------------------
                        ;
            lxi sp,512  ; Set stack pointer which is used with CALL and RET.
    Start:
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'1'
            out 3
            mvi a,','
            out 3
            mvi a,' '
            out 3
            call Called
            call Hello1
                        ;
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'2'
            out 3
            mvi a,','
            out 3
            mvi a,' '
            out 3
            call Called
            call Hello2
                        ;
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'3'
            out 3
            mvi a,','
            out 3
            mvi a,' '
            out 3
            call Called
            call Hello3
                        ; --------------------------------------
    Success:
            NOP
            mvi a,'\r'
            out 3
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
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
                        ; --------------------------------------
            hlt         ; The program will halt at each iteration, after the first.
            jmp Start   ; Jump back to the early halt command.
                        ;
                        ; --------------------------------------
                        ; Call routines
    Called:
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
            mvi a,':'
            out 3
            mvi a,' '
            out 3
            ret
            NOP
            jmp Error   ; Failed to return.
    Hello1:
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
            NOP
            jmp Error   ; Failed to return.
    Hello3:
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
            mvi a,'3'
            out 3
            ret
                        ; --------------------------------------
            NOP
            jmp Error   ; Failed to return.
                        ; --------------------------------------
    Error:
            mvi a,'\r'
            out 3
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
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ;
                        ; --------------------------------------
            end
                        ; --------------------------------------
                        ; Successful run:
+ Ready to receive command.
+ runProcessor()

++ 1, Called: Hello #1
++ 2, Called: Hello #2
                      ++ 3, Called: Hello #3
+ Success.
++ HALT, host_read_status_led_WAIT() = 0
                        ; --------------------------------------
