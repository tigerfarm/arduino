                                    ; ------------------------------------------
                                    ; Get and process a serial received data byte.
                                    ; 
                                    ; ------------------------------------------
    prompt:
                mvi a,'\r'
                out SERIAL_PORT
                mvi a,'\n'
                out SERIAL_PORT
                mvi a,'>'
                out SERIAL_PORT
                mvi a,' '
                out SERIAL_PORT
    GetByte:
                in SERIAL_PORT      ; Get input byte value into register A.
                cpi 3               ; Ctrl+c will exit this loop.
                jz HaltLoop         ;   Jump.
                cpi 0               ; No character input, nothing to print out.
                jz GetByte          ;   Jump.
                                    ;
                cpi '9' 
                jz  Cpi9            ; Jump, if A = #, zero bit = 1.
                jnc GetByte         ; Jump, if A > #, carry bit = 0.
    Cpi9:
                cpi '0'
                jc  GetByte         ; Jump, if A < #, carry bit = 1.
                out SERIAL_PORT
                                    ;
                jmp GetByte
                                    ; ----------------------
    HaltLoop:
                mvi a,'\r'
                out SERIAL_PORT
                mvi a,'\n'
                out SERIAL_PORT
                hlt                 ; Halt to get the data byte.
                jmp prompt
                                    ; ------------------------------------------
    SERIAL_PORT EQU 2               ; USB Serial2 port#.
                                    ; ------------------------------------------
                end
                                    ; ------------------------------------------
