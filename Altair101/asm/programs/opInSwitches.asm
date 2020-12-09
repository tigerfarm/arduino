                                    ; --------------------------------------
                                    ; Test LDAX.
                                    ;
                                    ; Load register A with data value from address B:C or D:E.
                                    ;
                                    ; Before running, DEPOSIT byte: 01010101, into address: 1024 (S10/key a).
                                    ;
                                    ; --------------------------------------
                lxi sp,255          ; Set stack pointer which is used with CALL and RET.
    Start:
                mvi a,'+'
                out PRINT_PORT
                mvi a,'s'
                out PRINT_PORT
                call printPrompt    ; Print prompt.
                                    ; --------------------------------------
                call PrintTestCounter
                mvi a,3
                out 37              ; Print > Register A
                                    ; --------------------------------------
                call printNL
                hlt                     ; The program will halt before another iteration.
                jmp Start
                                    ;
                                    ; --------------------------------------
                                    ; Routines
                                    ;
                                    ; ------------------------------------------
    printPrompt:
                mvi a,'+'
                out PRINT_PORT
                mvi a,'p'
                out PRINT_PORT
                call printNL
                lxi h,prompt
                call print
                ret
                                    ; ------------------------------------------
                                    ; Print a DB string which starts at address, M.
    print:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A.
                mov b,a
                out 37              ; Print > Register A
                mov a,b
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz printed
                out PRINT_PORT      ; Out register A to the print port.
                inr m               ; Increment H:L register pair.
                jmp print
    printed:
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
                                    ; ------------------------------------------
    PrintTestCounter:
                call printNL
                mvi a,'+'
                out PRINT_PORT
                mvi a,'c'
                out PRINT_PORT
                ret
                                    ; 
                                    ; --------------------------------------
                                    ; Variables
                                    ;
    PRINT_PORT  equ     3           ; Output port: print to the serial port.
    prompt      db      '+ Enter byte > '
                                    ; -------------------
                                    ; Special characters:
    TERMB       equ     0ffh        ; String terminator.
                                    ;
                                    ; --------------------------------------
            end
                                    ; --------------------------------------
                                    ; Successful run:
                                    ;
                                    ; --------------------------------------
