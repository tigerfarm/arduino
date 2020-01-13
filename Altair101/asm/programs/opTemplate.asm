                        ; --------------------------------------
                        ; Test opcode ANI.
                        ; AND # (immediate db) with register A.
                        ; --------------------------------------
            jmp Test    ; Jump to test section, which bypasses the success and error messages.
                        ;
        msgSuccess  db      '+ Success'
        msgError    db      '+ Error.'
                                    ;
                                    ; Special characters.
        SPACE   equ     32          ; 32 is space, ' '.
        NL      equ     10          ; 10 is new line, '\n'.
        TERMB   equ     0ffh        ; String terminator.
                        ;
                        ; --------------------------------------
    Success:
                lxi h,msgSuccess
                call PrintString
                mvi a,NL
                out 3
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
                        ; ------------------------------------------
                        ; Routine loop to print a DB string which starts a address, M.
    PrintString:
            mov a,m     ; Move the data from H:L address to register A. (HL) -> A. 
            cpi TERMB   ; Compare to see if it's the string terminate byte.
            jz Done
            out 3       ; Out register A to the serial terminal port.
            inr m       ; Increment H:L register pair.
            jmp PrintString
    Done:
            ret
                        ; End.
