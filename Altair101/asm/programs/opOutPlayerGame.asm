                                        ; ----------------------------------------
                                        ; Test MP3 player out ports.
                                        ; 
                                        ; OUT 13, play MP3 and delay based on the default of 300.
                                        ;  regA is the MP3 file number to be played.
                                        ;
                                        ; OUT 14, play MP3 and delay based on the regB value.
                                        ;  regA is the MP3 file number to be played.
                                        ;  regA is the time in milliseconds, to play the MP3.
                                        ;
                                        ; ----------------------------------------
    Start:
                                        ; ----------------------------------------
        sta     regA
        MVI     A,3                     ; Set: play MP3 file.
        OUT     13                      ; Play game sound effect.
        lda     regA
                                        ;
        HLT
                                        ; ----------------------------------------
HIT:
        sta     regA
        sta     regB
        MVI     A,7                     ; Set: play MP3 file.
        MVI     B,500                   ; Set: play MP3 file for 500 milliseconds.
        OUT     14                      ; Play game sound effect.
        lda     regA
        lda     regB
                                        ;
        HLT
                                        ; ----------------------------------------
                JMP Start
                                        ; ----------------------------------------
regA:   DB      0
regB:   DB      0
                                        ; ----------------------------------------
                end
