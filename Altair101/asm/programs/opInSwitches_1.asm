                                    ; ------------------------------------------
                                    ; Get and process a data byte from sense switches.
                                    ; 
                                    ; ------------------------------------------
            lxi sp,255              ; Set stack pointer which is used with CALL and RET.
    GetByte:
                ; mvi A,13            ; Play sound bite once and wait for completion: Enter when ready.
                ; out 12
                                    ; Need to test.
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
                mvi a,'O'
                out PRINT_PORT
                mvi a,'K'
                out PRINT_PORT
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'

                hlt
                call printPrompt    ; Print prompt.
                                    ; Get the sense switch data byte.
                hlt                 ; Halt to get the data byte.
                                    ; Use RUN to restart the program.
                                    ; The next command, is the first command run after re-starting.
                in SENSE_SW         ; Get toggle sense switch byte value into register A.
                out 37              ; Print register A to the serial port.
                                    ; ----------------------
                cpi 0               ; If input byte is 0,
                jz NoInput          ;   Jump.
                                    ; ------------------------------------------
                                    ; Process the input byte value.
                                    ;
                mov b,a             ; Move the byte to register B.
                out 30              ; Print register B to the serial port.
                                    ;
                call printNL
                lxi h,ECHO
                call print
                mov a,b             ; Move the input data back to register A.
                out PRINT_PORT
                mvi a,'.'
                out PRINT_PORT
                call printNL
                                    ; ----------------------
                jmp GetByte
                                    ; ------------------------------------------
    NoInput:
                call printNL
                lxi h,NO_INPUT
                call print
                call printNL
                jmp GetByte
                                    ; ------------------------------------------
    printPrompt:
                call printNL        ; Print prompt.
                lxi h,prompt
                call print
                ret
                                    ; ------------------------------------------
                                    ; Print a DB string which starts at address, M.
    print:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
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
                                    ; Data declarations
                                    ;
                                    ; Print prompt.
    prompt      db      '+ Enter byte > '
                                    ;
    SENSE_SW    equ     255         ; Input port address: toggle sense switch byte, into register A.
    PRINT_PORT  equ     3           ; Output port: print to the serial port.
                                    ; -------------------
    ECHO        db      '+ Input data: '
    NO_INPUT    db      '+ No input, value = 0.'
                                    ; -------------------
                                    ; Special characters:
    TERMB       equ     0ffh        ; String terminator.
                                    ;
                                    ; ------------------------------------------
                end
                                    ; ------------------------------------------
                                    ; Successful run:

                                    ; ------------------------------------------
