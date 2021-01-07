                                    ; ------------------------------------------
                                    ; Sample program lines that are parsed by asmProcessor.java
                                    ;
                                    ; ------------------------------------------
                                    ; The single indent ";" is for tested lines.
    ;
                                    ; No indent ";" is for non-tested lines.
;
                                    ; ------------------------------------------
                                    ; Lines to ignore.
                                    ;
                                    ; Blank line

                                    ; Comment line
                                    ;
                                    ; ------------------------------------------
                                    ; Comments are removed from lines
                                    ;
    ;        jmp Start               ; Line comment.
                                    ;
                                    ; Starting address. Pad with zero up to the ORG value.
    ;        ORG 0006H
                                    ;
            mov a,b                 ; For testing, takes address 0.
                                    ; ------------------------------------------
                                    ; Label line.
                                    ;
    ;Start:
    ;        hlt
                                    ; ------------------------------------------
                                    ; Label followed by an opcode.
                                    ;
    ;Halt:       hlt
                                    ; ------------------------------------------
                                    ; Directive followed by a string.
                                    ;
    ;        DB	' STARDATE  300'
    ;        DB  0
    ;        DB  2
    ;        DB  0FFH
                                    ;
    ;            lxi h,thePrompt         ; Print the prompt.
    ;thePrompt   db      '> '
    ;            db      0
                                    ; ------------------------------------------
                                    ; ------------------------------------------
                                    ; Labels with ":"
                                    ; Label + Directive followed by a byte or string.
    ;MSGSDP: DB  '0'
    ;        lxi h,MSGSDP
    ;DQUAD:  DB  010
    ;        lxi h,DQUAD
    ;COMMA:  DB  ','
    ;        lxi h,COMMA

    ;MSGSSS: DB  '  SPACE STATIONS'
    ;        lxi h,MSGSSS
                                    ;
    ;Data0:  ds  3                   ; 3 bytes at this location.
    ;        lxi h,Data0
    ;ct:                             ; Label to address the storage bytes.
    ;        ds 2                    ; Bytes to store an address.
    ;        lxi h,ct                ; Set the H:L register pair to the line buffer address.
                                    ;
    ;stack:  equ     $
    ;        lxi h,stack             ; Set the H:L register pair to the line buffer address.
                                    ;
    ;ALNMSK: EQU 2
    ;mvi     a,ALNMSK
                                    ; Test binary.
    ;STNMSK: EQU 00001000b
    ;mvi     a,STNMSK
                                    ; ------------------------------------------
                                    ; Label without ":"
                                    ;
    ;Addr1   equ 128
    ;mvi     a,Addr1
                                    ;
    ;okay    db  'okay, yes?'
    ;        lxi h,okay
                                    ;
    ;lbc     ds 2                    ; Place to store an address.
    ;        lxi h,lbc               ; Set the H:L register pair to the line buffer address.
                                    ; ------------------------------------------
                                    ; Opcode statements from Galaxy80.asm
;    IOINI:  MVI A,3                 ;RESET ACIA
                                    ;
;    LOCSET:
;    CNZ    LOCSET
                                    ;
    ;XRI    0FFH
    ;ORI    0FFH
    ;ANA     A
    ;SBB     B
;    XCHG
;    RM
;    RZ
;    RNZ
;    RLC
                                    ;
;    CALL   TWO
;    RET
                                    ;
                                    ; ------------------------------------------
                                    ; Special cases for Galaxy80.asm.
                                    ;
    CR  EQU 0DH
    LF  EQU 0AH
                                    ;
;MSGTTY:	DB	CR,LF
;  	DB	'TORPEDO TRAJECTORY(1-8.5) : '
;  	DB	0
;        db      '-'
;MSGASD:	DB	CR,LF
; 	DB	'ALIEN SHIP DESTROYED'
;  	DB	0
        db      '-'

DB	CR,LF,'O. SPACE SHIP movement'

;	RET
;SSPLS:
;	MVI	E,0F7H		;Mask to delete space station
;	JMP	PLS		;Delete excess space station
;PLS:
;	HLT
                                    ;
    ;MSGDYW:
    ;   DB '\n'
    ;    DB CR,LF
    ;   DB 'abc\n'
    ;   DB 0
    ;thePrompt   db      '> '
    ;            db      0
                                    ;
    ;MSGNEL: DB CR,LF
    ;    DB CR,LF
                                    ;
