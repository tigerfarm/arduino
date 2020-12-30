                                        ; --------------------------------------
                                        ; Binary to decimal conversion for printing.
                                        ;
                                        ; 10000100 : 84:204 > immediate: 132 : 132
                                        ; 10000000 : 128
                                        ; 00000100 :   4
                                        ;
                                        ; --------------------------------------
                                        ;
                lxi sp,STACK            ; Set stack pointer.
    Start:
                lxi h,StartMsg
                call printStr
                                        ; --------------------------------------
                mvi e,'A'               ; Register to be printed.
                ;
                mvi a,1
                call printByte
                mvi a,200
                call printByte
                mvi a,242
                call printByte
                mvi a,100
                call printByte
                mvi a,132
                call printByte
                mvi a,96
                call printByte
                mvi a,90
                call printByte
                mvi a,81
                call printByte
                mvi a,72
                call printByte
                mvi a,63
                call printByte
                mvi a,54
                call printByte
                mvi a,45
                ;call printNumA
                call printByte
                mvi a,36
                ;call printNumA
                call printByte
                mvi a,27
                ;call printNumA
                call printByte
                mvi a,18
                call printByte
                mvi a,9
                call printByte
                mvi a,0
                call printByte
                mvi a,255
                call printByte
                ;
                call println
                mvi a,'-'
                out PRINT_PORT
                out PRINT_PORT
                out PRINT_PORT
                out PRINT_PORT
                out PRINT_PORT
                out PRINT_PORT
                ;
                mvi a,30
                mvi e,'A'               ; Register to be printed.
                call printByte
                ;
                mvi b,31
                mov a,b
                mvi e,'B'               ; Register to be printed.
                call printByte
                mov b,a                 ; Restore the original register value.
                ;
                mvi c,32
                mov a,c
                mvi e,'C'               ; Register to be printed.
                call printByte
                mov c,a                 ; Restore the original register value.
                ;
                mvi d,33
                mov a,d
                mvi e,'D'               ; Register to be printed.
                call printByte
                mov d,a                 ; Restore the original register value.
                ;
                mvi e,34
                mov a,e
                mvi e,'E'               ; Register to be printed.
                call printByte
                mov e,a                 ; Restore the original register value.
                ;
                                        ; --------------------------------------
                call println
                hlt
                jmp Start
                                        ;
                                        ; --------------------------------------
                                        ; --------------------------------------
    printByte:                          ; Print register A as a byte string.
                mov b,a                 ; RegB for storing the print byte bits.
                ; out 37                  ; Print regA info: Register A = 248 = 370 = 11111000
                lxi h,RegAMsg
                call printStr
                mov a,e                 ; Print which register is being printed.
                out PRINT_PORT
                                        ;
                mvi a,' '
                out PRINT_PORT
                mvi a,'='
                out PRINT_PORT
                mvi a,' '
                out PRINT_PORT
                mov a,b
                call printBinaryA
                                        ;
                mov b,a
                mvi a,' '
                out PRINT_PORT
                mvi a,'='
                out PRINT_PORT
                mvi a,' '
                out PRINT_PORT
                mov a,b
                call printShortA
                                        ;
                ret
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ; Print register A as a binary string.
    printBinaryA:
                sta regA                ; Save, restore on return.
                mov b,a
                mvi c,0                 ; RegC for counting the printed bits.
                                        ;
                                        ; ------
    printBinaryBit:
                ani 128                 ; AND 10000000 with register A
                cpi 0
                jnz printBit1
                mvi a,'0'
                out PRINT_PORT
                jmp nextBit
    printBit1:
                mvi a,'1'
                out PRINT_PORT
    nextBit:
                mov a,c
                cpi 7
                jz printedBits         ; 8 bits printed.
                inr c
                mov a,b
                rlc
                mov b,a
                jmp printBinaryBit
    printedBits:
                lda regA                ; Restore
                ret
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ; Print register A digits. The decimal number from 0 to 255.
    printShortA:
                sta regA                ; Save, restore on return.
                cpi 100                 ; If less then 100, jump to tens.
                jc printX0a
                                        ;
                mvi c,'2'               ; RegC for counting the printed hundreds byte.
                mvi b,200               ; RegB is the hundreds counter.
                                        ; ------------------------
    printHundreds:
                cmp b                   ; Compare regA with regB.
                jc cmpX00               ; Jump if less than.
                jnz printX00            ; Jump if greater than.
                mov a,c                 ; Else equals, print hundred's number.
                out PRINT_PORT
                mvi a,'0'
                out PRINT_PORT
                mvi a,'0'
                out PRINT_PORT
                jmp printShortAret
    printX00:                           ; Print hundred's digit
                mov d,a
                mov a,c
                out PRINT_PORT
                mov a,d
                sub b                   ; 242-200 = 42 in register A.
                jmp printX0             ; Print the rest, example: 42.
    cmpX00:
                mov d,a
                mvi a,'0'               ; Hundreds are zero, move to tens.
                dcr c
                cmp c
                jz printX0
                                        ;
                mov a,b
                sui 100                 ; Check for the next lower hundreds.
                mov b,a
                mov a,d
                jmp printHundreds
    printX0a:
                mov d,a
                mvi a,'0'               ; Tens padding.
                out PRINT_PORT
                mov a,d
                cpi 10                  ; If less than 10, jump to single digit.
                jc printXa
                                        ; ------------------------
                                        ; Print tens digit.
    printX0:
                mvi c,'9'               ; RegC for counting the printed tens byte.
                mvi b,90                ; RegB is the tens counter.
    printTens:
                cmp b
                jc cmpX0                ; Jump if less than.
                mov d,a
                mov a,c
                out PRINT_PORT
                mov a,d
                sub b                   ; Subtract immediate number from register A.
                mov c,a                 ; Value example, d = 6, from 96.
                call printDigitC
                jmp printShortAret
    cmpX0:
                dcr c
                mov d,a
                mov a,b
                sui 10
                mov b,a
                mov a,d
                jmp printTens
    printXa:                            ; Pad the space
                mov c,a
                mvi a,'0'
                out PRINT_PORT
                mov a,c
                                        ; ------------------------
                                        ; Print ones digit.
    printOnes:
                mvi b,'0'
                add b
                out PRINT_PORT
                                        ; ------
    printShortAret:
                lda regA                ; Restore register A value.
                ret
                                        ; --------------------------------------
                                        ; --------------------------------------
    printDigitA:
                sta regA
                mvi b,'0'
                add b
                out PRINT_PORT
                lda regA
                ret
                                        ; ------
    printDigitC:                          ; Print single digit.
                mov a,c
                mvi b,'0'
                add b
                out PRINT_PORT
                ret
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ; Print a string.
    printStr:
                sta regA
    printStrNext:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz sPrintDone
                out PRINT_PORT          ; Out register A to the serial port.
                inx h                   ; Increment H:L register pair.
                jmp printStrNext
    sPrintDone:
                lda regA
                ret
                                        ; --------------------------------------
    println:
                lda regA
                mvi a,'\r'              ; Print newline, print CR and NL characters.
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
                lda regA
                ret
                                        ; --------------------------------------
                                        ;
    StartMsg    db      '\r\n+++ Print bytes as binary and decimal number strings.'
                db 0
    DigitMsg    db      '\r\n++ Number, '
                db 0
    RegAMsg     db      '\r\n > Register '
                db 0
    TERMB       equ     000h            ; String terminator.
                                        ;
    regA        db 0                    ; A variable for storing register A's value.
    PRINT_PORT  equ     2               ; When using port 2,
                                        ;   if port 3 is disable, then it defaults to 3, the default serial port.
                                        ; --------------------------------------
                ds 32                   ; Stack space.
    STACK:      equ $
                                        ; --------------------------------------
                end
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ; Successful run:
+ Download complete.
?- + r, RUN.
?- + runProcessor()

