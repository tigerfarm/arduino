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
                            ; First: Opcode fetch, on: MEMR MI WO   + On: MEMR    WO > MI should be on.
                            ; Second, Fetch lb, on: MEMR WO         + On: MEMR MI WO > MI should be off.
                            ; Third, Fetch hb, on: MEMR WO          + On: MEMR    WO > correct: memory read
                            ; Forth, writing data to memory         + On: MEMR    WO > All lights should be off.
                            ; All status lights are off. Write output, and WO is reverse logic, WO is off.
                            ; Address LED lights: 4.                + 4: 00000100
                            ;                                       + Data value of 6 is not displayed.
                            ; Data LED lights are all on because the lights are tied to the data input bus, which is floating.
                            ; Since I'm emulating with a poetic license, I'll set the data lights to the data value.
                            ; ---
                            ; Load the value 6 from memory location 64, into register A.
                            ; I moved 0, into register A.
                            ; Process instruction: lda 40Q          + Actual Status LED lights
                            ; First: Opcode fetch, on: MEMR MI WO   + On: MEMR    WO > MI should be on.
                            ; Second, Fetch lb, on: MEMR WO         + On: MEMR MI WO > MI should be off.
                            ; Third, Fetch hb, on: MEMR WO          + On: MEMR    WO > correct: memory read
                            ; Forth, Fetching the data from memory, + On: MEMR    WO > correct: memory read
                            ; Address LED lights: 11.               + 11: 00001011   > correct: memory read
                            ;                                       + Data value of 6 is not displayed.
                            ; Shows the data value (at 40Q) on the data LED lights.
                            ;
                            ; --------------------------------------
    Start:
            mvi a,6         ; Move # to register A.
            sta 64          ; Store register A's content to the address(hb:lb).
            out 36          ; Print the register values for H:L and the content at that address.
            mvi a,0         ; Move # to register A.
            lda 64          ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
            hlt
                            ; --------------------------------------
            jmp Start
            end