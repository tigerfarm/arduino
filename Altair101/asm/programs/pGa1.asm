; ------------------------------------------------
;
; Stacy, be a challenge to get this to compile using my assembler.
;
; Original site:
;   https://github.com/deltecent/scelbi-galaxy
;
; ------------------------------------------------
;
; +----------------------------------+
; |       SCELBI'S GALAXY GAME       |
; |           FOR THE 8080           |
; |                                  |
; |             AUTHOR:              |
; |          ROBERT FIDLEY           |
; |                                  |
; |       (C) COPYRIGHT 1976         |
; |                                  |
; | Scelbi Computer Consulting, Inc. |
; |   1322 Rear - Boston Post Road   |
; |        Milford, CT 06460         |
; |                                  |
; |       ALL RIGHTS RESERVED        |
; |                                  |
; +----------------------------------+
;
; **************************************************
;
; Along time ago in a magazine far, far away....
;
; In 1976 Scelbi Computer Consulting, Inc. published
; "Scelbi's Galaxy Game for the '8008/8080'" written
; by Robert Findley.
;
; The publication included the complete source code
; listings for Intel 8008 and 8080, flow charts, and
; detailed documentation of how the game functions.
;
; The source listings for GALAXY are for Intel's
; original 8008 mnemonics published in 1972.
;
; Examples:
;
; Old	New
; ---	-------
; JFC	JNC
; CAL	CALL
; RFZ	RNZ
; LMs	MOV M,s
; ADM	ADD M
;
; ******* CAPTAIN'S LOG ***************************
;
; -- Stardate: Aug 25, 2020 -----------------------
; -- Patrick Linstruth <patrick@deltecent.com -----
;
; This version of GALAXY is Findley's original 8080
; program listing modified for Digital Research's
; 8080 Assember (ASM.COM) and ZASM by Günter Woigk.
; The console I/O routines are for the MITS 88-2SIO
; serial adapter.
;
; This version is designed to be loaded at 0000H
; and executed at "GALAXY" which is currently
; located at 09A3H.
; -------------------------------------------------
;
; **************************************************
;
; Captain your own crusading starship against the
; logic of your "8008" or "8080". You must destroy
; a random number of alien ships without running
; out of stardates, out of fuel, out of ammunition,
; or out of the galaxy. Plan your mission. How much
; fuel is used for your warp factor? Don't run
; into roaming stars that can damage your ship.
; Suddenly. Condition Red. Alien in sight.
; How big is he? Fire a phasor or torpedo. He's
; damaged or destroyed. But, you've used valuable
; fuel. That's just the beginning. A new game
; every time.
;
; **************************************************

LF	EQU	0AH
CR	EQU	0DH

	ORG	0000H

	DB	2 		;Course 1.0
	DB	0
	DB	2 		;Course 1.5
	DB	0FFH
	DB	2 		;Course 2.0
	DB	0FEH
	DB	1 		;Course 2.5
	DB	0FEH
	DB	000 		;Course 3.0
	DB	0FEH
	DB	0FFH		;Course 3.5
	DB	0FEH
	DB	0FEH		;Course 4.0
	DB	0FEH
	DB	0FEH		;Course 4.5
	DB	0FFH
	DB	0FEH		;Course 5.0
	DB	000
	DB	0FEH		;Course 5.5
	DB	001
	DB	0FEH		;Course 6.0
	DB	002
	DB	0FFH		;Course 6.5
	DB	002
	DB	000 		;Course 7.0
	DB	002
	DB	001 		;Course 7.5
	DB	002
	DB	002 		;Course 8.0
	DB	002
	DB	002 		;Course 8.5
	DB	001

	ORG	0028H

	DB	000		;Register storage
	DB	000 		;Register storage
	DB	000 		;Register storage
	DB	000 		;Temporary storage

	ORG	0030H

	DB	000		;Crossing flag
	DB	000		;Crossing indicator
	DB	000		;Temporary storage
	DB	000		;Tern porary storage

	ORG	0040H

ALNMSK:	EQU	00110000b	;Num. aliens mask
STNMSK:	EQU	00001000b	;Space station mask
STRMSK:	EQU	00000111b	;Num. stars mask
ROWMSK:	EQU	00111000b	;Row mask
COLMSK:	EQU	00000111b	;Column mask
TORMSK:	EQU	00001111b	;Num. torpedos mask

	DB	000 ;Random number		//40
	DB	000 ;Ran. num. constant		//41
DQUAD:	DB	000 ;quadrant contents		//42
DSHPS:	DB	000 ;Sec. loco of S. ship	//43
DSTRS:	DB	000 ;Sector loco of star	//44
	DB	000 ;Sector loco of star	//45
	DB	000 ;Sector loco of star	//46
	DB	000 ;Sector loco of star	//47
	DB	000 ;Sector loco of star	//48
	DB	000 ;Sector loco of star	//49
	DB	000 ;Sector loco of star	//4A
DSSTS:	DB	000 ;Sec. loco of space st.	//4B
DAS1S:	DB	000 ;S. loc. of A.S. No.1	//4C
DAS2S:	DB	000 ;S. loc. of A.S. No.2	//4D
DAS3S:	DB	000 ;S. loc. of A.S. No.3	//4E
DMELS:	DB	000 ;Main nrgy L.S. half	//4F
DMEMS:	DB	000 ;Main nrgy M.S. ha lf	//50
DSELS:	DB	000 ;Shld nrgy L.S. half	//51
DSEMS:	DB	000 ;Shld nrgy M.S. half	//52
DAS1LS: DB	000 ;A.S. 1 nrgy L.S. half	//53
DAS1MS: DB	000 ;A.S. 1 nrgy MS half	//54
DAS2LS: DB	000 ;A.S. 2 nrgy L.S. half	//55
DAS2MS: DB	000 ;A.S. 2 nrgy MS half	//56
DAS3LS: DB	000 ;A.S. 3 nrgy L.S. half	//57
DAS3MS: DB	000 ;A.S. 3 nrgy MS half	//58
DSHPQ:	DB	000 ;Quad. loc. of S. ship	//59
DNTOR:	DB	000 ;Number torpedoes		//5A
DNSST:	DB	000 ;Num. space stations	//5B
DNALS:	DB	000 ;Num. alien ships		//5C
DNSTD:	DB	000 ;Num. stardates		//5D
DTMP1:	DB	000 ;Temporary storage		//5E
DTMP2:	DB	000 ;Temporary storage		//5F
DDIG1:	DB	000 ;Digit storage		//60
DDIG2:	DB	000 ;Digit storage		//61
DDIG3:	DB	000 ;Digit storage		//62
DDIG4:	DB	000 ;Digit storage		//63
DDIG5:	DB	000 ;Digit storage		//64

	ORG	0080H

	DB	CR,LF,'1',' ',' ',' ',' ',' '
	DB	'1',' ',' ',' ',' ',' ','1',' '
	DB	' ',' ',' ',' ','1',' ',' ',' '
	DB	' ',' ','1',' ',' ',' ',' ',' '
	DB	'1',' ',' ',' ',' ',' ','1',' '
	DB	' ',' ',' ',' ','1',' ',' ',' '
	DB	' ',' ','1'

;  through 377 reserved for Galaxy content table

	ORG	0100H	; Next page, Decimal = 256

MSGDYW:	DB	CR,LF
  	DB	'DO YOU WANT TO GO ON A SPACE VOYAGE? '
  	DB	0
MSGYJD:	DB	CR,LF
  	DB	'YOU MUST DESTROY  '
MSGSPS:	DB	'  ALIEN SHIPS IN  '
MSGDTS:	DB	'  STARDATES WITH '
MSGSSS:	DB	'  SPACE STATIONS'
  	DB	0
MSG123:	DB	CR,LF
  	DB	' -1--2--3--4-'
  	DB	'-5--6--7--8-'
  	DB	0
MSGSTDT:
	DB	CR,LF
MSGSTDT1:
	DB	'0'
MSGSTDT2:
	DB	'                        '
  	DB	0
MSGSTDT3:
  	DB	' STARDATE  300'
MSGSDP:	DB	'0'
  	DB	0
MSGCND:	DB	' CONDITION '
MSGGRN:	DB	'GREEN'
  	DB	0
MSGQAD:	DB	' QUADRANT  '
MSGPQD:	DB	'   '
  	DB	0
MSGSCT:	DB	' SECTOR    '
MSGSC1:	DB	'   '
  	DB	0
MSGENR:	DB	' ENERGY       '
MSGENP:	DB	' '
  	DB	0
MSGTRP:	DB	' TORPEDOES  '
MSGTPP:	DB	' '
  	DB	0
MSGSHD:	DB	' SHIELDS      '
MSGSHP:	DB	' '
  	DB	0
MSGCMD:	DB	CR,LF
  	DB	'COMMAND?'
  	DB	0
MSGCRS:	DB	CR,LF
  	DB	'COURSE (1-8.5)? '
  	DB	0
MSGWRP:	DB	CR,LF
  	DB	'WARP FACTOR (0.1-7.7)? '
  	DB	0
MSGLRS:	DB	CR,LF
  	DB	'L.R. SCAN FOR'
  	DB	0
MSGMSF:	DB	CR,LF
  	DB	'MISSION FAILED, YOU HAVE RUN OUT	OF STARDATES'
  	DB	0
MSGKAB:	DB	CR,LF
  	DB	'KA-BOOM, YOU CRASHED INTO A STAR. YOUR SHIP IS DESTROYED'
  	DB	0
MSGYMO:	DB	CR,LF
  	DB	'YOU MOVED OUT	OF THE GALAXY, YOUR SHIP IS LOST..LOST'
  	DB	0
