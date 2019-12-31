            org 0       ; An assembler directive on where to load this program.
    Start:
            LXI H,0     ; Move the lb hb data values into the register pair H(hb):L(lb). Initialize counter
            mvi D,080h  ; Move db to register D. Set initial display bit.  080h = 0200 = regD = 10 000 000
                        ; Fix: Change 080h to 0x80 with is decimal 128.
            LXI B,5     ; Load a(lb:hb) into register B:C. Higher value = faster. Default: 0016 = B:C  = 00 010 000
                        ; Fix: B00000001, 0, 5,     //   6: lxi b,5
                        ;  My default: 0005 = B:C  = 00 000 101
                        ;     Default: 0014 = B:C  = 00 010 000
                        ;     Slow:    0020 = B:C  = 00 010 000
                        ;     Nice:    0040 = B:C  = 00 100 000
                        ;     Fast:    0100 = B:C  = 01 000 000
                        ; Too fast:    0100 = B:C  = 01 001 000
                        ; ------------------------------------------------
                        ; Display bit pattern on upper 8 address lights.
    Begin:
            ; hlt         ; For testing.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
                        ; ------------------------------------------------
            DAD B       ; Add B:C to H:L. Set carry bit. Increments the display counter
            JNC Begin   ; If carry bit false, jump to lb hb, LDAX instruction start.

            IN 0ffh     ; Check for toggled input, at port 0377 (toggle sense switches), that can kill the bit.
                        ; Fix: Change 0ffh to 0xff which is 255.
            XRA D       ; Exclusive OR register with A
            RRC         ; Rotate A right (shift byte right 1 bit). Set carry bit. Rotate display right one bit
            MOV D,A     ; Move register A to register D. Move data to display reg

            JMP Begin   ; Jump to BEG, LDAX instruction start.
                        ; ------------------------------------------------
                        ; End of program.
