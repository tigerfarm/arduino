                            ; --------------------------------------
                            ; Test LDA and STA to the serial monitor.
                            ;
                            ; lda a Load register A with data from the address, a(hb:lb).
                            ; sta a Store register A to the address, a(hb:lb).
                            ;
                            ; Need to figure out more about addressing.
                            ; For example,
                            ;   Moving from addresses to registers and back.
                            ;
                            ; 8085 program to add two 8 bit numbers
                            ; https://www.geeksforgeeks.org/assembly-language-program-8085-microprocessor-add-two-8-bit-numbers/
                            ;
                            ; --------------------------------------
            jmp Start       ; Jump to start of the test program.
                            ;
    Addr1   equ     128
    Addr2   ds      2
                            ;
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
            mvi h,0         ; Address for memory address testing.
            mvi l,64
            out 36          ; Print the register values for H:L and the content at that address.
                            ;
                            ; --------------------------------------
                            ; Test using an number for the immediate address.
                            ;
            mvi a,6         ; Move # to register A.
            out 37          ; Print register A.
            sta 64          ; Store register A's content to the address(hb:lb).
            out 36          ; Print the register values for H:L and the content at that address.
            hlt
                            ;
            mvi a,0         ; Move # to register A.
            out 37          ; Print register A.
            lda 64          ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
            hlt
                            ; --------------------------------------
                            ; Test using an EQU for the immediate address.
                            ;
            mvi a,9         ; Move # to register A.
            sta Addr1       ; Store register A's content to the address(hb:lb).
            out 38          ; Print the register values.
            lxi h,Addr1     ; Load an address into H:L.
            out 36          ; Print the register values for H:L, and the content at that address.
            hlt
                            ;
            mvi a,0         ; Move # to register A.
            lda 60          ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
            hlt
                            ; --------------------------------------
                            ; Test using an DS value and label for the immediate address.
                            ; How to put an address into 2 bytes of memory?
                            ;
            mvi a,12        ; Move # to register A.
            sta Addr2       ; Store register A's content to the address at label: Addr2.
            mvi a,0         ; Move # to register A.
            lda 60          ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
            hlt
                            ; --------------------------------------
            NOP
            jmp Halt        ; Jump back to the early halt command.
                            ; --------------------------------------
            end