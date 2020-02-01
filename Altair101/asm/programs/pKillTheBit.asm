                        ; ------------------------------------------------
                        ; Kill the Bit program.
                        ; Reference online video:
                        ; https://www.youtube.com/watch?v=ZKeiQ8e18QY&list=PLLlz7OhtlfKYk8nkyF1u-cDwzE_S0vcJs&index=5
                        ; 
                        ; ------------------------------------------------
            org 0       ; An assembler directive on where to load this program.
    Start:
            LXI H,0     ; Move the lb hb data values into the register pair H(hb):L(lb). Initialize counter
            mvi D,080h  ; Move db to register D. Set initial display bit.  080h = 0200 = regD = 10 000 000
                        ;
                        ; ---------
                        ; Bit speed: Higher value = faster.
            LXI B,500h  ; Load a(lb:hb) into register B:C. 
                        ;  My default: 0005 = B:C  = 00 000 101 : 00 000 000
                        ;     Default: 0014 = B:C  = 00 010 000
                        ;     Slow:    0020 = B:C  = 00 010 000
                        ;     Nice:    0040 = B:C  = 00 100 000 Similar to the video speed. They use: 016h, I use
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
            end
