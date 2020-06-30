                                    ; ------------------------------------------------
                                    ; Increment a file counter for the sense switch value counter file number.
                                    ; 
                                    ; ------------------------------------------------
                ORG 0               ;
                                    ; ------------------------------------------------
    Begin:
                MVI A,255           ; Turn playing MP3 on.
                OUT 10              ; Single play.
                MVI A,255           ; Turn playing MP3 on.
                HLT                 ;
                                    ; ------------------------------------------------
                JMP Begin
                END
