                            ; --------------------------------------
                            ; Test CPI and conditional jumps.
                            ; Compare an number (immediate) to A, and then set Carry and Zero bit flags.
                            ;
                            ; If # = A, set Zero bit to 1. Carry bit to 0.
                            ; If # !=A, set Zero bit to 0.
                            ; If # > A, set Zero bit to 0. Carry bit = 1.
                            ; If # < A, set Zero bit to 0. Carry bit = 0.
                            ;
                            ; Note, register A remain the same after the compare.
                            ; 
                            ; Successful run indicator: 1S2S.
                            ;   "1S": Test 1 was success
                            ;   "2S": Test 2 was success
                            ; + Control, Run.
                            ; + runProcessor()
                            ; 1S2S3S4S
                            ; ++ Success: CPI
                            ; + HLT, program halted.
                            ; + runProcessorWait()
                            ; 
                            ; --------------------------------------
            jmp Start       ; Jump to start of the test.
                            ; --------------------------------------
    iValue  equ     42      ; Label immediate to test with.
                            ; --------------------------------------
    Error:
            mvi a,'\n'
            out 3
            mvi a,'-'       ; Move the byte value of "-" to register A.
            out 3           ; Output register A content to the serial port (serial monitor).
            mvi a,'-'
            out 3
            mvi a,' '
            out 3
            mvi a,'E'
            out 3
            mvi a,'r'
            out 3
            mvi a,'r'
            out 3
            mvi a,'o'
            out 3
            mvi a,'r'
            out 3
            out 39          ; Print the registers and other system values.
            hlt             ; Halt after the error.
            jmp Start
                            ;
                            ; --------------------------------------
    Start:
            mvi a,'1'       ; Test 1, immediate number = register A.
            out 3
            mvi a,73        ; Move # to register A.
            cpi 73          ; 73 = A. Zero bit flag is true. Carry bit is false.
            jnz Error       ; Zero bit flag is set, don't jump.
            jc Error        ; Carry bit flag is not set, don't jump.
            jz okaye1a      ; Zero bit flag is set, jump.
            jmp Error       ; The above should have jumped passed this.
    okaye1a:
            jnc okaye1b     ; Carry bit flag is not set, jump to the next test.
            jmp Error       ; The above should have jumped passed this.
    okaye1b:
            mvi a,'S'
            out 3
                            ; --------------------------------------
            mvi a,'2'       ; Test 2, EQU number = register A.
            out 3
            mvi a,42        ; Move # to register A.
            cpi iValue      ; 42 = A. Zero bit flag is true. Carry bit is false.
            jnz Error       ; Zero bit flag is set, don't jump.
            jc Error        ; Carry bit flag is not set, don't jump.
            jz okaye2a       ; Zero bit flag is set, jump.
            jmp Error       ; The above should have jumped passed this.
    okaye2a:
            jnc okaye2b     ; Carry bit flag is not set, jump to the end of this test.
            jmp Error       ; The above should have jumped passed this.
    okaye2b:
            mvi a,'S'
            out 3
                            ; --------------------------------------
            mvi a,'3'       ; Test 3, immediate number > register A.
            out 3
            mvi a,73        ; Move # to register A.
            cpi 76          ; 73 = A. Zero bit flag is true. Carry bit is false.
            jz Error        ; Zero bit flag is set.
            jnz okayg1b     ; Zero bit flag not set.
            jmp Error
    okayg1a:
            jc okayg1a      ; Carry bit flag is set.
            jmp Error
    okayg1b:
            jnc Error       ; Carry bit flag not set.
            mvi a,'S'
            out 3
                            ; --------------------------------------
            mvi a,'4'       ; Test 4, immediate number < register A.
            out 3
            mvi a,73        ; Move # to register A.
            cpi 70          ; 73 = A. Zero bit flag is true. Carry bit is false.
            jz Error        ; Zero bit flag is set.
            jnz okayl1b     ; Zero bit flag not set.
            jmp Error
    okayl1a:
            jc Error        ; Carry bit flag is set.
            jnc okayl1b     ; Carry bit flag not set.
            jmp Error
    okayl1b:
            mvi a,'S'
            out 3
                            ; --------------------------------------
            mvi a,'\n'
            out 3
            mvi a,'+'       ; Move the byte value of "+" to register A.
            out 3           ; Output register A content to the serial port (serial monitor).
            mvi a,'+'
            out 3
            mvi a,' '
            out 3
            mvi a,'S'
            out 3
            mvi a,'u'
            out 3
            mvi a,'c'
            out 3
            mvi a,'c'
            out 3
            mvi a,'e'
            out 3
            mvi a,'s'
            out 3
            mvi a,'s'
            out 3
            mvi a,':'
            out 3
            mvi a,' '
            out 3
            mvi a,'C'
            out 3
            mvi a,'P'
            out 3
            mvi a,'I'
            out 3
                            ; --------------------------------------
            hlt
            jmp Start
                            ; --------------------------------------
            end
