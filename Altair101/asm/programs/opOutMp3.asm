                                    ; ------------------------------------------------
                                    ; Test MP3 player out ports.
                                    ; 
                                    ; ------------------------------------------------
    Begin:
                MVI A,4             ; MP3 file to play for the tests.
                OUT 10              ; Single play.
                org 64              ; NOPs to give time to complete the playing of the MP3 and have silence after until the next MP3 is played.
                                    ; ------------------------------------------------
                MVI A,2             ; MP3 file to play for the tests.
                OUT 11              ; Play the MP3 in a loop.
                org 128             ; NOPs to give time to complete the playing of the MP3.
                                    ; ------------------------------------------------
                MVI A,8             ; MP3 file to play for the tests.
                OUT 12              ; Play MP3 to completion before moving to the opcode.
                                    ; ------------------------------------------------
                HLT                 ;
                JMP Begin
                END
