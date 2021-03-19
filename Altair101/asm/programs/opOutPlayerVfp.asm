                                        ; ------------------------------------------------
                                        ; Test MP3 player out ports.
                                        ; 
                                        ; OUT 10, MP3 play options.
                                        ;  regA == 0            Pause
                                        ;  regA == B11111111    Start play with play loop OFF, play the current MP3.
                                        ;  Else                 Play the regA MP3 file number, once.
                                        ;
                                        ; OUT 11, MP3 play loop options.
                                        ;  regA == 0            Pause
                                        ;  regA == B11111111    Start play with play loop ON, play the current MP3.
                                        ;  Else                 Loop play the regA MP3.
                                        ; ------------------------------------------------
    Start:
                                        ; ------------------------------------------------
                MVI A,10                ; Set the MP3 file number to play.
                OUT 10                  ; 10 is for playing an MP3 file once.
                org 100                 ; Enough NOPs to allow silence after completing the playing of the MP3.
                                        ; ------------------------------------------------
                MVI A,9                 ; Set the MP3 file number to play a long playing MP3.
                OUT 10
                org 160                 ; Enough NOPs to have the MP3 play for a while.
                                        ; ------------------------------------------------
                MVI A,0                 ; Pause the currently playing MP3 file.
                OUT 10
                org 260                 ; NOPs to show quiet after the pause and before the next play.
                                        ; ------------------------------------------------
                MVI A,255               ; Continue playing the current MP3.
                OUT 10
                org 400                 ; Enough NOPs to have the MP3 play for a while.
                                        ; ------------------------------------------------
                HLT                     ;
                                        ; ------------------------------------------------
                MVI A,2                 ; MP3 file to play for the tests.
                OUT 11                  ; Play the MP3 in a loop.
                org 600                 ; NOPs to give time to complete the playing of the MP3.
                                        ; ------------------------------------------------
                MVI A,255               ; Start playing the MP3 file that was looping.
                OUT 11
                org 700                 ; NOPs to give time to complete the playing of the MP3.
                                        ; ------------------------------------------------
                MVI A,8                 ; MP3 file to play for the tests.
                OUT 12                  ; Play MP3 to completion before moving to the next opcode.
                                        ; ------------------------------------------------
                HLT                     ;
                JMP Start
                                        ; --------------------------------------
                end
