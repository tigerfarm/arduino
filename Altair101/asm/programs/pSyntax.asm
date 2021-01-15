                                        ; ------------------------------------------
                                        ; Sample program lines that are parsed by asmProcessor.java
                                        ;
                                        ; ------------------------------------------
                                        ; Special cases for Galaxy80.asm.
;    CR  EQU 0DH
;    LF  EQU 0AH
                                        ; --------------------------------------
;                lxi sp,1024
                jmp Start
                                        ; --------------------------------------
NUML:	DB	000                     ;Temporary storage
NUMH:	DB	000                     ;Temporary storage
                                        ; ------------------------------------------
            ; mov a,l                   ; For testing, takes last address before "end".
Start:
        lda     NUML
	MOV	L,A                  ; Pointer to Low order byte storage
        lda     NUMH
	MOV	L,A                  ; Pointer to Low order byte storage


            end
                                    ; Lines after the "end" line are ignored.
                                    ; ------------------------------------------


                                    ; ------------------------------------------
