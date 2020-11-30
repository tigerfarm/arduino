                                    ; ------------------------------------------
                                    ; Get and process a serial received data byte.
                                    ; 
                                    ; ------------------------------------------
                                    ; Get the sense switch data byte.
    SERIAL_PORT EQU 2               ; USB serial port#
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
