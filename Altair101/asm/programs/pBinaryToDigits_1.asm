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
                jmp Start
                                        ;
                                        ; --------------------------------------
LF	EQU	0AH
CR	EQU	0DH

	ORG	0030H
	DB	000
	DB	000
	DB	000		;Temporary storage
	DB	000		;Temporary storage
MSGLOE:	DB	CR,LF
  	DB	'Energy loss:      '
MSGLOP:	DB	' '
  	DB	0
	ORG	0060H
DDIG1:	DB	000 ;Digit storage		//60
DDIG2:	DB	000 ;Digit storage		//61
DDIG3:	DB	000 ;Digit storage		//62
DDIG4:	DB	000 ;Digit storage		//63
DDIG5:	DB	000 ;Digit storage		//64

    Start:
                lxi h,StartMsg
                call printStr
                                        ; --------------------------------------
                                        ; Register to be printed.
                                        ;
	MVI	L,0032H		;Pointer to temporary storage
	MVI	M,2		; Low order byte of the value to print.
;++     110:00000000 01101110: 00000010 : 02:002 > immediate:  2 : 2
	INR	L		;Temporary storage
	MVI	M,1		; High order byte of the value to print.
;++     113:00000000 01110001: 00000001 : 01:001 > immediate:  1 : 1
                                ;
	DCR	L		;Pointer to energy loss
	MVI	B,002		;Number of bytes for BINDEC
	CALL	BINDEC		;Convert energy amount
	LXI	D,MSGLOP	;Set pointer to energy message
	MVI	B,004		;Counter to number of digits
	CALL	DIGPRT		;Put digits in message
	LXI	H,MSGLOE	;Set pointer to energy loss msg
	CALL	printStr	;Print the message
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
	LXI	H,0060H		;Set pointer to digit storage
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
	LXI	H,0064H		;Set pointer to 5th digit
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
                                ; ----------------------------------------------
MSG:
	MOV	A,M		;Fetch character
	ANA	A		;End of message?
	RZ			;Yes, return
        out PRINT_PORT          ; Out register A to the serial port.
 	INX	H		;Increment message pointer
	JMP	MSG		;Continue printout
                                ;
; ------------------------------------------------------------------------------
                                        ;
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
Energy loss:   0258
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
                                        ; --------------------------------------