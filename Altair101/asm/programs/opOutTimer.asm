                                    ; ------------------------------------------------
                                    ; Test timer OUT ports.
                                    ; 
                                    ; ------------------------------------------------
    Begin:
                MVI A,2             ; Set timer minutes.
                OUT 26              ; Go into timer mode, where the time is the register A value. Stay in timer after the time is complete.
                                    ; ------------------------------------------------
                MVI A,4             ; Set timer minutes.
                OUT 27              ; Go into timer mode, where the time is the register A value. Exit when the timer is complete.
                                    ; ------------------------------------------------
                HLT                 ;
                JMP Begin
                END
