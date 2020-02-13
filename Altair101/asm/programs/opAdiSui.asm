                        ; --------------------------------------
                        ; Test ADI and SUI.
                        ; Add immediate number to register A.
                        ; Subtract immediate number from register A.
                        ; --------------------------------------
            jmp Test    ; Jump to start the test.
                        ; --------------------------------------
    c3      equ     '3'
    i3      equ     3
    c6      equ     '6'
    i6      equ     6
                        ; --------------------------------------
    Error:
            out 39      ; Print the registers and other system values.
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
    Test:
            mvi a,73    ; Move # to register A.
            out 37      ; > Register A =  73 = 111 = 01001001
                        ; --------------------------------------
                        ; Test with decimal value.
            adi i3      ; Add immediate number to register A.
                        ; Note, i3 fails: > Register A =  54 = 066 = 00110110
                        ; Which implies, i3 = -?.
            out 37      ; > Register A =  76 = 114 = 01001100
            mvi b,76    ; Answer should be 76. Move # to register B.
            cmp b       ; B = A. Zero bit flag is true.
            jz okaya1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okaya1:
                        ; --------------------------------------
            mvi a,73    ; Move # to register A.
            out 37      ; > Register A =  73 = 111 = 01001001
                        ; --------------------------------------
                        ; Test with EQU value.
            adi i3      ; Add immediate number to register A.
            out 37      ; > Register A =  76 = 114 = 01001100
            mvi b,76    ; Answer should be 76. Move # to register B.
            cmp b       ; B = A. Zero bit flag is true.
            jz okaya2   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okaya2:
                        ; --------------------------------------
                        ; Test with decimal value.
            sui 6       ; Subtract immediate number from register A.
            out 37      ; > Register A =  70 = 106 = 01000110
            mvi c,70    ; Move # to register C.
            cmp c       ; C = A.
            jz okays1
            jmp Error
    okays1:
                        ; --------------------------------------
                        ; Test with EQU value.
            sui i6      ; Subtract immediate number from register A.
            out 37      ; > Register A =  70 = 106 = 01000110
            mvi c,64    ; Move # to register C.
            cmp c       ; C = A.
            jz okays2
            jmp Error
    okays2:
                        ; --------------------------------------
            jmp Halt    ; Jump back to the early halt start command.
            end

            push a      ; Retain register A value.
            mvi a,'\n'
            out 3
            mvi a,'A'
            out 3
            mvi a,'+'
            out 3
            mvi a,c3
            out 3
            mvi a,'='
            out 3
            pop a

            push a      ; Retain register A value.
            mvi a,'\n'
            out 3
            mvi a,'A'
            out 3
            mvi a,'-'
            out 3
            mvi a,c6
            out 3
            mvi a,'='
            out 3
            pop a
