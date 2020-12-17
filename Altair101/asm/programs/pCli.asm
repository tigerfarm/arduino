                                        ; --------------------------------------
                                        ; Command line interface program
                                        ;
                                        ; ASCII
                                        ;   https://en.wikipedia.org/wiki/ASCII
                                        ; See the section: Control code chart.
                                        ;   Examples,
                                        ;   3 ^c End of Text: Ctrl+c is ASCII 3.
                                        ;   7 ^G Bell
                                        ;   12 ^L FF (form feed)
                                        ;
                                        ; The VT100 code page:
                                        ;   https://en.wikipedia.org/wiki/VT100_encoding
                                        ; The first 128 of which are identical to ASCII
                                        ;
                                        ; VT100 reference:
                                        ;   http://ascii-table.com/ansi-escape-sequences-vt-100.php
                                        ;
                                        ; --------------------------------------
                                        ; --------------------------------------
                lxi sp,512              ; Set stack pointer.
    Start:
                lxi h,StartMsg
                call printStr
                lxi h,EnterMsg
                call printStr
    startNewline:
                mvi a,'\r'
                out INPUT_PORT
                mvi a,'\n'
                out INPUT_PORT
                lxi h,thePrompt         ; Print the prompt.
                call printStr
                mvi a,0                 ; Initialize cursor position counter.
                sta cp
                                        ; --------------------------------------
                                        ; Get an input byte
                                        ; + Ctrl+c to exit.
                                        ; + Process control keys
                                        ; + Output printable key characters to screen.
    GetByte:
                in INPUT_PORT           ; Get input byte value into register A.
                cpi 0                   ; No character input, nothing to print out.
                jz GetByte
                                        ; ---------------
                                        ; Handle special keys.
                cpi 3                   ; Ctrl+c will exit this loop.
                jz ExitGetByte
                cpi 13                  ; If carriage return, send line feed and print the prompt.
                jz startNewline
                cpi 8                   ; Handle Backspace.
                jz backSpace
                cpi 127                 ; Handle DEL.
                jz backSpace
                cpi 12                  ; Ctrl+l, FF to have the screen cleared.
                jz clear
                                        ; ---------------
                                        ; Only printable characters
                cpi 32
                jnc GetByte             ; Ignore less than 32.
                cpi 126
                jc GetByte              ; Ignore greater than 126.
                                        ; ---------------
                out INPUT_PORT          ; Else, out the character and get a new one.
                call inrCp              ; Increment the cursor position.
                jmp GetByte
                                        ; ---------------
    backSpace:
                lda cp
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
                call dcrCp
                jmp GetByte
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
                                        ; Output print processes.
                                        ; 
                                        ; --------------------------------------
        printPrompt:
                mvi a,'?'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
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
        inrCp:                          ; Increment the cursor position counter
                lda cp
                inr a
                sta cp
                ret
        dcrCp:                          ; Decrement the cursor position counter
                lda cp
                dcr a
                sta cp
                ret
                                        ; --------------------------------------
                                        ; Move the cursor home and clear the screen: '\033[H\033[2J'
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
                sta cp,0
                ret
                                        ; --------------------------------------
                                        ;
    StartMsg    db      '\r\n+++ Welcome to the machine.\r\n'
    EnterMsg    db      '+ Press keys. Ctrl+c to exit.\r\n> '
    thePrompt   db      '> '
    ExitMsg     db      '\r\n+ Later.\r\n'
                                        ;
    TERMB       equ     0ffh            ; String terminator.
    esc         equ     27              ; Escape character, which is 0x1B (hex).
    cp          ds      1               ; Cursor position
                                        ; --------------------------------------
                                        ; Use port 3 for testing, which is the default serial port.
                                        ; Use port 2 for the Serial2 port.
    PRINT_PORT  equ     2               ; Output port#. 
    INPUT_PORT  equ     2               ; Input port#.
                                        ; --------------------------------------
                end
                                        ; --------------------------------------
