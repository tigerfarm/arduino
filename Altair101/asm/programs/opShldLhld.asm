                                    ; --------------------------------------
                                    ; Test SHLD.
                                    ; 
                                    ; Store the HL register pair byte values
                                    ;   into a memory address byte and the memory address plus one byte.
                                    ; SHLD adr : L -> (adr); H -> (adr+1)
                                    ; Store register L to address: adr(hb:lb).
                                    ; Store register H to address: adr+1.
                                    ;
                                    ; Load the HL register pair byte values
                                    ;   from a memory address byte and the memory address plus one byte.
                                    ; LHLD adr : (adr) -> L; (adr+1) -> H
                                    ;
                                    ; --------------------------------------
                lxi sp,512          ; Set stack pointer which is used with CALL and RET.
    Start:
                                    ; --------------------------------------
                                    ; Initialize variables to 0.
                call printSeparator
                mvi h,0             ; Set address register pair H:L, to 0:0.
                mvi l,0
                out 36              ; Print the register values for H:L.
                                    ;
                mvi a,0             ; Initialize variables to 0. Set the address values to zero (register A value).
                sta byteL
                sta byteH
                lda byteL           ; Confirm set to 0. Load and print the address values.
                out 37
                lda byteH
                out 37
                                    ; --------------------------------------
                                    ; Steps to store an address using SHLD
                call printSeparator
                lxi h,Addr3         ; Load the address value of Addr3 into H:L.
                                    ; Register H:L = 3:18, address# 786.
                out 36              ; Print the register values for H:L, and the content at that address.
                                    ;
                shld byteL          ; Given byteL address(hb:lb),
                                    ;   Store register L to memory location of byteL             (byteL address).
                                    ;   Store register H to memory location of byteH = byteL + 1 (byteH address).
                                    ;
                lda byteH           ; Confirm the shld worked by loading and printing the address values.
                out 37
                lda byteL
                out 37
                                    ; --------------------------------------
                                    ; Steps to store an address using SHLD
                call printSeparator
                mvi h,0             ; Set address register pair H:L, to 0:0.
                mvi l,0
                out 36              ; Print the register values for H:L.
                lhld byteL          ; Given byteL address(hb:lb),
                                    ;   Store register L to memory location of byteL             (byteL address).
                                    ;   Store register H to memory location of byteH = byteL + 1 (byteH address).
                out 36              ; Print the register values for H:L, and the content at that address.
                                    ; --------------------------------------
                                    ; Sample use: loading a byte array with data.
                call printSeparator
                lxi h,lb            ; Set the H:L register pair to the line buffer address.
                out 36              ; Print the register values for H:L.
                shld lbc            ; Store H and L registers into memory, the line buffer counter.
                mvi h,0             ; Confirm the shld worked. Set address register pair H:L, to 0:0.
                mvi l,0
                out 36              ; Print the register values for H:L.
                                    ;
                lhld lbc            ; Load H and L registers from memory.
                out 36              ; Print the register values for H:L.
                mvi a,'a'
                mov m,a             ; Move register A to the H:L(register m) address. A -> (HL).
                lda lb              ; Confirm the data was stored.
                out 37
                                    ;
                inx h               ; Increment H:L register pair.
                shld lbc            ; Store the address into memory.
                                    ; At this point, H and L registers can be used for other purposes.
                lhld lbc            ; Restore the address from memory: load H and L registers from memory.
                out 36              ; Print the register values for H:L.
                mvi m,'b'           ; Store the next byte into the byte array.
                                    ;
                mvi a,'c'
                mov m,a
                inx h
                inr a
                mov m,a
                inx h
                inr a
                mov m,a
                                    ; 
                lxi h,lb
                mov a,m
                out 37
                inr m
                mov a,m
                out 37
                inr m
                mov a,m
                out 37
                inr m
                mov a,m
                out 37
                                    ; 
                                    ; --------------------------------------
                call println
                hlt
                jmp Start
                                    ; ------------------------------------------
                                    ; ------------------------------------------
    printSeparator:
                call println
                mvi a,'-'
                out 3
                mvi a,'-'
                out 3
                mvi a,'-'
                out 3
                mvi a,'-'
                out 3
                mvi a,'-'
                out 3
                mvi a,'-'
                out 3
                mvi a,'-'
                out 3
                ret
    println:
                mvi a,'\r'
                out 3
                mvi a,'\n'
                out 3
                ret
                                    ; --------------------------------------
                                    ; Variables
    byteL       ds      1
    byteH       ds      1
                                    ;
    lbc         ds      2           ; Address counter for lb. Address of last added key value = lb address + cpc-1.
    lb          ds      8           ; Place to store what is typed in, for the current line.
                                    ; Cursor position is also the length of the entered text.
                                    ;
                                    ;         H         L
    Addr1       equ     128         ; 0000 0000 1000 0000 H:L = 0:128
    Addr2       equ     512         ; 0000 0010 0000 0000 H:L = 2:000
    Addr3       equ     786         ; 0000 0011 0001 0010 H:L = 3:018
                                    ; --------------------------------------
                end
                                    ; --------------------------------------
                                    ; Successful run:
                                    ;
+ Download complete.
?- + r, RUN.
?- + runProcessor()

-------
 > Register H:L =   0:  0 = 00000000:00000000, Data:  49 = 061 = 00110001
 > Register A =   0 = 000 = 00000000
 > Register A =   0 = 000 = 00000000
-------
 > Register H:L =   3: 18 = 00000011:00010010, Data:   0 = 000 = 00000000
 > Register A =   3 = 003 = 00000011
 > Register A =  18 = 022 = 00010010
-------
 > Register H:L =   0:  0 = 00000000:00000000, Data:  49 = 061 = 00110001
 > Register H:L =   3: 18 = 00000011:00010010, Data:   0 = 000 = 00000000
-------
 > Register H:L =   0:185 = 00000000:10111001, Data:   0 = 000 = 00000000
 > Register H:L =   0:  0 = 00000000:00000000, Data:  49 = 061 = 00110001
 > Register H:L =   0:185 = 00000000:10111001, Data:   0 = 000 = 00000000
 > Register A =  97 = 141 = 01100001
 > Register H:L =   0:186 = 00000000:10111010, Data:   0 = 000 = 00000000
 > Register A =  97 = 141 = 01100001
 > Register A =  98 = 142 = 01100010
 > Register A =  99 = 143 = 01100011
 > Register A = 100 = 144 = 01100100
++ HALT, host_read_status_led_WAIT() = 0
                                    ;
                                    ; --------------------------------------
