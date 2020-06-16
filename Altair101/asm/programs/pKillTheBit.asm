                        ; ------------------------------------------------
                        ; Kill the Bit program.
                        ; Reference online video:
                        ; https://www.youtube.com/watch?v=ZKeiQ8e18QY&list=PLLlz7OhtlfKYk8nkyF1u-cDwzE_S0vcJs&index=5
                        ; 
                        ; ------------------------------------------------
            ORG 0       ; An assembler directive on where to load this program.
    Start:
            LXI H,0     ; Move the lb hb data values into the register pair H(hb):L(lb). Initialize counter
            MVI D,080h  ; Move db to register D. Set initial display bit.  080h = 0200 = regD = 10 000 000
                        ;
                        ; ---------
                        ; Bit speed: Higher value = faster.
                        ; Change address 7 to change the speed. Default is 5: 00 000 101.
            LXI B,500h  ; Load a(lb:hb) into register B:C.
                        ;                            Register B : C
                        ;                            Address  7 : 6
                        ;  My default: 0005 = B:C  = 00 000 101 : 00 000 000
                        ;     Faster:  0014 = B:C  = 00 001 000
                        ;     Nice:    0020 = B:C  = 00 010 000
                        ; Quite fast:  0040 = B:C  = 00 100 000 Similar to the video speed. They use: 016h.
                        ;     Fast:    0100 = B:C  = 01 000 000
                        ; Too fast:    0100 = B:C  = 01 001 000
                        ;
                        ; ------------------------------------------------
                        ; Display bit pattern on upper 8 address lights.
    Begin:
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
                        ; ------------------------------------------------
            DAD B       ; Add B:C to H:L. Set carry bit. Increments the display counter
            JNC Begin   ; If carry bit false, jump to address label.
                        ;
            IN 0ffh     ; Check for toggled input, at port 0377 (toggle sense switches), that can kill the bit.
            XRA D       ; Exclusive OR register with A, to either remove or add a bit.
                        ;
            RRC         ; Rotate right register A (shift byte right 1 bit). Set carry bit.
            MOV D,A     ; Register D, is used by LDAX to display the moving bit(s).
            JMP Begin   ; Jump to address label.
                        ; ------------------------------------------------
            END

+ Print Program Bytes and description.
++ Address:byte      databyte :hex:oct > description
++       0:00000000: 00100001 : 21:041 > opcode: lxi h,0
++       1:00000001: 00000000 : 00:000 > lb: 0
++       2:00000010: 00000000 : 00:000 > hb: 0
++       3:00000011: 00010110 : 16:026 > opcode: mvi d,080h
++       4:00000100: 10000000 : 80:200 > immediate: 080h : 128
++       5:00000101: 00000001 : 01:001 > opcode: lxi b,500h
++       6:00000110: 00000000 : 00:000 > lb: 0
++       7:00000111: 00000101 : 05:005 > hb: 5
++       8:00001000: 00011010 : 1A:032 > opcode: ldax d
++       9:00001001: 00011010 : 1A:032 > opcode: ldax d
++      10:00001010: 00011010 : 1A:032 > opcode: ldax d
++      11:00001011: 00011010 : 1A:032 > opcode: ldax d
++      12:00001100: 00001001 : 09:011 > opcode: dad b
++      13:00001101: 11010010 : D2:322 > opcode: jnc Begin
++      14:00001110: 00001000 : 08:010 > lb: 8
++      15:00001111: 00000000 : 00:000 > hb: 0
++      16:00010000: 11011011 : DB:333 > opcode: in 0ffh
++      17:00010001: 11111111 : FF:377 > immediate: 0ffh : 255
++      18:00010010: 10101010 : AA:252 > opcode: xra d
++      19:00010011: 00001111 : 0F:017 > opcode: rrc
++      20:00010100: 01010111 : 57:127 > opcode: mov d,a
++      21:00010101: 11000011 : C3:303 > opcode: jmp Begin
++      22:00010110: 00001000 : 08:010 > lb: 8
++      23:00010111: 00000000 : 00:000 > hb: 0
+ End of list.