MSGLOE:	DB	CR,LF
  	DB	'LOSS OF ENERGY    '
MSGLOP:	DB	' '
  	DB	0
MSGDSE:	DB	CR,LF
  	DB	'DANGER-SHIELD ENERGY 000'
  	DB	0
MSGSET:	DB	CR,LF
  	DB	'SHIELD ENERGY TRANSFER = '
  	DB	0
MSGNEE:	DB	CR,LF
  	DB	'NOT ENOUGH ENERGY'
  	DB	0
MSGTTY:	DB	CR,LF
  	DB	'TORPEDO TRAJECTORY(1-8.5) : '
  	DB	0
MSGASD:	DB	CR,LF
 	DB	'ALIEN SHIP DESTROYED'
  	DB	0
MSGYMA:	DB	CR,LF
  	DB	'YOU MISSED! ALIEN SHIP RETALIATES'
  	DB	0
MSGSSD:	DB	CR,LF
  	DB	'SPACE STATION '
MSGDES:	DB	'DESTROYED'
  	DB	0
MSGCYH:	DB	CR,LF
  	DB	'CONGRATULATIONS, YOU HAVE ELIMINATED ALL OF THE ALIEN SHIPS'
  	DB	0
MSGTRG:	DB	CR,LF
  	DB	'TRACKING: '
MSGTRK:	DB	'   '
  	DB	0
MSGGDY:	DB	CR,LF
  	DB	'GALAXY DISPLAY'
  	DB	0
MSGPEF:	DB	CR,LF
  	DB	'PHASOR ENERGY TO FIRE = '
  	DB	0
MSGASF:	DB	CR,LF
  	DB	'ALIEN SHIP AT SECTOR '
MSGSEC:	DB	' , : '
  	DB	0
MSGEGY:	DB	'ENERGY =    '
MSGDEY:	DB	' '
  	DB	0
MSGNAS:	DB	CR,LF
  	DB	'NO ALIEN SHIPS! WASTED SHOT'
MSGZRO:	DB	0
MSGNEL:	DB	CR,LF
  	DB	'ABANDON SHIP! NO ENERGY LEFT'
  	DB	0
MSGNTS:	DB	CR,LF
  	DB	'NO TORPEDOES'
  	DB	0
MSG111:	DB	CR,LF
  	DB	'1 '
MSG11A:	DB	'    1 '
MSG11B:	DB	'    1 '
MSG11C:	DB	'    1'
  	DB	0
MSGLST:	DB	CR,LF
  	DB	'LAST'
  	DB	0
MSGCHK:	DB	CR,LF
  	DB	'CHICKEN!'
  	DB	0

	ORG	0500H           ; Decimal = 1280

MSG:
	MOV	A,M		;Fetch character
	ANA	A		;End of message?
	RZ			;Yes, return
 	CALL	PRINT		;No, print character
 	INX	H		;Increment message pointer
	JMP	MSG		;Continue printout
RN:
	LXI	H,0040H		;Set random number pointer
	MOV	A,M		;The random number
	MOV	B,A		;Is generated by performing
	RLC			;The series of arithmetic
	XRA	B		;Operations presented
	RRC
	INR	L
	INR	M
	ADD	M
	DCR	L
	MOV	M,A		;Save random number
	RET
SSPLS:
	MVI	E,0F7H		;Mask to delete space station
	JMP	PLS		;Delete excess space station
SSMNS:
	MVI	E,STNMSK	;Mask to add space station
	JMP	MNS		;Add a space station
ASPLS:
	MVI	E,0CFH		;Mask to delete alien ship
PLS:
	CALL	RN		;Fetch random low address
	ORI	0C0H		;Set to point to galaxy
	MOV	L,A		;Set up galaxy pointer
	MOV	A,E		;Load mask into accumulator
	ANA	M		;Delete from galaxy
	MOV	M,A		;Put back in galaxy
	JMP	GLXCK		;Check galaxy again
ASMNS:
	MVI	E,010H		;Mask to add alien ship
MNS:
	CALL	RN		;Fetch random low address
	ORI	0C0H		;Set to point to galaxy
	MOV	L,A		;Set up galaxy pointer
	MOV	A,E		;Load mask into accumulator
	ORA	M		;Add to galaxy
	MOV	M,A		;Put back in galaxy
	JMP	GLXCK		;Check galaxy again
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
	MVI	C,10		;Least signficant half of 10
	CALL	BD		;Calculate 2nd digit
	DCR	L		;Set pointer to 1st digit
	MOV	M,E		;Store 1st digit
	RET			;Return to calling program
BD:
	INR	M		;Increment and save new digit
	MOV	A,E		;Fetch least significant half
	SUB	C		;Subract least signif. constant
	MOV	E,A		;Save least significant half
	MOV	A,D		;Fetch most significant half
	SBB	B		;Subtract most signif. constant
	MOV	D,A		;Save most significant half
	JNC	BD		;If greater than 0, continue calc.
	MOV	A,E		;Else, restore bin. & dec. value
	ADD	C		;Add least signficant constant
	MOV	E,A		;Save least significant half
	MOV	A,D		;Fetch most significant half
	ADC	B		;Add most significant constant
	MOV	D,A		;Save most significant half
	DCR	M		;Decrement digit stored
	RET			;Return
LOAD:
	MVI	L,04FH		;Space ship energy storage
	MVI	M,088H		;Least signif. half of 5000 units
	INR	L
	MVI	M,00BH		;Most signif. have of 5000 units
	INR	L		;Advance to shield energy
	MOV	M,H		;Initial shield energy = 0
	INR	L
	MOV	M,H		;Most signif. half of shield nrgy
	MVI	L,05AH		;Set pointer torpedo storage
	MVI	M,10		;Initial amount = 10 torpedoes
	RET
ROTR4:
	RRC
ROTR3:
	RRC
	RRC
	RRC
	RET
LOCSET:
	CALL	RN		;Fetch random location
	ANI	03FH		;Mask off most signficant bits
	MOV	B,A		;Save location
	CALL	MATCH		;New location match others?
	JZ	LOCSET		;Yes, find new location
	MOV	L,E		;Set pointer to storage location
	MOV	M,B		;Save indicated loc. in table
	INR	E		;Advance table pointer
	DCR	C		;Last entry filled?
	JNZ	LOCSET		;No, find next location
	RET			;Yes, return
ROWSET:
	LXI	H,MSGSTDT2	;Pointer to row message
RCLR:
	MVI	M,' '		;Store a space character
	INR	L		;Advance message pointer
	MVI	A,0A7H
	CMP	L		;Message cleared?
	JNZ	RCLR		;No, continue clearing
	MOV	A,C		;Set up row no. for message
	ADI	0B0H	
	LXI	H,MSGSTDT1	;Point to row number char.
	MOV	M,A		;Store row number in message
	DCR	C		;Set row number for check out
	LXI	H,0043H		;Set pointer to location table
	CALL	RWPNT		;Fetch space ship location
	JNZ	STR		;In this row? No
	MVI	M,'<'		;Yes, store space ship code
	INR	L
	MVI	M,'*'
	INR	L
	MVI	M,'>'
STR:
	MVI	L,044H		;Set pointer to star table
STR1:
	MVI	H,000
	CALL	RWPNT		;Fetch star location
	JNZ	NXSTR		;Star here? No, try next star
	INR	L		;Set pointer to store star
	MVI	M,'*'		;Store star code
	MOV	L,E		;Set pointer to star table
NXSTR:
	INR	L		;Advance star table pointer
	MVI	A,04BH		;End of table?
	CMP	L
	JNZ	STR1		;No, check next star
	MVI	H,000		;Restore page pointer
	CALL	RWPNT		;Fetch S.S. location
	JNZ	AS		;S.S. here? No, try A.S.
	MVI	M,'>'		;Store S.S. code
	INR	L
	MVI	M,'1'
	INR	L
	MVI	M,'<'
AS:
	MVI	L,04CH		;Pointer to A.S. table
AS1:
	MVI	H,000
	CALL	RWPNT		;Fetch A.S. location
	JNZ	NXAS		;A.S. here? No, try next
	MVI	M,'+'		;Yes, store A.S. code
	INR	L
	MVI	M,'+'
	INR	L
	MVI	M,'+'
	MOV	L,E		;Fetch A.S. table pointer
NXAS:		
	INR	L		;Advance A.S. pointer
	MVI	A,04FH		;End of table
	CMP	L
	JNZ	AS1		;No, try next A.S. location
	LXI	H,MSGSTDT	;Set up to Print short range scan line
	JMP	CMSG		;Print and return
RWPNT:
	MOV	A,M		;Fetch entry location
	ANA	A		;Anything here?
	RM			;No, return
	CALL	ROTR3		;Position row value
	ANI	007		;Separate row entry
	CMP	C		;Is row equal to current row?
	RNZ			;No, return
	MOV	A,M		;Yes, fetch column location
	ANI	COLMSK		;Separate column location
	MOV	B,A		;Save column
	RLC			;Multiply by two
	ADD	B		;Form pointer to row message
	ADI	08FH
	MOV	E,L		;Save table pointer
	MOV	L,A		;Set pointer to row message
	MVI	H,001H
	XRA	A		;Set Zero flag
	ANA	A
	RET			;Return with 'Z' flag set
RED:
	MVI	M,'R'		;Condition "RED"
	INR	L
	MVI	M,'E'
	INR	L
	MVI	M,'D'
	INR	L
	MVI	M,0
	JMP	CND		;Return to short range scan
QUAD:
	LXI	H,0059H		;Pointer to quadrant location
	LXI	D,MSGPQD	;Pointer to quadrant message
	CALL	TWO		;Put two digits in messag
	LXI	H,MSGQAD	;Pointer to quadrant message
	JMP	MSG		;Print quadrant message
