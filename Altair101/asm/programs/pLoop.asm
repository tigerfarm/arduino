                        ; Basic first program
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
