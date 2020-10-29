                            ; --------------------------------------
                            ; Test ADD, and moving data from addresses to registers and back.
                            ;
                            ; Before loading the program confirm the memory is all set to zeros.
                            ;   Double flip CLR.
                            ; Load the program by:
                            ;   Setting Sense switches to zero and double flipping AUX2 down.
                            ;   Upload from the laptop to the Altair.
                            ; EXAMINE address 64 (address of Addr1), which is 0.
                            ; Flip RESET, to set back to address zero.
                            ; Run the program.
                            ; EXAMINE address 64 (address of Addr1), which equals 6 from: sta Addr1.
                            ;
                            ; 8085 program to add two 8 bit numbers
                            ; https://www.geeksforgeeks.org/assembly-language-program-8085-microprocessor-add-two-8-bit-numbers/
                            ;
                            ; --------------------------------------
            jmp Start       ; Jump to start of the test program.
                            ;
    Addr1   equ     64
    Addr2   ds      2
                            ;
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
            mvi a,7         ; Initialize values to add.
            mvi b,1
            mvi c,2
            mvi d,3
            mvi e,4
            mvi h,5
            mvi l,6
            out 38          ; Print the register values.
                            ; + regA:   7 = 007 = 00000111
                            ; + regB:   1 = 001 = 00000001  regC:   2 = 002 = 00000010
                            ; + regD:   3 = 003 = 00000011  regE:   4 = 004 = 00000100
                            ; + regH:   5 = 005 = 00000101  regL:   6 = 006 = 00000110
                            ; 
                            ; --------------------------------------
                            ; Test register adding.
            add b           ; Add source register to A.
            mov b,a         ; Move the result back to the source register, to print the sum, later.
            add c
            mov c,a
            add d
            mov d,a
            add e
            mov e,a
            add h
            mov h,a
            add l
            mov l,a
            add a
            out 38          ; Print the register results. The following is correct.
                            ; + regA:  56 = 070 = 00111000 (regA + regB = 8)
                            ; + regB:   8 = 010 = 00001000  regC:  10 = 012 = 00001010
                            ; + regD:  13 = 015 = 00001101  regE:  17 = 021 = 00010001
                            ; + regH:  22 = 026 = 00010110  regL:  28 = 034 = 00011100
                            ; 
                            ; --------------------------------------
                            ; Add content at address H:L, to register A.
                            ;
            mvi a,6         ; Move # to register A.
            out 37          ; Print register A.
            sta Addr1       ; Store register A's content to the address(hb:lb).
            lxi h,Addr1     ; Load an address (value of Addr1) into H:L.
            out 36          ; Print the register values for H:L and the content at that address.
                            ; > Register A =   6 = 006 = 00000110
                            ; > Register H:L = 0:64, Data: 6
                            ;
            mvi a,3         ; Move # to register A.
            out 37          ; Print register A.
            add m           ; ADD: Content at address H:L(M) + Register A = 6 + 3 = 9
            out 37          ; Print register A == 9.
                            ; > Register A =   3 = 003 = 00000011
                            ; > Register A =   9 = 011 = 00001001
                            ;
            sta Addr2       ; Store register A's content to the address(hb:lb).
                            ;
                            ; EXAMINE address 64 (address of Addr1), which equals 6 from: sta Addr1.
                            ; EXAMINE address 03 (address of Addr2), which equals 9 from: sta Addr2.
                            ;
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ; --------------------------------------
            end