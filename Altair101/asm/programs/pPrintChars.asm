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
                mvi a,':'
                out 2                   ; Out register A to the serial terminal port.
    printChar:
                mov a,b
                cpi 127                 ; Max char value.
                jz printCharDone
                                        ;
                cpi 'a'
                jnz nextA
                call println
                mvi a,'a'
                jmp outA
    nextA:
                cpi 'A'
                jnz outA
                call println
                mvi a,'A'
    outA:
                out 2
                mvi a,':'
                out 2
                                        ;
                inr b                   ; Increment print character value.
                inr c                   ; Increment print-per-line counter.
                mov a,c
                cpi 26                  ; Max char per line.
                jnz printChar
                call println
                jmp printChar
                                        ;
        println:
                mvi a,'\r'              ; Print CR and NL characters.
                out 2
                mvi a,'\n'
                out 2
                mvi c,0                 ; Reset counter.
                mvi a,':'
                out 2
                ret
                                        ;
        printCharDone:
                lxi h,DoneMsg
                call sPrint
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
                                        ;
    StartMsg    db      '\r\n+ List printable characters...\r\n'
    DoneMsg     db      '\r\n+ Print characters completed.\r\n'
    TERMB       equ     0ffh            ; String terminator.
    esc         equ     27              ; Escape character, which is 0x1B (hex).
                                        ;
                                        ; --------------------------------------
                end
                                        ; --------------------------------------
                                        ; Successful run:
+ Start print characters...
: :!:":#:$:%:&:':(:):*:+:,:-:.:/:0:1:2:3:4:5:6:7:8:9:
:::;:<:=:>:?:@:
:A:B:C:D:E:F:G:H:I:J:K:L:M:N:O:P:Q:R:S:T:U:V:W:X:Y:Z:
:[:\:]:^:_:`:
:a:b:c:d:e:f:g:h:i:j:k:l:m:n:o:p:q:r:s:t:u:v:w:x:y:z:
:{:|:}:~:
+ Print chars completed.
++ HALT, host_read_status_led_WAIT() = 0
                                        ;
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ; Test that didn't work
                                        ;
    SpecialMsg  db      '\r\n+ Print special chars.\r\n'
                                        ;
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
                                        ;
                                        ; The following doesn't print properly: 
                                        ; :�:�:
                                        ;
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

How to print the following graphics chars?

┘┐┌└╋-_-_-_┣┫┻┳

┘┐┌└
-
┌---┐
└---┘

┌=┐
└ ┘
                                        ;
                                        ; --------------------------------------
                                        ;
