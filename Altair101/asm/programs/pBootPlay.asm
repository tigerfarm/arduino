                                    ; ------------------------------------------------
                                    ; Play MP3 files.
                                    ; 
                                    ; ------------------------------------------------
    Begin:
                                    ; ------------------------------------------------
                MVI A,1             ; MP3 file: Operational.
                OUT 12              ; Single play, wait until completed.
                HLT                 ;
                                    ; ------------------------------------------------
                MVI A,10            ; MP3 file: Good morning.
                OUT 12              ; Single play.
                HLT                 ;
                                    ; ------------------------------------------------
                MVI A,8             ; MP3 file: Play a game.
                OUT 12              ; Single play.
                HLT                 ;
                                    ; ------------------------------------------------
                JMP Begin
                END
