                            ; --------------------------------------
                            ; Program for quick tests.
                            ;
                            ; --------------------------------------
    Start:
            mvi a,1
            xra d
                            ; --------------------------------------

                            ; --------------------------------------
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            hlt             ; The program will halt at each iteration, after the first.
            jmp Start       ; Jump back to the early halt command.
                            ; --------------------------------------
            end
                            ; --------------------------------------

++      14:00000000 00001110: 11011011 : DB:333 > opcode: in 1
++      15:00000000 00001111: 00000001 : 01:001 > immediate: 1 : 1
++      16:00000000 00010000: 10101010 : AA:252 > opcode: xra d
++      17:00000000 00010001: 00001111 : 0F:017 > opcode: rrc
