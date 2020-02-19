                            ; --------------------------------------
                            ; Test LXI.
                            ; Load register pair with 16-bit immediate address value.
                            ;   LXI B, byte 2 -> C, byte 3 -> B.
                            ;   LXI D, byte 2 -> E, byte 3 -> D.
                            ;   LXI H, byte 2 -> L, byte 3 -> H.
                            ; If moving a 16-bit address:
                            ;   LXI a
                            ; Move a(lb:hb) data into the register pair RP address.
                            ; Move the lb data into C and hb data into B -> Register pair B:C = hb:lb.
                            ; Move the lb data into E and hb data into D -> Register pair D:E = hb:lb.
                            ; Move the lb data into L and hb data into H -> Register pair H:L = hb:lb.
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
