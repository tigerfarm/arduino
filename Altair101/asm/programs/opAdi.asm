                            ; --------------------------------------
                            ; Test ADI.
                            ; Add immediate number to register A.
                            ; Steampunk file# 110.
                            ;
                            ; To change the addition values:
                            ; + Change address 1, which is initialized as 2.
                            ; ++ mvi a,2
                            ; + Change address 3, which is immediate value, 3.
                            ; ++ adi 3
                            ; + RESET and RUN, to get the new result.
                            ;
                            ; --------------------------------------
    Start:
                            ; Add two numbers together:
                            ; Register A contents + ADI immediate value.
                            ;
            mvi a,2         ; Register A = 2.
            adi 3           ; Add immediate number to register A.
                            ;
            sta 64          ; Move result (register A) to address 64.
                            ;
                            ; Echo the result to the Arduino serial monitor.
            mvi h,0         ; Set H:L to address 64.
            mvi l,64
            out 36          ; Print the content at the H:L address.
                            ; --------------------------------------
            hlt
            jmp Start
            end

; --------------------------------------
; Program Byte listing.

++ Address:byte      databyte :hex > description
++       0:00000000: 00111110 : 3E > opcode: mvi a,2
++       1:00000001: 00000010 : 02 > immediate: 2 : 2
++       2:00000010: 11000110 : C6 > opcode: adi 3
++       3:00000011: 00000011 : 03 > immediate: 3 : 3
++       4:00000100: 00110010 : 32 > opcode: sta 64
++       5:00000101: 01000000 : 40 > lb: 64
++       6:00000110: 00000000 : 00 > hb: 0
++       7:00000111: 00100110 : 26 > opcode: mvi h,0
++       8:00001000: 00000000 : 00 > immediate: 0 : 0
++       9:00001001: 00101110 : 2E > opcode: mvi l,64
++      10:00001010: 01000000 : 40 > immediate: 64 : 64
++      11:00001011: 11100011 : E3 > opcode: out 36
++      12:00001100: 00100100 : 24 > immediate: 36 : 36
++      13:00001101: 01110110 : 76 > opcode: hlt
++      14:00001110: 11000011 : C3 > opcode: jmp Start
++      15:00001111: 00000000 : 00 > lb: 0
++      16:00010000: 00000000 : 00 > hb: 0

; --------------------------------------
eof