                            ; --------------------------------------
                            ; Test LDA and STA. Moving data from addresses to registers and back.
                            ;
                            ; STA a: Store register A data to an address a(hb:lb).
                            ; LDA a: Load register A with the data at the address a(hb:lb).
                            ;
                            ; 8085 program to add two 8 bit numbers
                            ; https://www.geeksforgeeks.org/assembly-language-program-8085-microprocessor-add-two-8-bit-numbers/
                            ;
                            ; --------------------------------------
                            ; Video demonstrating status lights:
                            ;    https://www.youtube.com/watch?v=3_73NwB6toY
                            ; Following, is using STEP to step through the program and view the LED lights.
                            ; ---
                            ; Process instruction: lda 40Q
                            ; First: Opcode fetch, on: MEMR MI WO
                            ; Second, Fetch lb, on: MEMR WO
                            ; Third, Fetch hb, on: MEMR WO
                            ; Forth, Fetching the data from memory, on: MEMR WO
                            ; Fetching the data, shows the address (40Q) on the address LED lights.
                            ; Shows the data value (at 40Q) on the data LED lights.
                            ; ---
                            ; Process instruction: sta 41Q
                            ; First: Opcode fetch, on: MEMR MI WO
                            ; Second, Fetch lb, on: MEMR WO
                            ; Third, Fetch hb, on: MEMR WO
                            ; Forth, writting data to memory, all status lights are off. Write output, and WO is reverse logic, WO is off.
                            ; Address LED lights: 41Q.
                            ; Data LED lights are all on because the lights are tied to the data input bus, which is floating.
                            ; Since I'm emulating with a poetic license, I'll set the data lights to the data value.
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
            lxi h,Addr1     ; Load an address (value of Addr1) into H:L.
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
                            ; ++ Byte array number:  60, Byte: 00111010 Octal:072 Decimal58
            out 37          ; Print register A.
            hlt
                            ; --------------------------------------
            NOP
            jmp Halt        ; Jump back to the early halt command.
                            ; --------------------------------------
            end