TWO:
	MOV	A,M		;Fetch row and column
	MOV	B,A		;Save row and column
	XCHG			;Set pointer to message
T1:
	CALL	ROTR3		;Position row number
	ANI	007		;Mask off other bits
	ADI	'1'		;Form ASCII digit
	MOV	M,A		;Save ASCII code in message
	MOV	A,B		;Fetch column number
	ANI	COLMSK		;Separate column number
	ADI	'1'		;Form ASCII digit
	INX	H		;Advance message pointer
	INX	H
	MOV	M,A		;Store digit in message
	RET			;Return
FNUM:
	MOV	A,M		;Fetch number
	CPI	'0'		;Is number valid?
	RM			;No, return with'S' flag set
	SUI	':'		;Yes, return with 'S' flag reset
	ADI	080H
	RET
NTN:
	MVI	H,19		;Set counter to 19 dashes
NT1:
	MVI	A,CR		;Print carriage return
	CALL	PRINT
	MVI	A,LF		;Print line feed
	CALL	PRINT
NT2:
	MVI	A,'-'		;ASCII code for dash
	CALL	PRINT		;Print "-"
	DCR	H		;Decrement counter = 0?
	JNZ	NT2		;No, print more dashes
	RET
LRR:
	ADI	00C0H		;Set pointer to galaxy
	MOV	B,A		;Save pointer
	ANI	COLMSK		;First column?
	JZ	CLC1		;Yes, first column zero
	MOV	A,B		;Fetch galaxy pointer
	SUI	1		;No, back up one column
	MOV	L,A		;Pointer to quadrant in galaxy
	MOV	A,M		;Fetch quadrant contents
LR3:
	LXI	H,MSG11A	;Set pointer to left quadrant
	CALL	QDS1		;Set quadrant contents
	MOV	L,B		;Pointer to quadrant in galaxy
	MVI	H,000
	MOV	A,M		;Fetch quadrant contents
	LXI	H,MSG11B	;Pointer to middle quadrant
	CALL	QDS1		;Set quadrant contents
	MOV	A,B		;Fetch quadrant location
	ANI	COLMSK		;Is quadrant in last column?
	CPI	7
	JZ	CLC2		;Yes, right column zero
	MOV	A,B		;No, fetch quadrant location
	ADI	1		;Set location to right quadrant
	MOV	L,A		;Set pointer to right quadrant
	MVI	H,000
	MOV	A,M		;Fetch quadrant contents
LR4:
	LXI	H,MSG11C	;Pointer to right quadrant
	CALL	QDS1		;Set quadrant contents
LRP:
	LXI	H,MSG111	;Set pntr. to L.R. row message
	JMP	MSG		;Print L.R. scan row and return
QDS1:
	MVI	H,004H		;Set message pointer
QDSET:
	MOV	C,A		;Save quadrant contents
	CALL	ROTR4		;Position alien ship number
	ANI	003		;Mask alien ship number
	ORI	'0'		;Form ASCII digit
	MOV	M,A		;Store in message
	INX	H		;Increment message pointer
	MOV	A,C		;Fetch quadrant contents
	CALL	ROTR3		;Position space ship number
	ANI	001		;Mask space ship number
	ORI	'0'		;Form ASCII digit
	MOV	M,A		;Store space ship in message
	INX	H		;Increment message pointer
	MOV	A,C		;Fetch quadrant contents
	ANI	STRMSK		;Mask star number
	ORI	'0'		;Form ASCII digit
	MOV	M,A		;Store in message
	RET
CLC1:
	XRA	A		;Clear column contents
	JMP	LR3		;Print 000 quadrant
CLC2:
	XRA	A		;Clear column contents
	JMP	LR4		;Print 000 quadrant
RWCM:
	MVI	L,05EH		;Pointer to adjusted column
	MOV	A,M		;Fetch adjusted column
	RRC			;Adjust position
	ANI	COLMSK		;Form column value
	MOV	B,A		;Save column
	INR	L		;Advance pointer
	MOV	A,M		;Fetch adjusted row
	RLC			;Position row value
	RLC
	ANI	ROWMSK		;For row value
	ADD	B		;Form row and column byte
	MOV	B,A		;Save in 'B'
	RET			;Return
TIME:
	LXI	H,MSGMSF	;Stardate's time has run
DONE:
	CALL	MSG		;Print message and start
	JMP	GALAXY		;A new game.
LOST:
	LXI	H,MSGYMO	;Moved out of known galaxy, player loses
	JMP	DONE		;Print message & start again
WPOUT:
	LXI	H,MSGKAB	;Smashed into star, space ship destroyed
	JMP	DONE		;Print message & start again
EOUT:
	LXI	H,MSGNEL	;Out of energy, abandon ship
	JMP	DONE		;Print message & start again
NWQD:
	MVI	L,044H		;Set pointer to start table
	MVI	E,11		;Set number of entries
CLR:
	MVI	M,080H		;Store terminate entry
	INR	L		;To clear table
	DCR	E		;Table cleared?
	JNZ	CLR		;No, clear more
	MVI	L,042H		;Set pointer to quad. contents
	MOV	A,M		;Fetch quadrant contents
	ANI	STRMSK		;Fetch number of stars
	MOV	C,A		;Save in 'C'
	MVI	E,044H		;Set pointer to star table
	CNZ	LOCSET		;Setup star locations
	MVI	L,042H		;Pointer to quadrant contents
	MOV	A,M		;Fetch quadrant contents
	CALL	ROTR3		;Move to space station position
	ANI	001		;Isolate space station entry
	MOV	C,A		;Save in 'C'
	MVI	E,04BH		;Set pointer to space station tbl
	CNZ	LOCSET		;If S.S. present, set position
	MVI	L,042H		;Pointer to quadrant contents
	MOV	A,M		;Fetch quadrant contents
	CALL	ROTR4		;Move to alien ship position
	ANI	003		;Isolate alient ship entry
	MOV	C,A		;Save in 'C'
	MVI	E,04CH		;Set pointer to alien ship table
	CNZ	LOCSET		;If A. ship present, set position
LDAS:
	CALL	RN		;Fetch ran. no. for A.S. energy
	MVI	L,053H		;Set pntr to A.S. no. 1 energy
 	CALL	LAS		;Store A.S. number 1 energy
	MVI	L,055H		;Set pointer to alien ship no. 2
 	CALL	LAS		;Store A.S. number 2 energy
	MVI	L,057H		;Set pointer to alien ship no. 3
	JMP	LAS		;Store A.S. number 3 energy & RET
LAS:
	MOV	M,A		;Store least significant half
	ANI	003		;Form most significant half
	INR	L
	MOV	M,A		;Store most significant half
	JMP	RN		;Fetch nxt ran. num. & Return
MATCH:
	MVI	L,044H		;Set pointer to star table
SCK:
	MOV	A,M		;Fetch first star location
	ANA	A		;Is location stored here?
	JM	NS		;No, check S.S. location
	CMP	B		;Are locations equal?
	RZ			;Yes, return
	INR	L		;No, increment pointer
	MVI	A,04BH		;Check for end of start table
	CMP	L		;End of star table?
	JNZ	SCK		;No check next star location
NS:
	MVI	L,04BH		;Set pointer to S.S. location
	MOV	A,M		;Fetch S.S. location
	CMP	B		;Locations equal?
	RZ			;Yes, return
ACK:
	INR	L		;Advance pointer to A.S. table
	MOV	A,M		;Fetch alien ship location
	CMP	B		;Are locations equal?
	RZ			;Yes, return
	MOV	A,L		;No, ck for end of A.S. table
	CPI	04EH		;End of alien ship table?
	JNZ	ACK		;No, try next location
	ANA	A		;Yes, reset 'Z' flag to 0
	RET			;Return
ELOS:
	MVI	L,0032H		;Pointer to temporary storage
	MOV	M,E		;Put energy amount in
	INR	L		;Temporary storage
	MOV	M,D
	DCR	L		;Pointer to energy loss
	MVI	B,002		;Number of bytes for BINDEC
	CALL	BINDEC		;Convert energy amount
	LXI	D,MSGLOP	;Set pointer to energy message
	MVI	B,004		;Counter to number of digits
	CALL	DIGPRT		;Put digits in message
	LXI	H,MSGLOE	;Set pointer to energy loss msg
	CALL	CMSG		;Print loss message
	MVI	L,032H		;Put energy amount back to
	MOV	E,M		;Allow the energy to be
	INR	L		;Removed from the shields
	MOV	D,M
ELS1:
	CALL	CKSD		;Is shield energy sufficient?
	JNC	FMSD		;Yes, delete from shield & RET
	MOV	E,M		;No, move shield energy to
	INR	L		;Main supply
	MOV	D,M
	CALL	FMSD		;Set shield energy to 0
	CALL	TOMN
	MVI	L,032H		;Then fetch energy loss
	MOV	E,M		;From temporary storage
	INR	L
	MOV	D,M
SD0:
	CALL	CKMN		;Enough energy?
	JC	EOUT		;No, ship out of energy
	CALL	FMMN		;Yes, take from main
	LXI	H,MSGDSE	;Print warning
	CALL	CMSG		;'Danger - Shield Energy 000'
	MVI	B,2		;Divide energy loss by 4
	CALL	DVD
	CALL	CKMN		;Delete from main as a
	JC	EOUT		;Penalty for no energy
	JMP	FMMN		;In shields
ELOM:
	CALL	CKMN		;Enough energy in main?
	JNC	FMMN		;Yes, take from main and return
	MOV	C,E		;No, save energy loss
	MOV	B,D
	MVI	L,051H		;Fetch shield energy
	MOV	E,M
	INR	L
	MOV	D,M
	CALL	FMSD		;Remove all shield energy
	CALL	TOMN		;And put in main supply
	MOV	E,C		;Restore energy loss
	MOV	D,B
	JMP	SD0		;Take energy from main