+++ Print bytes as binary and decimal number strings.
 > Register A = 00000001 = 001
 > Register A = 11001000 = 200
 > Register A = 11110010 = 242
 > Register A = 01100100 = 100
 > Register A = 10000100 = 132
 > Register A = 01100000 = 096
 > Register A = 01011010 = 090
 > Register A = 01010001 = 081
 > Register A = 01001000 = 072
 > Register A = 00111111 = 063
 > Register A = 00110110 = 054
 > Register A = 00101101 = 045
 > Register A = 00100100 = 036
 > Register A = 00011011 = 027
 > Register A = 00010010 = 018
 > Register A = 00001001 = 009
 > Register A = 00000000 = 000
 > Register A = 11111111 = 255
------
 > Register A = 00011110 = 030
 > Register B = 00011111 = 031
 > Register C = 00100000 = 032
 > Register D = 00100001 = 033
 > Register E = 00100010 = 034

++ HALT, host_read_status_led_WAIT() = 0

Print with debug (out 37):

+++ Print bytes as binary and decimal number strings.
 > Register A =   1 = 001 = 00000001 = 00000001 = 001
 > Register A = 200 = 310 = 11001000 = 11001000 = 200
 > Register A = 242 = 362 = 11110010 = 11110010 = 242
 > Register A = 100 = 144 = 01100100 = 01100100 = 100
 > Register A = 132 = 204 = 10000100 = 10000100 = 132
 > Register A =  96 = 140 = 01100000 = 01100000 = 096
 > Register A =  90 = 132 = 01011010 = 01011010 = 090
 > Register A =  81 = 121 = 01010001 = 01010001 = 081
 > Register A =  72 = 110 = 01001000 = 01001000 = 072
 > Register A =  63 = 077 = 00111111 = 00111111 = 063
 > Register A =  54 = 066 = 00110110 = 00110110 = 054
 > Register A =  45 = 055 = 00101101 = 00101101 = 045
 > Register A =  36 = 044 = 00100100 = 00100100 = 036
 > Register A =  27 = 033 = 00011011 = 00011011 = 027
 > Register A =  18 = 022 = 00010010 = 00010010 = 018
 > Register A =   9 = 011 = 00001001 = 00001001 = 009
 > Register A =   0 = 000 = 00000000 = 00000000 = 000
 > Register A = 255 = 377 = 11111111 = 11111111 = 255
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
                                        ; --------------------------------------