                                    ; --------------------------------------
                                    ; Test LXI.
                                    ;
                                    ; Load register pair with 16-bit immediate address value.
                                    ;   LXI B, byte 2 -> C, byte 3 -> B.
                                    ;   LXI D, byte 2 -> E, byte 3 -> D.
                                    ;   LXI H, byte 2 -> L, byte 3 -> H.
                                    ; If moving a 16-bit address:
                                    ;   LXI a
                                    ; Move a(lb:hb) data into the register pair RP address.
                                    ; Move the lb data into C and hb data into B -> Register pair B:C = hb:lb.
                                    ; Move the lb data into E and hb data into D -> Register pair D:E = hb:lb.
                                    ; Move the lb data into L and hb data into H -> Register pair H:L = hb:lb.
                                    ;
                                    ; --------------------------------------
    Start:
                mvi a,0             ; Initialize the test counter.
                sta testCounter
                                    ; --------------------------------------
                mvi a,1             ; Initialize register values for testing.
                mvi b,2
                mvi c,3
                mvi d,4
                mvi e,5
                mvi h,6
                mvi l,7
                out 38              ; Print the register values.
                                    ;
                                    ; --------------------------------------
                                    ;
                call NewTest
                lxi b,Addr1         ; Load an address (value of Addr1) into B:C.
                out 40              ; Print the register values for B:C, and the content at that address.
                lxi b,Addr2
                out 40
                lxi b,Addr3
                out 40
                                    ;
                                    ; --------------------------------------
                                    ;
                call NewTest
                lxi d,Addr1         ; Load an address (value of Addr1) into D:E.
                out 41              ; Print the register values for D:E, and the content at that address.
                lxi d,Addr2
                out 41
                lxi d,Addr3
                out 41
                                    ;
                                    ; --------------------------------------
                                    ;
                call NewTest
                lxi h,Addr1         ; Load an address (value of Addr1) into H:L.
                out 36              ; Print the register values for H:L, and the content at that address.
                lxi h,Addr2
                out 36
                lxi h,Addr3
                out 36
                                    ;
                                    ; --------------------------------------
                                    ;
                call NewTest
                lxi b,42            ; Load an address into register pairs.
                out 40
                lxi d,512
                out 41
                lxi h,786
                out 36
                                    ;
                                    ; --------------------------------------
                call NewTest
                out 43              ; Print the stack pointer value.
                lxi sp,64           ; Stack pointer address
                out 43
                lxi sp,1000         ; Stack pointer address
                out 43
                lxi sp,32000        ; Stack pointer address
                out 43
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
                lxi h,SeparatorStr
                call PrintStr
                ret
                                    ; -------------------
        TestStr     db  '\n++ '
        testCounter db  0           ; Initialize test counter.
    NewTest:
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
        STRTERM equ     0ffh        ; String terminator. ASM appends 0ffh at the end of db strings.
    PrintStr:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM         ; Compare to see if it's the string terminate byte.
                jz PrintStrDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintStr
        PrintStrDone:
                ret
                                    ;
                                    ; --------------------------------------
                                    ; Variables
                                    ;
                                    ;         H         L
    Addr1       equ     128         ; 0000 0000 1000 0000 H:L = 0:128
    Addr2       equ     512         ; 0000 0010 0000 0000 H:L = 2:000
    Addr3       equ     786         ; 0000 0011 0001 0010 H:L = 3:018
    data1       ds      2
                                    ; --------------------------------------
                end
                                    ; --------------------------------------
                                    ; Successful run:
                                    ;
I deposited values into address: 512(16) and 786(255).
++ Byte#  42, Byte: 00010001 011 021   17
++ Byte# 128, Byte: 00110010 032 062   50
------------
++ 1: 
 > Register B:C =   0:128, Data:  50 = 062 = 00110010
 > Register B:C =   2:  0, Data:  16 = 020 = 00010000
 > Register B:C =   3: 18, Data: 255 = 377 = 11111111
++ 2: 
 > Register D:E =   0:128, Data:  50 = 062 = 00110010
 > Register D:E =   2:  0, Data:  16 = 020 = 00010000
 > Register D:E =   3: 18, Data: 255 = 377 = 11111111
++ 3: 
 > Register H:L =   0:128, Data:  50 = 062 = 00110010
 > Register H:L =   2:  0, Data:  16 = 020 = 00010000
 > Register H:L =   3: 18, Data: 255 = 377 = 11111111
++ 4: 
 > Register B:C =   0: 42, Data:  17 = 021 = 00010001
 > Register D:E =   2:  0, Data:  16 = 020 = 00010000
 > Register H:L =   3: 18, Data: 255 = 377 = 11111111
++ 5: 
+ Stack pointer:     0, Zero bit flag: 1, Carry bit flag: 0
+ Stack pointer:    64, Zero bit flag: 1, Carry bit flag: 0
+ Stack pointer:  1000, Zero bit flag: 1, Carry bit flag: 0
+ Stack pointer: 32000, Zero bit flag: 1, Carry bit flag: 0
                                    ;
                                    ; --------------------------------------