DLET:
	MVI	M,080H		;Remove from quadrant table
	MOV	B,L		;Save table pointer
	MVI	L,059H		;Fetch current quad. location
	MOV	A,M
	ADI	00C0H		;Form pntr. to galaxy location
	MOV	L,A		;Set galaxy pointer
	MOV	A,B		;Fetch table pointer
	CPI	04BH		;Space station hit?
	JNZ	DLAS		;No, delete alien ship
	MOV	A,M		;Fetch location in galaxy
	ANI	037H		;Delete space station
	MOV	M,A		;Restore in galaxy
	MVI	L,042H		;Set pntr. to quad. contents
	MOV	M,A		;Save new contents
	MVI	L,05BH		;Set pointer to number of S.S.
	DCR	M		;Decrement number of S.S.
	RNZ			;If number not 0, return
	LXI	H,MSGDSE	;Print warning message
CMSG:
	CALL	MSG
	MVI	H,000		;Reset pointer to page 000
	RET
DLAS:
	MOV	A,M  		;Fetch location in galaxy
	SUI	010H		;Delete 1 alien ship from quad.
	MOV	M,A		;Restore in galaxy
	MVI	L,042H		;Fetch current quad. contents
	MOV	M,A		;Save new contents
	MVI	L,05CH		;Fetch number of A.S. counter
	DCR	M		;Subtract 1 from number
	RNZ			;If counter not = 0, return
	LXI	H,MSGCYH	;If counter = 0, game over
	JMP	DONE		;print CONGRATULATIONS
DRCT:
	CALL	INPUT		;Input first course number
	LXI	H,005EH		;Pointer to temporary storage
	CPI	'1'		;Is input less than 1?
	JC	ZRET		;Yes, illegal input
	CPI	'9'		;Is input greater than 8?
	JNC	ZRET		;Yes, illegal input
	ANI	00FH		;Mask off ASCII code
	RLC			;If good, times 2
	MOV	M,A		;And save in temporary storage
	MVI	A,'.'		;Print a decimal point
	CALL	PRINT
	CALL	INPUT		;Input second course number
	CPI	'0'		;Is digit = 0?
	JZ	CR1		;Yes, continue process
	CPI	'5'		;No, is digit = 5?
	JNZ	ZRET		;No, return with Z flag set
CR1:
	ANI	001		;Mask all but least signif. bit
	ADD	M		;Add 1st number input
	RLC			;Ad form pointer to course
	SUI	004		;Table
	MOV	M,A		;Save pointer in temporary storage
	RNZ			;Return with Z flag reset
	ADI	1		;If not reset, reset it
	RET
ZRET:
	XRA	A 		;Set Z flag
	RET			;And return
QCNT:
	LXI	H,0059H		;Set pointer to curr. quad. row & col storage
	MOV	A,M		;Fetch current quadrant
	ADI	00C0H		;Form pointer to galaxy
	MOV	L,A		;Set up pointer
	MOV	A,M		;Fetch quadrant contents
	MVI	L,042H		;Set pointer to quad. contents
	MOV	M,A		;And store new contents
	RET
ACTV:
	MVI	L,05EH		;Fetch course offset
	MOV	L,M		;Fetch column movement
	MOV	C,M
	INR	L		;Advance pointer
	MOV	D,M		;Fetch row movement
	MVI	L,043H		;Pointer to current sector
	MOV	A,M		;Fetch current sector
	MOV	B,A		;Save in 'B'
	ANI	007		;Mask off row
	MVI	L,05EH		;Pointer to temporary storage
	RLC			;Multiply times 2
	MOV	M,A		;Save adjusted column
	INR	L		;Advance storage pointer
	MOV	A,B		;Fetch current sector
	ANI	038H		;Mask off column
	RRC			;Set up times 2 value
	RRC
	MOV	M,A		;Save adjusted row
	RET
TRK:
	MVI	L,030H		;Set pointer to crossing flag
	MOV	M,H		;Clear quadrant crossing flag
	MVI	L,05EH
	MOV	A,M		;Fetch adjusted column
	ADD	C		;Add column move
	MOV	M,A		;Temp. save current column
	JP	NOBK		;If not left crossing, jump
	ANI	00FH		;Left crossing, correct and
	MOV	M,A		;Save new adjusted column
	MVI	L,030H		;Indicate left crossing in
	MOV	M,L		;Crossing flag by non-zero
	MVI	L,059H		;And decrement current quad.
	MOV	A,M		;Column entry
	ANI	COLMSK		;Is current quad. column = 0?
	RZ			;Yes, return with Z flag set
	DCR	M		;Decrement current quad. clmn
	JMP	RMV		;Do row move
NOBK:
	CPI	010H		;Quadrant crossing right?
	JC	RMV		;No, do row move
	ANI	00FH		;Yes, correct and
	MOV	M,A		;Save new adjusted column
	MVI	L,030H
	MOV	M,L
	MVI	L,059H
	MOV	A,M
	ANI	007
	ADI	001
	CPI	008H
	RZ
	INR	M

RMV:
	MVI	L,05FH
	MOV	A,M		;Fetch adjusted row
	ADD	D		;Add row move
	MOV	M,A		;Save new adjusted row
	JP	NOUP		;If not up, jump
	ANI	00FH		;Move up 1 quadrant, correct
	MOV	M,A		;And save new adjusted row
	MVI	L,030H		;Then indicate crossing in
	MOV	M,L		;Crossing flag by non-zero
	MVI	L,059H		;And decrement quadrant row
	MOV	A,M		;Fetch quadrant entry
	ANI	038H		;Is quadrant row = 0?
	RZ			;Yes, return with Z flag set
	MOV	A,M		;No, decr. current quad. row
	SUI	008H
	MOV	M,A		;Save new current quadrant
	JMP	CKX		;Then perform crossing logic
NOUP:
	CPI	010H		;Quadrant crossing down?
	JC	CKX		;No, check for crossing flag
	ANI	00FH		;Yes, correct and
	MOV	M,A		;Save new adjusted row
	MVI	L,030H		;Then indicate down crossing
	MOV	M,L		;In crossing flag by non-zero
	MVI	L,059H		;Then incr. current quad. row
	MOV	A,M
	ANI	038H		;Separate row entry
	ADI	008H		;Increment row entry
	CPI	040H		;Move out of galaxy?
	RZ			;Yes, return with Z flag set
	MOV	A,M		;No, then incr. crnt. quad. row
	ADI	008H
	MOV	M,A		;save new current quadrant
CKX:
	MVI	L,028H		;Set pointer to register storage
	MOV	M,E		;Save registers 'E' 'D' and 'C'
	INR	L
	MOV	M,D
	INR	L
	MOV	M,C
	RNZ			;Return with Z flag reset
	MVI	A,001		;If not reset
	ANA	A		;Reset it and return
	RET
DVD:
	ANA	A		;Divide the double
	MOV	A,D		;Precision value
	RAR			;By two by the number
	MOV	D,A		;Of times indicated
	MOV	A,E		;In 'B'
	RAR
	MOV	E,A
	DCR	B		;Finished divide?
	JNZ	DVD		;No, continue
	RET			;Yes, return
WASTE:
	CALL	ELOM		;Delete power from main
	LXI	H,MSGNAS	;Print 'No A.S. Wasted shot'
	CALL	MSG
	JMP	CMND		;Input new command
EIN:
	LXI	H,0064H		;Set pointer to sign indicator
	MOV	M,H		;Clear sign indicator
	MVI	L,063H		;Pet pointer to input table
	CALL	INPUT		;Input 1st character
	CPI	'-'		;Negative sign?
	JNZ	EN2		;No, check digit
	INR	L		;Yes, advance pntr to sign ind.
	MOV	M,L		;Set sign indicator to non-zero
	DCR	L		;Reset table pointer
EN1:
	CALL	INPUT		;Input digit
EN2:
	MOV	M,A   		;Save digit in table
	CALL	FNUM		;Valid digit?
	RM			;No, return with S flag set
	MOV	A,M		;Yes, fetch digit
	ANI	00FH		;Mask off ASCII code
	MOV	M,A		;Save binary value
	DCR	L		;Move table pointer
	MVI	A,05FH		;Is the table pointer
	CMP	L		;Indicating table full?
	RZ			;Yes, return with S flag set
	JMP	EN1		;No, input more digits
DCBN:
	MVI	L,060H		;Fetch unit's digit
	MOV	A,M
	DCR	L		;Move pointer to temp. storage
	MOV	M,H		;Set temporary storage to
	DCR	L		;Value of units digit
	MOV	M,A
	MVI	L,061H		;Fetch ten's digit
	MOV	A,M
	ANA	A		;Is ten's digit = 0;
	JZ	DC1		;Yes, do 100's digit
	MOV	B,A		;Save ten's digit
	MVI	E,00AH		;Set up binary value
	MOV	D,H		;Of 10 in 'E' and 'D'
	CALL	TOBN		;Add 10 X digit
DC1:
	MVI	L,062H		;Fetch 100's digit
	MOV	A,M
	ANA	A		;Is 100's digit = 0?
	JZ	DC2		;Yes, finish
	MOV	B,A		;Save 100's digit
	LXI	D,064H		;Set up binary value
	CALL	TOBN		;Add 100 X digit
DC2:
	MVI	L,063H		;Fetch 1000's digit
	MOV	A,M
	ANA	A		;Is 1000's digit = 0?
	JZ	DC3		;Yes, set bn val in 'E' & 'D'
	MOV	B,A		;Save 1000's digit
	LXI	D,03E8H		;Setup binary val of 1000 in 'E' and 'D'
	CALL	TOBN		;Add 1000 X digit
