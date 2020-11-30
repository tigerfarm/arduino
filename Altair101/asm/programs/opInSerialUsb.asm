                                    ; ------------------------------------------
                                    ; Get and process a serial received data byte.
                                    ; 
                                    ; ------------------------------------------
                                    ; Get the sense switch data byte.
    SERIAL_PORT EQU 3               ; USB serial port#.
                                    ; #3 for USB serial input, any character.
                                    ;   Exceptions: 's'(STOP) and 'R'(RESET) are captured for RUN mode processing.
                                    ; Can use #2, for USB sense switches: 8,9,a,b,c,d,e,f.
                                    ;   Note, 'x' is not captured. Use 's' to STOP the process.
    GetByte:
                in SERIAL_PORT      ; Get input byte value into register A.
                cpi 'x'             ; Send 'x' to exit this loop.
                jz HaltLoop         ;   Jump.
                jmp GetByte
                                    ; ----------------------
    HaltLoop:
                hlt                 ; Halt to get the data byte.
                jmp GetByte
                                    ; ------------------------------------------
                end
