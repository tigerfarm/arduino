                                    ; --------------------------------------
                                    ; Test SPHL.
                                    ; 
                                    ; SPHL       SP=HL      ... Set SP to same value as H:L
                                    ;
                                    ; --------------------------------------
                lxi sp,512          ; Set stack pointer which is used with CALL and RET.
    Start:
                call Initialize
                                    ; --------------------------------------
                                    ;  + Store SP to memory and retrieve SP from memory .
                call NewTest
                mvi h,2             ; Set address register pair H:L.
                mvi l,6
                out 36              ; Print the register values for H:L.
                lxi sp,128          ; Set stack pointer to a test value.
                out 45              ; Print the SP register address.
                sphl                ; Set SP to the same address value as H:L.
                out 45              ; Print the SP register address.

                                    ; --------------------------------------
                lxi sp,512          ; Set stack pointer which is used with CALL and RET.
                call NewTest
                lxi sp,518          ; Set stack pointer to test value.
                                    ;
                shld byteL          ; Given byteL address(hb:lb),
                                    ;   Store register L to memory location of byteL (byteL address).
                                    ;   Store register H to memory location of byteL + 1 (byteH address).
                                    ; --------------------------------------
                call println
                hlt
                jmp Start
                                    ; ------------------------------------------
                                    ; Subroutines
                                    ;
                                    ; -------------------
        InitializeStr db    '\r\n++ Initialize setting.'
    Initialize:
                lxi h,InitializeStr
                call PrintStr
                mvi a,0             ; Initialize the test counter.
                sta testCounter
                ret
                                    ; -------------------
        SeparatorStr    db  '\r\n---'
    Separator:
                push h
                lxi h,SeparatorStr
                call PrintStr
                pop h
                ret
                                    ; -------------------
        NewTestA    ds      1
        TestStr     db      '\r\n++ '
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
                sta PrintStrA       ; Retain register A value.
    PrintStrContinue:
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi STRTERM         ; Compare to see if it's the string terminate byte.
                jz PrintStrDone
                out 3               ; Out register A to the serial terminal port.
                inx h               ; Increment H:L register pair.
                jmp PrintStrContinue
        PrintStrDone:
                lda PrintStrA       ; Restore register A value.
                ret
                                    ;
    println:
                mvi a,'\r'
                out 3
                mvi a,'\n'
                out 3
                ret
                                    ; --------------------------------------
                                    ; Variables
    byteL       ds      1
    byteH       ds      1
                                    ; --------------------------------------
                end
                                    ; --------------------------------------
                                    ; Successful run:
                                    ;
+ Download complete.
+ r, RUN.
+ runProcessor()

++ Initialize setting.
++ 1: 
 > Register H:L =   2:  6 = 00000010:00000110, Data:   0 = 000 = 00000000
 > Register SP =   128 = 00000000:10000000
 > Register SP =   518 = 00000010:00000110
++ 2: 
++ HALT, host_read_status_led_WAIT() = 0
                                    ; --------------------------------------
                                    ; --------------------------------------