DC3:
	MVI	L,05EH		;Set pointer to binary value
	MOV	E,M		;Fetch least significant half
	INR	L
	MOV	D,M		;Fetch most significant half
	RET
TOBN:
	MVI	L,05EH
	CALL	TO1
	DCR	B
	RZ
	JMP	TOBN
TOMN:
	MVI	L,04FH		;Set pointer to main energy
TO1:
	MOV	A,M		;Fetch least significant half
	ADD	E		;Add 'E'
	MOV	M,A		;Save new least significant half
	INR	L		;Advance pntr. to most signif.
	MOV	A,M		;Fetch most significant half
	ADC	D		;Add 'D' with carry
	MOV	M,A		;Save new most significant half
	RET
TOSD:
	MVI	L,051H		;Set pointer to shield energy
	JMP	TO1		;Add 'E' & 'D' to shield
FMMN:
	MVI	L,04FH		;Set pointer to main energy
FM1:
	MOV	A,M		;Fetch least significant half
	SUB	E		;Subtract 'E'
	MOV	M,A		;Save new least significant half
	INR	L		;Advance pntr. to most signif.
	MOV	A,M		;Fetch most significant half
	SBB	D		;Subtract 'D' with carry
	MOV	M,A		;Save new most significant half
	RET
FMSD:
	MVI	L,051H		;Set pointer to shield energy
	JMP	FM1		;Subtr. 'E' & 'D' fm. shld ener.
CKMN:
	MVI	L,050H		;Set pointer to main energy
CK1:
	MOV	A,M		;Fetch most significant half
	DCR	L		;Set pointer to least signif. half
	CMP	D		;Is most significant half = 0?
	; RNZ			;No, return with flags set up
	ret			;Stacy, change to return so that there is always enough energy.
CK2:
	MOV	A,M		;If greater than or =, ret. with
	CMP	E		;'C' flag reset, if less than
	RET			;Return with 'C' flag set
CKSD:
	MVI	L,052H		;Check shield energy level
	JMP	CK1		;Against requested level
OVER:
	LXI	H,MSGCHK	;Print "CHICKEN"
	CALL	MSG
	HLT			;Halt
        jmp     START           ; Stacy, allow a restart with my emulator.

SPRC:
	MOV	A,M		;Fetch row and column
	ANI	07H		;Separate column
	MOV	C,A		;Save column in 'C'
	MOV	A,M		;Fetch row
	CALL	ROTR3		;Position row to right
	ANI	07H		;Separate row
	MOV	B,A		;Save row in 'B'
	RET

                                ; ----------------------------------------------
GALAXY:
; ++    2470:00001001 10100110: 00110001 : 31:061 > opcode: lxi sp,STACK
; Enter the following to get to the start byte: 12578bx
	LXI	SP,STACK	;Set stack pointer
	CALL	CONINI		;Initialize Console I/O
	LXI	H,MSGDYW
	CALL	MSG		;Print introduction
START:
	CALL	RN		;Increment random number
	;CALL	INPCK		;Input yet?
	;JP	START		;No, continue wait
	CALL	INPUT		;Wait for an input character, then echo it and continue.
	cpi	'n'		;No, stop game?
	JZ	OVER		;Yes, vanish from galaxy
	CPI	'N'		;No, stop game?
	JZ	OVER		;Yes, vanish from galaxy
	MVI	E,00C0H		;Set pointer to galaxy storage
                                ; ----------------------------------------------
GLXSET:
	CALL	RN		;Fetch random number
	ANI	07FH
	MOV	L,A
	MVI	H,00FH		;Set pointer to galaxy table
	MOV	A,M		;Fetch galaxy entry
	MOV	L,E
	MVI	H,000H		;Set pntr. to galaxy content tbl
	MOV	M,A		;Store quadrant contents
	INR	E		;Galaxy storage complete?
	JNZ	GLXSET		;No, fetch more sectors
GLXCK:
	MOV	D,H		;Space station count = 0;
	MOV	C,H		;Alien ship count = 0;
	MVI	L,0C0H		;Fetch quadrant contents
GLXCK1:
	MOV	A,M		;Fetch quadrant contents
	ANI	STNMSK		;Mask space station
	ADD	D		;Add to space station total
	MOV	D,A		;Save space station total
	MOV	A,M		;Fetch quadrant contents
	ANI	ALNMSK		;Mask alien ship
	RRC
	RRC
	ADD	C		;Add to alien ship total
	MOV	C,A		;Save alien ship total
	INR	L		;End of galaxy storage?
	JNZ	GLXCK1		;No, continue adding
	MOV	A,D		;Fetch space station total
	RRC			;Position total to right
	RRC
	RRC
	MOV	D,A		;Save space station total
	CPI	7		;Too many space stations?
	JNC	SSPLS		;Yes, delete 1
	CPI	2		;Too few space stations?
	JC	SSMNS		;Yes, add 1 more
	MOV	A,C		;Fetch alien ship total
	RRC
	RRC
	MOV	C,A		;Save alien ship total
	CPI	32		;Too many alien ships?
	JNC	ASPLS		;Yes, delete 1
	CPI	10		;Too few alien ships?
	JC	ASMNS		;Yes, add 1 more
	MVI	L,05BH		;Set pntr to store number S.S.
	MOV	M,D		;Save number of space stations
	INR	L		;Advance pntr to number A.S.
	MOV	M,C		;Save number of alien ships
	MOV	A,C
	ADI	005
	INR	L		;Adv. pntr to nmbr of stardates
	MOV	M,A		;Save number of stardates
	MVI	B,1		;Set nmbr bytes for BINDEC
	CALL	BINDEC		;Covert stardate value
	LXI	D,MSGDTS	;Set pointer to digit storage
	MVI	B,2		;Set counter to nmbr or digits
	CALL	DIGPRT		;Put digits in message
	LXI	H,005CH		;Set pointer to number A.S.
	MVI	B,001		;Set nmbr bytes for BINDEC
	CALL	BINDEC		;Convert alien ship value
	LXI	D,MSGSPS	;Set pntr to digit stor. in start msg.
	MVI	B,2		;Set counter to no. of digits
	CALL	DIGPRT		;Put digits in message
	LXI	H,005BH		;Set pointer to no. space stat.
	MOV	A,M		;Set no. bytes for BINDEC
	ORI	0B0H		;Covert space station value
	LXI	H,MSGSSS	;Set pntr to digit stor. in start msg.
	MOV	M,A		;Set counter to no. of digits
	LXI	H,MSGYJD	;Set pntr to start message
	CALL	MSG		;Print starting message
	CALL	RN		;Fresh start quadrant
	ANI	03FH		;Mask off most significant bits
	MVI	L,059H		;Set pntr. to quadrant storage
	MOV	M,A		;Save quadrant location
	CALL	QCNT		;Fetch current quad. contents
	CALL	LOAD		;set initial conditions
	CALL	NWQD		;Set quad. contents location
	MVI	C,1		;Set space ship counter
	MVI	E,043H		;Set space ship loc. storage
	CALL	LOCSET		;Set initial space ship location
SRSCN:
	LXI	H,MSG123	;Set pntr. for short range scan
	CALL	MSG		;Print initial row
	MVI	C,1		;Set row number
	CALL	ROWSET		;Set up row for printout
	LXI	H,005DH		;Set pointer to stardate
	MVI	A,032H
	SUB	M		;Calculate number used
	INR	L		;Adv pntr to temporary storage
	MOV	M,A		;Save nmber used
	MVI	B,1		;Set no. bytes for BINDEC
	CALL	BINDEC		;Covert to current stardate
	LXI	D,MSGSDP	;Set pointer to stardate msg.
	MVI	B,2		;Set counter to no. of digits
	CALL	DIGPRT		;Put digits in stardate message
	LXI	H,MSGSTDT3	;Set pointer to message
	CALL	MSG		;Print stardate message
	MVI	C,2		;Set row number 2
	CALL	ROWSET		;Set up row for printout
	MVI	L,042H		;Set pntr to current quadrant
	MOV	A,M		;Fetch current contents
	LXI	H,MSGGRN	;Set pointer to condition msg
	ANI	030H		;Alien ship in quadrant?
	JNZ	RED		;Yes, condition "RED"
	MVI	M,'G'		;Condition "GREEN"
	INR	L
	MVI	M,'R'
	INR	L
	MVI	M,'E'
	INR	L
	MVI	M,'E'
	INR	L
	MVI	M,'N'
