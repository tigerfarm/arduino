                                    ; ------------------------------------------
                                    ; Test opcodes RLC and RRC.
                                    ; Rotate (shift) register A left(RLC) and right(RRC).
                                    ; 
                                    ; ------------------------------------------
                                    ; First byte needs to be an opcode to run.
                jmp Start
                                    ; ------------------------------------------
    opcode1     db      'RLC'
    opcode2     db      'RRC'
    msgSuccess  db      '+ Success: '
    msgError    db      '- Error.'
    PRINT_PORT  equ     3           ; Print to the serial port.
                                    ;
                                    ; -------------------
                                    ; Special characters:
    NL          equ     10          ; New line, '\n'.
    TERMB       equ     0ffh        ; String terminator.
                                    ;
                                    ; ------------------------------------------
    Start:
                mvi a,1             ; Move # to register A:    00 000 001 = 1
                out 37              ; Print register A answer: 00 000 001 = 1
                cpi 1               ; Check zero bit flag is true.
                jnz Error           ; Zero bit flag is not set, jump.
                                    ; ------------------------------------------
                rlc                 ; Rotate left
                out 37              ; Print register A answer: 00 000 010 = 2
                cpi 2               ; Check zero bit flag is true.
                jnz Error           ; Zero bit flag is not set, jump.
                                    ;
                rlc                 ; Rotate left
                out 37              ; Print register A answer: 00 000 100 = 4
                cpi 4               ; Check zero bit flag is true.
                jnz Error           ; Zero bit flag is not set, jump.
                                    ;
                rlc                 ; Rotate left
                out 37              ; Print register A answer: 00 001 000 = 8
                cpi 8               ; Check zero bit flag is true.
                jnz Error           ; Zero bit flag is not set, jump.
                                    ;
                rlc                 ; Rotate left
                out 37
                cpi 16
                jnz Error
                rlc                 ; Rotate left
                out 37
                cpi 32
                jnz Error
                rlc                 ; Rotate left
                out 37
                cpi 64
                jnz Error
                rlc                 ; Rotate left
                out 37
                cpi 128
                jnz Error
                rlc                 ; Rotate left
                out 37
                cpi 1               ; Rotated all the way around.
                jnz Error
    Success1:
                call printNL
                lxi h,msgSuccess
                call print
                lxi h,opcode1
                call print
                mvi a,'.'
                out PRINT_PORT
                call printNL
                                    ; ------------------------------------------
                mvi a,128           ; Move # to register A:    10 000 000 = 128
                out 37              ; Print register A answer: 00 000 001 = 128
                cpi 128             ; Check zero bit flag is true.
                jnz Error           ; Zero bit flag is not set, jump.
                                    ; ------------------------------------------
                rrc                 ; Rotate right
                out 37              ; Print register A answer: 01 000 010 = 64
                cpi 64              ; Check zero bit flag is true.
                jnz Error           ; Zero bit flag is not set, jump.
                                    ;
                rrc                 ; Rotate right
                out 37
                cpi 32
                jnz Error
                                    ;
                rrc                 ; Rotate right
                out 37
                cpi 16
                jnz Error
                                    ;
                rrc                 ; Rotate right
                out 37
                cpi 8
                jnz Error
                                    ;
                rrc                 ; Rotate right
                out 37
                cpi 4
                jnz Error
                                    ;
                rrc                 ; Rotate right
                out 37
                cpi 2
                jnz Error
                                    ;
                rrc                 ; Rotate right
                out 37
                cpi 1
                jnz Error
                rrc                 ; Rotated all the way around.
                out 37
                cpi 128
                jnz Error
                                    ; ------------------------------------------
    Success2:
                call printNL
                lxi h,msgSuccess
                call print
                lxi h,opcode2
                call print
                mvi a,'.'
                out PRINT_PORT
                call printNL
                                    ;
                hlt                 ; The program will halt at each iteration, after the first.
                jmp Start
                                    ; ------------------------------------------
    Error:
                call printNL
                lxi h,msgError
                call print
                call printNL
                out 39              ; Print the registers and other system values.
                hlt                 ; Halt after the error.
                jmp Start
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

                end
