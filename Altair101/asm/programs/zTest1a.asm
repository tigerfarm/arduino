                            ; --------------------------------------
                            ; Test LDA and STA. Moving data from addresses to registers and back.
                            ;
                            ; --------------------------------------
                            ; Video demonstrating status lights:
                            ;    https://www.youtube.com/watch?v=3_73NwB6toY
                            ; Program listing: https://altairclone.com/downloads/status_lights.pdf
                            ; 
                            ; To match the video, move 235(octal 353, B11101011) into address 32.
                            ; 
                            ; --------------------------------------
    Start:
            mvi a,6         ; regA = 6
    Store:
            sta 96          ; Store regA to memory address 96.
            mvi a,0         ; Reset regA to zero.
            hlt             ; View the data byte at address 96 (=6). regA=0.
            lda 96          ; Read memory address 96 into regA.
            ;out ?           ; Out to serial port.
            hlt             ; View regA=6.
            inr a
            jmp Store
                            ; --------------------------------------
            end
