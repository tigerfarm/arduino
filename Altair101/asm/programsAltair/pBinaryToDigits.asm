; ------------------------------------------------------------------------------
; Conversion from a binary number to decimal digits for printing.
;
; Single precision,
;   An 8 bit number (1 byte)
; Or double precision,
;   A 16 bit number (2 byte)
;
; Range, from:
;   00000000b             > + Digits: 00000
; To:
;   11111111b + 11111111b > + Digits: 65535
;
; The program was written by Stacy David.
; The conversion routine is from the 1976 Galaxy80 program,
;   written by Robert Fidley (C) COPYRIGHT 1976 Scelbi Computer Consulting, Inc.
;
; ------------------------------------------------------------------------------
                                        ; --------------------------------------
                                        ;
                lxi     sp,STACK        ; Set stack pointer.
    Start:
                lxi     h,StartMsg
                call    printStr
                                        ; --------------------------------------
                                        ; Load the 16 bit binary value into memory.
                                        ;
                lxi     h,NUML          ; Low order byte memory address
                mvi     M,00000010b     ; Low order byte of the value to convert.
                inr     L               ; High order byte memory address
                mvi     M,00000001b     ; High order byte of the value to convert.
                                        ;
                lxi     h,BinaryMsg     ; Print the 16 bit binary number bytes.
                call    printStr
                lda     NUMH
                call    printBinaryA
                mvi     a,':'
                out     PRINT_PORT
                lda     NUML
                call    printBinaryA
                                        ; --------------------------------------
                                        ; Convert a 16 bit binary value into a decimal value.
                                        ;
                lxi     h,NUML          ; Load the low order byte memory address into HL(m).
                mvi     B,2             ; Set single(1) or double(2) precision. An 8 bit or 16 bit number.
                call    BINDEC          ; Convert to decimal digits.
                                        ;
                                        ; --------------------------------------
                                        ; Print the digits
                lxi     h,DigitMsg      ;
                call    printStr
                                        ;
                mvi     c,5             ; Counter of number of digits to print.
                lxi     h,DDIG5         ; Pointer to last digit(5th digit) memory address.
    PrintDigits:
                mov     a,m
                call    printDigitA
                dcx     h               ; Decrement H:L register pair.
                dcr     c
                jnz     PrintDigits
                                        ;
                                        ; --------------------------------------
                call println
                hlt
                jmp Start
                                        ;
; ------------------------------------------------------------------------------
; Binary to decimal processing
                                        ;
    BINDEC:
                XCHG			; Save binary pointer from HL to DE.
                LXI     H,DDIG1		; Set pointer to the first digit memory address.
                MOV     M,H		; Clear digit array.
                INR     L
                MOV     M,H
                INR     L
                MOV     M,H
                INR     L
                MOV     M,H
                INR     L
                MOV     M,H
                XCHG			; Restore binary number address to HL.
                                        ;
                MOV     E,M		;Fetch least significant half
                ;
                DCR     B		; Single(B=1) or double(B=2) precision
                JZ      BNDC		;Yes, most significant half = 0
                INR     L		;No, advance pointer
                MOV     D,M		;Fetch most significant half
    BNDC:
                LXI	H,DDIG5		;Set pointer to 5th digit
                LXI	B,10000		;BC = 10000
                CALL	BD		;Calculate 5th digit
                DCR	L		;Set pointer to 4th digit
                LXI	B,1000		;BC = 1000
                CALL	BD		;Calculate 4th digit
                DCR	L		;Set pointer to 3rd digit
                LXI	B,100		;BC = 100
                CALL	BD		;Calculate 3rd digit
                DCR	L		;Set pointer to 2nd digit
                MVI	C,10		;Least significant half of 10
                CALL	BD		;Calculate 2nd digit
                DCR	L		;Set pointer to 1st digit
                MOV	M,E		;Store 1st digit
                RET			;Return to calling program
    BD:
                INR	M		;Increment and save new digit
                MOV	A,E		;Fetch least significant half
                SUB	C		;Subtract least significant constant
                MOV	E,A		;Save least significant half
                MOV	A,D		;Fetch most significant half
                SBB	B		;Subtract most significant constant
                MOV	D,A		;Save most significant half
                JNC	BD		;If greater than 0, continue calculation.
                MOV	A,E		;Else, restore bin. & dec. value
                ADD	C		;Add least significant constant
                MOV	E,A		;Save least significant half
                MOV	A,D		;Fetch most significant half
                ADC	B		;Add most significant constant
                MOV	D,A		;Save most significant half
                DCR	M		;Decrement digit stored
                RET			;Return
                                        ;
