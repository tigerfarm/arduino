                            ; --------------------------------------
                            ; Test DAD.
                            ; DAD RP 00RP1001  Add register pair(RP: B:C or D:E) to H:L. Set carry bit.
                            ;          00=BC   (B:C as 16 bit register)
                            ;          01=DE   (D:E as 16 bit register)
                            ;
                            ; --------------------------------------
            lxi sp,518      ; Stack pointer address
    Start:
                            ; --------------------------------------
            mvi a,0         ; Initialize register values for testing.
            mvi b,1
            mvi c,2
            mvi d,3
            mvi e,5
            mvi h,0
            mvi l,0
            out 38          ; Print the register values.
                            ;
                            ; --------------------------------------
            out 36          ; Print register pair, H:L.
            dad b           ; Add register pair B:C to H:L.
            out 36          ; Print register pair, H:L.
            dad d           ; Add register pair D:E to H:L.
            out 36          ; Print register pair, H:L.
            dad h           ; Add register pair H:L to H:L.
            out 36          ; Print register pair, H:L.
                            ;
                            ; --------------------------------------
                            ; After making Processor.ino updates, DAD SP works.
                            ; However, I should implement the stack to Altair 8800 specifications.
                            ;
            out 43          ; Print data which includes the stack pointer.
            dad sp          ; Add the stack pointer address value to H:L.
            out 36          ; Print register pair, H:L.
            out 43          ; Print data which includes the stack pointer.
                            ;
            push b          ; Change the stack pointer value.
            push b
            out 43          ; Print data which includes the stack pointer.
            dad sp          ; Add the stack pointer address value to H:L.
            out 36
            out 43
            pop b
            pop b
            out 43
                            ;
                            ; --------------------------------------
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            hlt
            jmp Start
                            ;
                            ; --------------------------------------
            end
                            ; --------------------------------------
                            ; Successful run:
                            ;
+ Download complete.
+ r, RUN.
?- + runProcessor()
------------
+ regA:   0 = 000 = 00000000
+ regB:   1 = 001 = 00000001  regC:   2 = 002 = 00000010
+ regD:   3 = 003 = 00000011  regE:   5 = 005 = 00000101
+ regH:   0 = 000 = 00000000  regL:   0 = 000 = 00000000
------------
 > Register H:L =   0:  0 = 00000000:00000000, Data:  49 = 061 = 00110001
 > Register H:L =   1:  2 = 00000001:00000010, Data:   0 = 000 = 00000000
 > Register H:L =   4:  7 = 00000100:00000111, Data:   0 = 000 = 00000000
 > Register H:L =   8: 14 = 00001000:00001110, Data: 212 = 324 = 11010100
 > Register SP =   518 =  = 00000010:00000110 Status flag byte, regS00000000 :Sign=0:Zero=0:.:HalfCarry=0:.:Parity=0:.:Carry=0:
 > Register H:L =  10: 20 = 00001010:00010100, Data: 183 = 267 = 10110111
 > Register SP =   518 =  = 00000010:00000110 Status flag byte, regS00000000 :Sign=0:Zero=0:.:HalfCarry=0:.:Parity=0:.:Carry=0:
 > Register SP =   514 =  = 00000010:00000010 Status flag byte, regS00000000 :Sign=0:Zero=0:.:HalfCarry=0:.:Parity=0:.:Carry=0:
 > Register H:L =  12: 22 = 00001100:00010110, Data: 175 = 257 = 10101111
 > Register SP =   514 =  = 00000010:00000010 Status flag byte, regS00000000 :Sign=0:Zero=0:.:HalfCarry=0:.:Parity=0:.:Carry=0:
 > Register SP =   518 =  = 00000010:00000110 Status flag byte, regS00000000 :Sign=0:Zero=0:.:HalfCarry=0:.:Parity=0:.:Carry=0:
++ HALT, host_read_status_led_WAIT() = 0
                            ;
                            ; --------------------------------------
