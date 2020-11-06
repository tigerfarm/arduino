                                    ; --------------------------------------
                                    ; Test opcode ORA.
                                    ; OR register S, with register A, storing the result in register A.
                                    ; ORA S     10110SSS          ZSPCA   OR source register with A
                                    ; --------------------------------------
                jmp Start
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
                inr m               ; Increment H:L register pair.
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
        aNumber     equ  237         ; Test number.
                                    ; --------------------------------------
                end
                                    ; --------------------------------------
