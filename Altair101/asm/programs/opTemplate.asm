                                    ; --------------------------------------
                                    ; Test opcode ANI.
                                    ; AND # (immediate db) with register A.
                                    ; --------------------------------------
    opcode      db      'ANI'
    msgSuccess  db      '+ Success'
    msgError    db      '+ Error.'
    PRINT_PORT  equ     3           ; Print to the serial port.
                                    ;
                                    ; -------------------
                                    ; Special characters:
    SPACE       equ     32          ; 32 is space, ' '.
    NL          equ     10          ; 10 is new line, '\n'.
    TERMB       equ     0ffh        ; String terminator.
                                    ;
                                    ; --------------------------------------
    Test:
                NOP
                                    ; ani # 11 100 110
                mvi a,176           ; Move # to register A:    10 110 000 = 176
                ani 248             ; AND # with register A:   11 111 000 = 248
                out 37              ; Print register A answer: 10 110 000 = 176
                cpi 176             ; 176 = A. Zero bit flag is true.
                jz Success          ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
                                    ; --------------------------------------
    Success:
                call printNL
                lxi h,msgSuccess
                call print
                mvi a,':'
                out PRINT_PORT
                mvi a,' '
                out PRINT_PORT
                lxi h,opcode
                call print
                call printNL
                                    ;
                hlt                 ; The program will halt at each iteration, after the first.
                jmp Test
                                    ; --------------------------------------
    Error:
                call printNL
                lxi h,msgError
                call print
                call printNL
                out 39              ; Print the registers and other system values.
                hlt                 ; Halt after the error.
                jmp Test
                                    ; ------------------------------------------
                                    ; Routine loop to print a DB string which starts a address, M.
    print:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz Done
                out PRINT_PORT      ; Out register A to the print port.
                inr m               ; Increment H:L register pair.
                jmp print
    Done:
                ret
                                    ; ------------------------------------------
                                    ; Print a newline.
    printNL:
                mvi a,NL
                out PRINT_PORT
                ret

                                    ; End.
