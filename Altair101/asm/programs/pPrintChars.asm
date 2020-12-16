                                        ; --------------------------------------
                                        ; Test the various OUT options.
                                        ;
                                        ; ASCII character set:
                                        ;   https://en.wikipedia.org/wiki/ASCII
                                        ;
                                        ; The VT100 code page:
                                        ;   https://en.wikipedia.org/wiki/VT100_encoding
                                        ; The first 128 of which are identical to ASCII
                                        ;
                                        ; Article about printing special characters:
                                        ;   https://stackoverflow.com/questions/48039871/string-literal-vt100-representation-to-unicode/48042417
                                        ;
                                        ; --------------------------------------
                                        ;
                lxi sp,512              ; Set stack pointer.
                call clr                ; Clear screen.
                ; call s1
    Start:
                lxi h,StartMsg
                call sPrint
                                        ; --------------------------------------
                mvi b,32                ; First char value to print
                mvi c,0                 ; Number of characters printed on a single line.
        printChar:
                mvi a,':'
                out 2                   ; Out register A to the serial terminal port.
                mov a,b
                cpi 127                 ; Max char value.
                jz printCharDone
                out 2
                inr b                   ; Increment print character value.
                                        ;
                inr c                   ; Increment print-per-line counter.
                mov a,c
                cpi 20                  ; Max char per line.
                jz printNl
                                        ;
                jmp printChar
                                        ;
        printNl:
                mvi a,':'
                out 2
                mvi a,'\r'              ; Print CR and NL characters.
                out 2
                mvi a,'\n'
                out 2
                mvi c,0                 ; Reset counter.
                jmp printChar
                                        ;
        printCharDone:
                lxi h,DoneMsg
                call sPrint
                                        ; --------------------------------------
                lxi h,SpecialMsg
                call sPrint
                mvi a,':'
                out 2
                mvi a,'£'
                out 2
                mvi a,':'
                out 2
                mvi a,'¥'
                out 2
                mvi a,':'
                out 2
                mvi a,'\r'
                out 2
                mvi a,'\n'
                out 2

                                        ; --------------------------------------
            hlt
            jmp Start
                                        ;
                                        ; --------------------------------------
        sPrint:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz sPrintDone
                out 3                   ; Out register A to the serial terminal port.
                inx h                   ; Increment H:L register pair.
                jmp sPrint
        sPrintDone:
                ret
                                        ; --------------------------------------
        sPrint2:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz sPrintDone2
                out 2                   ; Out register A to the serial terminal port.
                inx h                   ; Increment H:L register pair.
                jmp sPrint2
        sPrintDone2:
                ret
                                        ; --------------------------------------
                                        ; Move home and clear entire screen: '\033[H\033[2J'
        clr:
                mvi a,esc
                out 2
                mvi a,'['
                out 2
                mvi a,'H'
                out 2
                mvi a,esc
                out 2
                mvi a,'['
                out 2
                mvi a,'2'
                out 2
                mvi a,'J'
                out 2
                ret
                                        ; --------------------------------------
        s1:                             ; 251C
                                        ;
                                        ; esc ( 0 changes the encoding to the alternate set.
                                        ; esc ( B changes it back.
                                        ;
                                        ; https://vt100.net/docs/vt100-ug/chapter3.html
                                        ;
                                        ; "\x1b(0l\x1b(BHeader"
                                        ; ┌Header
                                        ;
                                        ; https://en.wikipedia.org/wiki/DEC_Special_Graphics
                                        ; ▒␉␌␍␊°±␤␋┘┐┌└╋-_-_-_┣┫┻┳
                                        ; abcdefghijklmnopqrstuvwx0
                mvi a,':'
                out 2
                                        ;
                mvi a,esc               ; esc ( 0 changes the encoding to the alternate set.
                out 2
                mvi a,'('
                out 2
                mvi a,'0'
                out 2
                                        ;
                mvi a,'M'               ; Hex 6d
                out 2
                                        ;
                mvi a,esc               ; esc ( B changes it back.
                out 2
                mvi a,'('
                out 2
                mvi a,'B'
                out 2
                                        ;
                out 2
                mvi a,':'
                out 2
                ret
                                        ; --------------------------------------
                                        ;
    StartMsg    db      '\r\n+ Start print characters...\r\n'
    DoneMsg     db      '\r\n+ Print chars completed.\r\n'
    SpecialMsg  db      '\r\n+ Print special chars.\r\n'
    TERMB       equ     0ffh            ; String terminator.
    esc         equ     27              ; Escape character, which is 0x1B (hex).
                                        ;
                                        ; --------------------------------------
                end
