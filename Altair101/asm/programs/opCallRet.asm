                            ; --------------------------------------
                            ; Test CALL and RET.
                            ; 
                            ; --------------------------------------
            lxi sp,32       ; Set stack pointer, which is used with CALL and RET.
                            ; Tested with 512, a 16 bit address.
    Start:
            ;mvi a,'\n'
            ;out 3
            mvi a,'>'
            out 3
            call Test
            mvi a,'<'
            out 3
            hlt
            jmp Start
    Test:
            mvi a,'+'
            out 3
            ret
                            ; --------------------------------------
            end