                                    ; ------------------------------------------------
                                    ; Enter counter mode and display counter value for counter index in register A.
                                    ; 
                                    ; ------------------------------------------------
                ORG 0               ;
    SENSE_SW    EQU     255         ; Input port address: toggle sense switch byte, into register A.
                                    ;
                                    ; ------------------------------------------------
                MVI A,             ; Counter index file number.
                OUT 25              ; Enter counter mode, and display counter value for counter index in register A.
                                    ; Flip the counter switch (AUX2 down) to exit counter mode and continue the program.
                OUT 21              ; Increment counter value for counter index in register A.
                OUT 25              ; Redisplay the counter value.
                                    ;
                                    ; ------------------------------------------------
    Begin:
                HLT                 ; Halt to wait for the Sense switches to be set.
                IN SENSE_SW         ; Get the Sense switches value into register A.
                                    ;   1 - first response to a ticket.
                                    ;   2 - 2nd or more, response to a ticket.
                                    ; ------------------------------------------------
                OUT 25              ; Display Get the current value for the counter index, increment it and store it.
                                    ;
                                    ; ------------------------------------------------
                JMP Begin
                END