;       DB 'DO YOU WANT TO GO ON A SPACE VOYAGE? '
;       DB 0
                                    ;
    ;        DB 6
    ;        DB  ' STARDATE  300'
                                    ;
    ;        DB CR,LF
    ;        DB CR,LF,' ',' ','1'
    ;        DB 000000000b,000000001b,000000100b,000100011b,000001010b,000000011b,000000111b,000000000b
    ;        DB CR,LF,'1',' ',' ',' ',' ',' '
    ;        DB ' ',' ','1'
                                    ;
    ;        SUI ':'
                                    ;
    ;SIOCTL  EQU 10H                 ;88-2SIO CONTROL PORT
    ;IOST:   IN SIOCTL
                                    ;
    ;        MVI M,' '
    ;        XRA A
                                    ;
        ;DB CR,LF
    ;    MSGDYWa: DB CR
    ;    MSGDYWa: DB LF
                                    ;
    ;MSGCHK: DB CR,LF
    ;    DB 'CHICKEN!'
    ;    DB 0                        ; "0" End of string identifier.
;        LXI     H,MSGCHK            ;Print "CHICKEN"
;        CALL    MSG
                                    ;
                                    ; ------------------------------------------
                                    ; Special case: end file processing
                                    ;
            mov a,l                 ; For testing, takes last address before "end".
            end
                                    ; Lines after the "end" line are ignored.
                                    ; ------------------------------------------
                                    ;
                                    ; ------------------------------------------
                                    ; Printing a string with an end identifier of "0".
MSG:
	MOV	A,M		;Fetch character
	ANA	A		;End of message?
	RZ			;Yes, return
 	;CALL	PRINT		;No, print character
 	INX	H		;Increment message pointer
	JMP	MSG		;Continue printout
                                    ;
                                    ; ------------------------------------------
                                    ;

- Number of errors = 5
- List Error Messages:
-- 255:   	DB	'MISSION FAILED, YOU HAVE RUN OUT	OF STARDATES'
-- 620 :- Error, immediate label not found:  YOU HAVE RUN OUT OF STARDATES'.
-- 258:   	DB	'KA-BOOM, YOU CRASHED INTO A STAR. YOUR SHIP IS DESTROYED'
-- 630 :- Error, immediate label not found:  YOU CRASHED INTO A STAR. YOUR SHIP IS DESTROYED'.
-- 261:   	DB	'YOU MOVED OUT	OF THE GALAXY, YOUR SHIP IS LOST..LOST'
-- 660 :- Error, immediate label not found:  YOUR SHIP IS LOST..LOST'.
-- 290:   	DB	'CONGRATULATIONS, YOU HAVE ELIMINATED ALL OF THE ALIEN SHIPS'
-- 891 :- Error, immediate label not found:  YOU HAVE ELIMINATED ALL OF THE ALIEN SHIPS'.
-- 304: MSGSEC:	DB	' , : '
-- 976 :- Error, immediate label not found:  : '.
+ End of list.

****** Issue with DB strings containing a comma ",".

+ Parse |DB 'MISSION FAILED, YOU HAVE RUN OUT OF STARDATES'|
++ parseLine components theRest|'MISSION FAILED, YOU HAVE RUN OUT OF STARDATES'|
++ parseLine components part1asIs|DB| part1|db| part2|'MISSION FAILED, YOU HAVE RUN OUT OF STARDATES'| theDirective|| theRest||
++ parseDb( theLabel: , theValue: 'MISSION FAILED, YOU HAVE RUN OUT OF STARDATES' )
++ parseDb, cq == 0 ( theLabel: , theValue: 'MISSION FAILED, YOU HAVE RUN OUT OF STARDATES' ) cq=46 l=47

 012345678901234567890123456789012345678901234567890
'MISSION FAILED, YOU HAVE RUN OUT OF STARDATES'

++ parseLine2, DB bytes, theByte|'MISSION FAILED|
++ parseDbValue(, string of bytes: 'MISSION FAILED
++ parseDbAdd theLabel:: theValue:M:
++ parseDbAdd theLabel:: theValue:I:
++ parseDbAdd theLabel:: theValue:S:
++ parseDbAdd theLabel:: theValue:S:
++ parseDbAdd theLabel:: theValue:I:
++ parseDbAdd theLabel:: theValue:O:
++ parseDbAdd theLabel:: theValue:N:
++ parseDbAdd theLabel:: theValue: :
++ parseDbAdd theLabel:: theValue:F:
++ parseDbAdd theLabel:: theValue:A:
++ parseDbAdd theLabel:: theValue:I:
++ parseDbAdd theLabel:: theValue:L:
++ parseDbAdd theLabel:: theValue:E:
++ parseLine2, DB bytes, theByte| YOU HAVE RUN OUT OF STARDATES'|

+ getVariableValue, findName:  YOU HAVE RUN OUT OF STARDATES'
+ getVariableValue, returnString: -1
+ Not found:  YOU HAVE RUN OUT OF STARDATES'.
-- 119: 15 :- Error, immediate label not found:  YOU HAVE RUN OUT OF STARDATES'.
++ parseDbValue( variable name: '', single byte with a value of:  YOU HAVE RUN OUT OF STARDATES' = -1.
++ parseLine, DB multiple comma separated bytes are parsed.

                                    ; ------------------------------------------
