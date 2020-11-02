                            ; --------------------------------------
                            ; Test LDA and STA.
                            ; Moving data from an address to a register, and back from a register to an address.
                            ;
                            ; STA a: Store register A data to an address a(hb:lb).
                            ; LDA a: Load register A with the data at the address a(hb:lb).
                            ;
                            ; 8085 program to add two 8 bit numbers
                            ; https://www.geeksforgeeks.org/assembly-language-program-8085-microprocessor-add-two-8-bit-numbers/
                            ;
                            ; --------------------------------------
            jmp Start       ; Jump to start of the test program.
                            ;
                            ; --------------------------------------
                            ; Program variables
    Addr1   equ     128
    Addr2   ds      2
                            ; --------------------------------------
    Start:
            mvi h,0         ; Address for memory address testing.
            mvi l,96        ; Greater than 93, because: Write to serial port. Number of bytes: 93
            out 36          ; Print the register values for H:L and the content at that address.
                            ;
                            ; --------------------------------------
                            ; Test using an number for the immediate address.
                            ;
            mvi a,6         ; Move # to register A.
            out 37          ; Print register A.
            sta 96          ; Store register A's data value to the address(hb:lb).
            out 36          ; Print the register values for H:L and the content at that address.
            hlt
                            ; > Register H:L = 0:96, Data: 0
                            ; > Register A =   6 = 006 = 00000110
                            ; > Register H:L = 0:64, Data: 6
                            ;
            mvi a,0         ; Move # to register A.
            out 37          ; Print register A.
            lda 96          ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
            hlt
                            ; > Register A =   0 = 000 = 00000000
                            ; > Register A =   6 = 006 = 00000110
                            ;
                            ; --------------------------------------
                            ; Test using an EQU for the immediate address.
                            ;
            mvi a,9         ; Move immediate # to register A.
            out 37          ; Print register A.
            sta Addr1       ; Store register A's data value to the address(hb:lb).
            lxi h,Addr1     ; Load an address (value of Addr1) into H:L.
            out 36          ; Print the register values for H:L, and the content at that address.
            hlt
                            ; + regA:   9 = 011 = 00001001
                            ; + Register H:L = 0:128, Data: 9
                            ;
            mvi a,0         ; Move # to register A.
            lda 60          ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
            hlt
                            ; > Register A =  58 = 072 = 00111010
                            ; Program listing the value at address 60:
                            ; ++      60:00111100: 00111010 : 3A:072 > opcode: lda Addr2
                            ; 
                            ; --------------------------------------
                            ; Test using an DS value and label for the immediate address.
                            ; How to put an address into 2 bytes of memory?
                            ;
            mvi a,12        ; Move # to register A.
            sta Addr2       ; Store register A's content to the address at label: Addr2.
            mvi a,0         ; Move # to register A.
            lda Addr2       ; Load register A from the address(hb:lb).
                            ; ++ Byte array number:  60, Byte: 00111010 Octal:072 Decimal58
            out 37          ; Print register A.
                            ;
            inr a
            sta Addr2       ; Store the incremented value to: Addr2.
            mvi a,0         ; Move # to register A.
            lda Addr2       ; Load Addr2 into register A.
            out 37          ; Print register A.
            hlt
                            ; > Register A =  12 = 014 = 00001100
                            ; --------------------------------------
            NOP
            jmp Start
                            ;
                            ; --------------------------------------
            end
