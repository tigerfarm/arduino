                        ; --------------------------------------
                        ; Test opcode ANI.
                        ; AND # (immediate db) with register A.
                        ; --------------------------------------
            jmp Test    ; Jump to test section, which bypasses the success and error messages.
                        ;
                        ; --------------------------------------
    Success:
            mvi a,10    ; Print newline.
            out 3
            mvi a,'+'   ; Move the byte value of "+" to register A.
            out 3       ; Output register A content to the serial port (serial monitor).
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
            mvi a,'A'
            out 3
            mvi a,'N'
            out 3
            mvi a,'I'
            out 3
                        ;
            hlt         ; The program will halt at each iteration, after the first.
            jmp Test
                        ; --------------------------------------
    Error:
            mvi a,10    ; Print newline.
            out 3
            mvi a,'-'   ; Move the byte value of "-" to register A.
            out 3       ; Output register A content to the serial port (serial monitor).
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
            out 39      ; Print the registers and other system values.
            hlt         ; Halt after the error.
            jmp Test
                        ; --------------------------------------
    Test:
            NOP
                        ;          ani # 11 100 110
            mvi a,176   ; Move # to register A:    10 110 000 = 176
            ani 248     ; AND # with register A:   11 111 000 = 248
            out 37      ; Print register A answer: 10 110 000 = 176
            cpi 176     ; 176 = A. Zero bit flag is true.
            jz okayani  ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okayani:
                        ; --------------------------------------
            NOP
            jmp Success ; Jump back to the Success message and halt.
                        ;
                        ; End.
