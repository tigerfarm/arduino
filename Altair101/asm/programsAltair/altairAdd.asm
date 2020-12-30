                                    ; --------------------------------------
                                    ; Test ADD, and moving data from addresses to registers and back.
                                    ;
                                    ; Run the program.
                                    ; EXAMINE address 64 (address of Addr1), which equals 6 from: sta Addr1.
                                    ;
                                    ; --------------------------------------
            lxi sp,STACK            ; Set stack pointer.
    Start:
                                    ; --------------------------------------
            mvi a,7                 ; Initialize values to add.
            mvi b,1
            mvi c,2
            mvi d,3
            mvi e,4
            mvi h,5
            mvi l,6
            call printRegisters
                                    ; --------------------------------------
                                    ; Test register adding.
            add b                   ; Add source register to A.
            mov b,a                 ; Move the result back to the source register, to print the sum, later.
            add c
            mov c,a
            add d
            mov d,a
            add e
            mov e,a
            add h
            mov h,a
            add l
            mov l,a
            add a
            call printRegisters
                            ; --------------------------------------
                            ; Add content at address H:L, to register A.
                            ;
            mvi a,6         ; Move # to register A.
            mvi e,'A'       ; Print register A.
            call printByte
            sta Addr1       ; Store register A's content to the address(hb:lb).
            lxi h,Addr1     ; Load an address (value of Addr1) into H:L.
            out 36
            call printHL    ; Print the register values for H:L and the content at that address.
                            ; > Register H:L = 0:64, Data: 6
                            ;
            mvi a,3         ; Move # to register A.
            mvi e,'A'       ; Print register A.
            call printByte
                            ; > Register A =   3 = 003 = 00000011
            add m           ; ADD: Content at address H:L(M) + Register A = 6 + 3 = 9
            mvi e,'A'       ; Print register A.
            call printByte
                            ; > Register A =   9 = 011 = 00001001
                            ;
            sta Addr2       ; Store register A's content to the address(hb:lb).
                            ;
                            ; EXAMINE address 64 (address of Addr1), which equals 6 from: sta Addr1.
                            ; EXAMINE address 03 (address of Addr2), which equals 9 from: sta Addr2.
                            ;
                            ; --------------------------------------
            call println
            hlt             ; The program will halt at each iteration, after the first.
            jmp Start
                            ; --------------------------------------
                                    ;
                                    ; --------------------------------------
    Addr1       equ 024             ; This program is less than 1024 bytes.
    Addr2       ds 2                ; 2 bytes at this location, address 3. The address of Addr2, is 3.
                                    ;
                                    ; --------------------------------------
    printHL:
                sta RegA
                mov a,h
                sta RegH
                mov a,l
                sta RegL
                ;
                lxi h,RegMsg
                call printStr
                mvi a,'H'
                out PRINT_PORT
                mvi a,':'
                out PRINT_PORT
                mvi a,'L'
                out PRINT_PORT
                mvi a,' '
                out PRINT_PORT
                ;
                lda regH
                call printShortA
                mvi a,':'
                out PRINT_PORT
                lda regL
                call printShortA
                ;
                mvi a,' '
                out PRINT_PORT
                mvi a,'='
                out PRINT_PORT
                mvi a,' '
                out PRINT_PORT
                ;
                lda regH
                call printBinaryA
                mvi a,':'
                out PRINT_PORT
                lda regL
                call printBinaryA
                ;
                call println
                lda regH
                mov h,a
                lda regL
                mov l,a
                lda regA
                ret
                                        ; --------------------------------------
    printRegisters:
                push A
                push B
                push D
                push H
                sta RegA
                mov a,b
                sta RegB
                mov a,c
                sta RegC
                mov a,d
                sta RegD
                mov a,e
                sta RegE
                mov a,h
                sta RegH
                mov a,l
                sta RegL
                                        ;
                lda regA
                mvi e,'A'               ; Register to be printed.
                call printByte
                ;
                lda regB
                mvi e,'B'               ; Register to be printed.
                call printByte
                ;
                lda regC
                mvi e,'C'
                call printByte
                ;
                lda regD
                mvi e,'D'
                call printByte
                ;
                lda regE
                mvi e,'E'
                call printByte
                ;
                lda regH
                mvi e,'H'
                call printByte
                ;
                lda regL
                mvi e,'L'
                call printByte
                ;
                pop H
                pop D
                pop B
                pop A
                call println
                ret
                                        ; --------------------------------------
    printByte:                          ; Print register A as a byte string.
                mov b,a                 ; RegB for storing the print byte bits.
                ; out 37                  ; Print regA info: Register A = 248 = 370 = 11111000
                lxi h,RegMsg
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
    printBinaryAit:
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
                jmp printBinaryAit
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
    RegMsg      db      '\r\n > Register '
                db 0
    TERMB       equ     000h            ; String terminator.
                                        ;
    regA        db 0                    ; A variable for storing register A's value.
    PRINT_PORT  equ     2               ; When using port 2,
                                        ;   if port 3 is disable, then it defaults to 3, the default serial port.
                                        ; --------------------------------------
    RegA        db      0
    RegB        db      0
    RegC        db      0
    RegD        db      0
    RegE        db      0
    RegH        db      0
    RegL        db      0
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

 > Register A = 00000111 = 007
 > Register B = 00000001 = 001
 > Register C = 00000010 = 002
 > Register D = 00000011 = 003
 > Register E = 00000100 = 004
 > Register H = 00000101 = 005
 > Register L = 00000110 = 006

 > Register A = 00110110 = 054
 > Register B = 00000111 = 007
 > Register C = 00001001 = 009
 > Register D = 00001100 = 012
 > Register E = 00010000 = 016
 > Register H = 00010101 = 021
 > Register L = 00011011 = 027

 > Register A = 00000110 = 006
 > Register H:L =   0: 69 = 00000000:01000101, Data:   6 = 006 = 00000110
 > Register H:L 000:069 = 00000000:01000101

 > Register A = 00000011 = 003
 > Register A = 00000011 = 003
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
                                        ; --------------------------------------
