; ------------------------------------------------
; Stacy, be a challenge to get this to compile using my assembler.
;
; ------------------------------------------------
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

	END
