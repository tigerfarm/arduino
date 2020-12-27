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
    SIOCTL  EQU 10H                 ;88-2SIO CONTROL PORT
    IOST:   IN SIOCTL
                                    ;
    ;        MVI M,' '
    ;        XRA A
                                    ;
    ;CR  EQU 0DH
    ;LF  EQU 0AH
                                    ;
        ;DB CR,LF
    ;    MSGDYWa: DB CR
    ;    MSGDYWa: DB LF
                                    ;
    ;MSGCHK: DB CR,LF
;        DB 'CHICKEN!'
;        DB 0                        ; "0" End of string identifier.
;        LXI     H,MSGCHK            ;Print "CHICKEN"
;        CALL    MSG
                                    ;
;   DB  000000000b,000000001b,000000100b,000100011b,000001010b,000000011b,000000111b,000000000b
;   DB	CR,LF,'1',' ',' ',' ',' ',' '
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
+ Parse |IOST: IN SIOCTL|
++ parseLine componets theRest|IN SIOCTL|
++ parseLine componets part1asIs|IOST:| part1|iost:| part2|IN| theDirective|in| theRest|SIOCTL|
++ parseLabel, Name: IOST, Address: 4030
++ parseLabel, Name: IOST, Address: 4030
-- Error2, programTop: 4030 -- INVALID, Opcode: SIOCTL 11001001
                                    ;
                                    ; I use for listing a successful sample run
                                    ; ------------------------------------------
Sample line:
    okay    db  'okay, yes?'
Parsed:
++       4:00000000 00000100: 01101111 : 6F:157 > databyte: okay : o : 111
++       5:00000000 00000101: 01101011 : 6B:153 > databyte: okay : k : 107
++       6:00000000 00000110: 01100001 : 61:141 > databyte: okay : a : 97
++       7:00000000 00000111: 01111001 : 79:171 > databyte: okay : y : 121
++       8:00000000 00001000: 00101100 : 2C:054 > databyte: okay : , : 44
++       9:00000000 00001001: 00100000 : 20:040 > databyte: okay :   : 32
++      10:00000000 00001010: 01111001 : 79:171 > databyte: okay : y : 121
++      11:00000000 00001011: 01100101 : 65:145 > databyte: okay : e : 101
++      12:00000000 00001100: 01110011 : 73:163 > databyte: okay : s : 115
++      13:00000000 00001101: 00111111 : 3F:077 > databyte: okay : ? : 63
++      14:00000000 00001110: 11111111 : FF:377 > dbterm: 255
                                    ; 
                                    ; ------------------------------------------
