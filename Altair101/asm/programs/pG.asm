                                ; ----------------------------------------------
                                ; Programing challenge:
                                ; + Done: Compile using my assembler.
                                ; + Get components to run.
                                ; + Get the complete program to run.
                                ;
                                ; ----------------------------------------------
LF	EQU	0AH
CR	EQU	0DH

	ORG	0000H
                                ; ...
                                ; ----------------------------------------------
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

                                ; ----------------------------------------------
	ORG	0500H

                                ; ----------------------------------------------
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
	RET                                ; 
                                ; ----------------------------------------------
                                ;
                                ; ...
                                ; ----------------------------------------------
                                ;
GALAXY:
	LXI	SP,STACK	;Set stack pointer
	CALL	CONINI		;Initialize Console I/O
	LXI	H,MSGDYW
	CALL	MSG		;Print introduction
START:
	CALL	RN		;Increment random number
	CALL	INPCK		;Input yet?
	JP	START		;No, continue wait
	CALL	INPUT		;Input character
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
	HLT			;Halt

                                ; ----------------------------------------------
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


; Input a character from the system
; Return character in register 'A'
INPUT:
	CALL	IOIN
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
        IN	SIOCTL		;WAIT UNTIL OK TO XMIT
	ANI	SIOTXR
	JZ	WLOOP

	MOV	A,C		
	OUT	SIODAT		;SEND THE CHARACTER

	RET


CONINI:
        CALL    IOINI
	RET

	END
                                    ; --------------------------------------
                                    ; Assembler needs updates.


                                    ; 
                                    ; --------------------------------------