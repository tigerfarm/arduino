                                    ; ------------------------------------------
                                    ; Test opcodes RLC and RRC.
                                    ; Rotate (shift) register A left(RLC) and right(RRC).
                                    ; 
                                    ; ------------------------------------------
                                    ; First byte needs to be an opcode to run.
                                    ; ------------------------------------------
                lxi sp,1024             ; Set stack pointer.
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
                lxi h,msgSuccess
                call printStr
                lxi h,opcode1
                call printStr
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
                lxi h,msgSuccess
                call printStr
                lxi h,opcode2
                call printStr
                mvi a,'.'
                out PRINT_PORT
                call printNL
                                    ;
                hlt                 ; The program will halt at each iteration, after the first.
                jmp Start
                                    ; ------------------------------------------
    Error:
                lxi h,msgError
                call printStr
                out 39              ; Print the registers and other system values.
                hlt                 ; Halt after the error.
                jmp Start
                                    ; ------------------------------------------
                                    ; Routine loop to print a DB string which starts a address, M.
                                        ; --------------------------------------
                                        ; Print a string.
    printStr:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz sPrintDone
                out PRINT_PORT          ; Out register A to the serial port.
                inx h                   ; Increment H:L register pair.
                jmp printStr
    sPrintDone:
                ret
                                    ; ------------------------------------------
                                    ; Print a newline.
    printNL:
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
                ret
                                    ; ------------------------------------------
    opcode1     db      'RLC'
    opcode2     db      'RRC'
    msgSuccess  db      '\r\n+ Success: '
    msgError    db      '\r\n- Error.\r\n'
    PRINT_PORT  equ     2           ; Print to the serial port.
                                    ;
                                    ; -------------------
                                    ; Special characters:
    TERMB       equ     0ffh        ; String terminator.
                                    ;
                                    ; ------------------------------------------
                end
                                    ; --------------------------------------
                                    ; Successful run:
+ Download complete.
?- + r, RUN.
?- + runProcessor()

 > Register A =   1 = 001 = 00000001
 > Register A =   2 = 002 = 00000010
 > Register A =   4 = 004 = 00000100
 > Register A =   8 = 010 = 00001000
 > Register A =  16 = 020 = 00010000
 > Register A =  32 = 040 = 00100000
 > Register A =  64 = 100 = 01000000
 > Register A = 128 = 200 = 10000000
 > Register A =   1 = 001 = 00000001
+ Success: RLC.

 > Register A = 128 = 200 = 10000000
 > Register A =  64 = 100 = 01000000
 > Register A =  32 = 040 = 00100000
 > Register A =  16 = 020 = 00010000
 > Register A =   8 = 010 = 00001000
 > Register A =   4 = 004 = 00000100
 > Register A =   2 = 002 = 00000010
 > Register A =   1 = 001 = 00000001
 > Register A = 128 = 200 = 10000000
+ Success: RRC.
++ HALT, host_read_status_led_WAIT() = 0
                                    ;
                                    ; --------------------------------------