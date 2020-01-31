                        ; --------------------------------------
                        ; Test CMP and conditional jumps.
                        ; Compare a register to A, and then set Carry and Zero bit flags.
                        ; If #=A, set Zero bit to 1, Carry bit to 0. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
                        ; Note, register A remain the same after the compare.
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
            mvi a,73    ; Initialize register A for comparison.
                        ; --------------------------------------
            mvi a,'B'
            out 3
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
            mvi a,'S'
            out 3
            NOP
            jmp Halt    ; Jump back to the early halt command.
                        ; --------------------------------------
            end