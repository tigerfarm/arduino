                                        ; --------------------------------------
                                        ; Test the various OUT options.
                                        ;
                                        ; Ability to set output port.
                                        ;
                                        ; --------------------------------------
                                        ;
                lxi sp,512              ; Set stack pointer.
    Start:
                                        ; --------------------------------------
                                        ; Print characters to hard coded port#.
                mvi a,'\r'
                out 3
                out 2
                mvi a,NL
                out 3
                out 2
                mvi a,'+'
                out 3
                out 2
                mvi a,'1'
                out 3
                out 2
                                        ; --------------------------------------
                                        ; Print character in register C, to the port# in register B.
                                        ;
                mvi c,'2'               ; Character to print
                mvi b,2                 ; Output Port # into register B.
                call outPrint
                mvi c,'3'
                mvi b,3
                call outPrint
                                        ; --------------------------------------
                                        ; Set output port and print strings.
                                        ;
                ;lxi h,StartMsg
                ;call sPrint
                ;mvi a,NL
                ;out 3
                                        ; --------------------------------------
                mvi a,'\r'
                out 3
                out 2
                mvi a,NL
                out 3
                out 2
                hlt                     ; The program will halt at each iteration, after the first.
                jmp Start               ; Re-start.
                                        ;
                                        ; --------------------------------------
        sPrint:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz sPrintDone
                out 3                   ; Out register A to the serial terminal port.
                inr m                   ; Increment H:L register pair.
                jmp sPrint
        sPrintDone:
                ret
                                        ; --------------------------------------
        outPrint:
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
                                        ;
    PRINT_PORT  equ      3              ; Output port: print to the serial port.
    StartMsg    db      'Start...'      ; Strings to print out.
    Again       db      'Again.'        ; Strings to print out.
    TERMB       equ     0ffh            ; String terminator.
    NL          equ     10              ; 10 is new line, '\n'.
                                        ; --------------------------------------
                end
