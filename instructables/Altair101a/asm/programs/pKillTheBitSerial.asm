                        ; ------------------------------------------------
                        ; Kill the Bit program.
                        ; Reference online video:
                        ; https://www.youtube.com/watch?v=ZKeiQ8e18QY&list=PLLlz7OhtlfKYk8nkyF1u-cDwzE_S0vcJs&index=5
                        ; 
                        ; ------------------------------------------------
    Start:
            MVI D,080h  ; Set initial display bit.  080h = 10000000 binary high byte
                        ; ------------------------------------------------
                        ; To have the bit move faster, use less LDAX instructions, or slower baud rate.
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
            IN 1        ; Check the sense switch serial inputs that can kill the bit.
                        ;
            XRA D       ; Exclusive OR register with A, to either remove or add a bit.
            RRC         ; Rotate right register A (shift byte right 1 bit). Set carry bit.
            MOV D,A     ; Register D, is used by LDAX to display the moving bits.
                        ;
            JMP Begin
                        ; ------------------------------------------------
            END
                        ; ------------------------------------------------
++      Address:16-bit bytes  databyte :hex:oct > description
++       0:00000000 00000000: 00010110 : 16:026 > opcode: mvi d,080h
++       1:00000000 00000001: 10000000 : 80:200 > immediate: 080h : 128
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
++      12:00000000 00001100: 00011010 : 1A:032 > opcode: ldax d
++      13:00000000 00001101: 00011010 : 1A:032 > opcode: ldax d
++      14:00000000 00001110: 11011011 : DB:333 > opcode: in 1
++      15:00000000 00001111: 00000001 : 01:001 > immediate: 1 : 1
++      16:00000000 00010000: 10101010 : AA:252 > opcode: xra d
++      17:00000000 00010001: 00001111 : 0F:017 > opcode: rrc
++      18:00000000 00010010: 01010111 : 57:127 > opcode: mov d,a
++      19:00000000 00010011: 11000011 : C3:303 > opcode: jmp Begin
++      20:00000000 00010100: 00000010 : 02:002 > lb: 2
++      21:00000000 00010101: 00000000 : 00:000 > hb: 0
+ End of list.
                        ; ------------------------------------------------
