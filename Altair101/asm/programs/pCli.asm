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
                                        ; --------------------------------------
                                        ; Set output port and print strings.
                                        ;
                lxi h,StartMsg
                call printStr
                                        ; --------------------------------------
                                        ; Get an input byte and echo it.
                                        ; Ctrl+c to exit.
                lxi h,EnterMsg
                call printStr
    GetByte:
                in INPUT_PORT           ; Get input byte value into register A.
                cpi 0                   ; No character input, nothing to print out.
                jz GetByte
                cpi 3                   ; Ctrl+c will exit this loop.
                jz ExitGetByte
                cpi 13                  ; If carriage return, send line feed and print the prompt.
                jz startNewline
                cpi 8                   ; Handle Backspace.
                jz backSpace
                cpi 127                 ; Handle DEL.
                jz backSpace
                cpi 12                  ; Ctrl+l, FF, clear screen.
                jz clear
                out INPUT_PORT          ; Else, out the character and get a new one.
                jmp GetByte
    startNewline:
                out INPUT_PORT
                mvi a,'\n'
                out INPUT_PORT
                lxi h,thePrompt
                call printStr
                jmp GetByte
    backSpace:                          ; Need logic for these.
                                        ; Esc6n	Get cursor position
                                        ; Esc[r;cH  Move cursor to a specific row(r) and column(c).
                                        ; Maybe: Esc[ValueD	Move cursor left n lines
                ; mvi a,' '
                ; out INPUT_PORT
                jmp GetByte
    clear:
                call clr
                lxi h,thePrompt
                call printStr
                jmp GetByte
                                        ;
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
                                        ; Move home and clear entire screen: '\033[H\033[2J'
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
    StartMsg    db      '\r\n+++ Welcome to the machine.\r\n'
    EnterMsg    db      '\r\n+ Enter keys. Ctrl+c to exit.\r\n> '
    ExitMsg     db      '\r\n+ Later.\r\n'
    TERMB       equ     0ffh            ; String terminator.
                                        ;
    thePrompt   db     '> '
    esc         equ     27              ; Escape character, which is 0x1B (hex).
                                        ; --------------------------------------
                                        ; Use port 3 for testing, which is the default serial port.
                                        ; Use port 2 for the Serial2 port.
    PRINT_PORT  equ     3               ; Output port#. 
    INPUT_PORT  equ     3               ; Input port#.
                                        ; --------------------------------------
                end
                                        ; --------------------------------------
