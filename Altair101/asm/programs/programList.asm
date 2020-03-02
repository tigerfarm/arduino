                                        ; ------------------------------------------
                                        ; List program information to the LCD.
                                        ; ------------------------------------------
                jmp Start
                                        ; ------------------------------------------
                                        ; Data declarations
                                        ;
                                        ; ------------------------------------------
                    ;1234567890123456   ; At most, 16 characters which is the max line length.
    prompt      db  'Program num > '
                                        ;
    SENSE_SW    equ     255             ; Input port address: toggle sense switch byte, into register A.
    LCD_PORT    equ     1               ; Output port: print to the serial port.
    CLEAR_SCR   equ     2               ; Clear screen.
                                        ; -------------------
                      ;1234567890123456 Note, the newline characters don't count.
    p00000000   db  '\n+ All NOPs\n'
    p00000001   db  '\n+ LCD on/off\n'
    p00000010   db  '\n+ Jump loop\n'
    p00000011   db  '\n+ Kill the Bit\n'
    p00000100   db  '\n+ Program List\n'
    p00000101   db  '\n+ Add 2 numbers\n'
                      ;1234567890123456
                                        ; -------------------
                                        ; Special characters:
    NL          equ     10              ; New line, '\n'.
    TERMB       equ     0ffh            ; String terminator.
                                        ;
                                        ; ------------------------------------------
    Start:
                mvi a,1                 ; Turn LCD backlight on.
                out LCD_PORT
                mvi a,CLEAR_SCR
                out LCD_PORT
                                        ; ------------------------------------------
                                        ; Get the sense switch data byte.
    GetByte:
                call printPrompt        ; Print prompt.
                hlt                     ; Halt to get the data byte.
                                        ; Use RUN to restart the program.
                in SENSE_SW             ; Get toggle sense switch byte value into register A.
                out 37                  ; Print register A to the serial port.
                                        ; ------------------------------------------
                                        ; Process the input byte value.
                cpi 0                   ; If input byte is 0,
                jz s00000000            ;   jump.
                cpi 1
                jz s00000001
                cpi 2
                jz s00000010
                cpi 3
                jz s00000011
                cpi 4
                jz s00000100
                cpi 5
                jz s00000101
                                        ; Else, unknown.
                mvi a,'?'
                out LCD_PORT
                call printNL
                jmp GetByte
                                        ; ------------------------------------------
    s00000000:
                lxi h,p00000000
                call print
                jmp GetByte
    s00000001:
                lxi h,p00000001
                call print
                jmp GetByte
    s00000010:
                lxi h,p00000010
                call print
                jmp GetByte
    s00000011:
                lxi h,p00000011
                call print
                jmp GetByte
    s00000100:
                lxi h,p00000100
                call print
                jmp GetByte
    s00000101:
                lxi h,p00000101
                call print
                jmp GetByte
                                        ; ------------------------------------------
                                        ; Print the prompt.
    printPrompt:
                lxi h,prompt
                call print
                ret
                                        ; ------------------------------------------
                                        ; Print a DB string which starts at address, M.
    print:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz printed
                out LCD_PORT            ; Out register A to the print port.
                inr m                   ; Increment H:L register pair.
                jmp print
    printed:
                ret
                                        ; ------------------------------------------
                                        ; Print a newline.
    printNL:
                mvi a,NL
                out LCD_PORT
                ret

                end

