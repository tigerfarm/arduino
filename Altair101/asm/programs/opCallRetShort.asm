                        ; --------------------------------------
                        ; Test CALL and RET.
                        ; --------------------------------------
            jmp Test    ; Jump to the test section.
    Error:
            mvi a,'-'
            out 3
    Halt:
            mvi a,'\n'
            out 3
            hlt         ; The program will halt at each iteration, after the first.
                        ;
                        ; --------------------------------------
    Test:
            mvi a,1
            out 37
            call Hello1 ; Test using a label.
            mvi a,2
            out 37
            call Hello2
                        ;
            mvi a,3
            out 37
            call 76     ; Test using an immediate value instead of a label.
                        ;
                        ; --------------------------------------
    Success:
            mvi a,'\n'
            out 3
            mvi a,'+'
            out 3
                        ; --------------------------------------
            jmp Halt    ; Jump back to the early halt command.
                        ;
                        ; --------------------------------------
                        ; Routines
    Hello1:
            mvi a,'\n'
            out 3
            mvi a,'#'
            out 3
            mvi a,'1'
            out 3
            ret
            jmp Error   ; Failed to return.
    Hello2:
            mvi a,'\n'
            out 3
            mvi a,'#'
            out 3
            mvi a,'2'
            out 3
            ret
            jmp Error   ; Failed to return.
    Hello3:
            mvi a,'\n'
            out 3
            mvi a,'#'
            out 3
            mvi a,'3'
            out 3
            ret
            jmp Error   ; Failed to return.
                        ; --------------------------------------
            end