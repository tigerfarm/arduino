                            ; --------------------------------------
                            ; Test LDAX.
                            ; Load register A with data value from address B:C or D:E.
                            ;
                            ; --------------------------------------
    Test1:
            mvi b,0         ; Load B:C with the address of 0, which is the MVI(b) ocode.
            mvi c,0
            ldax b          ; Load register A with data value from address B:C.
            out 37          ; Print > Register A =   6 = 006 = 00000110
    Test2:
            mvi d,0         ; Load D:E with an address, which is the LDAX opcode.
            mvi e,4
            ldax d          ; Load register A with data value from address D:E.
            out 37          ; Print > Register A =  10 = 012 = 00001010
                            ;
                            ; --------------------------------------
            hlt             ; The program will halt before another iteration.
            jmp Test1
                            ;
                            ; --------------------------------------
            end
