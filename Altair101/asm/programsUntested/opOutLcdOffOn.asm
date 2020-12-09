                                        ; --------------------------------------
    Start:
                                        ; Turn LCD back light off.
            mvi a,0
            out 1
            hlt
                                        ; Turn LCD back light on.
            mvi a,1
            out 1
            hlt
                                        ; Clear screen.
            mvi a,2
            out 1
            hlt
                                        ; Turn LCD back light off.
            mvi a,0
            out 1
            hlt
                                        ; Turn LCD back light on.
            mvi a,1
            out 1
            hlt
                                        ; Initialize with splash message.
            mvi a,3
            out 1
            hlt
                                        ;
            jmp Start
                                        ; --------------------------------------
            end
