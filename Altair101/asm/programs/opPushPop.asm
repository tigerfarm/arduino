                            ; --------------------------------------
                            ; Test PUSH and POP.
                            ;
                            ; Set registers to values. PUSH the registers.
                            ; Set registers to zero.
                            ; POP registers to values. Check the push values are restored.
                            ;
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
                            ; Test A:F push and pop.
            call NewTest
            mvi a,'A'
            out 3
            mvi a,':'
            out 3
            mvi a,'F'
            out 3
            mvi a,'\n'
            out 3
                            ;
            mvi a,3
            cpi 3           ; Set zero flag on/1, carry bit off/0;
            out 39          ; Print all the register values and the flags.
            push f
            mvi a,0
            out 37          ; Register A.
            pop f
            out 39
                            ;
            mvi a,1
            cpi 3           ; Set zero flag off/0, carry bit on/1;
            out 39          ; Print all the register values and the flags.
            push f
            mvi a,0
            out 37
            pop f
            out 39
                            ;
            mvi a,6
            cpi 3           ; Set zero flag off/0, carry bit off/0;
            out 39          ; Print all the register values and the flags.
            push f
            mvi a,0
            out 37
            pop f
            out 39
                            ; --------------------------------------
                            ; Test B:C push and pop.
            call NewTest
            mvi a,'B'
            out 3
            mvi a,':'
            out 3
            mvi a,'C'
            out 3
                            ;
            mvi b,2
            mvi c,3
            push b
            out 30          ; Register B.
            out 31          ; Register C.
            mvi b,0
            mvi c,0
            out 30          ; Register B.
            out 31          ; Register C.
            pop b
            out 30          ; Register B.
            out 31          ; Register C.
                            ; B:C
                            ; > Register B =   2 = 002 = 00000010
                            ; > Register C =   3 = 003 = 00000011
                            ; > Register B =   0 = 000 = 00000000
                            ; > Register C =   0 = 000 = 00000000
                            ; > Register B =   2 = 002 = 00000010
                            ; > Register C =   3 = 003 = 00000011
                            ; 
                            ; --------------------------------------
                            ; Test D:E push and pop.
            call NewTest
            mvi a,'D'
            out 3
            mvi a,':'
            out 3
            mvi a,'E'
            out 3
                            ;
            mvi d,2
            mvi e,3
            push d
            out 32          ; Register D.
            out 33          ; Register E.
            mvi d,0
            mvi e,0
            out 32          ; Register D.
            out 33          ; Register E.
            pop d
            out 32          ; Register D.
            out 33          ; Register E.
                            ; D:E
                            ; > Register D =   2 = 002 = 00000010
                            ; > Register E =   3 = 003 = 00000011
                            ; > Register D =   0 = 000 = 00000000
                            ; > Register E =   0 = 000 = 00000000
                            ; > Register D =   2 = 002 = 00000010
                            ; > Register E =   3 = 003 = 00000011
                            ;
                            ; --------------------------------------
                            ; Test H:L push and pop.
            call NewTest
            mvi a,'H'
            out 3
            mvi a,':'
            out 3
            mvi a,'L'
            out 3
                            ;
            mvi h,2
            mvi l,3
            push h
            out 34          ; Register H.
            out 35          ; Register L.
            mvi h,0
            mvi l,0
            out 34          ; Register H.
            out 35          ; Register L.
            pop h
            out 34          ; Register H.
            out 35          ; Register L.
                            ; H:L
                            ; > Register H =   2 = 002 = 00000010
                            ; > Register L =   3 = 003 = 00000011
                            ; > Register H =   0 = 000 = 00000000
                            ; > Register L =   0 = 000 = 00000000
                            ; > Register H =   2 = 002 = 00000010
                            ; > Register L =   3 = 003 = 00000011
                            ;
                            ; --------------------------------------
            call NewTest
            mvi a,'B'
            out 3
            mvi a,':'
            out 3
            mvi a,'C'
            out 3
            mvi a,' '
            out 3
            mvi a,'*'
            out 3
                            ;
            mvi b,2
            mvi c,3
            push b
            out 30          ; Register B.
            out 31          ; Register C.
                            ;
            mvi b,4
            mvi c,5
            push b
            out 30          ; Register B.
            out 31          ; Register C.
                            ;
            mvi b,6
            mvi c,7
            push b
            out 30          ; Register B.
            out 31          ; Register C.
                            ;
            mvi b,0
            mvi c,0
            out 30          ; Register B.
            out 31          ; Register C.
                            ;
            pop b
            out 30          ; Register B.
            out 31          ; Register C.
            pop b
            out 30          ; Register B.
            out 31          ; Register C.
            pop b
            out 30          ; Register B.
            out 31          ; Register C.
                            ; ++ B:C *
                            ; > Register B =   2 = 002 = 00000010
                            ; > Register C =   3 = 003 = 00000011
                            ; > Register B =   4 = 004 = 00000100
                            ; > Register C =   5 = 005 = 00000101
                            ; > Register B =   6 = 006 = 00000110
                            ; > Register C =   7 = 007 = 00000111
                            ; > Register B =   0 = 000 = 00000000
                            ; > Register C =   0 = 000 = 00000000
                            ; > Register B =   6 = 006 = 00000110
                            ; > Register C =   7 = 007 = 00000111
                            ; > Register B =   4 = 004 = 00000100
                            ; > Register C =   5 = 005 = 00000101
                            ; > Register B =   2 = 002 = 00000010
                            ; > Register C =   3 = 003 = 00000011
                            ; 
                            ; --------------------------------------
            hlt
            jmp Start
                            ; 
                            ; --------------------------------------
                            ; Routines
    NewTest:
            mvi a,'\n'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            ret
                            ; 
                            ; --------------------------------------
            end
++ Address:16-bit bytes       databyte :hex:oct > description
++       0:00000000 00000000: 11001101 : CD:315 > opcode: call NewTest
++       1:00000000 00000001: 11111001 : F9:371 > lb: 249
++       2:00000000 00000010: 00000000 : 00:000 > hb: 0
++       3:00000000 00000011: 00111110 : 3E:076 > opcode: mvi a,'A'
...
++     246:00000000 11110110: 11000011 : C3:303 > opcode: jmp Start
++     247:00000000 11110111: 00000000 : 00:000 > lb: 0
++     248:00000000 11111000: 00000000 : 00:000 > hb: 0
++     249:00000000 11111001: 00111110 : 3E:076 > opcode: mvi a,'\n'
++     250:00000000 11111010: 00001010 : 0A:012 > immediate: '\n' : 10

+ runProcessor()
+ Addr:    1: Data: 249 = 371 = 11111001 > < call, lb:  249:
+ Addr:    2: Data:   0 = 000 = 00000000 > < call, hb:    0: 
                                           > call, jumping from address: 2, to the subroutine address: 512. stackPointer = 62
+ Addr:  512: Data:   0 = 000 = 00000000 >  > nop ---------------------------
+ Addr:  513: Data:   0 = 000 = 00000000 >  > nop ---------------------------

