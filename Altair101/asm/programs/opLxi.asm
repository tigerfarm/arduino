                                    ; --------------------------------------
                                    ; Test LXI.
                                    ;
                                    ; Load register pair with 16-bit immediate address value(a).
                                    ;   LXI B,a     byte 2 of immediate -> C, byte 3 -> B.
                                    ;   LXI D,a     byte 2 of immediate -> E, byte 3 -> D.
                                    ;   LXI H,a     byte 2 of immediate -> L, byte 3 -> H.
                                    ; If moving a 16-bit address:
                                    ;   LXI a
                                    ; Move a(lb:hb) data into the register pair RP address.
                                    ; Move the lb data into C and hb data into B -> Register pair B:C = hb:lb.
                                    ; Move the lb data into E and hb data into D -> Register pair D:E = hb:lb.
                                    ; Move the lb data into L and hb data into H -> Register pair H:L = hb:lb.
                                    ;
                                    ; --------------------------------------
                lxi sp,512          ; Set stack pointer which is used with CALL and RET.
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
                                    ; --------------------------------------
                call NewTest
                lxi b,Addr1         ; Load an address (value of Addr1) into B:C.
                out 40              ; Print the register values for B:C, and the content at that address.
                lxi b,Addr2
                out 40
                lxi b,Addr3
                out 40
                                    ; --------------------------------------
                call NewTest
                lxi d,Addr1         ; Load an address (value of Addr1) into D:E.
                out 41              ; Print the register values for D:E, and the content at that address.
                lxi d,Addr2
                out 41
                lxi d,Addr3
                out 41
                                    ; --------------------------------------
                                    ;
                call NewTest
                lxi h,Addr1         ; Load an address (value of Addr1) into H:L.
                out 36              ; Print the register values for H:L, and the content at that address.
                lxi h,Addr2
                out 36
                lxi h,Addr3
                out 36
                                    ; --------------------------------------
                call NewTest
                lxi b,42            ; Load an address into register pairs.
                out 40
                lxi d,512
                out 41
                lxi h,786
                out 36
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
                lxi sp,512          ; Reset the stack pointer for the next CALL and RET.
                                    ; --------------------------------------
                call println
                hlt
                jmp Start
                                    ; ------------------------------------------
                                    ; ------------------------------------------
                                    ; Subroutines
                                    ;
                                    ; -------------------
        SeparatorStr    db  '\r\n---'
    Separator:
                lxi h,SeparatorStr
                call PrintStr
                ret
                                    ; -------------------
        TestStr     db  '\r\n++ '
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
                inx h               ; Increment H:L register pair.
                jmp PrintStr
        PrintStrDone:
                ret
                                    ;
    println:
                mvi a,'\r'
                out 3
                mvi a,'\n'
                out 3
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
+ Download complete.
+ r, RUN.
+ runProcessor()
------------
+ regA:   1 = 001 = 00000001
+ regB:   2 = 002 = 00000010  regC:   3 = 003 = 00000011
+ regD:   4 = 004 = 00000100  regE:   5 = 005 = 00000101
+ regH:   6 = 006 = 00000110  regL:   7 = 007 = 00000111
------------
++ 1: 
 > Register B:C =   0:128, Data:  45 = 055 = 00101101
 > Register B:C =   2:  0, Data:   0 = 000 = 00000000
 > Register B:C =   3: 18, Data:   0 = 000 = 00000000
++ 2: 
 > Register D:E =   0:128, Data:  45 = 055 = 00101101
 > Register D:E =   2:  0, Data:   0 = 000 = 00000000
 > Register D:E =   3: 18, Data:   0 = 000 = 00000000
++ 3: 
 > Register H:L =   0:128, Data:  45 = 055 = 00101101
 > Register H:L =   2:  0, Data:   0 = 000 = 00000000
 > Register H:L =   3: 18, Data:   0 = 000 = 00000000
++ 4: 
 > Register B:C =   0: 42, Data: 205 = 315 = 11001101
 > Register D:E =   2:  0, Data:   0 = 000 = 00000000
 > Register H:L =   3: 18, Data:   0 = 000 = 00000000
++ 5: 
 > Register SP =   51200000010:00000000
+ Status byte, regS00000100:Sign=0:Zero=0:.:HalfCarry=0:.:Parity=1:.:Carry=0:
 > Register SP =    6400000000:01000000
+ Status byte, regS00000100:Sign=0:Zero=0:.:HalfCarry=0:.:Parity=1:.:Carry=0:
 > Register SP =  100000000011:11101000
+ Status byte, regS00000100:Sign=0:Zero=0:.:HalfCarry=0:.:Parity=1:.:Carry=0:
 > Register SP = 3200001111101:00000000
+ Status byte, regS00000100:Sign=0:Zero=0:.:HalfCarry=0:.:Parity=1:.:Carry=0:
++ HALT, host_read_status_led_WAIT() = 0
                                    ;
                                    ; --------------------------------------
