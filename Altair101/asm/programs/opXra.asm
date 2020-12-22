                            ; --------------------------------------
                            ; Test XRA.
                            ; Exclusive OR a register, with register with A.
                            ;
                            ; A or B, but not, A and B
                            ;   01010001
                            ;   01000101
                            ;   --------
                            ;   00010100
                            ;
                            ; --------------------------------------
            lxi sp,1024     ; Set stack pointer.
    Test:
            mvi a,5         ; 00000101
            mvi b,10        ; 00001010
            mvi c,11        ; 00001011
            mvi d,5         ; 00000101
            mvi e,10        ; 00001010
            mvi h,11        ; 00001011
            mvi l,5         ; 00000101
                            ; --------------------------------------
            call newTest
            mvi a,6         ; Initialize register values for testing.
            out 37          ; Print register A
            out 30          ; Register B.
            xra b           ; Exclusive OR register, with register with A.
            out 37          ; Print register A
                            ;
            call newTest
            mvi a,6
            out 37
            out 31          ; Register C.
            xra c
            out 37
                            ;
            call newTest
            mvi a,6
            out 37
            out 32          ; Register D.
            xra d
            out 37
                            ;
            call newTest
            mvi a,6
            out 37
            out 33
            xra e           ; Exclusive OR register, with register with A.
            out 37
                            ;
            call newTest
            mvi a,6
            out 37
            out 34
            xra h           ; Exclusive OR register B, with register with A.
            out 37
                            ;
            call newTest
            mvi a,6
            out 37
            out 35          ; Register L.
            xra l
            out 37
                            ; --------------------------------------
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            hlt
            jmp Test
                            ; --------------------------------------
    newTest:
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            mvi a,'-'
            out 3
            mvi a,'-'
            out 3
            mvi a,'-'
            out 3
            mvi a,'-'
            out 3
            mvi a,'-'
            out 3
            mvi a,'-'
            out 3
            ret

            end
                            ; --------------------------------------
                            ;
+ Download complete.
?- + r, RUN.
?- + runProcessor()

------
 > Register A =   6 = 006 = 00000110
 > Register B =  10 = 012 = 00001010
 > Register A =   0 = 000 = 00000000
------
 > Register A =   6 = 006 = 00000110
 > Register C =  11 = 013 = 00001011
 > Register A =  13 = 015 = 00001101
------
 > Register A =   6 = 006 = 00000110
 > Register D =   5 = 005 = 00000101
 > Register A =   3 = 003 = 00000011
------
 > Register A =   6 = 006 = 00000110
 > Register E =  10 = 012 = 00001010
 > Register A =  12 = 014 = 00001100
------
 > Register A =   6 = 006 = 00000110
 > Register H =  11 = 013 = 00001011
 > Register A =  13 = 015 = 00001101
------
 > Register A =   6 = 006 = 00000110
 > Register L =   5 = 005 = 00000101
 > Register A =   3 = 003 = 00000011
++ HALT, host_read_status_led_WAIT() = 0
                            ;
                            ; --------------------------------------
