                                    ; --------------------------------------
                                    ; Test opcode ORA.
                                    ; OR register S, with register A, storing the result in register A.
                                    ; ORA S     10110SSS          ZSPCA   OR source register with A
                                    ; --------------------------------------
                jmp Start           ; Jump opcode byte value is used later in this program.
    Start:
                mvi a,0             ; Initialize the test counter.
                sta testCounter
                                    ; --------------------------------------
                call NewTest
                mvi a,73
                out 37              ; Register A = 
                lxi h,OrStr
                call PrintStr
                mvi b,70
                out 30              ; Register B = 
                lxi h,EqualStr
                call PrintStr
                ora b               ; OR register B, with register A. Answer: 01 001 111 = 79.
                out 37              ; Echo register A.
                cpi 79              ; 79 = A. Zero bit flag is true.
                jz okayb1           ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
    okayb1:
                                    ; --------------------------------------
                call NewTest
                mvi a,73
                out 37
                lxi h,OrStr
                call PrintStr
                mvi c,70
                out 31
                lxi h,EqualStr
                call PrintStr
                ora c               ; OR with register A.
                out 37
                cpi 79
                jz okayc1
                jmp Error
    okayc1:
                                    ; --------------------------------------
                call NewTest
                mvi a,73
                out 37
                lxi h,OrStr
                call PrintStr
                mvi d,70
                out 32
                lxi h,EqualStr
                call PrintStr
                ora d               ; OR with register A.
                out 37
                cpi 79
                jz okayd1
                jmp Error
    okayd1:
                                    ; --------------------------------------
                call NewTest
                mvi a,73
                out 37
                lxi h,OrStr
                call PrintStr
                mvi e,70
                out 33
                lxi h,EqualStr
                call PrintStr
                ora e               ; OR with register A.
                out 37
                cpi 79
                jz okaye1
                jmp Error
    okaye1:
                                    ; --------------------------------------
                call NewTest
                mvi a,73
                out 37
                lxi h,OrStr
                call PrintStr
                mvi h,70
                out 34
                push h
                lxi h,EqualStr
                call PrintStr
                pop h
                ora h               ; OR with register A.
                out 37
                cpi 79
                jz okayh1
                jmp Error
    okayh1:
                                    ; --------------------------------------
                call NewTest
                mvi a,73
                out 37
                lxi h,OrStr
                call PrintStr
                mvi l,70
                out 35
                push h
                lxi h,EqualStr
                call PrintStr
                pop h
                ora l               ; OR with register A.
                out 37
                cpi 79
                jz okayl1
                jmp Error
    okayl1:
                                    ; --------------------------------------
                call NewTest
                mvi a,73
                out 37
                                    ;
                mvi h,0
                mvi l,0             ; Register M address data (opcode jmp) =                 11 000 011
                out 36
                                    ;
                ora m               ; OR data a register M address, with register A. Answer: 11 001 011 = ?
                out 37
                cpi 203
                jz okaym1
                jmp Error
    okaym1:
                                    ; --------------------------------------
                lxi h,SeparatorStr
                call PrintStr
                hlt
                jmp Start
                                    ; ------------------------------------------
                                    ; ------------------------------------------
                                    ; Subroutines
                                    ;
                                    ; -------------------
        SeparatorStr    db  '\n--------------------------------------'
    Separator:
                push h
                lxi h,SeparatorStr
                call PrintStr
                pop h
                ret
                                    ; -------------------
        TestStr     db  '\n++ '
        testCounter db  0           ; Initialize test counter.
    NewTest:
                lxi h,SeparatorStr
                call PrintStr
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
        ErrorStr     db  '\n-- Error'
    Error:
                lxi h,ErrorStr
                call PrintStr
                out 39              ; Print the registers and other system values.
                hlt                 ; Halt after the error.
                jmp Start
                                    ;
                                    ; -------------------
        PrintDigitA    db 0         ; A variable for storing register A's value.
    PrintDigit:                     ; Print the digit (0-9) in register A.
                push f              ; Retain register A value.
                mvi b,'0'
                add b
                out 3
                pop f               ; Restore register A value.
                ret
                                    ; -------------------
                                    ; Routines to print a DB strings.
                                    ;
        STRTERM equ     0ffh        ; String terminator. ASM appends 0ffh at the end of db strings.
    PrintStr:
                push f              ; Retain register A value.
    PrintStrContinue:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM         ; Compare to see if it's the string terminate byte.
                jz PrintStrDone
                out 3               ; Out register A to the serial terminal port.
                inx h               ; Increment H:L register pair.
                jmp PrintStrContinue
        PrintStrDone:
                pop f               ; Restore register A value.
                ret
                                    ;
                                    ; --------------------------------------
                                    ; Variables
                                    ;
        OrStr       db  '\n--- OR ---'
        EqualStr    db  '\n=========='
        aNumber     equ  237        ; Test number.
                                    ; --------------------------------------
                end
                                    ; --------------------------------------
                                    ; Successful run:
                                    ;
--------------------------------------
++ 1: 
 > Register A =  73 = 111 = 01001001
--- OR ---
 > Register B =  70 = 106 = 01000110
==========
 > Register A =  79 = 117 = 01001111
--------------------------------------
++ 2: 
 > Register A =  73 = 111 = 01001001
--- OR ---
 > Register C =  70 = 106 = 01000110
==========
 > Register A =  79 = 117 = 01001111
--------------------------------------
++ 3: 
 > Register A =  73 = 111 = 01001001
--- OR ---
 > Register D =  70 = 106 = 01000110
==========
 > Register A =  79 = 117 = 01001111
--------------------------------------
++ 4: 
 > Register A =  73 = 111 = 01001001
--- OR ---
 > Register E =  70 = 106 = 01000110
==========
 > Register A =  79 = 117 = 01001111
--------------------------------------
++ 5: 
 > Register A =  73 = 111 = 01001001
--- OR ---
 > Register H =  70 = 106 = 01000110
==========
 > Register A =  79 = 117 = 01001111
--------------------------------------
++ 6: 
 > Register A =  73 = 111 = 01001001
--- OR ---
 > Register L =  70 = 106 = 01000110
==========
 > Register A =  79 = 117 = 01001111
--------------------------------------
++ 7: 
 > Register A =  73 = 111 = 01001001
 > Register H:L =   0:  0, Data: 195 = 303 = 11000011
 > Register A = 203 = 313 = 11001011
--------------------------------------
+ HLT, program halted.
                                    ;
                                    ; --------------------------------------
