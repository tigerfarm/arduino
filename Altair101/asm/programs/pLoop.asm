                        ; Basic first program
                        ;
                        ; As there is no log output, 
                        ;   The way to test this program is to use SINGLE STEP.
                        ;   Flip RESET to insure to be at the start of the program.
                        ;   Flip SINGLE STEP and watch the program go through the byte code:
                        ; Byte address: 0 (opcode), 1 then 2 (address)
                        ; Jump to 6 (opcode), 7 then 8 (address)
                        ; Jump back to 0 where the cycle continues.
                        ;
    Start:
            jmp There
            nop
            nop
            nop
    There:
            jmp Start

; --------------------------------------
; Program Byte listing.

++ Address:byte      databyte :hex:oct > description
++       0:00000000: 11000011 : C3:303 > opcode: jmp There
++       1:00000001: 00000110 : 06:006 > lb: 6
++       2:00000010: 00000000 : 00:000 > hb: 0
++       3:00000011: 00000000 : 00:000 > opcode: nop
++       4:00000100: 00000000 : 00:000 > opcode: nop
++       5:00000101: 00000000 : 00:000 > opcode: nop
++       6:00000110: 11000011 : C3:303 > opcode: jmp Start
++       7:00000111: 00000000 : 00:000 > lb: 0
++       8:00001000: 00000000 : 00:000 > hb: 0
+ End of list.
