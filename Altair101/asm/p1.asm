                                    ; --------------------------------------
                                    ; Test asm parsing. Not meant as a valid program.
                                    ; --------------------------------------
                org 0
                jmp Start           ; Jump to Start to bypass declarations halt.
                                    ;
                                    ; --------------------------------------
                                    ; Data declarations
        abc     db     'okay, yes?'
                                    ; --------------------------------------
    Start:
                call PrintLoop
                mov a,b
                jmp Start
                                    ; ------------------------------------------
                def     db     'now'
        PrintLoop:
                mov c,d
                ret
                                    ; ------------------------------------------
                ds      2           ;stack space
        stack   equ     $
                org     36
        total   ds      2
        scoreL  ds      1           ;score for left paddle 
        scoreR  ds      1           ;score for right paddle
                                    ; ------------------------------------------
                End