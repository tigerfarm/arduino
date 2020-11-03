                                    ; --------------------------------------
                                    ; Test ADI and SUI.
                                    ; Add immediate number to register A.
                                    ; Subtract immediate number from register A.
                                    ; 
                                    ; 
                                    ; --------------------------------------
                jmp Start           ; Jump to start the test.
                                    ; --------------------------------------
        i3      equ     3
        i2      equ     2
                                    ; 
                                    ; --------------------------------------
        Start:
                                    ; --------------------------------------
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
                sta regA            ; Retain register A value.
                call PrintDigit
                mvi b,5             ; Answer should be 5.
                cmp b               ; B = A. Zero bit flag is true.
                jz okaya1           ; Zero bit flag is set, jump.
                jmp Error           ; The above should have jumped passed this.
    okaya1:
                                    ; --------------------------------------
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
                sta regA            ; Retain register A value.
                call PrintDigit
                mvi b,8             ; Answer should be 8.
                cmp b               ; B = A. Zero bit flag is true.
                jz okaya2           ; Zero bit flag is set, jump.
            j   mp Error            ; The above should have jumped passed this.
    okaya2:
                        ; --------------------------------------
                        ; Test with decimal value.
            call NewTest
            mov b,a 
            mvi a,'\n'
            out 3
            mvi a,'A'
            out 3
            mvi a,'-'
            out 3
            mvi a,i2
            call PrintDigit
            mvi a,'='
            out 3
            mov a,b
                        ;
            sui 6       ; Subtract immediate number from register A.
            out 37      ; > Register A =  73
            mvi c,73    ; Move # to register C.
            cmp c       ; C = A.
            jz okays1
            jmp Error
    okays1:
                        ; --------------------------------------
                        ; Test SUI with EQU value.
            call NewTest
            mov b,a 
            mvi a,'\n'
            out 3
            mvi a,'A'
            out 3
            mvi a,'-'
            out 3
            mvi a,i2
            call PrintDigit
            mvi a,'='
            out 3
            mov a,b
                        ;
            sui i2      ; Subtract immediate number from register A.
            out 37      ; > Register A =  67
            mvi c,67    ; Move # to register C.
            cmp c       ; C = A.
            jz okays2
            jmp Error
    okays2:
                                    ; --------------------------------------
                hlt
                jmp Start
                                    ;
                                    ; ------------------------------------------
                                    ; ------------------------------------------
                                    ; Routines
                                    ;
                                    ; -------------------
        TestStr  db      '\n++ '
        NewTest:
                lxi h,TestStr
                call PrintStr
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
                                    ; Special characters.
        SPACE   equ     32          ; 32 is space, ' '.
        NL      equ     10          ; 10 is new line, '\n'.
        STRTERM equ     0ffh        ; String terminator.
                                    ;
        PrintStr:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM           ; Compare to see if it's the string terminate byte.
                jz PrintStrDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintStr
        PrintStrDone:
                ret
                                    ;
        PrintStrln:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM           ; Compare to see if it's the string terminate byte.
                jz PrintStrlnDone
                out 3               ; Out register A to the serial terminal port.
                inr m               ; Increment H:L register pair.
                jmp PrintStrln
        PrintStrlnDone:
                mvi a,NL            ; Finish by printing a new line character.
                out 3
                ret
                                    ; --------------------------------------
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
                                    ; --------------------------------------
                                    ; Variables
                                    ;
        regA    db 0                ; A variable for storing register A's value.
        counter db 0                ; Initialize test counter which is used in PrintTestCounter.
                                    ;
                                    ; ------------------------------------------
                end
