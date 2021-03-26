                                        ; ----------------------------------------
                                        ; Command line interface program
                                        ;   to test escape codes.
                                        ;
; --------------------------------------------------------------------------------
                lxi sp,STACK            ; Set stack pointer
                lxi h,lb                ; Set the H:L register pair to the line buffer.
                shld lbc                ; Store H and L registers into memory, the line buffer counter.
    Start:
                lxi h,StartMsg
                call printStr
                call HelpMenu
; --------------------------------------------------------------------------------
    startNewline:
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
    startPrompt:
                lxi h,thePrompt         ; Print the prompt.
                call printStr
    mtLine:
                mvi a,0                 ; Initialize cursor line position counter.
                sta cpc
                lxi h,0                 ; Set line buffer address counter to zero.
                shld lbc
; --------------------------------------------------------------------------------
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
                                        ; ----------------------------------------
                                        ; Handle special keys.
                cpi 3                   ; Ctrl+c will exit this loop.
                jz ExitGetByte
                cpi 13                  ; If carriage return, send line feed and print the prompt.
                jz processLine
                cpi 127                 ; Handle Backspace.
                jz backSpace
                cpi 12                  ; Ctrl+l, Clear screen.
                jz clear
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
                                        ; ----------------------------------------
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
                                        ; ----------------------------------------
                                        ; Move the cursor home "Esc[H" and clear the screen "Esc[2J".
    clear:
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
                jmp startNewline
                                        ;
; --------------------------------------------------------------------------------
                                        ; Process a command line entry.
    processLine:
                lda cpc
                cpi 0
                jz startNewline         ; If nothing entered in the line (MT line), nothing to process.
                                        ;
                call printNewline
                lxi h,processMsg
                call printStr
                jmp startNewline
                                        ;
; --------------------------------------------------------------------------------
    Menu1:
                ;lxi h,Menu1str
                ;call printStr
                call SCRCLR
                jmp startPrompt
    Menu2:
                call CRSUP6
                jmp startPrompt
    Menu3:
                call SCRD1C
                jmp startPrompt
    Menu4:
                lxi h,Menu4str
                call printStr
                jmp startPrompt
    Menu5:
                lxi h,Menu5str
                call printStr
                jmp startPrompt
    Menu6:
                lxi h,Menu6str
                call printStr
                jmp startPrompt
    Menu7:
                lxi h,Menu7str
                call printStr
                jmp startPrompt
                                        ; ----------------------------------------
                                        ; Menu
    HelpMenu:
                lxi h,MenuStart
                call printStr
                lxi h,Menu1str
                call printStr
                lxi h,Menu2str
                call printStr
                lxi h,Menu3str
                call printStr
                lxi h,MenuBar           ; ------
                call printStr
                lxi h,Menu4str
                call printStr
                lxi h,Menu5str
                call printStr
                lxi h,Menu6str
                call printStr
                lxi h,MenuBar           ; ------
                call printStr
                lxi h,Menu7str
                call printStr
                lxi h,MenuEnd
                call printStr
                jmp startNewline
                                        ; ---------------
    MenuStart   db      '\r\n+ Escape Sequence Test Menu'
                db      '\r\n  -------------------------\r\n'
                db      000h            ; Indicator of the end of a text block.
    Menu1str    db      '  1. Clear screen: Esc[H Esc[2J \r\n'
                db      0
    Menu2str    db      '  2. Move cursor up 6 lines: Esc[6A \r\n'
                db      0
    Menu3str    db      '  3. Clear screen from cursor down: Esc[1B Esc[0J \r\n'
                db      0
    MenuBar     db      '  ------\r\n'
                db      0
    Menu4str    db      '  4. NA\r\n'
                db      0
    Menu5str    db      '  5. NA\r\n'
                db      0
    Menu6str    db      '  6. NA\r\n'
                db      0
    Menu7str    db      '  7. NA\r\n'
                db      0
    MenuEnd     db      '\r\n  ------'
                db      '\r\n  Ctrl+l : clear screen.'
                db      '\r\n  Ctrl+c : exit.\r\n'
                db      0
                                        ;
