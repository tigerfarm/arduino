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
                End