; ------------------------------------------------------------------------------
                                        ; --------------------------------------
                                        ; Print regA as a digit.
    printDigitA:
                sta     regA
                mvi     b,'0'
                add     b
                out     PRINT_PORT
                lda     regA
                ret
                                        ; --------------------------------------
                                        ; Print register A as a binary string.
    printBinaryA:
                sta     regA            ; Save, restore on return.
                push    b
                mov     b,a
                mvi     c,0             ; For counting the printed bits.
                                        ; ------
    printBinaryBit:
                ani     128             ; AND 10000000 with register A
                cpi     0
                jnz     printBit1
                mvi     a,'0'
                out     PRINT_PORT
                jmp     nextBit
    printBit1:
                mvi     a,'1'
                out     PRINT_PORT
    nextBit:
                mov     a,c
                cpi     7
                jz      printedBits     ; 8 bits printed.
                inr     c
                mov     a,b
                rlc
                mov     b,a
                jmp     printBinaryBit
    printedBits:
                pop     b               ; Restore
                lda     regA
                ret
                                        ; --------------------------------------
                                        ; Print a string.
    printStr:
                sta     regA
    printStrNext:
                mov     a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi     TERMB           ; Compare to see if it's the string terminate byte.
                jz      sPrintDone
                out     PRINT_PORT      ; Out register A to the serial port.
                inx     h               ; Increment H:L register pair.
                jmp     printStrNext
    sPrintDone:
                lda     regA
                ret
                                        ; --------------------------------------
    println:
                lda     regA
                mvi     a,'\r'          ; Print newline, print CR and NL characters.
                out     PRINT_PORT
                mvi     a,'\n'
                out     PRINT_PORT
                lda     regA
                ret
; ------------------------------------------------------------------------------
; Declarations
                                        ;
                                        ; 16 bit number which is converted to decimal digits.
    NUML:	DB 0                    ; Number low byte
    NUMH:	DB 0                    ; Number high byte
                                        ;
                                        ; Array of memory bytes to store the decimal digits.
                                        ; Sample output for the digits: 00258
    DDIG1:      DB 0                    ;   8
    DDIG2:      DB 0                    ;   5
    DDIG3:      DB 0                    ;   2
    DDIG4:      DB 0                    ;   0
    DDIG5:      DB 0                    ;   0
                                        ;
                                        ; --------------------------------------
    StartMsg:   db '\r\n+++ Print a binary number as a decimal number string.'
                db 0
    BinaryMsg:	db '\r\n+ Binary: '
                db 0
    DigitMsg:	db '\r\n+ Digits: '
                db 0
                                        ;
    TERMB       equ     000h            ; String terminator.
                                        ;
    regA        db 0                    ; A variable for storing register A's value.
    PRINT_PORT  equ     2               ; When using port 2,
                                        ;   if port 3 is disable, then it defaults to 3, the default serial port.
                                        ;
                                        ; --------------------------------------
                ds 32                   ; Stack space.
    STACK:      equ $
                                        ; --------------------------------------
                end
; ------------------------------------------------------------------------------
                                        ; Successful run:
                                        ;
+++ Print a binary number as a decimal number string.
+ Binary: 00000001:00000010
+ Digits: 00258
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
; ------------------------------------------------------------------------------
