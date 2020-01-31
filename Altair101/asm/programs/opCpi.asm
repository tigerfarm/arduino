                            ; --------------------------------------
                            ; Test CPI and conditional jumps.
                            ; Compare an number (immediate) to A, and then set Carry and Zero bit flags.
                            ; If #=A, set Zero bit to 1, Carry bit to 0. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
                            ; Note, register A remain the same after the compare.
                            ; --------------------------------------
            jmp Test        ; Jump to start of the test.
                            ; --------------------------------------
    iValue  equ     42      ; Label immediate to test with.
                            ; --------------------------------------
    Error:
            mvi a,'\n'
            out 3
            mvi a,'-'       ; Move the byte value of "-" to register A.
            out 3           ; Output register A content to the serial port (serial monitor).
            mvi a,'-'
            out 3
            mvi a,' '
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
            out 39          ; Print the registers and other system values.
            hlt             ; Halt after the error.
            jmp Test        ; Jump to bypass the halt.
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Test:
            mvi a,'1'
            out 3
            mvi a,73        ; Move # to register A.
            cpi 73          ; 73 = A. Zero bit flag is true. Carry bit is false.
            jnz Error       ; Zero bit flag is set, don't jump.
            jc Error        ; Carry bit flag is not set, don't jump.
            jz okay1        ; Zero bit flag is set, jump.
            jmp Error       ; The above should have jumped passed this.
    okay1:
            jnc Test2       ; Carry bit flag is not set, jump to the next test.
            jmp Error       ; The above should have jumped passed this.
                            ;
                            ; --------------------------------------
                            ; Use and EQU value.
    Test2:
            mvi a,'2'
            out 3
            mvi a,42        ; Move # to register A.
            cpi iValue      ; 42 = A. Zero bit flag is true. Carry bit is false.
            jnz Error       ; Zero bit flag is set, don't jump.
            jc Error        ; Carry bit flag is not set, don't jump.
            jz okay1b       ; Zero bit flag is set, jump.
            jmp Error       ; The above should have jumped passed this.
    okay1b:
            jnc okay2b      ; Carry bit flag is not set, jump to the end of this test.
            jmp Error       ; The above should have jumped passed this.
                            ; --------------------------------------
    okay2b:
            mvi a,'S'
            out 3
                            ; --------------------------------------
            mvi a,'\n'
            out 3
            mvi a,'+'       ; Move the byte value of "+" to register A.
            out 3           ; Output register A content to the serial port (serial monitor).
            mvi a,' '
            out 3
            mvi a,'S'
            out 3
            mvi a,'u'
            out 3
            mvi a,'c'
            out 3
            mvi a,'c'
            out 3
            mvi a,'e'
            out 3
            mvi a,'s'
            out 3
            mvi a,'s'
            out 3
            mvi a,':'
            out 3
            mvi a,' '
            out 3
            mvi a,'C'
            out 3
            mvi a,'P'
            out 3
            mvi a,'I'
            out 3
                            ; --------------------------------------
            NOP
            jmp Halt        ; Jump back to the early halt command.
                            ; --------------------------------------
            end
