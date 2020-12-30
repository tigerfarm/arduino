                                        ; --------------------------------------
                                        ; Binary to decimal conversion for printing.
                                        ;
                                        ; 10000100 : 84:204 > immediate: 132 : 132
                                        ; 10000000 : 128
                                        ; 00000100 :   4
                                        ;
                                        ; --------------------------------------
                                        ;
                lxi sp,1024             ; Set stack pointer.
    Start:
                lxi h,StartMsg
                call printStr
                                        ; --------------------------------------
                mvi a,1
                call printByteA
                mvi a,200
                call printByteA
                mvi a,242
                call printByteA
                mvi a,100
                call printByteA
                mvi a,132
                call printByteA
                mvi a,96
                call printByteA
                mvi a,90
                call printByteA
                mvi a,81
                call printByteA
                mvi a,72
                call printByteA
                mvi a,63
                call printByteA
                mvi a,54
                call printByteA
                mvi a,45
                ;call printNumA
                call printByteA
                mvi a,36
                ;call printNumA
                call printByteA
                mvi a,27
                ;call printNumA
                call printByteA
                mvi a,18
                call printByteA
                mvi a,9
                call printByteA
                mvi a,0
                call printByteA
                mvi a,255
                call printByteA
                                        ; --------------------------------------
                call println
                hlt
                jmp Start
                                        ;
                                        ; --------------------------------------
                                        ; --------------------------------------
    printByteA:                         ; Print register A as a byte string.
                mov b,a                 ; RegB for storing the print byte bits.
                out 37                  ; Print regA info: Register A = 248 = 370 = 11111000
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
    printBinaryA:                       ; Print register A as a binary string.
                sta regA                ; Save
                mov a,b
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
    printShortA:                        ; Print register A digits. The number from 0 to 255.
                sta regA                ; Retain register A value.
                cpi 100
                jc printX0a
                mvi c,'2'               ; RegC for counting the printed bits.
                mvi b,200               ; RegB is the hundreds counter.
                                        ;
                                        ; ------------------------
    printHundreds:
                cmp b
                jc cmpX00               ; Jump if less than.
                jnz printX00
                mov a,c                 ; Print hundred's digit
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
                sub b                   ; 242-200 = 42 register A.
                jmp cpi90
    cmpX00:
                mov d,a
                                        ;
                mvi a,'0'               ; Hundreds are zero, move to tens.
                dcr c
                cmp c
                jz printX0
                                        ;
                mov a,b
                sui 100
                mov b,a
                mov a,d
                jmp printHundreds
                                        ; ------------------------
    printX0:                            ; Print tens digit.
                mov a,d
                jmp cpi90
    printX0a:
                mov d,a
                mvi a,'0'               ; Hundreds are zero, move to tens.
                out PRINT_PORT
                mov a,d
                cpi 10
                jc cpi00a
    cpi90:
                cpi 90
                jc cpi80                ; Jump if less than.
                sui 90                  ; Subtract immediate number from register A.
                mov c,a                 ; Value example, c = 6, from 96.
                mvi a,'9'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi80:
                cpi 80
                jc cpi70
                sui 80
                mov c,a
                mvi a,'8'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi70:
                cpi 70
                jc cpi60
                sui 70
                mov c,a
                mvi a,'7'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi60:
                cpi 60
                jc cpi50
                sui 60
                mov c,a
                mvi a,'6'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi50:
                cpi 50
                jc cpi40
                sui 50
                mov c,a
                mvi a,'5'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi40:
                cpi 40
                jc cpi30
                sui 40
                mov c,a
                mvi a,'4'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi30:
                cpi 30
                jc cpi20
                sui 30
                mov c,a
                mvi a,'3'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi20:
                cpi 20
                jc cpi10
                sui 20
                mov c,a
                mvi a,'2'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi10:
                cpi 10
                jc cpi00a
                sui 10
                mov c,a
                mvi a,'1'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi00a:                             ; Pad the space
                mov c,a
                mvi a,'0'
                out PRINT_PORT
                mov a,c
    cpi00:
                call printRegA
                jmp printShortAret
                                        ; ------
    printShortAret:
                lda regA                ; Restore register A value.
                ret
                                        ; --------------------------------------
    printRegA:
                sta regA
                mvi b,'0'
                add b
                out PRINT_PORT
                lda regA
                ret
                                        ; ------
    printRegC:                          ; Print single digit.
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
    StartMsg    db      '\r\n+++ Print bytes as binary number strings and as decimal number strings.'
                db 0
    DigitMsg    db      '\r\n++ Number, '
                db 0
    TERMB       equ     000h            ; String terminator.
                                        ;
    regA        db 0                    ; A variable for storing register A's value.
    PRINT_PORT  equ     2               ; When using port 2,
                                        ;   if port 3 is disable, then it defaults to 3, the default serial port.
                                        ; --------------------------------------
                end
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ; Successful run:
+ Download complete.
?- + r, RUN.
?- + runProcessor()

+++ Print bytes as decimal number strings and as binary number strings.
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