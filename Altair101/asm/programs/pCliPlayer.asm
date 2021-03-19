                                        ; --------------------------------------
                                        ; Command line interface program
                                        ;   to make player OUT opcode calls.
                                        ;
                                        ; --------------------------------------
                                        ; --------------------------------------
                lxi sp,1024             ; Set stack pointer.
                lxi h,lb                ; Set the H:L register pair to the line buffer.
                shld lbc                ; Store H and L registers into memory, the line buffer counter.
    Start:
                lxi h,StartMsg
                call printStr
                lxi h,Menu0
                call printStr
    startNewline:
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
                lxi h,thePrompt         ; Print the prompt.
                call printStr
                                        ;
                ;lda cpc
                ;jz mtLine               ; If nothing entered in the line (MT line), nothing to process.
                ;call processLine
    mtLine:
                mvi a,0                 ; Initialize cursor line position counter.
                sta cpc
                lxi h,0                 ; Set line buffer address counter to zero.
                shld lbc
                                        ; --------------------------------------
                                        ; Get an input byte
                                        ; + Ctrl+c to exit.
                                        ; + Process other control keys
                                        ; + Output printable key characters to screen.
    GetByte:
                in INPUT_PORT           ; Get input byte value into register A.
                cpi 0                   ; No character input, nothing to print out.
                jz GetByte
                                        ; ---------------
                                        ; Program options
                cpi 'h'
                jz HelpMenu
                cpi '1'
                jz Menu1
                cpi '2'
                jz Menu2
                cpi '3'
                jz Menu3
                cpi '4'
                jz Menu4
                cpi '5'
                jz Menu5
                cpi '6'
                jz Menu6
                cpi '7'
                jz Menu7
                                        ; ---------------
                                        ; Handle special keys.
                cpi 3                   ; Ctrl+c will exit this loop.
                jz ExitGetByte
                cpi 13                  ; If carriage return, send line feed and print the prompt.
                jz startNewline
                cpi 127                 ; Handle Backspace.
                jz backSpace
                cpi 7                   ; Ctrl+g, bell.
                jz bell
                cpi 7                   ; Ctrl+g, bell.
                jz bell
                                        ; ---------------
                                        ; Only printable characters
                cpi 32
                jc GetByte              ; Ignore less than 32.
                cpi 126
                jnc GetByte             ; Ignore greater than 126.
                                        ; ---------------
                                        ; Else,
                out PRINT_PORT          ; output the character and get a new one.
                call inrCpc             ; Increment the cursor position.
                jmp GetByte
                                        ; -----------------------------------------------------------------
    HelpMenu:
                lxi h,Menu0
                call printStr
                jmp startNewline
    Menu1:
                MVI A,9                 ; Set the MP3 file number to play.
                OUT 10                  ; OUT 10 is for playing an MP3 file once.
                jmp startNewline
    Menu2:
                MVI A,0                 ; Pause the currently playing MP3 file.
                OUT 10
                jmp startNewline
    Menu3:
                MVI A,255               ; Continue playing the current MP3.
                OUT 10
                jmp startNewline
    Menu4:
                MVI A,11                ; Set the MP3 file number to play.
                OUT 11                  ; Turn loop ON. OUT 11 is for playing an MP3 file and looping.
                jmp startNewline
    Menu5:
                MVI A,0                 ; Turn loop OFF. Pause the currently playing MP3 file.
                OUT 11
                jmp startNewline
    Menu6:
                MVI A,255               ; Turn loop ON. Continue playing the current MP3 and looping.
                OUT 11
                jmp startNewline
    Menu7:
                MVI A,13
                OUT 12                  ; Play the MP3 until it is complete.
                lxi h,PlayCompleted
                call printStr
                jmp startNewline

                                        ; -----------------------------------------------------------------
    bell:
                out PRINT_PORT          ; Bell tone.
                jmp GetByte
                                        ; ---------------
    backSpace:
                lda cpc
                cpi 0                   ; Don't backspace over the prompt.
                jz GetByte
                                        ;
                mvi a,esc               ; Esc[ValueD : Move the cursor left n lines (left one space).
                out PRINT_PORT
                mvi a,'['
                out PRINT_PORT
                mvi a,'1'
                out PRINT_PORT
                mvi a,'D'
                out PRINT_PORT
                                        ;
                mvi a,' '               ; Overwrite the character with a blank.
                out PRINT_PORT
                                        ;
                mvi a,esc               ; Move back to the blank space.
                out PRINT_PORT
                mvi a,'['
                out PRINT_PORT
                mvi a,'1'
                out PRINT_PORT
                mvi a,'D'
                out PRINT_PORT
                                        ;
                call dcrCpc
                jmp GetByte
                                        ; --------------------------------------
                                        ; Store the key press values to the line buffer and increment the counter.
        inrCpc:
                lhld lbc                ; Load H and L registers from memory.
                mov m,a                 ; Move register A to the H:L address. A -> (HL).
                inx h                   ; Increment H:L register pair.
                shld lbc                ; Store H and L registers into memory.
                                        ;
                lda cpc                 ; Increment the cursor position counter
                inr a
                sta cpc
                ret
                                        ; --------------------------------------
                                        ; Decrement the position counter and store the key press values to the line buffer.
        dcrCpc:
                lda cpc
                dcr a
                sta cpc
                ret
                                        ; --------------------------------------
    clear:
                call clr
                jmp startNewline
                                        ;
                                        ; --------------------------------------
    ExitGetByte:                        ; Exit the input loop.
                lxi h,ExitMsg
                call printStr
                                        ; --------------------------------------
                hlt 
                jmp Start 
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ; Subroutines
                                        ; 
                                        ; --------------------------------------
    processLine:
                lxi h,echoMsg
                call printStr
                call printNewline
                ret
                                        ; --------------------------------------
                                        ; Print a string.
    printStr:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz sPrintDone
                out PRINT_PORT          ; Out register A to the serial port.
                inx h                   ; Increment H:L register pair.
                jmp printStr
    sPrintDone:
                ret
                                        ; --------------------------------------
    printNewline:
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
                ret
                                        ; --------------------------------------
                                        ; Move the cursor home "Esc[H" and clear the screen "Esc[2J".
    clr:
                mvi a,esc
                out PRINT_PORT
                mvi a,'['
                out PRINT_PORT
                mvi a,'H'
                out PRINT_PORT
                mvi a,esc
                out PRINT_PORT
                mvi a,'['
                out PRINT_PORT
                mvi a,'2'
                out PRINT_PORT
                mvi a,'J'
                out PRINT_PORT
                ret
                                        ; --------------------------------------
                                        ;
    StartMsg    db      '\r\n+++ Program to test MP3 player opcode options.\r\n\r\n'
                db      0
    thePrompt   db      '> '
                db      0
    echoMsg     db      '++ Echo not implemented, yet(v1).'
                db      0
    ExitMsg     db      '\r\n+ Later.\r\n'
                db      0
                                        ;
    TERMB       equ     0               ; String terminator.
    esc         equ     27              ; Escape character, which is 0x1B (hex).
    ;
    cpc         ds      1               ; Cursor position counter variable.
    lbc         ds      2               ; Address of last added key value = lb address + cpc-1.
    lb          ds      80              ; Place to store what is typed in, for the current line.
                                        ; Cursor position is also the length of the entered text.
                                        ;
                                        ; --------------------------------------
                                        ; Menu
    Menu0       db      '\r\n+ Player Menu'
                db      '\r\n  -----------'
                db      '\r\n  1. Play MP3 file number.         (play)'
                db      '\r\n  2. Pause MP3.                    (pause)'
                db      '\r\n  3. Continue playing the MP3.     (start)'
                db      '\r\n  ------'
                db      '\r\n  4. Loop play MP3 file number.    (loop)'
                db      '\r\n  5. Pause MP3.                    (pause)'
                db      '\r\n  6. Continue looping the MP3.     (loop)'
                db      '\r\n  ------'
                db      '\r\n  7. Play MP3 until it is completed before moving to the next opcode.'
                db      '\r\n  ------'
                db      '\r\n  Ctrl+c to exit.\r\n'
                db      000h            ; Indicator of the end of a text block.
                                        ;
    PlayCompleted db    '\r\n+ MP3 finished playing.'
                db      000h
                                        ;
                                        ; --------------------------------------
                                        ; When using port 2,
                                        ;   if port 3 is disable, then it defaults to 3, the default serial port.
    PRINT_PORT  equ     3               ; Output port#. 
    INPUT_PORT  equ     3               ; Input port#.
                                        ; --------------------------------------
                end
                                        ; --------------------------------------
                                        ; 
                                        ; 
                                        ; --------------------------------------
