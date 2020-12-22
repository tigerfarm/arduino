                        ; ------------------------------------------------
                        ; Show the Bit program.
                        ; 
                        ; Turn LED light on above the sense switches that are toggled on.
                        ; 
                        ; ------------------------------------------------
    Begin:
                        ; Display bit pattern on upper 8 address lights, the value of DE register pair.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D
            LDAX D
            LDAX D
            LDAX D
            LDAX D
            LDAX D
            LDAX D
            LDAX D
            LDAX D
            LDAX D
            LDAX D
                        ;
            IN 1        ; Check the sense switch serial inputs.
            MOV D,A     ; Register D, is used by LDAX to display the bits.
                        ;
            JMP Begin
                        ; ------------------------------------------------
            END
                        ; ------------------------------------------------
++ Address:16-bit bytes       databyte :hex:oct > description
++       0:00000000 00000000: 00011010 : 1A:032 > opcode: ldax d
++       1:00000000 00000001: 00011010 : 1A:032 > opcode: ldax d
++       2:00000000 00000010: 00011010 : 1A:032 > opcode: ldax d
++       3:00000000 00000011: 00011010 : 1A:032 > opcode: ldax d
++       4:00000000 00000100: 00011010 : 1A:032 > opcode: ldax d
++       5:00000000 00000101: 00011010 : 1A:032 > opcode: ldax d
++       6:00000000 00000110: 00011010 : 1A:032 > opcode: ldax d
++       7:00000000 00000111: 00011010 : 1A:032 > opcode: ldax d
++       8:00000000 00001000: 00011010 : 1A:032 > opcode: ldax d
++       9:00000000 00001001: 00011010 : 1A:032 > opcode: ldax d
++      10:00000000 00001010: 00011010 : 1A:032 > opcode: ldax d
++      11:00000000 00001011: 00011010 : 1A:032 > opcode: ldax d
++      12:00000000 00001100: 11011011 : DB:333 > opcode: in 1
++      13:00000000 00001101: 00000001 : 01:001 > immediate: 1 : 1
++      14:00000000 00001110: 01010111 : 57:127 > opcode: mov d,a
++      15:00000000 00001111: 11000011 : C3:303 > opcode: jmp Begin
++      16:00000000 00010000: 00000000 : 00:000 > lb: 0
++      17:00000000 00010001: 00000000 : 00:000 > hb: 0
+ End of list.
                        ; ------------------------------------------------
