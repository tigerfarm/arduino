                                    ; --------------------------------------
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
        TERMB   equ     0ffh        ; String terminator.
        aPrint  ds      2           ; Address of the byte to be sent to the serial terminal port.
                                    ; --------------------------------------
    Halt:       hlt                 ; The program will halt at each iteration, after the first.
                                    ; --------------------------------------
    Start:
                shld Hello          ; Copy string address to H:L.
                out 34              ; Echo the register H.
                out 35              ; Echo the register L.
                jmp Halt            ; Stop  to confirm the correct address.
                                    ; ------------------------------------------
                call PrintString    ; Print the string.
                lxi h,There
                call PrintString
                jmp Halt
                                    ; ------------------------------------------
                                    ; Routine loop to print a DB string which starts a address, M.
        PrintString:
                lda aPrint          ; Load register A with data from the address, a(hb:lb).
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz Done
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintString
                ; ...
        Done:
                ret
                                    ; ------------------------------------------
                                    ; End