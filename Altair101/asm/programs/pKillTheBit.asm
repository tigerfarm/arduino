                        ; ------------------------------------------------
                        ; Kill the Bit program.
                        ; Reference online video:
                        ; https://www.youtube.com/watch?v=ZKeiQ8e18QY&list=PLLlz7OhtlfKYk8nkyF1u-cDwzE_S0vcJs&index=5
                        ; Source code: https://altairclone.com/downloads/killbits.pdf
                        ; 
                        ; ------------------------------------------------
            ORG 0       ; An assembler directive on where to load this program.
            LXI H,0     ; Move the lb hb data values into the register pair H(hb):L(lb). Initialize counter
            MVI D,080h  ; Move db to register D. Set initial display bit.  080h = 0200 = regD = 10 000 000
                        ;
                        ; ---------
                        ; Bit speed: lower value, slower bit movement. Requires a faster emulator to move at the same speed.
            ;LXI B,0Eh  ; 0Eh was the original value because the original goes faster than my Mega version.
            ;LXI B,500h ; 500h was the speed used from my original emulator: Processor.ino, which ran faster than the new emulator (Altair101b)?
            LXI B,800h   ; Load a(lb:hb) into register B:C.
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
            ;IN 0ffh     ; Original: port octal 377.
            IN 004h     ; Check for toggled input, at port 4 (toggle sense switches), that can kill the bit.
            XRA D       ; Exclusive OR register with A, to either remove or add a bit.
                        ;
            RRC         ; Rotate right register A (shift byte right 1 bit). Set carry bit.
            MOV D,A     ; Register D, is used by LDAX to display the moving bit(s).
            JMP Begin   ; Jump to address label.
                        ; ------------------------------------------------
            END

+ Print Program Bytes and description.
++ Address:16-bit bytes       databyte :hex:oct > description
++       0:00000000 00000000: 00100001 : 21:041 > opcode: lxi h,0
++       1:00000000 00000001: 00000000 : 00:000 > lb: 0
++       2:00000000 00000010: 00000000 : 00:000 > hb: 0
++       3:00000000 00000011: 00010110 : 16:026 > opcode: mvi d,080h
++       4:00000000 00000100: 10000000 : 80:200 > immediate:  080h : 128
++       5:00000000 00000101: 00000001 : 01:001 > opcode: lxi b,800h
++       6:00000000 00000110: 00000000 : 00:000 > lb: 0
++       7:00000000 00000111: 00001000 : 08:010 > hb: 8
++       8:00000000 00001000: 00011010 : 1A:032 > opcode: ldax d
++       9:00000000 00001001: 00011010 : 1A:032 > opcode: ldax d
++      10:00000000 00001010: 00011010 : 1A:032 > opcode: ldax d
++      11:00000000 00001011: 00011010 : 1A:032 > opcode: ldax d
++      12:00000000 00001100: 00001001 : 09:011 > opcode: dad b
++      13:00000000 00001101: 11010010 : D2:322 > opcode: jnc Begin
++      14:00000000 00001110: 00001000 : 08:010 > lb: 8
++      15:00000000 00001111: 00000000 : 00:000 > hb: 0
++      16:00000000 00010000: 11011011 : DB:333 > opcode: in 004h
++      17:00000000 00010001: 00000100 : 04:004 > immediate:  004h : 4
++      18:00000000 00010010: 10101010 : AA:252 > opcode: xra d
++      19:00000000 00010011: 00001111 : 0F:017 > opcode: rrc
++      20:00000000 00010100: 01010111 : 57:127 > opcode: mov d,a
++      21:00000000 00010101: 11000011 : C3:303 > opcode: jmp Begin
++      22:00000000 00010110: 00001000 : 08:010 > lb: 8
++      23:00000000 00010111: 00000000 : 00:000 > hb: 0
+ End of list.
