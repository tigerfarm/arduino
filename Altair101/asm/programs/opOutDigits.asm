                                        ; --------------------------------------
                                        ; Binary to decimal conversion for printing.
                                        ;
                                        ; dec_value = 1*(2^2) + 1*(2^1) + 1*(2^0) = 7
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
                call printNumA
                call printByteA
                mvi a,200
                call printNumA
                call printByteA
                mvi a,242
                call printNumA
                call printByteA
                mvi a,100
                call printNumA
                call printByteA
                mvi a,132
                call printNumA
                call printByteA
                mvi a,96
                call printNumA
                call printByteA
                mvi a,90
                call printNumA
                call printByteA
                mvi a,81
                call printNumA
                call printByteA
                mvi a,72
                call printNumA
                call printByteA
                mvi a,63
                call printNumA
                call printByteA
                mvi a,54
                call printNumA
                call printByteA
                mvi a,45
                call printNumA
                call printByteA
                mvi a,36
                call printNumA
                call printByteA
                mvi a,27
                call printNumA
                call printByteA
                mvi a,18
                call printNumA
                call printByteA
                mvi a,9
                call printNumA
                call printByteA
                mvi a,0
                call printNumA
                call printByteA
                mvi a,255
                call printNumA
                call printByteA
                                        ; --------------------------------------
                call println
                hlt
                jmp Start
                                        ;
                                        ; --------------------------------------
    printByteA:                         ; Print register A as a number.
                mov b,a
                ; out 37                  ; Register A = 248 = 370 = 11111000
                mvi a,' '
                out PRINT_PORT
                mvi a,'='
                out PRINT_PORT
                mvi a,' '
                out PRINT_PORT
                                        ;
                                        ; ------
                mov a,b
                ani 128                 ; AND 10000000 with register A
                cpi 0
                jnz printZero8-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero7
    printZero8-1:
                mvi a,'1'
                out PRINT_PORT
    printZero7:
                mov a,b
                ani 64                  ; AND 01000000 with register A
                cpi 0
                jnz printZero7-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero6
    printZero7-1:
                mvi a,'1'
                out PRINT_PORT
    printZero6:
                mov a,b
                ani 32                  ; AND 00100000 with register A
                cpi 0
                jnz printZero6-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero5
    printZero6-1:
                mvi a,'1'
                out PRINT_PORT
    printZero5:
                mov a,b
                ani 16                  ; AND 00010000 with register A
                cpi 0
                jnz printZero5-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero4
    printZero5-1:
                mvi a,'1'
                out PRINT_PORT
    printZero4:
                mov a,b
                ani 8                   ; AND 00001000 with register A
                cpi 0
                jnz printZero4-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero3
    printZero4-1:
                mvi a,'1'
                out PRINT_PORT
    printZero3:
                mov a,b
                ani 4                   ; AND 00000100 with register A
                cpi 0
                jnz printZero3-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero2
    printZero3-1:
                mvi a,'1'
                out PRINT_PORT
    printZero2:
                mov a,b
                ani 2                   ; AND 00000010 with register A
                cpi 0
                jnz printZero2-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero1
    printZero2-1:
                mvi a,'1'
                out PRINT_PORT
    printZero1:
                mov a,b
                ani 1                   ; AND 00000001 with register A
                cpi 0
                jnz printZero1-1
                mvi a,'0'
                out PRINT_PORT
                ret
    printZero1-1:
                mvi a,'1'
                out PRINT_PORT
                ret
                                        ; --------------------------------------
                                        ; --------------------------------------
    printNumA:                          ; Print register A as a number.
                lxi h,DigitMsg
                call printStr
                call printShortA
                ret
                                        ;
                                        ; --------------------------------------
                                        ;
    printShortA:                        ; Print register A digits. The number from 0 to 255.
                sta regA                ; Retain register A value.
                                        ; ------
                cpi 200
                jc cpi100               ; Jump if less than.
                jnz print2xx
                mvi a,'2'
                out PRINT_PORT
                mvi a,'0'
                out PRINT_PORT
                mvi a,'0'
                out PRINT_PORT
                jmp printShortAret
    print2xx:
                sui 200                 ; 242-200 = 42 register A.
                mov c,a
                mvi a,'2'
                out PRINT_PORT
                mov a,c
                jmp cpi90
    cpi100:
                cpi 100
                jc cpi90a                ; Jump if less than.
                jnz print1xx
                mvi a,'1'
                out PRINT_PORT
                mvi a,'0'
                out PRINT_PORT
                mvi a,'0'
                out PRINT_PORT
                jmp printShortAret
    print1xx:
                sui 100                 ; 136-100 = 36 register A.
                mov c,a
                mvi a,'1'
                out PRINT_PORT
                mov a,c
                jmp cpi90
                                        ; ------
    cpi90a:                             ; Pad the space
                mov c,a
                mvi a,'0'
                out PRINT_PORT
                mov a,c
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
    StartMsg    db      '\r\n+++ Print digits.'
    DigitMsg    db      '\r\n++ Digit = '
    TERMB       equ     0ffh            ; String terminator.
                                        ;
    regA        db 0                    ; A variable for storing register A's value.
    PRINT_PORT  equ     2               ; When using port 2,
                                        ;   if port 3 is disable, then it defaults to 3, the default serial port.
                                        ; --------------------------------------
                end
                                        ; --------------------------------------
                                        ; Successful run:
+ Download complete.
?- + r, RUN.
?- + runProcessor()

+++ Print digits.
++ Digit = 001 = 00000001
++ Digit = 200 = 11001000
++ Digit = 242 = 11110010
++ Digit = 100 = 01100100
++ Digit = 132 = 10000100
++ Digit = 096 = 01100000
++ Digit = 090 = 01011010
++ Digit = 081 = 01010001
++ Digit = 072 = 01001000
++ Digit = 063 = 00111111
++ Digit = 054 = 00110110
++ Digit = 045 = 00101101
++ Digit = 036 = 00100100
++ Digit = 027 = 00011011
++ Digit = 018 = 00010010
++ Digit = 009 = 00001001
++ Digit = 000 = 00000000
++ Digit = 255 = 11111111
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
                                        ; --------------------------------------