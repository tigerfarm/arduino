                                    ; ------------------------------------------------
                                    ; Increment a file counter for the sense switch value counter file number.
                                    ; 
                                    ; ------------------------------------------------
                ORG 0               ;
    SENSE_SW    EQU     255         ; Input port address: toggle sense switch byte, into register A.
    Begin:
                IN SENSE_SW         ; Get the Sense switches value into register A.
                OUT 26              ; Go into timer mode, where the time is the register A value.
                                    ;
                                    ; ------------------------------------------------
                HLT                 ;
                JMP Begin
                END
