                            ; --------------------------------------
                            ; Test INX.
                            ; INX RP 00RP0011 Increment register pair
                            ;          00=BC   (B:C as 16 bit register)
                            ;          01=DE   (D:E as 16 bit register)
                            ;          10=HL   (H:L as 16 bit register)
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
            inx b           ; Increment register pair B:C.
            inx d           ; Increment register pair D:E.
            inx m           ; Increment register pair H:L.
            out 38          ; Print the register value results.
                            ;
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ;
                            ; --------------------------------------
            end
