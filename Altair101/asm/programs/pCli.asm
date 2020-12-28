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
                lxi sp,1024             ; Set stack pointer.
                lxi h,lb                ; Set the H:L register pair to the line buffer.
                shld lbc                ; Store H and L registers into memory, the line buffer counter.
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
                                        ;
                lda cpc
                jz mtLine               ; If nothing entered in the line (MT line), nothing to process.
                call processLine
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
                                        ; Handle special keys.
                cpi 3                   ; Ctrl+c will exit this loop.
                jz ExitGetByte
                cpi 13                  ; If carriage return, send line feed and print the prompt.
                jz startNewline
                cpi 127                 ; Handle Backspace.
                jz backSpace
                cpi 12                  ; Ctrl+l, FF to have the screen cleared.
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
                out INPUT_PORT          ; output the character and get a new one.
                call inrCpc             ; Increment the cursor position.
                jmp GetByte
                                        ; ---------------
    bell:
                out INPUT_PORT          ; Bell tone.
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
    StartMsg    db      '\r\n+++ Welcome to the machine.\r\n'
                db      0
    EnterMsg    db      '+ Press keys. Ctrl+c to exit.\r\n'
                db      0
    thePrompt   db      '> '
                db      0
    echoMsg     db      '++ Echo not implemented, yet.'
                db      0
    ExitMsg     db      '\r\n+ Later.\r\n'
                db      0
                                        ;
    TERMB       equ     000h            ; String terminator.
    esc         equ     27              ; Escape character, which is 0x1B (hex).
    ;
    cpc         ds      1               ; Cursor position counter variable.
    lbc         ds      2               ; Address of last added key value = lb address + cpc-1.
    lb          ds      80              ; Place to store what is typed in, for the current line.
                                        ; Cursor position is also the length of the entered text.
                                        ;
                                        ; --------------------------------------
                                        ; Multiple line text block.
    Menu0       db      '\r\n+ Sample Menu.'
    Menu1       db      '\r\n 1. Menu item #1'
    Menu2       db      '\r\n 2. Menu item #2'
    Menu3       db      '\r\n 3. Menu item #3'
    MenuEnd     db      000h            ; Indicator of the end of a text block.
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
                lxi h,EnterMsg
                call printStr
...
    StartMsg    db      '\r\n+++ Welcome to the machine.\r\n'
                db      0
    EnterMsg    db      '+ Press keys. Ctrl+c to exit.\r\n> '
                db      0
...

+ Parse |StartMsg    db      '\r\n+++ Welcome to the machine.\r\n'|
++ parseLine componets theRest|db      '\r\n+++ Welcome to the machine.\r\n'|
++ parseLine componets part1asIs|StartMsg| part1|startmsg| part2|db| theDirective|db| theRest|'\r\n+++ Welcome to the machine.\r\n'|
++ parseLabel, Name: StartMsg, Address: 270

+ Parse |lxi h,EnterMsg|
++ parseLine componets theRest|h,EnterMsg|
++ parseLine componets part1asIs|lxi| part1|lxi| part2|h,EnterMsg| theDirective|| theRest||
++ parseLine1 2 componets, part1|lxi| part2|h,EnterMsg|
++ parseLine, opcode|lxi| p1|h| p2|EnterMsg|
++ Opcode: lxi 00100001 register|h| addressLabel|addressNumber|EnterMsg|

+ Parse |thePrompt   db      '> '|
++ parseLine componets theRest|db      '> '|
++ parseLine componets part1asIs|thePrompt| part1|theprompt| part2|db| theDirective|db| theRest|'> '|
++ parseLabel, Name: thePrompt, Address: 338
...

+ Parse |EnterMsg    db      '+ Press keys. Ctrl+c to exit.\r\n> '|
++ parseLine componets theRest|db      '+ Press keys. Ctrl+c to exit.\r\n> '|
++ parseLine componets part1asIs|EnterMsg| part1|entermsg| part2|db| theDirective|db| theRest|'+ Press keys. Ctrl+c to exit.\r\n> '|
++ parseLabel, Name: EnterMsg, Address: 303

++      15:00000000 00001111: 00100001 : 21:041 > opcode: lxi h,EnterMsg
++      16:00000000 00010000: 00101111 : 2F:057 > lb: 47
++      17:00000000 00010001: 00000001 : 01:001 > hb: 1
++      18:00000000 00010010: 11001101 : CD:315 > opcode: call printStr
++      19:00000000 00010011: 11011011 : DB:333 > lb: 219
++      20:00000000 00010100: 00000000 : 00:000 > hb: 0

...
++     270:00000001 00001110: 00001101 : 0D:015 > databyte: StartMsg : '\r' : 13
++     271:00000001 00001111: 00001010 : 0A:012 > databyte: StartMsg : '\n' : 10
++     272:00000001 00010000: 00101011 : 2B:053 > databyte: StartMsg : + : 43
++     273:00000001 00010001: 00101011 : 2B:053 > databyte: StartMsg : + : 43
++     274:00000001 00010010: 00101011 : 2B:053 > databyte: StartMsg : + : 43
...
++     302:00000001 00101110: 00101011 : 2B:053 > databyte: EnterMsg : + : 43
++     303:00000001 00101111: 00100000 : 20:040 > databyte: EnterMsg :   : 32
++     304:00000001 00110000: 01010000 : 50:120 > databyte: EnterMsg : P : 80
++     305:00000001 00110001: 01110010 : 72:162 > databyte: EnterMsg : r : 114
++     306:00000001 00110010: 01100101 : 65:145 > databyte: EnterMsg : e : 101
++     307:00000001 00110011: 01110011 : 73:163 > databyte: EnterMsg : s : 115
++     308:00000001 00110100: 01110011 : 73:163 > databyte: EnterMsg : s : 115
...
++     336:00000001 01010000: 00111110 : 3E:076 > databyte: thePrompt : > : 62
++     337:00000001 01010001: 00100000 : 20:040 > databyte: thePrompt :   : 32
++     338:00000001 01010010: 00000000 : 00:000 > dbterm: 0
++     339:00000001 01010011: 00101011 : 2B:053 > databyte: echoMsg : + : 43
                                        ; 
                                        ; --------------------------------------
