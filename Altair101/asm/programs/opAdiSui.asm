                                    ; ------------------------------------------
                                    ; Test ADI and SUI.
                                    ; Add immediate number to register A.
                                    ; Subtract immediate number from register A.
                                    ; 
                                    ; Successful run:
                                    ; ++ A=2 A+3=5
                                    ; ++ A=5 A+3=8
                                    ; ++ A=8 A-2=6
                                    ; ++ A=6 A-2=4
                                    ; 
                                    ; ------------------------------------------
    Start:
                mvi a,0             ; Initialize the test counter.
                sta testCounter
                                    ; ------------------------------------------
                                    ; Test ADI with decimal value.
                                    ;
                mvi a,2             ; Move # to register A.
                sta regA            ; Retain register A value.
                call NewTest
                mvi a,'A'
                out 3
                mvi a,'='
                out 3
                lda regA            ; Restore register A value.
                call PrintDigit
                mvi a,' '
                out 3
                mvi a,'A'
                out 3
                mvi a,'+'
                out 3
                mvi a,3
                call PrintDigit
                mvi a,'='
                out 3
                mov a,b
                                    ;
                lda regA            ; Restore register A value.
                adi 3               ; Add immediate number to register A.
                sta regA            ; Retain register A sum.
                call PrintDigit
                mvi b,5             ; Answer should be 5.
                cmp b               ; B = A. Zero bit flag is true.
                jz okaya1           ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
        okaya1:
                                    ; ------------------------------------------
                                    ; Test ADI with EQU value.
                                    ;
                call NewTest
                mvi a,'A'
                out 3
                mvi a,'='
                out 3
                lda regA            ; Restore register A value.
                call PrintDigit
                mvi a,' '
                out 3
                mvi a,'A'
                out 3
                mvi a,'+'
                out 3
                mvi a,i3
                call PrintDigit
                mvi a,'='
                out 3
                mov a,b
                                    ;
                lda regA            ; Restore register A value.
                adi i3              ; Add immediate number to register A.
                sta regA            ; Retain register A sum.
                call PrintDigit
                mvi b,8             ; Answer should be 8.
                cmp b               ; B = A. Zero bit flag is true.
                jz okaya2           ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
        okaya2:
                                    ; ------------------------------------------
                                    ; Test SUI with decimal value.
                                    ;
                call NewTest
                mvi a,'A'
                out 3
                mvi a,'='
                out 3
                lda regA            ; Restore register A value.
                call PrintDigit
                mvi a,' '
                out 3
                mvi a,'A'
                out 3
                mvi a,'-'
                out 3
                mvi a,2
                call PrintDigit
                mvi a,'='
                out 3
                mov a,b
                                    ;
                lda regA            ; Restore register A value.
                sui 2               ; Subtract immediate number from register A.
                sta regA            ; Retain register A difference.
                call PrintDigit
                mvi b,6             ; Answer should be 6.
                cmp b               ; B = A. Zero bit flag is true.
                jz okays1           ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
        okays1:
                                    ; ------------------------------------------
                                     ; Test SUI with decimal value.
                                    ;
                call NewTest
                mvi a,'A'
                out 3
                mvi a,'='
                out 3
                lda regA            ; Restore register A value.
                call PrintDigit
                mvi a,' '
                out 3
                mvi a,'A'
                out 3
                mvi a,'-'
                out 3
                mvi a,2
                call PrintDigit
                mvi a,'='
                out 3
                mov a,b
                                    ;
                lda regA            ; Restore register A value.
                sui i2              ; Subtract immediate number from register A.
                sta regA            ; Retain register A difference.
                call PrintDigit
                mvi b,4             ; Answer should be 4.
                cmp b               ; B = A. Zero bit flag is true.
                jz okays2           ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
        okays2:
                                    ; ------------------------------------------
                hlt
                jmp Start
                                    ;
                                    ; ------------------------------------------
                                    ; ------------------------------------------
                                    ; Subroutines
                                    ;
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
        STRTERM equ     0ffh        ; String terminator.
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
    PrintStrln:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM         ; Compare to see if it's the string terminate byte.
                jz PrintStrlnDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintStrln
        PrintStrlnDone:
                mvi a,'\n'          ; Finish by printing a new line character.
                out 3
                ret
                                    ; -------------------
                                    ; Error handling.
                                    ;
        ErrorStr  db      '\n- Error\n'
    Error:
                lxi h,ErrorStr
                call PrintStr
                out 39              ; Print the registers and other system values.
                                    ; Print first, else register A will be changed.
                hlt
                jmp Start
                                    ; ------------------------------------------
                                    ; Variables
                                    ;
    i3          equ     3
    i2          equ     2
    regA        db      0           ; A variable for storing register A's value.
                                    ;
                                    ; ------------------------------------------
                end
