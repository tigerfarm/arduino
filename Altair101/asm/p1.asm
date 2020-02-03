                                    ; --------------------------------------
                                    ; Test asm parsing. Not meant as a valid program.
                                    ; --------------------------------------
                org 0
                jmp Start           ; Jump to Start to bypass declarations halt.
                                    ;
                                    ; --------------------------------------
                                    ; Data declarations
        abc     db     'okay, yes?'
                mvi a,080h
                mvi a,80h
                mvi a,'a'
                mvi a,'\n'
                mvi a,'a'
                mvi a,Final
                ; mvi a,Fianl
                out 3
                                    ; --------------------------------------
    Halt:
                out 30
                hlt
    Start:
                call PrintOut
                mov a,b
                jmp Halt
                                    ; ------------------------------------------
        def     db     'now'
        PrintOut:
                out 37
                mov c,d
                ret
                                    ; ------------------------------------------
                ds      2           ;stack space
        stack   equ     $
                org     42
        total   ds      2
        scoreL  ds      1           ;score for left paddle 
        scoreR  ds      1           ;score for right paddle
        Final   equ     42
                jmp Final
                                    ; ------------------------------------------
                End