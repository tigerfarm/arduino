                            ; --------------------------------------
                            ; Test DAD.
                            ; DAD RP 00RP1001  Add register pair(RP: B:C or D:E) to H:L. Set carry bit.
                            ;          00=BC   (B:C as 16 bit register)
                            ;          01=DE   (D:E as 16 bit register)
                            ;
                            ; --------------------------------------
    Start:
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
            hlt
            jmp Start
                            ;
                            ; --------------------------------------
            end
                                    ; --------------------------------------
                                    ; UnSuccessful run:
                                    ;
--------------------------------------

+ regA:   6 = 006 = 00000110
+ regB:   0 = 000 = 00000000  regC:   1 = 001 = 00000001
+ regD:   2 = 002 = 00000010  regE:   3 = 003 = 00000011
+ regH:   4 = 004 = 00000100  regL:   5 = 005 = 00000101
------------
 > Register H:L =   4:  6, Data:   0 = 000 = 00000000
- Error, unknown opcode instruction:   0 = 000 = 00000000
- Error, at programCounter:  23 = 027 = 00010111
++      23:00000000 00010111: 00101001 : 29:051 > opcode: dad d

                                    ;
                                    ; --------------------------------------