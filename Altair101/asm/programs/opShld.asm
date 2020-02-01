                            ; --------------------------------------
                            ; Test SHLD.
                            ; SHLD adr   (adr) <-L; (adr+1)<-H
                            ; Store data value from memory location: register L to a(address hb:lb).
                            ; Store data value from memory location: register H to a+1.
                            ;
                            ; --------------------------------------
            jmp Test        ; Jump to start of the test.
                            ;
                            ; --------------------------------------
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Test:
                            ; --------------------------------------
            mvi h,0
            mvi l,0
            out 36          ; Print register pair, H:L.
            shld 3          ; Store L value to memory location: 3(0:3). Store H value at: 3 + 1.
                            ;
            lda 3           ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
            lda 4           ; Load register A from the address(hb:lb).
            out 37          ; Print register A.
                            ;
            out 38          ; Print the register values.
                            ;
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ;
                            ; --------------------------------------
            end
