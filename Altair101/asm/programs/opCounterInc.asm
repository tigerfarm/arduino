                                    ; ------------------------------------------------
                                    ; Increment a file counter for the sense switch value counter file number.
                                    ; 
                                    ; ------------------------------------------------
                ORG 0               ;
    SENSE_SW    EQU     255         ; Input port address: toggle sense switch byte, into register A.
    Begin:
                IN SENSE_SW         ; Get the Sense switches value into register A.
                                    ;   1 - first response to a ticket.
                                    ;   2 - 2nd or more, response to a ticket.
                                    ; ------------------------------------------------
                OUT 21              ; Get the current value for the counter index, increment it and store it.
                HLT                 ;
                                    ; ------------------------------------------------
                JMP Begin
                END
