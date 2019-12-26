                        ; --------------------------------------
                        ; Test CMP and conditional jumps.
                        ; Compare a register to A, and then set Carry and Zero bit flags.
                        ; If #=A, set Zero bit to 1, Carry bit to 0. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
                        ; Note, register A remain the same after the compare.
                        ; --------------------------------------
            jmp Test    ; Jump to bypass the halt.
    Error:
            out 39      ; Print the registers and other system values.
    Halt:
            hlt         ; The program will halt at each iteration, after the first.
                        ; --------------------------------------
    Test:
            mvi a,73    ; Move # to register A.
                        ; --------------------------------------
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
            mvi c,73    ; Move # to register C.
            cmp c       ; C = A. Zero bit flag is true. Carry bit is false.
            jnz Error   ; Zero bit flag is set, don't jump.
            jc Error    ; Carry bit flag is not set, don't jump.
            jz okayc1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okayc1:
            jnc okayc2  ; Carry bit flag is not set, jump to the end of this test.
            jmp Error   ; The above should have jumped passed this.
    okayc2:
                        ; --------------------------------------
            mvi d,73    ; Move # to register D.
            cmp d       ; D = A. Zero bit flag is true. Carry bit is false.
            jnz Error   ; Zero bit flag is set, don't jump.
            jc Error    ; Carry bit flag is not set, don't jump.
            jz okayd1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okayd1:
            jnc okayd2  ; Carry bit flag is not set, jump to the end of this test.
            jmp Error   ; The above should have jumped passed this.
    okayd2:
                        ; --------------------------------------
            mvi e,73    ; Move # to register E.
            cmp e       ; D = A. Zero bit flag is true. Carry bit is false.
            jnz Error   ; Zero bit flag is set, don't jump.
            jc Error    ; Carry bit flag is not set, don't jump.
            jz okaye1   ; Zero bit flag is set, jump.
            jmp Error   ; The above should have jumped passed this.
    okaye1:
            jnc okaye2  ; Carry bit flag is not set, jump to the end of this test.
            jmp Error   ; The above should have jumped passed this.
    okaye2:
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
                        ; End.