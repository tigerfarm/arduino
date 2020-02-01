                            ; --------------------------------------
                            ; Test DAD.
                            ; DAD RP 00RP1001  Add register pair(RP: B:C or D:E) to H:L. Set carry bit.
                            ;          00=BC   (B:C as 16 bit register)
                            ;          01=DE   (D:E as 16 bit register)
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
            dad b           ; Add register pair B:C to H:L.
            out 36          ; Print register pair, H:L.
            dad d           ; Add register pair D:E to H:L.
            out 36          ; Print register pair, H:L.
                            ;
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ;
                            ; --------------------------------------
            end