CND:
	LXI	H,MSGCND	;Set pointer to condition msg
	CALL	MSG		;Print condition message
	MVI	C,3		;Set row number 3
	CALL	ROWSET		;Set up row to printout
	CALL	QUAD		;Print current quadrant
	MVI	C,4		;Set row number 4
	CALL	ROWSET		;Set up row for printout
	MVI	L,043H		;Pointer to current sector
	MVI	E,0E3H		;Set digit storage
	INR	D
	CALL	TWO		;Put two digits in message
	MVI	L,0D8H		;Set pointer to sector message
	CALL	MSG		;Print sector message
	MVI	C,5		;Set row number 5
	CALL	ROWSET		;Set up row for printout
	MVI	L,04FH		;Set pointer to energy storage
	MVI	B,2		;Number of bytes for BINDEC
	CALL	BINDEC		;Covert to energy stored
	LXI	D,MSGENP	;Set pointer to energy message
	MVI	B,4		;Set counter to no. of digits
	CALL	DIGPRT		;Put digits in message
	LXI	H,MSGENR	;Set pointer to energy message
	CALL	MSG		;Print current energy message
	MVI	C,6		;Set row number 6
	CALL	ROWSET		;Set up row for printout
	MVI	L,05AH		;Set point to no. torpedoes
	MVI	B,1		;Number of bytes for BINDEC
	CALL	BINDEC		;Covert number of torpedoes
	LXI	D,MSGTPP	;Set pointer to torpedo message
	MVI	B,2		;Set counter to no. of digits
	CALL	DIGPRT		;Put no. torpedoes in message
	LXI	H,MSGTRP	;Set pointer to torpedo msg
	CALL	MSG		;Print torpedo message
	MVI	C,7		;Set row number 7
	CALL	ROWSET		;Set up row for printout
	MVI	L,051H		;Set pointer to shield energy
	MVI	B,2		;Number of bytes for BINDEC
	CALL	BINDEC		;Convert shield energy
	LXI	D,MSGSHP	;Set pointer to shield message
	MVI	B,4		;Set counter for no. of digits
	CALL	DIGPRT		;Put shield energy in message
	LXI	H,MSGSHD	;Set pointer to shield message
	CALL	MSG		;Print shield message
	MVI	C,8H		;Set row number 8
	CALL	ROWSET		;Set up row for printout
	LXI	H,MSG123	;Set pointer to final row
	CALL	MSG		;Print final row of S.R. scan
CMND:
	MVI	H,000
	LXI	SP,STACK	;Reset stack pointer
	MVI	E,10		;Delete 10 units of
	MOV	D,H		;Energy for each command
	CALL	ELOM
	MVI	L,041H		;Set pointer to random number
	INR	M		;Fetch random nmbr. constant

; ------------------------------------------------------------------------------
; Command options

CMD:
	LXI	H,MSGCMD	;Set pointer to command msg
	CALL	CMSG		;Request command input
	CALL	INPUT		;Input command
	CPI	'0'		;Ship movement?
	JZ	CRSE		;Yes, input course
	CPI	'1'		;Short range scan?
	JZ	SRSCN		;Yes, print short range scan
	CPI	'2'		;Long range scan?
	JZ	LRSCN		;Yes, print long range scan
	CPI	'3'		;Galaxy printout?
	JZ	GXPRT		;Yes, print galaxy
	CPI	'4'		;Shield energy?
	JZ	SHEN		;Yes, adjust shield energy
	CPI	'5'		;Phasor control?
	JZ	PHSR		;Yes, fire phasers
	CPI	'6'		;Torpedo shot?
	JZ	TRPD		;Yes, shoot torpedo
	JMP	CMD		;Illegal command, try again
LRSCN:
	LXI	H,MSGLRS	;Set pntr to long range msg
	CALL	MSG		;Print long range scan
	CALL	QUAD		;Print quadrant location
	CALL	NTN		;Print row of dashes
	MVI	L,059H		;Pointer to current quadrant
	MOV	A,M		;Fetch current quadrant
	ANI	038H		;Current quadrant in row 1?
	JZ	RWC1		;Yes, top row clear
	MOV	A,M		;No, set up quadrant to
	SUI	008H		;Indicate row - 1
	CALL	LRR		;Set & print top row
LR1:
	CALL	NTN		;Print separating row
	MVI	L,059H		;Set pointer to current quad.
	MOV	A,M		;Fetch current quadrant
	CALL	LRR		;Set & print middle row
	CALL	NTN		;Print separating row
	MVI	L,059H		;Set pointer to current quad.
	MOV	A,M		;Fetch current quadrant
	CPI	038H		;Current quadrant in row 8?
	JNC	RWC2		;Yes, bottom row clear
	ADI	008H		;No, set quadrant to row + 1
	CALL	LRR		;Set & print bottow row
LR2:
	CALL	NTN		;Print separating row
	JMP	CMND		;Input next command
RWC1:
	CALL	RWC		;Print clear row
	JMP	LR1		;Continue long range scan
RWC2:
	CALL	RWC		;Print clear row
	JMP	LR2		;Finish long range scan
RWC:
	LXI	H,MSG11A	;Set pointer to left quadrant
	XRA	A		;Set zero entry
	CALL	QDS1		;Set quadrant contents
	LXI	H,MSG11B	;Set pointer to middle quad.
	XRA	A		;Set zero entry
	CALL	QDS1		;Set quadrant contents
	LXI	H,MSG11C	;Set pointer to right quadrant
	XRA	A		;Set zero contents
	CALL	QDS1		;Set quadrant contents
	JMP	LRP		;Pring long range row
CRSE:
	LXI	H,MSGCRS	;Pointer to "Course" message
 	CALL	MSG		;Request course input
 	CALL	DRCT		;Input course direction
	JZ	CRSE		;Input error, try again
WRP:
	LXI	H,MSGWRP	;Pointer to "Warp" message
 	CALL	CMSG		;Request warp input
	MVI	L,05FH		;Set pntr. to temporary storage
 	CALL	INPUT		;Input warp factor number 1
	CPI	'0'		;Is digit less than 0?
	JC	WRP		;No, request input again
	CPI	'8'		;Is input greater than 7?
	JNC	WRP		;Yes, try again
	ANI	007		;Mask off ASCII code
	RLC			;Position to 3rd bit
	RLC
	RLC
	MOV	M,A		;Save in temporary storage
	MVI	A,'.'		;Print decimal point
 	CALL	PRINT
 	CALL	INPUT		;Input 2nd warp factor number
	CPI	'0'		;Is digit less than 0?
	JC	WRP		;Yes, no good
	CPI	'8'		;Is digit greater than 7
	JNC	WRP		;Yes, no good
	ANI	007		;Mask off ASCII code
	ADD	M		;Add warp digit number 1
	JZ	WRP		;If 0, no good
	MOV	E,A		;Save warp factor in 'E'
	CALL	ACTV		;Fetch adjusted row & column
	MVI	L,031H		;Set pntr to crossing indicator
	MOV	M,H		;Clear crossing indicator
MOVE:
	CALL	TRK		;Track 1 sector
	JZ	LOST		;Out of galaxy? Yes, lost
	MVI	L,030H		;Fetch crossing flag
	MOV	A,M
	ANA	A		;Quadrant crossed?
	JZ	CLSN		;No, check collision
	INR	L		;Advance to crossing indicator
	MOV	M,L		;Set crossing indicator to non-0
	MVI	E,25		;Delete 25 units of
	MOV	D,H		;Energy from main supply
	CALL	ELOM
	CALL	QCNT		;Fetch new quadrant contents
	CALL	NWQD		;Set up new quadrant
CLSN:
	CALL	RWCM		;Form rwo and column byte
	CALL	MATCH		;Collision?
	JNZ	MVDN		;No, complete move
	MOV	B,L		;Yes, save object location
	MOV	A,B		;Set flags to determine
	CPI	04BH		;What was hit
	MVI	L,031H		;Pointer to crossing indicator
	MOV	A,M		;Fetch crossing indicator
	JZ	SSOUT		;Space station collision
	JNC	ASOUT		;Alient ship collision
	ANA	A		;Star, initial quadrant?
	JZ	WPOUT		;Yes, ship wiped out
MVDN:
	LXI	H,0028H		;Restore registers 'E' 'D' & 'C'
	MOV	E,M
	INR	L
	MOV	D,M
	INR	L
	MOV	C,M
	DCR	E		;Decrement warp factor
	JNZ	MOVE		;Not 0, continue move
	MVI	L,031H		;Fetch crossing indicator
	MOV	A,M
	ANA	A		;Quadrant crossing occurred?
	JZ	NOX		;No, complete move
	MVI	L,05DH		;Yes, fetch stardate
	MOV	B,M
	DCR	B		;Decrement stardate counter
	JZ	TIME		;If 0, end of game
	MOV	M,B		;Else save new date
NOX:
	CALL	RWCM		;Form row and column byte
	MVI	L,043H		;Set pointer to current sector
	MOV	M,B		;Save new sector
	CALL	MATCH		;Last move a collision?
	CZ	CHNG		;Yes, change object location
	CALL	DKED		;Check for docking
	JMP	SRSCN		;Do short range scan
SSOUT:
	ANA	A		;Initial quadrant?
	JNZ	MVDN		;No, no loss
	MOV	L,B		;Yes, set object pointer
	CALL	DLET		;Remove space station fm glxy
	LXI	D,600		;Then delete 600 units of engy from S.S.
SSO1:
	CALL	ELOS		;Delete energy
	JMP	MVDN		;Finish move
ASOUT:
	ANA	A		;Initial quadrant?
	JNZ	MVDN		;No, no loss
	MOV	L,B		;Yes, delete alien ship
	CALL	DLET
	LXI	D,1500		;Delete 1500 units of engy. from S.S.
	JMP	SSO1		;And finish move
CHNG:
	MOV	E,L		;Set table location and
	MVI	C,1		;Number of objects counter for
	JMP	LOCSET		;Move object and return
DKED:
	MVI	L,04BH		;Fetch space station byte
	MOV	A,M
	ANA	A		;Space station in quadrant?
	RM			;No, return
	MOV	A,B		;Fetch space ship location
	ANI	ROWMSK		;Separate row location
	MOV	C,A		;Save in 'C'
	MOV	A,B		;Fetch space ship location
	ANI	COLMSK		;Separate column location
	MOV	B,A		;Save in 'B'
	MOV	A,M		;Fetch space station location
	ANI	COLMSK		;Separate space station clmn loc
	MOV	E,A		;Save in 'E'
	MOV	A,M		;Fetch space station location
	ANI	ROWMSK		;Separate row as space ship
	CMP	C		;Same row and space ship?
	RNZ			;No, return
	MOV	A,B		;Fetch space ship column
	ADI	1		;Advance one column
	CMP	E		;Space ship adjacent?
	JZ	LOAD		;Yes, load up space ship
	SUI	2		;No, try column to left
	CMP	E		;Space station adjacent
	RNZ			;No, return
	JMP	LOAD		;Yes, load space ship & return
