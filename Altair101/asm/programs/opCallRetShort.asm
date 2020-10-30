                        ; --------------------------------------
                        ; Test CALL and RET.
                        ; 
                        ; Successful run:
                        ; + Control, Run.
                        ; + runProcessor()
                        ; ++ 1>:1<
                        ; ++ 2>:2<
                        ; ++ 3>:3<
                        ; ++ S
                        ; + HLT, program halted.
                        ; 
                        ; --------------------------------------
            jmp Test    ; Jump to the test section.
    Error:
            mvi a,'-'
            out 3
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ;
                        ; --------------------------------------
    Test:
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
            call 140    ; Test using an immediate value instead of a label.
                        ;
                        ; --------------------------------------
    Success:
            mvi a,'\n'
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
            jmp Halt    ; Jump back to the early halt command.
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
            end