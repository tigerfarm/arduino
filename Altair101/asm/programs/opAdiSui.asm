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
                        ; Print first, else register A will be changed.
            mvi a,'\n'
            out 3
            mvi a,'-'
            out 3
            mvi a,'E'
            out 3
            mvi a,'r'
            out 3
            mvi a,'r'
            out 3
            mvi a,'o'
            out 3
            mvi a,'r'
            out 3
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
    Test:
            mvi a,73    ; Move # to register A.
            out 37      ; > Register A =  73 = 111 = 01001001
                        ; --------------------------------------
                        ; Test with decimal value.
                        ;
            mov b,a     ; Retain register A value.
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
            mov a,b
                        ;
            adi i3      ; Add immediate number to register A.
            out 37      ; > Register A =  76 = 114 = 01001100
            mvi b,76    ; Answer should be 76. Move # to register B.
            cmp b       ; B = A. Zero bit flag is true.
            jz okaya1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okaya1:
                        ; --------------------------------------
                        ; Test with EQU value.
                        ;
            mov b,a     ; Retain register A value.
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
            mov a,b
                        ;
            adi i3      ; Add immediate number to register A.
            out 37
            mvi b,79    ; Answer should be 79. Move # to register B.
            cmp b       ; B = A. Zero bit flag is true.
            jz okaya2   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okaya2:
                        ; --------------------------------------
                        ; Test with decimal value.
            mov b,a 
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
            mov a,b
                        ;
            sui 6       ; Subtract immediate number from register A.
            out 37      ; > Register A =  73
            mvi c,73    ; Move # to register C.
            cmp c       ; C = A.
            jz okays1
            jmp Error
    okays1:
                        ; --------------------------------------
                        ; Test with EQU value.
            mov b,a 
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
            mov a,b
                        ;
            sui i6      ; Subtract immediate number from register A.
            out 37      ; > Register A =  67
            mvi c,67    ; Move # to register C.
            cmp c       ; C = A.
            jz okays2
            jmp Error
    okays2:
                        ; --------------------------------------
            jmp Halt    ; Jump back to the early halt start command.
            end
