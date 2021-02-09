                                    ; --------------------------------------
                                    ; Program for quick tests.
                                    ;
                                    ; --------------------------------------
                mvi a,'\n'
                out 3
                mvi a,'\r'
                out 3
                mvi a,'>'
                out 3
                mvi a,' '
                out 3
                                    ; --------------------------------------
    Start:
                mvi a,0
                ANI 01              ; AND 1 with register A.
                JNZ InputChar       ; 1 indicates NO input character in serial input 01, jump.
                                    ; 0 indicates an input character in serial input 01, Don't jump.
                mvi a,'N'
                out 3
                jmp halt
    InputChar:
                mvi a,'Y'
                out 3
                                    ; --------------------------------------
    halt:
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            hlt             ; The program will halt at each iteration, after the first.
            jmp Start       ; Jump back to the early halt command.
                            ; --------------------------------------
            end
                            ; --------------------------------------
