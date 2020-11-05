                                    ; --------------------------------------
                                    ; Test SHLD.
                                    ; SHLD adr : L -> (adr); H -> (adr+1)
                                    ; Store data value from memory location: register L to a(address hb:lb).
                                    ; Store data value from memory location: register H to a+1.
                                    ;
                                    ; --------------------------------------
    Start:
                mvi a,0             ; Initialize the test counter.
                sta testCounter
                                    ; --------------------------------------
                lda byte1           ; Load register A from the address(hb:lb).
                out 37              ; Print register A.
                lda byte2           ; Load register A from the address(hb:lb).
                out 37              ; Print register A.
                                    ;
                mvi h,0             ; Set address register pair H:L, to 0:0.
                mvi l,0
                out 36              ; Print the register values for H:L, and the content at that address.
                                    ;
                                    ; --------------------------------------
                call NewTest
                lxi h,Addr3         ; Load the address value of Addr3 into H:L.
                                    ; Register H:L = 3:18, address# 786.
                out 36              ; Print the register values for H:L, and the content at that address.
                                    ;
                call Separator
                shld byte1          ; Given byte1 address(hb:lb),
                                    ;   Store register L to memory location of byte1.
                                    ;   Store register H to memory location of byte1 + 1.
                lda byte1           ; Load byte1 into register A from the memory location of byte1.
                out 37              ; Print register A.
                lda byte2           ; Load byte2 into register A from the memory location of byte1 + 1 which is byte2.
                out 37              ; Print register A.
                                    ;
                                    ; --------------------------------------
                hlt
                jmp Start
                                    ; ------------------------------------------
                                    ; ------------------------------------------
                                    ; Subroutines
                                    ;
                                    ; -------------------
        SeparatorStr    db  '\n---'
    Separator:
                push h
                lxi h,SeparatorStr
                call PrintStr
                pop h
                ret
                                    ; -------------------
        NewTestA    ds      1
        TestStr     db  '\n++ '
        testCounter db  0           ; Initialize test counter.
    NewTest:
                push h
                sta NewTestA
                sta PrintDigitA     ; Retain register A value.
                lxi h,TestStr
                call PrintStr
                lda testCounter     ; Increment and print the counter
                inr a
                sta testCounter
                call PrintDigit
                mvi a,':'
                out 3
                mvi a,' '
                out 3
                lda NewTestA
                pop h
                ret
                                    ; -------------------
        PrintDigitA    db 0         ; A variable for storing register A's value.
    PrintDigit:                 ; Print the digit (0-9) in register A.
                sta PrintDigitA     ; Retain register A value.
                mvi b,'0'
                add b
                out 3
                lda PrintDigitA     ; Restore register A value.
                ret
                                    ; -------------------
                                    ; Routines to print a DB strings.
                                    ;
        PrintStrA ds    1
        STRTERM equ     0ffh        ; String terminator. ASM appends 0ffh at the end of db strings.
    PrintStr:
                sta PrintDigitA     ; Retain register A value.
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM         ; Compare to see if it's the string terminate byte.
                jz PrintStrDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintStr
        PrintStrDone:
                lda PrintDigitA     ; Restore register A value.
                ret
                                    ;
                                    ; --------------------------------------
                                    ; Variables
                                    ;
                                    ;         H         L
    Addr1       equ     128         ; 0000 0000 1000 0000 H:L = 0:128
    Addr2       equ     512         ; 0000 0010 0000 0000 H:L = 2:000
    Addr3       equ     786         ; 0000 0011 0001 0010 H:L = 3:018
    byte1       ds      1
    byte2       ds      1
                                    ; --------------------------------------
                end
                                    ; --------------------------------------
                                    ; Successful run:
                                    ;
 > Register A =   0 = 000 = 00000000
 > Register A =   0 = 000 = 00000000
 > Register H:L =   0:  0, Data:  62 = 076 = 00111110
++ 1: 
 > Register H:L =   3: 18, Data:   0 = 000 = 00000000
---
 > Register A =  18 = 022 = 00010010
 > Register A =   3 = 003 = 00000011
                                    ;
                                    ; --------------------------------------
