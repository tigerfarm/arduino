                                        ; --------------------------------------
                                        ; Test the various OUT options.
                                        ;
                                        ; Ability to set output port.
                                        ;
                                        ; --------------------------------------
                                        ; --------------------------------------
                lxi sp,512              ; Set stack pointer.
    Start:
                                        ; --------------------------------------
                                        ; Print characters to hard coded port#.
                mvi a,'\r'
                out 3
                out 2
                mvi a,'\n'
                out 3
                out 2
                mvi a,'+'
                out 3
                out 2
                                        ; --------------------------------------
                                        ; Print character in register C, to the port# in register B.
                                        ;
                mvi c,'2'               ; Character to print
                mvi b,2                 ; Output Port # into register B.
                call printChar
                mvi c,'3'
                mvi b,3
                call printChar
                                        ; --------------------------------------
                                        ; Set output port and print strings.
                                        ;
                mvi a,3                 ; Set output port
                sta PRINT_PORT
                lxi h,printTo3          ; Output a string to that port.
                call printStr
                call printNewline
                                        ;
                mvi a,2                 ; Set output port
                sta PRINT_PORT
                lxi h,printTo2          ; Output a string to that port.
                call printStr
                call printNewline
                                        ; --------------------------------------
                hlt 
                jmp Start 
                                        ; --------------------------------------
                                        ; --------------------------------------
        printStr:
                lda PRINT_PORT          ; Set print port#.
                mov b,a
        sPrint:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz sPrintDone
                                        ; --------------
                                        ; The following is more efficient than: call printChar.
                mov c,a                 ; Store the output char.
                mvi a,2                 ; Port# to compare
                cmp b
                jz PortStr2             ; If B = A, jump.
                mvi a,3
                cmp b
                jz PortStr3
                jmp sPrintDone          ; Invalid output port#
        PortStr2:
                mov a,c                 ; Move the print character to register A.
                out 2                   ; Output register A to the selected port.
                jmp sPrintNext
        PortStr3:
                mov a,c                 ; Move the print character to register A.
                out 3
                ;jmp sPrintNext
        sPrintNext:
                                        ; --------------
                inx h                   ; Increment H:L register pair.
                jmp sPrint
        sPrintDone:
                ret
                                        ; --------------------------------------
                                        ; --------------------------------------
        printChar:
                mvi a,2                 ; Port# to compare
                cmp b
                jz Port2                ; If B = A, jump.
                mvi a,3
                cmp b
                jz Port3
                ret
        Port2:
                mov a,c                 ; Move the print character to register A.
                out 2                   ; Output register A to the selected port.
                ret
        Port3:
                mov a,c                 ; Move the print character to register A.
                out 3
                ret
                                        ; --------------------------------------
                                        ; --------------------------------------
    printNewline:
                lda PRINT_PORT          ; Set print port#.
                mov b,a
                mvi c,'\r'
                call printChar
                mvi c,'\n'
                call printChar
                ret
                                        ; --------------------------------------
                                        ; --------------------------------------
                                        ;
    PRINT_PORT  db      1               ; Store Output port#
    TERMB       equ     0ffh            ; String terminator.
                                        ;
    printTo2    db      'printTo2...'   ; Strings to print out.
    printTo3    db      'printTo3...'   ; Strings to print out.
                                        ;
                                        ; --------------------------------------
                                        ; --------------------------------------
                end
