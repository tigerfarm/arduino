                                    ; ------------------------------------------------
                                    ; Time log program
                                    ; 
                                    ; ------------------------------------------------
                ORG 0               ;
    SENSE_SW    equ     255         ; Input port address: toggle sense switch byte, into register A.
    Begin:
                MVI A,12            ; Timer minutes
                OUT 20              ; Run timer.
                MVI A,4             ; MP3 file number.
                OUT 20              ; Play the MP3.
                OUT 52              ; Flash success lights.
                ORG 24              ; Add NOPs to allow the MP3 to play.
                                    ; ------------------------------------------------
                HLT                 ; Set Sense switches to Counter index number.
                IN 255              ; Get the Sense switches value into register A.
                                    ;   1 - first response to a ticket.
                                    ;   2 - 2nd or more, response to a ticket.
                                    ; ------------------------------------------------
                OUT 21              ; Get the current value for the counter index, increment it and store it.
                HLT                 ;
                                    ; ------------------------------------------------
                OUT 20              ; Set the counter index to register A, for input.
                IN 21               ; Get counter value into register A.
                                    ; ------------------------------------------------
                INR A               ; Increment the counter.
                OUT 21              ; Store the incremented value to the counter index.
                HLT                 ;
                                    ; ------------------------------------------------
                JMP Begin
                END
