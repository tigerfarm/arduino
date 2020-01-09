                                    ; --------------------------------------
                                    ; Sample routine to print DB strings.
                                    ;
                                    ; --------------------------------------
                jmp Start           ; Jump to Start test section.
                                    ;
                                    ; --------------------------------------
                                    ; Data declarations.
                                    ;
        TERMB   equ     0ffh        ; Name for a value. Similar to: TERMB = 0ffh;
        Hello   db      'Hello'
        There   db      'there.'
                                    ; --------------------------------------
    Halt:       hlt                 ; The program will halt at each iteration, after the first.
                                    ; --------------------------------------
    Start:
                lxi h,Hello         ; Set address to start of the string to print.
                call PrintString    ; Print the string.
                lxi h,There
                call PrintString
                jmp Halt
                                    ; ------------------------------------------
                                    ; Routine loop to print a DB string which starts a address, M.
        PrintString:
                mov a,m             ; Move first character, at the address H:L, to register A.
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz Done
                out 3               ; Out A.
                inx h               ; Increment H:L register pair.
                jmp PrintString
                ; ...
        Done:
                ret
                                    ; ------------------------------------------
                                    ; End