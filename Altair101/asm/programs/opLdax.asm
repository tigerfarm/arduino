                                    ; --------------------------------------
                                    ; Test LDAX.
                                    ;
                                    ; Load register A with data value from address B:C or D:E.
                                    ;
                                    ; Before running, DEPOSIT byte: 01010101, into address: 1024 (S10/key a).
                                    ;
                                    ; --------------------------------------
            lxi sp,255              ; Set stack pointer which is used with CALL and RET.
    Start:
            mvi a,0                 ; Initialize the test counter.
            sta counter
                                    ; --------------------------------------
            call PrintTestCounter
            mvi b,0                 ; Load B:C with the address of 0.
            mvi c,0
            ldax b                  ; Load register A with data value from address B:C, the LXI opcode(B00110001).
            out 37                  ; Print > Register A =  49 = 061 = 00110001
                                    ; ---------
            mvi d,0                 ; Load D:E with an address, which is the LDAX opcode(B00011010).
            mvi e,15                ; ++ 15:00000000 00001111: 00001010 : 0A:012 > opcode: ldax b
            ldax d                  ; Load register A with data value from address D:E.
            out 37                  ; Print > Register A =  10 = 012 = 00001010
                                    ;
                                    ; --------------------------------------
                                    ; Before running, DEPOSIT byte: 01010101, into address: 1024 (S10/key a).
            call PrintTestCounter
            mvi d,4                 ; Load D:E with the address: 1024 (0000 0100 0000 0000).
            mvi e,0
            ldax d                  ; Load register A with data value from address D:E.
            out 37                  ; Print the DEPOSITed value > Register A =  85 = 125 = 01010101
                                    ;
                                    ; --------------------------------------
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            hlt                     ; The program will halt before another iteration.
            jmp Start
                                    ;
                                    ; --------------------------------------
                                    ; Routines
    PrintTestCounter:
            mvi a,'\r'
            out 3
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
                                    ; --------------------------------------
                                    ; Successful run:
+ runProcessor()
++ 1
 > Register A =  49 = 061 = 00110001
 > Register A =  10 = 012 = 00001010
++ 2
 > Register A =  85 = 125 = 01010101
++ HALT, host_read_status_led_WAIT() = 0
                                    ;
                                    ; --------------------------------------
