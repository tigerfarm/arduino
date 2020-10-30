                            ; --------------------------------------
                            ; Test MOV using serial monitor output.
                            ; --------------------------------------
            jmp Start       ; Jump to bypass the halt.
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
                            ; Test MOV with register A.
            mvi a,0         ; Set initial register A value.
            inr a
            mov b,a         ; Move register A content to each other registers.
            inr a           ; Increment register A each time.
            mov c,a
            inr a
            mov d,a
            inr a
            mov e,a
            inr a
            mov h,a
            inr a
            mov l,a
            out 38
                            ; + regA:   6 = 006 = 00000110
                            ; + regB:   1 = 001 = 00000001  regC:   2 = 002 = 00000010
                            ; + regD:   3 = 003 = 00000011  regE:   4 = 004 = 00000100
                            ; + regH:   5 = 005 = 00000101  regL:   6 = 006 = 00000110
                            ; 
                            ; --------------------------------------
                            ; Test MOV with register B.
            inr a           ; Keep incrementing up.
            mov b,a
            mov a,b
            inr b
            mov c,b
            inr b
            mov d,b
            inr b
            mov e,b
            inr b
            mov h,b
            inr b
            mov l,b     ; regB is the same regL
            out 38
                        ; + regA:   7 = 007 = 00000111
                        ; + regB:  12 = 014 = 00001100  regC:   8 = 010 = 00001000
                        ; + regD:   9 = 011 = 00001001  regE:  10 = 012 = 00001010
                        ; + regH:  11 = 013 = 00001011  regL:  12 = 014 = 00001100
                        ; 
                        ; --------------------------------------
                        ; Test MOV with register C.
            inr b
            mov c,b     ; Keep incrementing up.
            mov a,c
            inr c
            mov b,c
            inr c
            mov d,c
            inr c
            mov e,c
            inr c
            mov h,c
            inr c
            mov l,c
            out 38
                        ; + regA:  13 = 015 = 00001101
                        ; + regB:  14 = 016 = 00001110  regC:  18 = 022 = 00010010
                        ; + regD:  15 = 017 = 00001111  regE:  16 = 020 = 00010000
                        ; + regH:  17 = 021 = 00010001  regL:  18 = 022 = 00010010
                        ; 
                        ; --------------------------------------
                        ; Test MOV with register D.
            inr c
            mov d,c     ; Keep incrementing up.
            mov a,d
            inr d
            mov b,d
            inr d
            mov c,d
            inr d
            mov e,d
            inr d
            mov h,d
            inr d
            mov l,d
            out 38
                        ; + regA:  19 = 023 = 00010011
                        ; + regB:  20 = 024 = 00010100  regC:  21 = 025 = 00010101
                        ; + regD:  24 = 030 = 00011000  regE:  22 = 026 = 00010110
                        ; + regH:  23 = 027 = 00010111  regL:  24 = 030 = 00011000
                        ; 
                        ; --------------------------------------
                        ; Test MOV with register E.
            inr d
            mov e,d     ; Keep incrementing up.
            mov a,e
            inr e
            mov b,e
            inr e
            mov c,e
            inr e
            mov d,e
            inr e
            mov h,e
            inr e
            mov l,e
            out 38
                        ; + regA:  25 = 031 = 00011001
                        ; + regB:  26 = 032 = 00011010  regC:  27 = 033 = 00011011
                        ; + regD:  28 = 034 = 00011100  regE:  30 = 036 = 00011110
                        ; + regH:  29 = 035 = 00011101  regL:  30 = 036 = 00011110
                        ; 
                        ; --------------------------------------
                        ; Test MOV with register H.
            inr e
            mov h,e     ; Keep incrementing up.
            mov a,h
            inr h
            mov b,h
            inr h
            mov c,h
            inr h
            mov d,h
            inr h
            mov e,h
            inr h
            mov l,h
            out 38
                        ; + regA:  31 = 037 = 00011111
                        ; + regB:  32 = 040 = 00100000  regC:  33 = 041 = 00100001
                        ; + regD:  34 = 042 = 00100010  regE:  35 = 043 = 00100011
                        ; + regH:  36 = 044 = 00100100  regL:  36 = 044 = 00100100
                        ; 
                        ; --------------------------------------
                        ; Test MOV with register L.
            inr h
            mov l,h     ; Keep incrementing up.
            mov a,l
            inr l
            mov b,l
            inr l
            mov c,l
            inr l
            mov d,l
            inr l
            mov e,l
            inr l
            mov h,l
            out 38
                        ; + regA:  37 = 045 = 00100101
                        ; + regB:  38 = 046 = 00100110  regC:  39 = 047 = 00100111
                        ; + regD:  40 = 050 = 00101000  regE:  41 = 051 = 00101001
                        ; + regH:  42 = 052 = 00101010  regL:  42 = 052 = 00101010
                        ; 
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
            end
