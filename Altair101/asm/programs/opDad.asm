                            ; --------------------------------------
                            ; Test DAD.
                            ; DAD RP 00RP1001  Add register pair(RP: B:C or D:E) to H:L. Set carry bit.
                            ;          00=BC   (B:C as 16 bit register)
                            ;          01=DE   (D:E as 16 bit register)
                            ;
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
            mvi a,0         ; Initialize register values for testing.
            mvi b,1
            mvi c,2
            mvi d,3
            mvi e,5
            mvi h,0
            mvi l,0
            out 38          ; Print the register values.
                            ;
                            ; --------------------------------------
            out 36          ; Print register pair, H:L.
            dad b           ; Add register pair B:C to H:L.
            out 36          ; Print register pair, H:L.
            dad d           ; Add register pair D:E to H:L.
            out 36          ; Print register pair, H:L.
            dad h           ; Add register pair H:L to H:L.
            out 36          ; Print register pair, H:L.
                            ;
                            ; --------------------------------------
                            ; After making Processor.ino updates, DAD SP works.
                            ; However, I should implement the stack to Altair 8800 specifications.
                            ;
            dad sp          ; Add the stack pointer address value to H:L.
            out 36          ; Print register pair, H:L.
            out 39          ; Print data which includes the stack pointer.
                            ;
            push b          ; Change the stack pointer value.
            push b
            dad sp          ; Add the stack pointer address value to H:L.
            out 36
            out 39
            pop b
            pop b
            out 39
                            ;
                            ; --------------------------------------
            hlt
            jmp Start
                            ;
                            ; --------------------------------------
            end
                            ; --------------------------------------
                            ; Successful run:
                            ;
------------
+ regA:   0 = 000 = 00000000
+ regB:   1 = 001 = 00000001  regC:   2 = 002 = 00000010
+ regD:   3 = 003 = 00000011  regE:   5 = 005 = 00000101
+ regH:   0 = 000 = 00000000  regL:   0 = 000 = 00000000
------------
 > Register H:L =   0:  0, Data:  62 = 076 = 00111110
 > Register H:L =   1:  2, Data:   0 = 000 = 00000000
 > Register H:L =   4:  7, Data:   0 = 000 = 00000000
 > Register H:L =   8: 14, Data: 197 = 305 = 11000101
 > Register H:L =   8: 78, Data:   0 = 000 = 00000000
------------
+ regA:   0 = 000 = 00000000
+ regB:   1 = 001 = 00000001  regC:   2 = 002 = 00000010
+ regD:   3 = 003 = 00000011  regE:   5 = 005 = 00000101
+ regH:   8 = 010 = 00001000  regL:  78 = 116 = 01001110
+ Zero bit flag: 1, Carry bit flag: 0
+ Stack pointer: 64
------------
 > Register H:L =   8:138, Data: 116 = 164 = 01110100
------------
+ regA:   0 = 000 = 00000000
+ regB:   1 = 001 = 00000001  regC:   2 = 002 = 00000010
+ regD:   3 = 003 = 00000011  regE:   5 = 005 = 00000101
+ regH:   8 = 010 = 00001000  regL: 138 = 212 = 10001010
+ Zero bit flag: 1, Carry bit flag: 0
+ Stack pointer: 60
------------
------------
+ regA:   0 = 000 = 00000000
+ regB:   1 = 001 = 00000001  regC:   2 = 002 = 00000010
+ regD:   3 = 003 = 00000011  regE:   5 = 005 = 00000101
+ regH:   8 = 010 = 00001000  regL: 138 = 212 = 10001010
+ Zero bit flag: 1, Carry bit flag: 0
+ Stack pointer: 64
------------
                            ;
                            ; --------------------------------------
