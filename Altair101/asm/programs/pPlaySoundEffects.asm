                            ; --------------------------------------
                            ; Play sound effects: on or off.
                            ; 
                            ; --------------------------------------
    Start:
            mvi a,1         ; Play sound effects.
            out 69
            hlt
                            ; --------------------------------------
            mvi a,0         ; Don't play sound effects.
            out 69
            hlt
                            ; --------------------------------------
            jmp Start
            end