SHEN:
	LXI	H,MSGSET	;Print "Shield Energy
	CALL	MSG		;       Transfer =  "
	CALL	EIN		;Input energy amount
	JM	SHEN		;Invalid input, try again
	CALL	DCBN		;Convert to binary
	MVI	L,064H		;Fetch sign indicator
	MOV	A,M
	ANA	A		;Is sign positive
	JZ	POS		;Yes, from main to shield
	CALL	CKSD		;No, check shield energy
	JC	NE		;If shield less than req, no good
	CALL	FMSD		;Subtract from shield
	CALL	TOMN		;Add to main
	JMP	CMND		;Input new command
POS:
	CALL	CKMN		;Check main energy
	JC	NE		;If main less than req, no good
	CALL	FMMN		;Subtract from main
	CALL	TOSD		;Add to shield energy
	JMP	CMND		;Input new command
NE:
	LXI	H,MSGNEE	;Print "Not Enough Energy"
	CALL	MSG
	JMP	CMND		;Input new command
TRPD:
	MVI	L,05AH		;Fetch number of torpedoes
	MOV	A,M
	ANA	A		;Any torpedoes left?
	JZ	NTPD		;No, print no torpedo message
	MVI	E,250		;Set up 250 units
	MOV	D,H		;Of energy to delete
	CALL	CKMN		;Enough in main supply?
	JC	NE		;No, report not enough
	CALL	FMMN		;Yes, delete from main
	MVI	L,05AH
	DCR	M		;Remove one torpedo
TR1:
	LXI	H,MSGTTY	;Print "Torpedo Trajectory"
	CALL	MSG
	CALL	DRCT		;Input direction
	JZ	TR1		;Invalid input, try again
	CALL	ACTV		;Form adjusted row & column
	MVI	L,059H		;Save current quadrant location
	MOV	A,M		;In temporary storage
	MVI	L,02BH
	MOV	M,A
TR2:
	CALL	TRK		;Move torpedo one sector
	JZ	QOUT		;Out of quadrant? Missed
	MVI	L,030H		;Fetch crossing flag
	MOV	A,M
	ANA	A		;Crossed quadrant
	JNZ	QOUT		;Yes, missed
	CALL	RWCM		;No, form row and column
	MOV	C,B		;Save row and column byte
	LXI	H,MSGTRK	;Set up tracking message by inserting
	CALL	T1		;Row and column in message
	MVI	L,012H		;Set pointer to message
	CALL	CMSG		;Print 'Tracking: R,C'
	MOV	B,C		;Fetch row and column byte
	CALL	MATCH		;Torpedo hit anything?
	JZ	HIT		;Yes, analyze
	MVI	L,028H		;No, restore registers
	MOV	E,M
	INR	L
	MOV	D,M
	INR	L
	MOV	C,M
	JMP	TR2		;Continue tracking
HIT:
	MOV	A,L		;What was hit?
	CPI	04BH		;Was it a star?
	JC	QOUT		;Yes, missed alien ship
	JZ	SSTA		;Space stat.? Yes, delete S.S.
	CALL	DLET		;No, delete alient ship
	LXI	H,MSGASD	;Print alien ship hit message
	CALL	MSG
	JMP	CMND		;Input new command
SSTA:
	CALL	DLET		;Delete space station fm galaxy
	LXI	H,MSGSSD	;Print message of loss
	CALL	MSG		;Space station
QOUT:
	LXI	H,MSGYMA	;Print missed message
	CALL	CMSG
	MVI	E,200		;Set up loss of 200
	MOV	D,H		;Units due to alien ship
	CALL	ELOS		;Rtaliating
	MVI	L,02BH		;Restore current quadrant
	MOV	A,M		;Location
	MVI	L,059H
	MOV	M,A
	JMP	CMND		;Input new command
NTPD:
	LXI	H,MSGZRO	;Set pointer to No Torpedo message
	CALL	MSG		;Print message
	JMP	CMND		;Jump to input command
PHSR:
	LXI	H,MSGPEF	;Print 'Phasor Energy to Fire='
	CALL	MSG
	CALL	EIN		;Input energy amount
	JM	PHSR		;Input error, try again
	CALL	DCBN		;Convert energy to binary
	CALL	ELOM		;Delete energy from main
	MVI	L,042H		;Fetch current quad. contents
	MOV	A,M
	ANI	030H		;Any alien ships?
	JZ	WASTE		;No, waste of energy
	CALL	ROTR4		;Position number of alien ship
	SUI	001		;1 alien ship, full energy
	JZ	PH1		;2 alien ships, half energy
	MOV	B,A		;3 alien ships, quarter energy
	CALL	DVD
PH1:
	MVI	L,05EH		;Set pointer to energy storage
	MOV	M,E		;Save energy amount
	INR	L
	MOV	M,D
	MVI	L,028H		;Save energy in temp. storage
	MOV	M,E
	INR	L
	MOV	M,D
	INR	L		;Save loc. of alien ship in table
	MVI	M,04CH
	CALL	ASPH		;Calc. phsr dmg to A.S. No. 1
	MVI	L,02AH		;Set pntr to A.S. loc. storage
	MVI	M,04DH		;Save location of A.S. in table
	CALL	ASPH		;Calc. phsr dmg to A.S. No. 2
	MVI	L,02AH		;Set pntr to A.S. loc. storage
	MVI	M,04EH		;Save location of 3rd alien ship
	CALL	ASPH		;Calc. phsr dmg to A.S. No. 3
	JMP	CMND		;Input new command
ASPH:
	MOV	L,M		;Set pntr to alient ship table
	MOV	A,M		;Fetch alien ship location
	ANA	A		;Alien ship in this location?
	RM			;No, return
	LXI	D,MSGSEC	;Set pointer to sector Storage in msg
	CALL	TWO		;set sector coordinates
	MVI	L,04EH		;Print 'A.S. at sector X,Y:'
	CALL	CMSG
	MVI	L,043H		;Fetch space ship location
	CALL	SPRC		;Separate row and column
	MOV	L,E		;Save space ship row & column
	MOV	H,D
	MOV	E,C
	MOV	D,B
	CALL	SPRC		;Separate A.S. row & column
	MOV	A,B		;Fetch alien ship row
	SUB	D		;Subract space ship row
	JP	PH2		;To calculate distance between
	XRI	0FFH		;Alien ship and space ship
	ADI	001
PH2:
	MOV	B,A		;Save row distance
	MOV	A,C		;Fetch alien ship column
	SUB	E		;Subtract space ship column
	JP	PH3		;To calcualate column distance
	XRI	0FFH		;Between A. ship & S. ship
	ADI	001
PH3:
	ADD	B		;Add row distance
	RRC			;Form distance factor
	RRC			;To be used to calculate
	ANI	003		;Energy that reaches alien ship
	MOV	B,A		;Save in 'B'
	MOV	C,L		;Save pointer in 'C'
	MVI	L,028H		;Fetch phasor energy
	MOV	E,M
	INR	L
	MOV	D,M
	DCR	B		;Divide energy by
	INR	B
	CNZ	DVD		;Distance factor
	MOV	A,C		;Fetch table pointer
	ANI	003
	RLC			;And set pointer to alien ship
	ADI	053H		;Energy storage
	MVI	L,02BH		;Save energy pointer
	MOV	M,A
	MOV	L,A		;Set pntr. to alien ship energy
	CALL	FM1		;Delete energy fm alien ship
	JM	DSTR		;If negative, A. ship destroyed
	JNZ	ALOS		;If non-0, print A. ship energy
	DCR	L		;Check 2nd have of alien ship
	MOV	A,M		;Energy to see if zero.
	INR	L
	ANA	A		;Alien ship energy = 0?
	JZ	DSTR		;Yes, remove from galaxy
ALOS:
	DCR	L		;Set pntr to alien ship energy
	MVI	B,2		;Set number for BINDEC
	CALL	BINDEC		;Convert A.S. energy to decimal
	LXI	D,MSGDEY
	MVI	B,4		;Set number of digits
	CALL	DIGPRT		;Put energy in message
	LXI	H,MSGEGY	;Print energy of alien ship
	CALL	CMSG
	MVI	L,02BH		;Fetch alien ship energy
	MOV	L,M
	MOV	E,M		;Fetch alien ship energy
	INR	L
	MOV	D,M
	MVI	B,2		;Divide alien ship energy
	CALL	DVD		;By 4 as retaliation by A.S.
	JMP	ELOS		;Remove fm shield nrgy & ret
DSTR:
	LXI	H,MSGDES	;Print "Destroyed"
	CALL	CMSG
	MVI	L,02AH		;Fetch alient ship location in tbl
	MOV	L,M
	JMP	DLET		;Remove A.S. fm glxy & ret

; ------------------------------------------------------------------------------
GXPRT:
	LXI	H,MSGGDY	;Print galaxy display
	CALL	MSG
	MVI	H,031H
 	CALL	NT1		;Print border
	MVI	L,0C0H		;Set pointer to galaxy
GL1:
	MOV	D,H		;Set printout pointer
	MVI	E,084H
