                        ; --------------------------------------
                        ; Test opcode ANI and ORA.
                        ; AND # (immediate db) with register A.
                        ; OR register S, with register A.
                        ; --------------------------------------
            jmp Test    ; Jump to bypass the halt.
    Error:
            mvi a,'\n'
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
            jmp Test    ; Jump to bypass the halt.
                        ; --------------------------------------
    Halt:
            mvi a,'\n'
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
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
    Test:
                        ;          ani # 11 100 110
            mvi a,176   ; Move # to register A:    10 110 000 = 176
            ani 248     ; AND # with register A:   11 111 000 = 248
            out 37      ; Print register A answer: 10 110 000 = 176
            cpi 176     ; 176 = A. Zero bit flag is true.
            jz okayani  ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okayani:
            mvi a,'\n'
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
            mvi a,'-'
            out 3
            mvi a,' '
            out 3
            mvi a,'A'
            out 3
            mvi a,'N'
            out 3
            mvi a,'I'
            out 3
                        ; --------------------------------------
                        ; ORA S     10110SSS          ZSPCA   OR source register with A
            mvi a,73    ; Move # to register A:                   01 001 001 = 73
            mvi b,70    ; Move # to register B:                   01 000 110 = 70
            ora b       ; OR register B, with register A. Answer: 01 001 111 = 79.
            out 37      ; Echo register A.
            cpi 79      ; 79 = A. Zero bit flag is true.
            jz okayb1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okayb1:
            mvi a,73
            mvi c,70
            ora c
            out 37
            cpi 79
            jz okayc1
            jmp Error
    okayc1:
            mvi a,73
            mvi d,70
            ora d
            cpi 79
            jz okayd1
            jmp Error
    okayd1:
            mvi a,73
            mvi e,70
            ora e
            out 37
            cpi 79
            jz okaye1
            jmp Error
    okaye1:
            mvi a,73
            mvi h,70
            ora h
            out 37
            cpi 79
            jz okayh1
            jmp Error
    okayh1:
            mvi a,73
            mvi l,70
            ora l
            out 37
            cpi 79
            jz okayl1
            jmp Error
    okayl1:
            mvi a,73    ; Move # to register A:                                  01 001 001 = 73
            mvi h,0
            mvi l,0     ; Register M address data (opcode jmp) =                 11 000 011
            ora m       ; OR data a register M address, with register A. Answer: 11 001 011 = ?
            out 37
            cpi 203     ; Wrong compare value will dump the answer.
            jz okaym1
            jmp Error
    okaym1:
            mvi a,'\n'
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
            mvi a,'-'
            out 3
            mvi a,' '
            out 3
            mvi a,'O'
            out 3
            mvi a,'R'
            out 3
            mvi a,'A'
            out 3
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
                        ; End.