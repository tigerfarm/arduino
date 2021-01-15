                                        ; --------------------------------------
                                        ; Binary to decimal conversion for printing.
                                        ;
                                        ; 10000100 : 84:204 > immediate: 132 : 132
                                        ; 10000000 : 128
                                        ; 00000100 :   4
                                        ;
                                        ; --------------------------------------
LF	EQU	0AH
CR	EQU	0DH
                                        ; --------------------------------------
                lxi sp,STACK            ; Set stack pointer.
                jmp Start
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
	MVI	M,2                     ; Low order byte of the value to print.
	INR	L                       ; High order byte storage
	MVI	M,1                     ; High order byte of the value to print.
	DCR	L                       ; Pointer back to Low order byte storage
                                        ;
	MVI	B,002                   ; Number of digits, 1 byte per digit, used in BINDEC
	CALL	BINDEC                  ; Convert to decimal digits.
	LXI	D,MSGNUM                ; Set pointer to energy message
	MVI	B,004                   ; Counter to number of digits
	CALL	DIGPRT                  ; Put digits in message
	LXI	H,MSGLAB                ; Set pointer to energy loss msg
	CALL	printStr                ; Print the message
                                        ;
                                        ; --------------------------------------
                call println
                hlt
                jmp Start
                                        ;
; ------------------------------------------------------------------------------
                                ; Binary to decimal processing
DIGPRT:
	MOV	A,M		;Fetch digit
	ADI	'0'		;Form ASCII code
 	INX	H		;Increment digit table pointer
 	XCHG			;Set pointer to message area
	MOV	M,A		;Put digit in message
 	DCX	H		;Move message pointer
	DCR	B		;Last digit in message?
	RZ			;Yes, return
 	XCHG			;No, set pointer to digit table
	JMP	DIGPRT		;Move more digits

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
    StartMsg    db '\r\n+++ Print bytes as binary and decimal number strings.'
                db 0
    TERMB       equ     000h            ; String terminator.
                                        ;
    regA        db 0                    ; A variable for storing register A's value.
    PRINT_PORT  equ     2               ; When using port 2,
                                        ;   if port 3 is disable, then it defaults to 3, the default serial port.
                                        ; --------------------------------------
    NUML:	DB 0                    ; Number low byte storage
    NUMH:	DB 0                    ; Number high byte storage
                                        ;
    MSGLAB:	DB CR,LF                ; Output label tring.
                   ;Decimal: 0258       ; Sample output.
                DB 'Decimal:    '
    MSGNUM:	DB ' '
                DB 0
                                        ;
    DDIG1:      DB 0                    ; Place to store the decimal digits.
    DDIG2:      DB 0
    DDIG3:      DB 0
    DDIG4:      DB 0
    DDIG5:      DB 0
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
Energy loss:   0258
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
                                        ; --------------------------------------
