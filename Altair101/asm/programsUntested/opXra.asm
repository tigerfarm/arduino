                            ; --------------------------------------
                            ; Test XRA.
                            ; Exclusive OR a register, with register with A.
                            ;
                            ; --------------------------------------
            jmp Test        ; Jump to start of the test.
                            ;
                            ; --------------------------------------
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Test:
                            ; --------------------------------------
            mvi a,6         ; Initialize register values for testing.
            mvi b,0
            mvi c,1
            mvi d,2
            mvi e,3
            mvi h,4
            mvi l,5
            out 38          ; Print the register values.
                            ;
                            ; --------------------------------------
            mvi a,6         ; Initialize register values for testing.
            xra b           ; Exclusive OR register B, with register with A.
            out 37          ; Print register A
                            ;
            mvi a,6         ; Initialize register values for testing.
            xra c           ; Exclusive OR register B, with register with A.
            out 37          ; Print register A
                            ;
            mvi a,6         ; Initialize register values for testing.
            xra d           ; Exclusive OR register B, with register with A.
            out 37          ; Print register A
                            ;
            mvi a,6         ; Initialize register values for testing.
            xra e           ; Exclusive OR register B, with register with A.
            out 37          ; Print register A
                            ;
            mvi a,6         ; Initialize register values for testing.
            xra h           ; Exclusive OR register B, with register with A.
            out 37          ; Print register A
                            ;
            mvi a,6         ; Initialize register values for testing.
            xra l           ; Exclusive OR register B, with register with A.
            out 37          ; Print register A
                            ;
            out 38          ; Print the updated register values.
                            ;
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ;
                            ; --------------------------------------
            end
