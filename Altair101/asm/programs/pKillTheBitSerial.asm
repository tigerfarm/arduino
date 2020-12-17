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
                        ; ------------------------------------------------
                        ; To have the bit move faster, use less LDAX instructions, or slower baud rate.
    Begin:
                        ; Display bit pattern on upper 8 address lights.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
            LDAX D      ; Move data from address D:E, to register A.
                        ;
            DAD B       ; Add B:C to H:L. Set carry bit. Increments the display counter
            IN 6        ; Check the sense switch serial inputs that can kill the bit.
            XRA D       ; Exclusive OR register with A, to either remove or add a bit.
                        ;
            RRC         ; Rotate right register A (shift byte right 1 bit). Set carry bit.
            MOV D,A     ; Register D, is used by LDAX to display the moving bit(s).
            JMP Begin   ; Jump to address label.
                        ; ------------------------------------------------
            END

+ Print Program Bytes and description.
++ Address:byte      databyte :hex:oct > description
+ End of list.
