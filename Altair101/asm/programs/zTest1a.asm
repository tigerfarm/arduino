                            ; --------------------------------------
                            ; Test LDA and STA. Moving data from addresses to registers and back.
                            ;
                            ; --------------------------------------
                            ; Video demonstrating status lights:
                            ;    https://www.youtube.com/watch?v=3_73NwB6toY
                            ; Program listing: https://altairclone.com/downloads/status_lights.pdf
                            ; 
                            ; 
                            ; --------------------------------------
    Start:
            mvi a,6         ; regA = 6
    Store:
            sta 96          ; Store regA to memory address 96.
            hlt
            inr a
            jmp Store
                            ; --------------------------------------
            end
