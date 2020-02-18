                            ; --------------------------------------
                            ; Test ADD, and moving data from addresses to registers and back.
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
                            ;
            out 38          ; Print the register results.
                            ; --------------------------------------
                            ; Add content at address H:L, to register A.
                            ;
            mvi a,6         ; Move # to register A.
            out 37          ; Print register A.
            sta Addr1       ; Store register A's content to the address(hb:lb).
            lxi h,Addr1     ; Load an address (value of Addr1) into H:L.
            out 36          ; Print the register values for H:L and the content at that address.
                            ;
            mvi a,3         ; Move # to register A.
            out 37          ; Print register A.
            add m           ; ADD: Content at address H:L(M) + Register A = 6 + 3 = 9
            out 36          ; Print the register results.
                            ;
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ; --------------------------------------
            end