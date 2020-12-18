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
                out 36              ; Print the register values for H:L, and the content at that address.
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
                lda byteH           ; Load and print the address values.
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
                                    ;
                                    ; --------------------------------------