GL2:
	MOV	A,M		;Fetch quadrant contents
	XCHG
	CALL	QDSET		;Set quad. contents in message
	MOV	A,L		;Fetch message pointer
	ADI	004		;Advance to next quad. in msg
	MOV	L,A
	XCHG			;Set galaxy pointer
	INR	L		;Advance to next quad. in glxy
	CPI	0B4H		;This end of line?
	JNZ	GL2		;No, set next quad. in msg
	XCHG			;Save galaxy pointer
	MVI	L,080H		;Print current line of galaxy
	CALL	MSG
	MVI	H,031H
	CALL	NT1		;Print dividing line
	MOV	A,E		;Fetch galaxy pointer
	CMP	H		;End of galaxy printed? =0?
	JZ	CMND		;Yes, return command input
	XCHG			;No, set up galaxy pointer
	JMP	GL1		;Continue printout

; ------------------------------------------------------------------------------
	ORG	0F00H           ; Decimal = 3840

	DB	000000000b,000000001b,000000100b,000100011b,000001010b,000000011b,000000111b,000000000b
	DB	000000000b,000011010b,000100011b,000000101b,000000011b,000010100b,000010110b,000010010b
	DB	000000000b,000000000b,000000000b,000000000b,000000000b,000000101b,000000100b,000010111b
	DB	000000101b,000000001b,000010100b,000000000b,000000000b,000000100b,000000101b,000000000b
	DB	000000111b,000000010b,000010001b,000001001b,000000000b,000000100b,000000000b,000000000b
	DB	000100011b,000000000b,000000010b,000100100b,000000000b,000000000b,000000011b,000000111b
	DB	000000000b,000010101b,000000000b,000000101b,000001100b,000000000b,000000010b,000000110b
	DB	000010101b,000000000b,000000011b,000000010b,000010011b,000000000b,000110100b,000000011b
	DB	000000111b,000000001b,000000000b,000000000b,000000000b,000000011b,000010101b,000000000b
	DB	000000000b,000000100b,000000000b,000011111b,000000100b,000000001b,000000011b,000000010b
	DB	000000011b,000010100b,000000000b,000000000b,000000000b,000010110b,000001101b,000000000b
	DB	000000000b,000000100b,000010011b,000000011b,000000000b,000000000b,000000000b,000010100b
	DB	000001011b,000000001b,000010101b,000010011b,000000000b,000000000b,000000100b,000000011b
	DB	000000111b,000000000b,000000000b,000000000b,000011101b,000000100b,000000000b,000010110b
	DB	000000000b,000010011b,000010101b,000000000b,000000000b,000000100b,000000110b,000000010b
	DB	000000011b,000010101b,000000000b,000000000b,000010101b,000000000b,000100111b,000000000b

	ORG	0F80H

; Test status of input device for character
; Sets sign flag if character coming in
INPCK:
	CALL	IOST		;CF
	ANI	0FFH		;Non-zero?
	JZ	INPCK1
	ORI	0FFH		;Set sign flag
INPCK1:
	RET


; ------------------------------------------------------------------------------
; Input a character from the system
; Return character in register 'A'
INPUT:
	;CALL	IOIN
        in      SIOCTL          ; Stacy, check for input character.
        cpi     0
	jz	INPUT           ; No input character
        OUT	SIOCTL          ; Stacy, IOIN also echos the character.
	RET

; Output a character to the system
; Character to output is contained in 'A'
PRINT:
	PUSH	B		;Save BC
	ANI	7FH		;String parity bit
	CALL	IOOUT
	POP	B		;Restore BC
	RET

; Stack
	DS	32		;Stack Area
STACK:	EQU	$

;******************************************************
; 88-2SIO CHANNEL A SERIAL INTERFACE FUNCTIONS        *
;******************************************************

SIOCTL	EQU	10H		;88-2SIO CONTROL PORT
SIODAT	EQU	11H		;88-2SIO DATA PORT
SIOTXR	EQU	02H		;XMIT READY MASK
SIORDR	EQU	01H		;RCV READY MASK

;******************************************************
; IOINI - RESET AND INITIALIZE 2SIO PORT A            *
;******************************************************
IOINI:	MVI	A,3		;RESET ACIA
	OUT	SIOCTL
	MVI	A,015H		;RTS ON, 8N1
	OUT	SIOCTL
	RET

;******************************************************
; IOST - TEST SERIAL PORT A FOR A CHARACTER. RETURN   *
; WITH A=0 AND Z SET IF NO CHARACTER AVAILABLE.       *
;******************************************************
IOST:	IN	SIOCTL		;WAIT FOR A CHARACTER
	ANI	SIORDR		;SET Z, CLEAR A IF NO CHR		
	RET

;******************************************************
; IOIN - RETURN A CHARACTER FROM THE SERIAL PORT IN A.*
; MSB IS CLEARED.                                     *
; Z IS CLEARED UNLESS RECEIVED CHR IS A NULL.         *
;******************************************************
IOIN:	CALL	IOST		;WAIT FOR A CHARACTER
	JZ	IOIN

	IN	SIODAT		;A = RECEIVED CHARACTER
	ANI	07FH		;STRIP PARITY, CLEAR Z UNLESS NULL
	JMP	IOOUT		;ECHO CHARACTER

;******************************************************
; IOOUT - SEND THE CHARACTER IN A OUT THE SERIAL PORT.*
; CLOBBERS C.                                         *
;******************************************************
IOOUT:	MOV	C,A
WLOOP:
        ;IN	SIOCTL		;WAIT UNTIL OK TO XMIT
	;ANI	SIOTXR
	;JZ	WLOOP

	MOV	A,C		
	OUT	SIODAT		;SEND THE CHARACTER

	RET


CONINI:
        CALL    IOINI
	RET
                                ; --------------------------------------
                                ; Stacy, I've added.
    println:
                mvi a,'\r'      ; Print CR and NL characters.
                out 2
                mvi a,'\n'
                out 2
                ret

	END
                                ; --------------------------------------
                                ; --------------------------------------
                                ; Notes to get the program to run.

--------------------------------------------------------------------------------
; Program is stored on the SD card:
;   ++ File:      00001001.BIN  4608
;
; Start byte, set switches: 
++    2470:00001001 10100110: 00110001 : 31:061 > opcode: lxi sp,STACK
;
; Enter the following to get to the start byte: 12578bx
; Then, hit Enter to show the following. The data byte is correct: 00110001.
;
?- + x, EXAMINE: 2470
?- 
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    *   *   .    .   .   *
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   *   .   .   *    *   .    *   .   .    *   *   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   ^   v   v   ^    ^   v    ^   v   v    ^   ^   v

--------------------------------------------------------------------------------
Now, run the program.

+ Ready to receive command.
?- + r, RUN.
?- + runProcessor()

DO YOU WANT TO GO ON A SPACE VOYAGE? j
YOU MUST DESTROY 85 ALIEN SHIPS IN 90 STARDATES WITH 6 SPACE STATIONS
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3088
2                         CONDITION GREEN
3                         QUADRANT  8 7
4                         SECTOR    6 6
5                         ENERGY    g808
6                         TORPEDOES 08
7                         SHIELDS   g808
8                        
 -1--2--3--4--5--6--7--8-
COMMAND?

--------------------------------------------------------------------------------
COMMAND options:
O. SPACE SHIP movement
1. Short range scanner
2. Long  range scanner
3. Galaxy display 
4. Shields
5. Phasors
6. TORPEDO

------------------------------------------
O. SPACE SHIP movement

Directions:
   3
 4   2
5     1
 6   8
   7

COMMAND?0
COURSE (1-8.5)? 3.2
COURSE (1-8.5)? 3.0
WARP FACTOR (0.1-7.7)? 1.0
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3091
2                         CONDITION GREEN
3                         QUADRANT  1 8
4                         SECTOR    2 7
5                         ENERGY    l:68
6                         TORPEDOES 08
7                         SHIELDS   g808
8                        
 -1--2--3--4--5--6--7--8-

------------------------------------------
1. Short range scanner

COMMAND?1
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3092
2                         CONDITION GREEN
3                         QUADRANT  5 5
4                         SECTOR    1 8
5                         ENERGY    l=08
6                         TORPEDOES 08
7                         SHIELDS   g808
8                        
 -1--2--3--4--5--6--7--8-

------------------------------------------
2. Long  range scanner

COMMAND?2
L.R. SCAN FOR QUADRANT  5 5
-------------------
1     1     1     1
-------------------
1 123 1     1     1
-------------------
1     1     1     1
-------------------

123 : 1 alien ship, 2 space stations, 3 stars

------------------------------------------
3. Galaxy display 

COMMAND?3
GALAXY DISPLAY
-------------------------------------------------
1 002   000   002   105   004   002
-------------------------------------------------
1 004   000   000   000   000   004
-------------------------------------------------
1 004   103   007   103   000   000
-------------------------------------------------
1 304   000   117   015   004   000
-------------------------------------------------
1 000   007   000   007   000   005
-------------------------------------------------
1 012   103   001   001   000   000
-------------------------------------------------
1 003   003   000   103   105   000
-------------------------------------------------
1 105   007   003   203   003   105
-------------------------------------------------

--------------------------------------------------------------------------------
For some reason, the values are incorrect.
The following change allows the energy to always be sufficient.

; Pointer to energy level: 050H = 1010000 80 
; CKMN:
;	MVI	L,050H		;Set pointer to main energy
; Change rnz to ret, then always have enough energy.
++    2439:00001001 10000111: 11000000 : C0:300 > opcode: rnz
++    2442:00001001 10001010: 11001001 : C9:311 > opcode: ret
;

CKMN:
	MVI	L,050H		;Set pointer to main energy
CK1:
	MOV	A,M		;Fetch most significant half
	DCR	L		;Set pointer to least signif. half
	CMP	D		;Is most significant half = 0?
	RNZ			;No, return with flags set up
CK2:
	MOV	A,M		;If greater than or =, ret. with
	CMP	E		;'C' flag reset, if less than
	RET			;Return with 'C' flag set


                                ; 
                                ; --------------------------------------
