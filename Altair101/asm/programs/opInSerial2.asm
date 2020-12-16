                                    ; ------------------------------------------
                                    ; Get and process a serial received data byte.
                                    ; 
                                    ; ------------------------------------------
                                    ; Get the sense switch data byte.
    SERIAL_PORT EQU 2               ; USB Serial2 port#.
                                    ;
                mvi a,'\r'
                out 3
                out 2
                mvi a,'\n'
                out 3
                out 2
                mvi a,'G'
                out 3
                out 2
                mvi a,'o'
                out 3
                out 2
                mvi a,' '
                out 3
                out 2
    GetByte:
                in SERIAL_PORT      ; Get input byte value into register A.
                ; cpi 'x'             ; With port #3, input 'x' will exit this loop.
                ; jz HaltLoop         ;   Jump.
                cpi 3               ; Ctrl+c will exit this loop.
                jz HaltLoop         ;   Jump.
                cpi 0               ; No character input, nothing to print out.
                jz GetByte          ;   Jump.
                out SERIAL_PORT
                cpi 13              ; If carriage return, send line feed.
                jnz GetByte         ;   Jump if not carriage return.
                mvi a,'\n'
                out SERIAL_PORT
                jmp GetByte
                                    ; ----------------------
    HaltLoop:
                hlt                 ; Halt to get the data byte.
                jmp GetByte
                                    ; ------------------------------------------
                end
