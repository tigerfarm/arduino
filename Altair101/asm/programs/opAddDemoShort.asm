                            ; --------------------------------------
                            ; Add content of address 1 and 3, and store the answer in address 64.
                            ; --------------------------------------
    Start:  mvi a,2         ; Register A = 2.
            adi 3           ; Add immediate number to register A.
            sta 64          ; Move result (register A) to address 64.
                            ; --------------------------------------
            hlt
            jmp Start
            end

+ Print Program Bytes and description.
++ Address:byte      databyte :hex:oct > description
++       0:00000000: 00111110 : 3E:076 > opcode: mvi a,2
++       1:00000001: 00000010 : 02:002 > immediate: 2 : 2
++       2:00000010: 11000110 : C6:306 > opcode: adi 3
++       3:00000011: 00000011 : 03:003 > immediate: 3 : 3
++       4:00000100: 00110010 : 32:062 > opcode: sta 64
++       5:00000101: 01000000 : 40:100 > lb: 64
++       6:00000110: 00000000 : 00:000 > hb: 0
++       7:00000111: 01110110 : 76:166 > opcode: hlt
++       8:00001000: 11000011 : C3:303 > opcode: jmp Start
++       9:00001001: 00000000 : 00:000 > lb: 0
++      10:00001010: 00000000 : 00:000 > hb: 0
+ End of list.
