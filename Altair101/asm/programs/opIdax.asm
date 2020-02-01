                            ; --------------------------------------
                            ; Test LDAX.
                            ; Load register A with data value from address B:C or D:E.
                            ;
                            ; --------------------------------------
            jmp Test1       ; Jump to start of the test.
                            ;
                            ; --------------------------------------
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Test1:
            mvi b,0         ; Load B:C with the address of 0, which is the JMP code.
            mvi c,0
            ldax b          ; B:C data = octal 0303
            out 37          ; Print register A = 195 = 0303 = B11000011
    Test2:
            mvi d,0         ; Load D:E with the address of 3, which is the HLT code.
            mvi e,3
            ldax b          ; D:E data = octal 0166
            out 37          ; Print register A = 118 = 0166 = B01110110
                            ;
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ;
                            ; --------------------------------------
            end
