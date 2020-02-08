                            ; --------------------------------------
                            ; Test LXI.
                            ; Load register pair with immediate address value.
                            ;   LXI B,D16-bit  B <- byte 3, C <- byte 2
                            ;   LXI D,D16-bit  D <- byte 3, E <- byte 2
                            ;   LXI H,D16-bit  H <- byte 3, L <- byte 2
                            ;   Move a(hb:lb) data into the register pair RP address.
                            ;   Move the lb hb data, into register pair B:C = hb:lb.
                            ;   Move the lb hb data, into register pair D:E = hb:lb.
                            ;   Move the lb hb data, into register pair H:L = hb:lb.
                            ;
                            ; --------------------------------------
            jmp Test        ; Jump to start of the test.
                            ;
    Addr1   equ     128
    Addr2   ds      2
                            ; --------------------------------------
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Test:
                            ; --------------------------------------
            mvi a,6         ; Initialize register values for testing.
            mvi b,0
            mvi c,1
            mvi d,2
            mvi e,3
            mvi h,4
            mvi l,5
            out 38          ; Print the register values.
                            ;
                            ; --------------------------------------
                            ;
            lxi h,Addr1     ; Load an address (value of Addr1) into H:L.
            out 36          ; Print the register values for H:L, and the content at that address.
            hlt
                            ;
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ;
                            ; --------------------------------------
            end
