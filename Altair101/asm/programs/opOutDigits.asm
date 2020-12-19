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
                lxi sp,512              ; Set stack pointer.
    Start:
                lxi h,StartMsg
                call printStr
                                        ; --------------------------------------
                mvi a,1
                call printNumA
                mvi a,200
                call printNumA
                mvi a,242
                call printNumA
                mvi a,100
                call printNumA
                mvi a,132
                call printNumA
                mvi a,96
                call printNumA
                mvi a,90
                call printNumA
                mvi a,81
                call printNumA
                mvi a,72
                call printNumA
                mvi a,63
                call printNumA
                mvi a,54
                call printNumA
                mvi a,45
                call printNumA
                mvi a,36
                call printNumA
                mvi a,27
                call printNumA
                mvi a,18
                call printNumA
                mvi a,9
                call printNumA
                mvi a,0
                call printNumA
                mvi a,255
                call printNumA
                                        ; --------------------------------------
                call println
                hlt
                jmp Start
                                        ;
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
                jc cpi90                ; Jump if less than.
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
                                        ; ------
    cpi60:
                cpi 60
                jc cpi50               ; Jump if less than.
                sui 60
                mov c,a
                mvi a,'6'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
                                        ; ------
    cpi50:
                cpi 50
                jc cpi40               ; Jump if less than.
                sui 50
                mov c,a
                mvi a,'5'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
                                        ; ------
    cpi40:
                cpi 40
                jc cpi30               ; Jump if less than.
                sui 40
                mov c,a
                mvi a,'4'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
                                        ; ------
    cpi30:
                cpi 30
                jc cpi20               ; Jump if less than.
                sui 30
                mov c,a
                mvi a,'3'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
                                        ; ------
    cpi20:
                cpi 20
                jc cpi10               ; Jump if less than.
                sui 20
                mov c,a
                mvi a,'2'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
    cpi10:
                cpi 10
                jc cpi00               ; Jump if less than.
                sui 10
                mov c,a
                mvi a,'1'
                out PRINT_PORT
                call printRegC
                jmp printShortAret
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
++ Digit = 1
++ Digit = 200
++ Digit = 242
++ Digit = 100
++ Digit = 132
++ Digit = 96
++ Digit = 90
++ Digit = 81
++ Digit = 72
++ Digit = 63
++ Digit = 54
++ Digit = 45
++ Digit = 36
++ Digit = 27
++ Digit = 18
++ Digit = 9
++ Digit = 0
++ Digit = 255
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
                                        ; --------------------------------------
