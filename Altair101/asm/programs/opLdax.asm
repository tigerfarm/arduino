                                    ; --------------------------------------
                                    ; Test LDAX.
                                    ; Load register A with data value from address B:C or D:E.
                                    ;
                                    ; --------------------------------------
    Start:
            mvi c,0                 ; Initialize test counter which is used in PrintTestCounter.
            call PrintTestCounter
            mvi b,0                 ; Load B:C with the address of 0, which is the MVI(b) opcode.
            mvi c,0
            ldax b                  ; Load register A with data value from address B:C.
            out 37                  ; Print > Register A =   6 = 006 = 00000110
            mvi d,0                 ; Load D:E with an address, which is the LDAX opcode.
            mvi e,4
            ldax d                  ; Load register A with data value from address D:E.
            out 37                  ; Print > Register A =  10 = 012 = 00001010
                                    ;
                                    ; --------------------------------------
                                    ; Before running, DEPOSIT byte: 01010101, into address: 1024.
            call PrintTestCounter
            mvi d,2                 ; Load D:E with the address: 1024 (0000 0010 0000 0000).
            mvi e,0
            ldax d                  ; Load register A with data value from address D:E.
            out 37                  ; Print the DEPOSITed value > Register A =  85 = 125 = 01010101
                                    ;
                                    ; --------------------------------------
            call PrintTestCounter
            mvi a,6                 ; Move # to register A.
            out 37                  ; Print register A.
            sta 96                  ; Store register A's data value to the address(hb:lb).
            out 37                  ; Print register A.
                                    ; > Register A =   0 = 000 = 00000000
                                    ; > Register A =   6 = 006 = 00000110
                                    ;
            mvi a,0                 ;
            out 37                  ;
            lda 96                  ; Load register A from the address(hb:lb).
            out 37                  ; > Register A =   0 = 000 = 00000000
                                    ; > Register A =   6 = 006 = 00000110
                                    ;
                                    ; --------------------------------------
            hlt                     ; The program will halt before another iteration.
            jmp Start
                                    ;
                                    ; --------------------------------------
                                    ; Routines
    PrintTestCounter:
            mvi a,'\n'
            out 3
            mvi a,'+'
            out 3
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            lda counter             ; Increment counter
            inr a
            sta counter
            call PrintDigit
            ret
                                    ; 
    PrintDigit:
            mvi b,'0'
            add b
            out 3
            ret
                                    ;
                                    ; --------------------------------------
                                    ; Variables
                                    ;
    counter db 0                    ; Initialize test counter which is used in PrintTestCounter.
                                    ;
                                    ; --------------------------------------
            end
