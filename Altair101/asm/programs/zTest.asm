                            ; --------------------------------------
                            ; Program for quick tests.
                            ;
                            ; --------------------------------------
    Start:
            mvi l,42
                            ; --------------------------------------
                            ; Test MOV with register M, an address(hb:lb).
                            ;
            inr l
            mov a,l         ; Move # to register A.
            sta 128         ; Store register A's data value to the address H:L.
            mvi a,0         ; Reset A.
            mvi l,128       ; Set address H:L to 128 which is the address to m.
            mvi h,0
                            ;
            mov a,m
            inr m           ; Increment the byte(M) at address H:L.
            mov b,m
            inr m           ; Increment the byte(M) at address H:L.
            mov c,m
            inr m           ; Increment the byte(M) at address H:L.
            mov d,m
            inr m           ; Increment the byte(M) at address H:L.
            mov e,m
            inr m           ; Increment the byte(M) at address H:L.
            mov h,m         ; This changes the address value
                            ; --------------------------------------
            sta 130         ; Store register A.
            mov a,h
            sta 131         ; Store register H.
            mvi h,0         ; Reset regH, which is part of address H:L to 128 which is the address to m.
            ;
            inr m           ; Increment the byte(M) at address H:L.
            mov l,m         ; This changes the address value
            ;
            lda 131         ; Restore register H.
            mov h,a
            lda 130         ; Restore register A.
            ;
            out 38          ; Print the registers.
                            ; --------------------------------------
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            hlt             ; The program will halt at each iteration, after the first.
            jmp Start       ; Jump back to the early halt command.
                            ; --------------------------------------
            end

byte regM = 0;   110=M  m  Memory reference for a memory byte at address in H:L

0x7e  1           MOV A,M    A <- (HL)
0x46  1           MOV B,M    B <- (HL)
0x4e  1           MOV C,M    C <- (HL)  
0x56  1           MOV D,M    D <- (HL)  
0x5e  1           MOV E,M    E <- (HL)  
0x66  1           MOV H,M    H <- (HL)  
0x6e  1           MOV L,M    L <- (HL)  

movam:01111110:|(HL)->A
movbm:01000110:|(HL)->B
movcm:01001110:|(HL)->C
movdm:01010110:|(HL)->D
movem:01011110:|(HL)->E
movhm:01100110:|(HL)->H
movlm:01101110:|(HL)->L
