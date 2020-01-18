                            ; --------------------------------------
                            ; Test MOV using serial monitor output.
                            ; --------------------------------------
            jmp Start       ; Jump to bypass the halt.
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
                            ; --------------------------------------
                            ; Test MOV with register A.
            mvi a,0         ; Set initial register A value.
            inr a
            mov b,a         ; Move register A content to each other registers.
            inr a           ; Increment register A each time.
            mov c,a
            inr a
            mov d,a
            inr a
            mov e,a
            inr a
            mov h,a
            inr a
            mov l,a
            out 38
                            ; --------------------------------------
                            ; Test MOV with register B.
            inr a           ; Keep incrementing up.
            mov b,a
            mov a,b
            inr b
            mov c,b
            inr b
            mov d,b
            inr b
            mov e,b
            inr b
            mov h,b
            inr b
            mov l,b
            out 38
                        ; --------------------------------------
                        ; Test MOV with register C.
            inr b
            mov c,b     ; Keep incrementing up.
            mov a,c
            inr c
            mov b,c
            inr c
            mov d,c
            inr c
            mov e,c
            inr c
            mov h,c
            inr c
            mov l,c
            inr c
            out 38
                        ; --------------------------------------
                        ; Test MOV with register D.
            inr c
            mov d,c     ; Keep incrementing up.
            mov a,d
            inr d
            mov b,d
            inr d
            mov c,d
            inr d
            mov e,d
            inr d
            mov h,d
            inr d
            mov l,d
            inr d
            out 38
                        ; --------------------------------------
                        ; Test MOV with register E.
            inr d
            mov e,d     ; Keep incrementing up.
            mov a,e
            inr e
            mov b,e
            inr e
            mov c,e
            inr e
            mov d,e
            inr e
            mov h,e
            inr e
            mov l,e
            inr e
            out 38
                        ; --------------------------------------
                        ; Test MOV with register H.
            inr e
            mov h,e     ; Keep incrementing up.
            mov a,h
            inr h
            mov b,h
            inr h
            mov c,h
            inr h
            mov d,h
            inr h
            mov e,h
            inr h
            mov l,h
            inr h
            out 38
                        ; --------------------------------------
                        ; Test MOV with register L.
            inr h
            mov l,h
            inr l
            mov b,l
            inr l
            mov c,l
            inr l
            mov d,l
            inr l
            mov e,l
            inr l
            mov h,l
            inr l
            out 38
                        ; --------------------------------------
            NOP
            jmp Halt    ; Jump back to the early halt command.
            end
