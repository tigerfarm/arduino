                        ; --------------------------------------
                        ; Test CMP and conditional jumps.
                        ; Compare each register to register A, and then set Carry bit and Zero bit flags.
                        ; If Register data = regA, Zero bit = 1, Carry bit = 0.
                        ; If Register data > regA, Zero bit = 0, Carry bit = 1.
                        ; If Register data < regA, Zero bit = 0, Carry bit = 0.
                        ; Note, register A (regA) remains the same after the compare.
                        ; Also, compare M, data value at address H:L, to register A.
                        ; 
                        ; Following is a sample successful run:
                        ; + Control, Run.
                        ; + runProcessor()
                        ; =BCDEHLMS
                        ; >BCDEHLMS
                        ; <LS
                        ; + HLT, program halted.
                        ; + runProcessorWait()
                        ; 
                        ; --------------------------------------
                        ;
            jmp Test    ; Jump to start of the test.
                        ; --------------------------------------
    Error:
            mvi a,'-'
            out 3
            mvi a,'E'
            out 3
            out 39      ; Print the registers and other system values.
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
    Test:
            mvi a,'='
            out 3
                        ; 
                        ; --------------------------------------
                        ; Test when registers are equal.
                        ; --------------------------------------
            mvi a,'B'
            out 3
            mvi a,73    ; Initialize register A for comparison.
            mvi b,73    ; Move # to register B.
            cmp b       ; B = A. Zero bit flag is true. Carry bit is false.
            jnz Error   ; Zero bit flag is set, don't jump.
            jc Error    ; Carry bit flag is not set, don't jump.
            jz okayb1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okayb1:
            jnc okayb2  ; Carry bit flag is not set, jump to the end of this test.
            jmp Error   ; The above should have jumped passed this.
    okayb2:
                        ; --------------------------------------
            mvi a,'C'
            out 3
            mvi a,73    ; Initialize register A for comparison.
            mvi c,73
            cmp c       ; C = A. Zero bit flag is true. Carry bit is false.
            jnz Error
            jc Error
            jz okayc1
            jmp Error
    okayc1:
            jnc okayc2
            jmp Error
    okayc2:
                        ; --------------------------------------
            mvi a,'D'
            out 3
            mvi a,73    ; Initialize register A for comparison.
            mvi d,73
            cmp d       ; D = A. Zero bit flag is true. Carry bit is false.
            jnz Error
            jc Error 
            jz okayd1
            jmp Error 
    okayd1:
            jnc okayd2 
            jmp Error 
    okayd2:
                        ; --------------------------------------
            mvi a,'E'
            out 3
            mvi a,73    ; Initialize register A for comparison.
            mvi e,73 
            cmp e       ; E = A. Zero bit flag is true. Carry bit is false.
            jnz Error 
            jc Error 
            jz okaye1
            jmp Error
    okaye1:
            jnc okaye2
            jmp Error 
    okaye2:
                        ; --------------------------------------
            mvi a,'H'
            out 3
            mvi a,73    ; Initialize register A for comparison.
            mvi h,73
            cmp h       ; H = A. Zero bit flag is true. Carry bit is false.
            jnz Error
            jc Error
            jz okayh1
            jmp Error
    okayh1:
            jnc okayh2
            jmp Error
    okayh2:
                        ; --------------------------------------
            mvi a,'L'
            out 3
            mvi a,73    ; Initialize register A for comparison.
            mvi l,73
            cmp l       ; L = A. Zero bit flag is true. Carry bit is false.
            jnz Error
            jc Error
            jz okayl1
            jmp Error
    okayl1:
            jnc okayl2
            jmp Error
    okayl2:
                        ; --------------------------------------
                        ; Test where the compare value, is the data value at memory location M(H:L).
            mvi a,'M'
            out 3
            mvi a,c3h   ; Initialize register A for comparison. c3h is 
            mvi h,0     ; hlValue = regH * 256 + regL;
            mvi l,0     ; Register M address data (opcode jmp) = 11 000 011 = c3h = 195
            cmp m       ; L = A. Zero bit flag is true. Carry bit is false.
            jnz Error
            jc Error
            jz okaym1
            jmp Error
    okaym1:
            jnc okaym2
            jmp Error
    okaym2:
            mvi a,'S'
            out 3
                        ; --------------------------------------
                        ; --------------------------------------
                        ; Test where the register data > regA.
            mvi a,'\n'
            out 3
            mvi a,'>'
            out 3
                        ; --------------------------------------
            mvi a,'B'
            out 3
            mvi a,73    ; Initialize register A for comparison.
            mvi b,76    ; Register data != A, set Zero bit to 0.
            cmp b       ; regB > regA, Carry bit = 1.
            jz Error    ; Jump to a, if zero bit equals 1, flag is set.
            jnc Error   ; Jump to a, if Carry bit equals 0, flag is not set.
            jnz okayb1g ; Jump to a, if Zero bit equals 0, flag is not set.
            jmp Error
    okayb1g:
            jc okayb2g
            jmp Error
    okayb2g:
                        ; --------------------------------------
            mvi a,'C'
            out 3
            mvi a,73
            mvi c,76
            cmp c       ; regC > regA, Carry bit = 1.
            jz Error
            jnc Error
            jnz okayc1g
            jmp Error
    okayc1g:
            jc okayc2g
            jmp Error
    okayc2g:
                        ; --------------------------------------
            mvi a,'D'
            out 3
            mvi a,73
            mvi d,76
            cmp d       ; regD > regA, Carry bit = 1.
            jz Error
            jnc Error
            jnz okayd1g
            jmp Error
    okayd1g:
            jc okayd2g
            jmp Error
    okayd2g:
                        ; --------------------------------------
            mvi a,'E'
            out 3
            mvi a,73
            mvi e,76
            cmp e       ; regE > regA, Carry bit = 1.
            jz Error
            jnc Error
            jnz okaye1g
            jmp Error
    okaye1g:
            jc okaye2g
            jmp Error
    okaye2g:
                        ; --------------------------------------
            mvi a,'H'
            out 3
            mvi a,73
            mvi h,76
            cmp h       ; regH > regA, Carry bit = 1.
            jz Error
            jnc Error
            jnz okayh1g
            jmp Error
    okayh1g:
            jc okayh2g
            jmp Error
    okayh2g:
                        ; --------------------------------------
            mvi a,'L'
            out 3
            mvi a,73
            mvi l,76
            cmp l       ; regL > A, Carry bit = 1.
            jz Error
            jnc Error
            jnz okayl1g
            jmp Error
    okayl1g:
            jc okayl2g  ; Jump to a, if Carry bit equals 1, flag is set.
            jmp Error
    okayl2g:
                        ; --------------------------------------
                        ; Test where the compare value, is the data value at memory location M(H:L).
            mvi a,'M'
            out 3
            mvi a,c1h   ; Initialize register A for comparison to c3h, which is 
            mvi h,0     ; hlValue = regH * 256 + regL;
            mvi l,0     ; Register M address data (opcode jmp) = 11 000 011 = c3h = 195
            cmp m       ; L > A. Zero bit flag is true. Carry bit is false.
            jz Error
            jnc Error
            jnz okaym1g
            jmp Error
    okaym1g:
            jc okaym2g
            jmp Error
    okaym2g:
                        ; --------------------------------------
            mvi a,'S'
            out 3
                        ; --------------------------------------
                        ; --------------------------------------
                        ; Test where the register data < regA.
            mvi a,'\n'
            out 3
            mvi a,'<'
            out 3
                        ; --------------------------------------
                        ; Do an error test where the register data < A.
            mvi a,'L'
            out 3
            mvi a,73    ; Initialize register A for comparison.
            mvi l,70    ; Register data != A, set Zero bit to 0.
            cmp l       ; Register < A, Carry bit = 0.
            jz Error    ; Jump to a, if zero bit equals 1, flag is set.
            jc Error    ; Jump to a, if Carry bit equals 1, flag is set.
            jnz okaye1b ; Jump to a, if Zero bit equals 0, flag is not set.
            jmp Error
    okaye1b:
            jnc okaye2b ; Jump to a, if Carry bit equals 0, flag is not set.
            jmp Error
    okaye2b:
            mvi a,'S'
            out 3
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
                        ; --------------------------------------
            end