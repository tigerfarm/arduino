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
                                    ; Strings to print out.
        TestNL  db      'Test\n'
        Test1   db      'Test1'
        Test2   db      'Test2'
        Hello   db      'Hello'
        There   db      'there.'
                                    ; -------------------
                                    ; Special characters.
        SPACE   equ     32          ; 32 is space, ' '.
        NL      equ     10          ; 10 is new line, '\n'.
        TERMB   equ     0ffh        ; String terminator.
                                    ;
                                    ; --------------------------------------
    Halt:       hlt                 ; The program will halt at each iteration, after the first.
                                    ; --------------------------------------
    Start:
                lxi h,TestNL
                call sPrint
                mvi a,NL
                out 3
                                    ;
                lxi h,Test1
                call sPrint
                mvi a,NL
                out 3
                                    ;
                                    ; ------------------------------------------
                lxi h,Test2
                call sPrintln
                                    ;
                mvi a,NL
                out 3
                                    ; Init the address and call routine to print the string.
                lxi h,Hello         ; Copy string address, Hello, to H:L. Byte 1 is lxi. Byte 2 move to L. Byte 3 move to H.
                call sPrint
                mvi a,SPACE
                out 3
                lxi h,there
                call sPrintln
                                    ; ------------------------------------------
                jmp Halt            ; Stop to confirm the correct addresses and data.
                                    ;
                                    ; ------------------------------------------
                                    ; Routines to print a DB strings.
        sPrint:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz sPrintDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp sPrint
        sPrintDone:
                ret
                                    ;
        sPrintln:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz sPrintlnDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp sPrintln
        sPrintlnDone:
                mvi a,NL            ; Finish by printing a new line character.
                out 3
                ret
                                    ; ------------------------------------------
                                    ; End