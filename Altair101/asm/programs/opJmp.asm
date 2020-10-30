                            ; --------------------------------------
                            ; Test JMP using types of addresses.
                            ;
                            ; If the output is, 123S, then the jumps were a success.
                            ; If the jumps failed, then the output would include an "e", for error.
                            ;
                            ; --------------------------------------
                            ; Test with an immediate hex value.
            mvi a,'1'       ; '1' first jump.
            out 3
            jmp 0Ch         ; 0Ch = 12
            mvi a,'e'       ; 'e' for error.
            out 3
            hlt
                            ; --------------------------------------
                            ; Test with an immediate decimal value.
            mvi a,'2'       ; '2' second jump.
            out 3
            jmp 24
            mvi a,'e'       ; 'e' for error.
            out 3
            hlt
                            ; --------------------------------------
                            ; Test with a label.
            mvi a,'3'       ; '3' second jump.
            out 3
            jmp Success
            mvi a,'e'       ; 'e' for error.
            out 3
            hlt
                            ; --------------------------------------
    Success:
            mvi a,'S'       ; 'S' for Success. 
            out 3
            hlt             ; Program output before the HLT, is: "123S".
                            ; --------------------------------------
            end