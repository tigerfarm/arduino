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
                            ; Store the value 6 into memory location 64.
                            ; I moved 6, into register A.
                            ; Process instruction: sta 64           + Actual Status LED lights
                            ; First: Opcode fetch, on: MEMR MI WO   + On: MEMR MI WO
                            ; Second, Fetch lb, on: MEMR WO         + On: MEMR    WO
                            ; Third, Fetch hb, on: MEMR WO          + On: MEMR    WO
                            ; Forth, writing data to memory         + On: MEMR    WO
                            ; All status lights are off. Write output, and WO is reverse logic, WO is off.
                            ; Data LED lights:    6.                + 6: 00000110
                            ; Address LED lights: 4.                + 4: 00000100
                            ; Data LED lights are all on because the lights are tied to the data input bus, which is floating.
                            ; Since I'm emulating with a poetic license, I'll set the data lights to the data value.
                            ; ---
                            ; Load the value 6 from memory location 64, into register A.
                            ; I moved 0, into register A.
                            ; Process instruction: lda 64           + Actual Status LED lights
                            ; First: Opcode fetch, on: MEMR MI WO   + On: MEMR MI WO
                            ; Second, Fetch lb, on: MEMR WO         + On: MEMR    WO
                            ; Third, Fetch hb, on: MEMR WO          + On: MEMR    WO
                            ; Forth, Fetching the data from memory, + On: MEMR    WO
                            ; Data LED lights:    6.                + 6: 00000110
                            ; Address LED lights: 15.               + 15: 00001111
                            ; Data lights shows the data value at address 64.
                            ;
                            ; MEMR - Bar LED #1
                            ; M1   - Bar LED #3
                            ; HLTA - Bar LED #5
                            ; WO   - Bar LED #7
                            ; WAIT - Green LED
                            ; HLDA - Red LED

                            ; --------------------------------------
    Start:
            mvi a,6         ; Move # to register A.
            sta 64          ; Store register A's content to the address(hb:lb).
            mvi h,0         ; Set H:L address value to 64.
            mvi l,64
            out 36          ; Print the register values for H:L and the content at that address.
            mvi a,0         ; Move # to register A.
            lda 64          ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
            hlt
                            ; --------------------------------------
            jmp Start
            end

++ Address:byte      databyte :hex > description
++       0:00000000: 00111110 : 3E > opcode: mvi a,6
++       1:00000001: 00000110 : 06 > immediate: 6 : 6
++       2:00000010: 00110010 : 32 > opcode: sta 64
++       3:00000011: 01000000 : 40 > lb: 64
++       4:00000100: 00000000 : 00 > hb: 0
++       5:00000101: 00100110 : 26 > opcode: mvi h,0
++       6:00000110: 00000000 : 00 > immediate: 0 : 0
++       7:00000111: 00101110 : 2E > opcode: mvi l,64
++       8:00001000: 01000000 : 40 > immediate: 64 : 64
++       9:00001001: 11100011 : E3 > opcode: out 36
++      10:00001010: 00100100 : 24 > immediate: 36 : 36
++      11:00001011: 00111110 : 3E > opcode: mvi a,0
++      12:00001100: 00000000 : 00 > immediate: 0 : 0
++      13:00001101: 00111010 : 3A > opcode: lda 64
++      14:00001110: 01000000 : 40 > lb: 64
++      15:00001111: 00000000 : 00 > hb: 0
++      16:00010000: 11100011 : E3 > opcode: out 37
++      17:00010001: 00100101 : 25 > immediate: 37 : 37
++      18:00010010: 01110110 : 76 > opcode: hlt
++      19:00010011: 11000011 : C3 > opcode: jmp Start
++      20:00010100: 00000000 : 00 > lb: 0
++      21:00010101: 00000000 : 00 > hb: 0
+ End of list.