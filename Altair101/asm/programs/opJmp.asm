                            ; --------------------------------------
                            ; Test JMP using types of addresses.
                            ;
                            ; --------------------------------------
                            ; Test with an immediate hex value.
            jmp 05h
            nop
            hlt
                            ; --------------------------------------
                            ; Test with an immediate decimal value.
            jmp 10
            nop
            hlt
                            ; --------------------------------------
                            ; Test with a label.
            jmp Success
            nop
            hlt
                            ; --------------------------------------
    Success:
            hlt             ; Success, if no NOP commands were executed.
                            ; --------------------------------------
            end