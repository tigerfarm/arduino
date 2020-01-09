                                    ; --------------------------------------
                                    ; Test OUT to the serial monitor.
                                    ; --------------------------------------
                org 0
                jmp Start           ; Jump to Start to bypass declarations halt.
                                    ;
                                    ; --------------------------------------
                                    ; Data declarations
        TERMB   equ     0ffh        ; Name for a value. Similar to: TERMB = 0ffh;
        Hello   db      'Hello'     ; Translate into bytes with a terminate byte (TERMB).
                                    ; --------------------------------------
    Halt:
                hlt                 ; The program will halt at each iteration, after the first.
                                    ; --------------------------------------
    Start:
                call PrintLoop
                jmp Halt
                                    ; ------------------------------------------
                                    ; Routine that uses Hello to OUT the "Hello" string.
                ; (Needs development, the following is not complete)
        PrintLoop:
                lxi h,Start         ; Move label address to registers H:L.
                mov a,b
                mov a,m             ; Move the content ('H') at the address H:L to register A.
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz Done
                out 3               ; Out A.
                inx h               ; Increment H:L register pair.
                mov a,m             ; Move the content ('e') at the address H:L to register A.
                out 3
                ...
        Done:
                ret
                                    ; ------------------------------------------
                                    ; End