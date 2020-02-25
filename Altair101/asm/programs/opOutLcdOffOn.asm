                                        ; --------------------------------------
                                        ; Turn LCD back light off.
    Start:
            mvi a,0
            out 1
            hlt
                                        ; Turn LCD back light on.
            mvi a,1
            out 1
            hlt
                                        ;
            jmp Start
                                        ; --------------------------------------
            end
