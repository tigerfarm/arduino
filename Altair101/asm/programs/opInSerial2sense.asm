                                        ; ------------------------------------------
                                        ; Get the sense switch data byte.
                                        ; 
                                        ; ------------------------------------------
                lxi sp,1024             ; Set stack pointer.
                mvi b,0                 ; Previous sense byte value
    prompt:
                mvi a,'>'
                out PRINT_PORT
                mvi a,' '
                out PRINT_PORT
    GetByte:
                in SENSE_SW             ; Get toggle sense switch byte value into register A.
                cmp b                   ; Compare with previous value.
                jz GetByte              ; No change, Jump.
                                        ;
                sta SenseData
                call printByteA
                call println
                                        ;
                lda SenseData           ; Set previous sense byte value
                mov b,a
                                        ;
                jmp prompt
                                        ; ----------------------
    HaltLoop:
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
                hlt                     ; Halt to get the data byte.
                jmp prompt
                                        ; ------------------------------------------
                                        ; ------------------------------------------
                                        ;
    println:
                mvi a,'\r'              ; Print CR and NL characters.
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
                ret
    printByteA:                         ; Print register A as a byte string.
                mov b,a
                    ; out 37            ; For debugging: Register A = 248 = 370 = 11111000
                                        ; ------
                mov a,b
                ani 128                 ; AND 10000000 with register A
                cpi 0
                jnz printZero8-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero7
    printZero8-1:
                mvi a,'1'
                out PRINT_PORT
    printZero7:
                mov a,b
                ani 64                  ; AND 01000000 with register A
                cpi 0
                jnz printZero7-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero6
    printZero7-1:
                mvi a,'1'
                out PRINT_PORT
    printZero6:
                mov a,b
                ani 32                  ; AND 00100000 with register A
                cpi 0
                jnz printZero6-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero5
    printZero6-1:
                mvi a,'1'
                out PRINT_PORT
    printZero5:
                mov a,b
                ani 16                  ; AND 00010000 with register A
                cpi 0
                jnz printZero5-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero4
    printZero5-1:
                mvi a,'1'
                out PRINT_PORT
    printZero4:
                mov a,b
                ani 8                   ; AND 00001000 with register A
                cpi 0
                jnz printZero4-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero3
    printZero4-1:
                mvi a,'1'
                out PRINT_PORT
    printZero3:
                mov a,b
                ani 4                   ; AND 00000100 with register A
                cpi 0
                jnz printZero3-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero2
    printZero3-1:
                mvi a,'1'
                out PRINT_PORT
    printZero2:
                mov a,b
                ani 2                   ; AND 00000010 with register A
                cpi 0
                jnz printZero2-1
                mvi a,'0'
                out PRINT_PORT
                jmp printZero1
    printZero2-1:
                mvi a,'1'
                out PRINT_PORT
    printZero1:
                mov a,b
                ani 1                   ; AND 00000001 with register A
                cpi 0
                jnz printZero1-1
                mvi a,'0'
                out PRINT_PORT
                ret
    printZero1-1:
                mvi a,'1'
                out PRINT_PORT
                ret
                                        ; --------------------------------------
    SenseData   ds      1
    SENSE_SW    equ     255             ; Input port address: toggle sense switch byte, into register A.
    PRINT_PORT  EQU 2                   ; USB Serial2 port#.
                                        ; --------------------------------------
                end
                                        ; --------------------------------------
