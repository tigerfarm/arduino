                                    ; --------------------------------------
                                    ; Test asm parsing. Not meant as a valid program.
                                    ; --------------------------------------
                org 0
                                    ; --------------------------------------
                                    ; Address sample testing
                jmp Start
        nl1     db      'nl\n'
        nl2     db      '\nnl\n'
        NL      equ     '\n'
                mvi a,'\n'
                mvi a,NL
                                    ;
    Start:
                call PrintOut
                jmp Halt
                                    ;
                                    ; --------------------------------------
                                    ; Immediate sample testing
                mvi a,080h
                mvi a,80h
                mvi a,'a'
                mvi a,'\n'
                mvi a,'a'
                mvi a,Final
                mvi a,addr1         ; The addr1 value is over 255, which makes it too high for a byte. No error checking for this.
                ; mvi a,Fianl
                out 3
                                    ; --------------------------------------
                                    ; Other tests
        abc     db     'okay, yes?'
    Halt:
                hlt
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
        ; Finalh  equ     43
        addr1   equ     422
                jmp Final
                                    ; ------------------------------------------
                End