                                    ; ------------------------------------------------
                                    ; Enter counter mode and display counter value for counter index in register A.
                                    ; 
                                    ; ------------------------------------------------
                ORG 0               ;
                                    ; ------------------------------------------------
                MVI A,6             ; Counter index file number.
                OUT 25              ; Enter counter mode and display counter value for counter index in register A.
                HLT                 ;
                                    ; ------------------------------------------------

    SENSE_SW    EQU     255         ; Input port address: toggle sense switch byte, into register A.
    Begin:
                IN SENSE_SW         ; Get the Sense switches value into register A.
                                    ;   1 - first response to a ticket.
                                    ;   2 - 2nd or more, response to a ticket.
                                    ; ------------------------------------------------
                OUT 25              ; Get the current value for the counter index, increment it and store it.
                HLT                 ;
                                    ; ------------------------------------------------
                JMP Begin
                END