; --------------------------------------------------------------------------------
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
    ExitGetByte:                        ; Exit the input loop.
                lxi h,ExitMsg
                call printStr
                hlt 
                jmp Start 
                                        ; ----------------------------------------------
    SCRCLR:
                mvi a,esc               ; Esc[H
                call PRINT
                mvi a,'['
                call PRINT
                mvi a,'H'
                call PRINT
                mvi a,esc               ; Esc[2J
                call PRINT
                mvi a,'['
                call PRINT
                mvi a,'2'
                call PRINT
                mvi a,'J'
                call PRINT
                ret
                                        ;
    SIODAT      EQU 11H                 ;88-2SIO DATA PORT
    SCRD1C:
                push a
                mvi a,'|'
                out SIODAT
    hlt
                mvi a,esc               ; Move down 1 line: Esc[1B
                out SIODAT
                mvi a,'['
                out SIODAT
                mvi a,'1'
                out SIODAT
                mvi a,'B'
                out SIODAT
                mvi a,' '
                out SIODAT
                mvi a,esc               ; Clear screen from cursor down. Esc[0J
                out SIODAT
                mvi a,'['
                out SIODAT
                mvi a,'0'
                out SIODAT
                mvi a,'J'
                out SIODAT
                pop a
    hlt
                ret
                                        ;
    CRSUP6:
                push a
                mvi a,esc               ; Esc[6A
                call PRINT
                mvi a,'['
                call PRINT
                mvi a,'6'
                call PRINT
                mvi a,'A'
                call PRINT
                pop a
                ret

    PRINT:
                out PRINT_PORT          ; Out register A to the serial port.
                ret

; --------------------------------------------------------------------------------
                                        ; Subroutines
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
                                        ; --------------------------------------
                                        ;
    StartMsg    db      '\r\n+++ Program to test VT100 escape code sequences.\r\n\r\n'
                db      0
    thePrompt   db      'Esc ?- '
                db      0
    processMsg  db      '++ processLine, Not implemented, yet.'
                db      0
    ExitMsg     db      '\r\n+ Later.\r\n'
                db      0
                                        ;
    TERMB       equ     0               ; String terminator.
    esc         equ     27              ; Escape character, which is 0x1B (hex).
    ;
    cpc         ds      1               ; Cursor position counter variable.
    lbc         ds      2               ; Address of last added key value = lb address + cpc-1.
    ;
    lineLength  equ     80
    lb          ds      80              ; Place to store what is typed in, for the current line.
                                        ; Cursor position is also the length of the entered text.
                                        ;
                                        ; ----------------------------------------
                                        ; ----------------------------------------
                                        ; When using port 2,
                                        ;   if port 3 is disable, then it defaults to 3, the default serial port.
    PRINT_PORT  equ     3               ; Output port#. 
    INPUT_PORT  equ     3               ; Input port#.
                                        ;
                DS      32              ; Stack space
    STACK:      EQU     $
                                        ; ----------------------------------------
                end
; --------------------------------------------------------------------------------

  VT100 reference:
       http://ascii-table.com/ansi-escape-sequences-vt-100.php
    Esc[H     Move cursor to upper left corner, example: Serial.print(F("\033[H"));
    Esc[J     Clear screen from cursor down, example: Serial.print(F("\033[J"));
    Esc[2J    Clear entire screen, example: Serial.print(F("\033[H"));
    Example:  Serial.print(F("\033[H\033[2J"));         // Move home and clear entire screen.
    Esc[K     Clear line from cursor right
    Esc[nA    Move cursor up n lines.
    Example:  Serial.print(F("\033[3A"));  Cursor Up 3 lines.
    Esc[nB    Move cursor down n lines.
    Example:  Serial.print(F("\033[6B"));  Cursor down 6 lines.
    Esc[nC    Move cursor right n positions.
    Example:  Serial.print(F("\033[H\033[4B\033[2C"));  // Print on: row 4, column 2.
    Esc[r;cH  Move cursor to a specific row(r) and column(c).
    Example:  Serial.print(F("\033[4;2H*"));            // Print on: row 4, column 2 and print "*".

  Reference: printf/sprintf formats:
  http://www.cplusplus.com/reference/cstdio/printf/

; --------------------------------------------------------------------------------
