                                    ; ------------------------------------------
                                    ; Test counter I/O using IN and OUT opcode counter ports.
                                    ; 
                                    ; ------------------------------------------
    Start:
                MVI A,6             ; Counter index file number.
                OUT 25              ; Enter counter mode, and display counter value for counter index in register A.
                                    ; Flip the counter switch (AUX2 down) to exit counter mode and continue the program.
                                    ;
                                    ; ------------------------------------------
                OUT 21              ; Increment counter value for counter index in register A.
                OUT 25              ; Redisplay the counter value while in counter mode.
                                    ;
                                    ; ------------------------------------------
                OUT 22              ; Decrement counter value for counter index in register A.
                OUT 25              ; Redisplay the counter value while in counter mode.
                                    ;
                                    ; ------------------------------------------
                OUT 20              ; Set processor counter address to the value in register A, which is use by the IN opcode.
                IN 21               ; Get the processor counter address data value, into register A.
                STA 64              ; Move register A content to memory address 64 (A6).
                                    ;
                                    ; ------------------------------------------
                HLT                 ; Halt. Check the content of memory address 64 (A6).
                JMP Start
                END
