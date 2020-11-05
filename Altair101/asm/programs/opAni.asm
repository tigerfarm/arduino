                                    ; --------------------------------------
                                    ; Test opcode ANI.
                                    ; AND # (immediate db) with register A.
                                    ; --------------------------------------
    Start:
                mvi a,0             ; Initialize the test counter.
                sta testCounter
                                    ; --------------------------------------
                call NewTest
                                    ;          ani # 11 100 110
                mvi a,248
                out 37              ; Register A = 248 = 370 = 11111000
                lxi h,AndStr
                call PrintStr
                mvi a,176
                out 37              ; Register A = 176 = 260 = 10110000
                lxi h,EqualStr
                call PrintStr
                ani 248             ; AND immediate # with register A
                out 37              ; Register A = 176 = 260 = 10110000
                cpi 176             ; 176 = A. Zero bit flag is true.
                jz okayani          ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
    okayani:
                                    ; --------------------------------------
                call NewTest
                                    ;          ani # 11 100 110
                mvi a,aNumber
                out 37              ; Register A = 237 = 355 = 11101101
                lxi h,AndStr
                call PrintStr
                mvi a,176
                out 37              ; Register A = 176 = 260 = 10110000
                lxi h,EqualStr
                call PrintStr
                ani aNumber         ; AND immediate # with register A
                out 37              ; Register A = 160 = 240 = 10100000
                cpi 160
                jz okayani2         ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
    okayani2:
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
        TestStr     db  '\n++ --------------------------------------'
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
        PrintStrA db    1
        STRTERM equ     0ffh        ; String terminator. ASM appends 0ffh at the end of db strings.
    PrintStr:
                sta PrintStrA       ; Retain register A value.
    PrintStrContinue:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM         ; Compare to see if it's the string terminate byte.
                jz PrintStrDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintStrContinue
        PrintStrDone:
                lda PrintStrA       ; Restore register A value.
                ret
                                    ;
                                    ; --------------------------------------
                                    ; Variables
                                    ;
        AndStr      db  '\n--- AND ---'
        EqualStr    db  '\n==========='
        aNumber     equ  237         ; Test number.
                                    ; --------------------------------------
                end
                                    ; --------------------------------------
                                    ; Successful run:
                                    ;
++ 1: 
 > Register A = 248 = 370 = 11111000
--- AND ---
 > Register A = 176 = 260 = 10110000
===========
 > Register A = 176 = 260 = 10110000
+ HLT, program halted.
                                    ;
                                    ; --------------------------------------
