                                    ; --------------------------------------
                                    ; Test address values and variables and strings.
                                    ; Sample routine to print DB strings.
                                    ;
                                    ; --------------------------------------
                jmp Start           ; Jump to Start test section.
                                    ;
                                    ; --------------------------------------
                                    ; Data declarations.
                                    ;
        Hello   db      'Hello'     ; Strings to print out.
        There   db      'there.'
        SPACE   equ     32          ; 32 is space, ' '.
        NL      equ     10          ; 10 is new line, '\n'.
        TERMB   equ     0ffh        ; String terminator.
                                    ; --------------------------------------
    Halt:       hlt                 ; The program will halt at each iteration, after the first.
                                    ; --------------------------------------
    Start:
                lxi h,Hello         ; Copy string address, Hello, to H:L. Byte 1 is lxi. Byte 2 move to L. Byte 3 move to H.
                out 34              ; Echo the register H.
                out 35              ; Echo the register L.
                lda Hello           ; Load the data from Hello address to register A.
                out 37              ; Echo the register A.
                mov a,m
                out 37              ; Echo the register A.
                mvi a,NL
                out 3               ; Out register A to the serial terminal port.
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                out 3 
                mvi a,NL
                out 3
                                    ; ------------------------------------------
                                    ; Print the next character.
                inr m               ; Increment M, H:L address. (HL)+1 -> (HL).
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                out 3 
                mvi a,NL
                out 3
                                    ; ------------------------------------------
                mvi a,NL
                out 3
                                    ; Init the address and call routine to print the string.
                lxi h,Hello         ; Copy string address, Hello, to H:L. Byte 1 is lxi. Byte 2 move to L. Byte 3 move to H.
                call PrintString
                mvi a,SPACE
                out 3
                lxi h,there
                call PrintString
                mvi a,NL
                out 3
                                    ; ------------------------------------------
                jmp Halt            ; Stop to confirm the correct addresses and data.
                                    ;
                                    ; ------------------------------------------
                                    ; Routine loop to print a DB string which starts a address, M.
        PrintString:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz Done
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintString
        Done:
                ret
                                    ; ------------------------------------------
                                    ; End