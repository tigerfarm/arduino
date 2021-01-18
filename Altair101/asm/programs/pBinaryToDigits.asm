                                        ; --------------------------------------
                                        ; Binary to decimal conversion for printing.
                                        ;
                                        ; This program works from 0 to 9999.
                                        ;
                                        ; --------------------------------------
                                        ;
                lxi sp,STACK            ; Set stack pointer.
                                        ;
                                        ; --------------------------------------
                                        ; 16 bit number to convert to decimal digits.
    NUML:	DB 0                    ; Number low byte
    NUMH:	DB 0                    ; Number high byte
                                        ;
                                        ; Place to store the decimal digits.
                                        ; Value after running, with output: 0258
    DDIG1:      DB 0                    ;   8
    DDIG2:      DB 0                    ;   5
    DDIG3:      DB 0                    ;   2
    DDIG4:      DB 0                    ;   0
    DDIG5:      DB 0                    ;   0
                                        ;
                                        ; --------------------------------------
    Start:
                lxi h,StartMsg
                call printStr
                                        ; --------------------------------------
                                        ; Convert a 16 bit binary value into a decimal value.
                                        ;
                                        ; Load the the 16 bit binary value into memory.
	shld	NUML                    ; Pointer to Low order byte storage
;++      13:00000000 00001101: 00000010 : 02:002 > immediate:  2 : 2
	MVI	M,2                     ; Low order byte of the value to print.
	INR	L                       ; High order byte storage
;++      16:00000000 00010000: 00000001 : 01:001 > immediate:  1 : 1
;	MVI	M,1                     ; High order byte of the value to print.
	MVI	M,31                    ; High order byte of the value to print.
                                        ;
                                        ; Get leading characters, if greater the 31.
                                        ; 31 ... + Digits: 7938 (7936+2)
                                        ; 36 ... + Digits: 9218
                                        ;
                                        ; 
	DCR	L                       ; Pointer back to Low order byte storage
	MVI	B,002                   ; Number of digits, 1 byte per digit, used in BINDEC
	CALL	BINDEC                  ; Convert to decimal digits.
                                        ;
                                        ; --------------------------------------
                                        ; Print the digits
                call println
                lxi h,DigitMsg
                call printStr
                                        ;
                mvi c,5                 ; Count of digits to print.
                LXI h,DDIG5             ; Pointer to last digit (5th) in storage
    PrintDigits:
                mov a,m
                call printDigitA
                dcx h                   ; Decrement H:L register pair.
                dcr c
                jnz PrintDigits
                                        ;
                                        ; --------------------------------------
                call println
                hlt
                jmp Start
                                        ;
; ------------------------------------------------------------------------------
                                ; Binary to decimal processing
BINDEC:
	XCHG			;Save binary pointer
	LXI	H,DDIG1		;Set pointer to digit storage
	MOV	M,H		;Clear digit table
	INR	L
	MOV	M,H
	INR	L
	MOV	M,H
	INR	L
	MOV	M,H
	INR	L
	MOV	M,H
	XCHG			;Set pointer to binary number
	MOV	E,M		;Fetch least significant half
	DCR	B		;Single precision?
	JZ	BNDC		;Yes, most significant half = 0
	INR	L		;No, advance pointer
	MOV	D,M		;Fetch most significant half
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
                sta regA
                mvi b,'0'
                add b
                out PRINT_PORT
                lda regA
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
                                        ; --------------------------------------
                                        ; Declarations
                                        ;
    StartMsg    db '\r\n+++ Print bytes as binary and decimal number strings.'
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
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ; Successful run:
+ Download complete.
?- + r, RUN.
?- + runProcessor()

+++ Print bytes as binary and decimal number strings.
Decimal: 0258
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
                                        ; --------------------------------------
