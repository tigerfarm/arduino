                                    ; ------------------------------------------
                                    ; Sample program lines that are parsed by asmProcessor.java
                                    ;
                                    ; ------------------------------------------
                                    ;
                                    ; ------------------------------------------
                                    ; Lines to ignore.
                                    ;
                                    ; Blank line

                                    ; Comment line
                                    ;
                                    ; ------------------------------------------
                                    ; Comments are removed from lines
                                    ;
    ;            jmp Start           ; Line comment.

                                    ; ------------------------------------------
                                    ; Label line.
                                    ;
    ;Start:
                                    ; ------------------------------------------
                                    ; Label followed by an opcode.
                                    ;
    ;Halt:       hlt
                                    ; ------------------------------------------
                                    ; Directive followed by a string.
                                    ;
    ;        DB	' STARDATE  300'

                                    ; ------------------------------------------
                                    ; ------------------------------------------
                                    ; Label + Directive followed by a string.
                                    ;
                                    ; Label with ":"
    ALNMSK:     EQU 00110000b
    MSGSDP: DB  '0'
    DQUAD:  DB  000

    MSGSSS: DB  '  SPACE STATIONS'
            lxi h,MSGSSS
                                    ;
                                    ; Label without ":"
    okay    db  'okay, yes?'
            lxi h,okay
                                    ; ------------------------------------------
                                    ; Special case: end file processing
                end
                                    ; Lines after the "end" line are ignored.
                                    ;
                                    ; ------------------------------------------
                                    ; I use for listing a successful sample run
                                    ; ------------------------------------------
Sample line:
    okay    db  'okay, yes?'
Parsed:
++       4:00000000 00000100: 01101111 : 6F:157 > databyte: okay : o : 111
++       5:00000000 00000101: 01101011 : 6B:153 > databyte: okay : k : 107
++       6:00000000 00000110: 01100001 : 61:141 > databyte: okay : a : 97
++       7:00000000 00000111: 01111001 : 79:171 > databyte: okay : y : 121
++       8:00000000 00001000: 00101100 : 2C:054 > databyte: okay : , : 44
++       9:00000000 00001001: 00100000 : 20:040 > databyte: okay :   : 32
++      10:00000000 00001010: 01111001 : 79:171 > databyte: okay : y : 121
++      11:00000000 00001011: 01100101 : 65:145 > databyte: okay : e : 101
++      12:00000000 00001100: 01110011 : 73:163 > databyte: okay : s : 115
++      13:00000000 00001101: 00111111 : 3F:077 > databyte: okay : ? : 63
++      14:00000000 00001110: 11111111 : FF:377 > dbterm: 255
                                    ; 
                                    ; ------------------------------------------
