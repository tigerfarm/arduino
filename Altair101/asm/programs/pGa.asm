; ------------------------------------------------
;
; Stacy, be a challenge to get this to compile using my assembler.
;
; I/O ports:
; SIOCTL    EQU 10H   ;88-2SIO CONTROL PORT
;   IN	SIOCTL
; SIODAT    EQU 11H   ;88-2SIO DATA PORT
;   OUT	SIODAT
;
; Original site:
;   https://github.com/deltecent/scelbi-galaxy

; ----------------------------------------------
; Programing challenge:
; + Done: Compile using my assembler.
; + Done: Interactive basics: input and output.
; + Done: Run the start to get the initial prompt and "n" to halt the program.
; + Get the complete program to run by adding section and section.
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
                                ; ----------------------------------------------
        JMP     GALAXY          ; Stacy, allow using start from zero.
                                ; ----------------------------------------------

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
	DB	000 		;Tern porary storage

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

	ORG	0100H	; Next page

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

	ORG	0500H

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
                                ; Up to line 355 in pGalaxy80.asm
                                ; ----------------------------------------------
SSPLS:SSPLS:
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
                                ; Line 382 in pGalaxy.asm
                                ; ----------------------------------------------
                                ; ...
GLXCK:
	MOV	D,H		;Space station count = 0;
	MOV	C,H		;Alien ship count = 0;
	MVI	L,0C0H		;Fetch quadrant contents
                                ; ...
GLXCK1:
                                ; ----------------------------------------------
                                ; Program starts running from here.
                                ; Line 1128 in pGalaxy.asm
GALAXY:
	LXI	SP,STACK	;Set stack pointer
	; CALL	CONINI		;Initialize Console I/O
	LXI	H,MSGDYW
	CALL	MSG		;Print introduction
START:
	CALL	RN		;Increment random number
        ;
	;CALL	INPCK		;Input yet?
	;JP	START		;No, continue wait
	CALL	INPUT		;Input character
	cpi	0
	jz	START           ; No input character
        call    PRINT
        ;
	cpi	'n'		;No, stop game?
	JZ	OVER		;Yes, vanish from galaxy
	CPI	'N'		;No, stop game?
	JZ	OVER		;Yes, vanish from galaxy
	MVI	E,00C0H		;Set pointer to galaxy storage
                                ; ----------------------------------------------
        jmp START
                                ; ----------------------------------------------
                                ; ...
OVER:
	LXI	H,MSGCHK	;Print "CHICKEN"
	CALL	MSG
        call    println
	HLT			;Halt

                                ; ----------------------------------------------
;	ORG	0F80H

; Test status of input device for character
; Sets sign flag if character coming in
INPCK:
	CALL	IOST		;CF
	ANI	0FFH		;Non-zero?
	JZ	INPCK1
	ORI	0FFH		;Set sign flag
INPCK1:
	RET

; Input a character from the system
; Return character in register 'A'
INPUT:
	;CALL	IOIN
        in      SIOCTL          ; Stacy, check for input character.
	RET

; Output a character to the system
; Character to output is contained in 'A'
PRINT:
	PUSH	B		;Save BC
	ANI	7FH		;String parity bit
	CALL	IOOUT
	POP	B		;Restore BC
	RET

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
                                    ; Assembler needs updates.

                                    ; 
                                    ; --------------------------------------
