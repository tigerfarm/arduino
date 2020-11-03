                                    ; --------------------------------------
                                    ; Test address values and variables and strings.
                                    ; Sample routine to print DB strings.
                                    ; 
                                    ; Sample successful test run:
                                    ; 
                                    ; Test
                                    ; 
                                    ; Test1
                                    ; Test2
                                    ; 
                                    ; Hello there.
                                    ; Yes, Hello there.
                                    ; 
                                    ; --------------------------------------
                jmp Start           ; Jump to Start test section.
                                    ;
                                    ; --------------------------------------
                                    ; Data declarations.
                                    ; Sample strings to print.
        TestNL  db      '\nTest\n'
        Test1   db      'Test1'
        Test2   db      'Test2'
        Hello   db      'Hello'
        There   db      'there.'
        YesHello db     'Yes, Hello there.'
                                    ;
                                    ; --------------------------------------
    Halt:       hlt                 ; The program will halt at each iteration, after the first.
                                    ; --------------------------------------
    Start:
                lxi h,TestNL
                call PrintStr
                mvi a,NL
                out 3
                                    ;
                lxi h,Test1
                call PrintStr
                mvi a,NL
                out 3
                                    ;
                                    ; ------------------------------------------
                lxi h,Test2
                call PrintStrln
                                    ;
                mvi a,NL
                out 3
                                    ; Init the address and call routine to print the string.
                lxi h,Hello         ; Copy string address, Hello, to H:L. Byte 1 is lxi. Byte 2 move to L. Byte 3 move to H.
                call PrintStr
                mvi a,SPACE
                out 3
                lxi h,there
                call PrintStrln
                                    ;
                lxi h,YesHello
                call PrintStrln
                                    ; ------------------------------------------
                jmp Halt            ; Stop to confirm the correct addresses and data.
                                    ;
                                    ; ------------------------------------------
                                    ; Routines to print a DB strings.
                                    ;
                                    ; -------------------
                                    ; Special characters.
        SPACE   equ     32          ; 32 is space, ' '.
        NL      equ     10          ; 10 is new line, '\n'.
        STRTERM equ     0ffh        ; String terminator.
                                    ;
        PrintStr:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM           ; Compare to see if it's the string terminate byte.
                jz PrintStrDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintStr
        PrintStrDone:
                ret
                                    ;
        PrintStrln:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM           ; Compare to see if it's the string terminate byte.
                jz PrintStrlnDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintStrln
        PrintStrlnDone:
                mvi a,NL            ; Finish by printing a new line character.
                out 3
                ret
                                    ; ------------------------------------------
                end
