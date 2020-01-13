                                    ; --------------------------------------
                                    ; Test OUT to the serial monitor.
                                    ; --------------------------------------
                org 0
                jmp Start           ; Jump to Start to bypass declarations halt.
                                    ;
                                    ; --------------------------------------
                                    ; Data declarations
        abc     equ     3
        TERMB   equ     0ffh        ; Name for a value. Similar to: TERMB = 0ffh;
        Hello   db      'Hello'     ; Translate into bytes with a terminate byte (TERMB).
                                    ; --------------------------------------
                jmp 42              ; Jump to Start to bypass declarations halt.

    Halt1:      hlt                 ; This needs to be parsed.
    Halt:                           ; The program will halt at each iteration, after the first.
                hlt                 ; The program will halt at each iteration, after the first.
                                    ; --------------------------------------
                                    ; Test lines from Pong.asm.
                lxi     h,scoreL    ;increment left misses
                                    ; --------------------------------------
    Start:
                call PrintLoop
                jmp Halt
                jmp CPI3
                                    ; ------------------------------------------
                                    ; Routine that uses Hello to OUT the "Hello" string.
        PrintLoop:
                lxi h,Hello         ; Move label address to registers H:L.
                mov a,b
                mov a,m             ; Move the content ('H') at the address H:L to register A.
                cpi 42              ; Test parsing.
        CPI3:   cpi 43              ; Test label line parsing.
                cpi abc             ; Compare to see if it's the string terminate byte.
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz Done
                out 3               ; Out A.
                inx h               ; Increment H:L register pair.
                mov a,m             ; Move the content ('e') at the address H:L to register A.
                out 3
                ; ...
        Done:
                ret
                                    ; ------------------------------------------
                org 80h
                                    ;
                ds 2                ; Stack space
        stack   equ $               ; Confirm the meaning of "$". It seems to be an address, not an immediate value.
                                    ;
        scoreL  ds 1                ;score for left paddle 
        scoreR  ds 1                ;score for right paddle
                                    ; ------------------------------------------
        Errors:
                ; jz Nowhere
                ; cpi Nothere
                                    ; ------------------------------------------
                                    ; End