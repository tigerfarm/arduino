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
        ;SPACE   equ     ' '
        ;There   db      'there.'
        ;TERMB   equ     0ffh        ; String terminator.
                                    ; --------------------------------------
    Halt:       hlt                 ; The program will halt at each iteration, after the first.
                                    ; --------------------------------------
    Start:
                lxi h,Hello         ; Copy string address, Hello, to H:L. Byte 1 is lxi. Byte 2 move to L. Byte 3 move to H.
                out 34              ; Echo the register H.
                out 35              ; Echo the register L.
                lda Hello           ; Load the data from Hello address to register A.
                out 37              ; Echo the register A.
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                out 37              ; Echo the register A.
                out 3               ; Out register A to the serial terminal port.
                jmp Halt            ; Stop to confirm the correct addresses and data.
                                    ; ------------------------------------------
                                    ; End