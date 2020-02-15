                            ; --------------------------------------
                            ; Test ADI.
                            ; Add immediate number to register A.
                            ;
                            ; To change the addition values:
                            ; + Change address 1, which is initialized as 2.
                            ; ++ mvi a,2
                            ; + Change address 3, which is immediate value, 3.
                            ; ++ adi 3
                            ; + RESET and RUN, to get the new result.
                            ;
                            ; --------------------------------------
    Start:
            mvi a,2         ; Register A = 2.
            adi 3           ; Add immediate number to register A.
                            ;
            mvi h,0         ; Move register A to address 64.
            mvi l,64
            sta 64          ; Store register A's content to the address(hb:lb).
                            ;
            out 36          ; Print the register values for H:L and the content at that address.
                            ; --------------------------------------
            hlt
            jmp Start
            end
