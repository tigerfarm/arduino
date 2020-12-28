                                    ; ------------------------------------------
                                    ; Get and process a serial received data byte.
                                    ; 
                                    ; ------------------------------------------
                                    ; Get the sense switch data byte.
    SERIAL_PORT EQU 3               ; USB serial port#.
                                    ; #3 for USB serial input of any character.
                                    ;   Exceptions: 's'(STOP) and 'R'(RESET) are captured by RUN mode processing.
                                    ; Can use #2, for USB sense switches: 8,9,a,b,c,d,e,f.
    GetByte:
                in SERIAL_PORT      ; Get input byte value into register A.
                cpi 0
                jz GetByte
                cpi 'x'             ; With port #3, input 'x' will exit this loop.
                jz HaltLoop         ;   Jump.
                cpi 3               ; Ctrl+c will exit this loop.
                jz HaltLoop         ;   Jump.
                out SERIAL_PORT
                jmp GetByte
                                    ; ----------------------
    HaltLoop:
                hlt                 ; Halt to get the data byte.
                jmp GetByte
                                    ; ------------------------------------------
                end
