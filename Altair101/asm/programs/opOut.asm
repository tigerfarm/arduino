                            ; --------------------------------------
                            ; Test the various OUT options.
                            ; --------------------------------------
            jmp Start       ; Jump to bypass the halt.
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
            mvi a,1         ; Move an immediate number to each register.
            mvi b,2
            mvi c,3
            mvi d,4
            mvi e,5
            mvi h,6
            mvi l,7
            out 38          ; Print all the register values.
                            ; --------------------------------------
                            ; Print each register value.
            out 37          ; Register A.
            out 30          ; Register B.
            out 31          ; Register C.
            out 32          ; Register D.
            out 33          ; Register E.
            out 34          ; Register H.
            out 35          ; Register L.
                            ;
                            ; --------------------------------------
                            ; Print H:L register values.
                            ; And the data at the H:L (hb:lob) address.
            mvi h,0         ; High byte (hb)
            mvi l,0         ; Low byte (lb)
            out 36          ; Data is the value of the JMP opcode = 11000011.
                            ;
            mvi h,0         ; High byte (hb)
            mvi l,1         ; Low byte (lb)
            out 36          ; Data is the value of the Start address = 5.
                            ;
                            ; --------------------------------------
                            ; Print all the register values, and other system data.
                            ; If #==A, ZeroBit=1 & CarryBit=0.
                            ; If #!=A, ZeroBit=0.
                            ; If #>A, CarryBit=1.
                            ; If #<A, CarryBit=0.
            mvi a,42        ; Move # to register A.
            cpi 42          ; 42 = A. Zero bit flag is true(1). Carry bit is false(0).
            out 39          ; Print all the register values, and other system data.
            cpi 43          ; 43 > A. Zero bit flag is true(1). Carry bit is false(0).
            out 39          ; Print the registers and system data.
            cpi 41          ; 41 < A. Zero bit flag is true(1). Carry bit is false(0).
            out 39          ; Print the registers and system data.
                            ;
                            ; --------------------------------------
                            ; Print "Hello" to the Arduino IDE serial monitor.
            mvi a,'\n'
            out 3
            mvi a,'H'       ; Move the byte value of "h" to register A.
            out 3           ; Output register A content to the serial port (serial monitor).
            mvi a,'e'
            out 3
            mvi a,'l'
            out 3
            mvi a,'l'
            out 3
            mvi a,'o'
            out 3
            mvi a,'\n'
            out 3
                        ; --------------------------------------
                        ; - Error, OUT not implemented on this port address: <pa>.
            out 13
                        ; --------------------------------------
            jmp Halt    ; Jump back to the early halt command.
                        ; End.