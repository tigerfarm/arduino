                                        ; ------------------------------------------------
                                        ; Test MP3 player out ports.
                                        ; 
    Start:
                                        ; ------------------------------------------------
                MVI A,4                 ; MP3 file to play for the tests.
                OUT 10                  ; Single play.
                org 64                  ; NOPs, time to complete the playing of the MP3 and have silence after.
                                        ; ------------------------------------------------
                MVI A,2                 ; MP3 file to play for the tests.
                OUT 11                  ; Play the MP3 in a loop.
                org 128                 ; NOPs to give time to complete the playing of the MP3.
                                        ; ------------------------------------------------
                MVI A,0                 ; Pause any playing MP3 file.
                OUT 10                  ; Out port 10 or 11.
                org 196                 ; NOPs to give time to complete the playing of the MP3.
                                        ; ------------------------------------------------
                MVI A,255               ; Start any playing MP3 file that was looping.
                OUT 10                  ; Out port 10 or 11.
                org 256                 ; NOPs to give time to complete the playing of the MP3.
                                        ; ------------------------------------------------
                MVI A,8                 ; MP3 file to play for the tests.
                OUT 12                  ; Play MP3 to completion before moving to the opcode.
                                        ; ------------------------------------------------
                HLT                     ;
                JMP Start
                                        ; --------------------------------------
                end
