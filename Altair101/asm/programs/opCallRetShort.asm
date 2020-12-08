                        ; --------------------------------------
                        ; Test CALL and RET.
                        ; --------------------------------------
            lxi sp,512  ; Set stack pointer, which is used with CALL and RET.
    Start:
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'1'
            out 3
            mvi a,'>'
            out 3
            call Hello1 ; Test using a label.
                        ; --------------------------------------
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'2'
            out 3
            mvi a,'>'
            out 3
            call Hello2
                        ; --------------------------------------
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'3'
            out 3
            mvi a,'>'
            out 3
            call 156    ; Test using an immediate value instead of a label.
                        ;
                        ; --------------------------------------
    Success:
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'S'
            out 3
                        ; --------------------------------------
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            hlt         ; The program will halt at each iteration, after the first.
            jmp Start   ; Jump back to the early halt command.
                        ;
                        ; --------------------------------------
                        ; Routines
    Hello1:
            mvi a,':'
            out 3
            mvi a,'1'
            out 3
            mvi a,'<'
            out 3
            ret
            jmp Error   ; Failed to return.
    Hello2:
            mvi a,':'
            out 3
            mvi a,'2'
            out 3
            mvi a,'<'
            out 3
            ret
            jmp Error   ; Failed to return.
    Hello3:
            mvi a,':'
            out 3
            mvi a,'3'
            out 3
            mvi a,'<'
            out 3
            ret
            jmp Error   ; Failed to return.
                        ; --------------------------------------
    Error:
            mvi a,'-'
            out 3
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
            end
                        ; --------------------------------------
                        ; Successful run:
+ Ready to receive command.
+ runProcessor()
++ 1>:1<
++ 2>:2<
++ 3>:3<
++ S
++ HALT, host_read_status_led_WAIT() = 0
                        ; --------------------------------------
