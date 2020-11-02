                                        ; ------------------------------------------
                                        ; List program information to the LCD.
                                        ; ------------------------------------------
                jmp Start
                                        ; ------------------------------------------
                                        ; Data declarations
                                        ;
    SENSE_SW    equ     255             ; Input port address: toggle sense switch byte, into register A.
    LCD_PORT    equ     1               ; Output port: print to the serial port.
    CLEAR_SCR   equ     2               ; Clear screen.
    TERMB       equ     0ffh            ; String terminator.
                                        ;
                                        ; -------------------
                    ;1234567890123456   ; At most, 16 characters which is the max line length.
    prompt      db  'Program num > '
                      ;1234567890123456
    p00000000   db  '\n+ All NOPs\n'
    p00000001   db  '\n+ LCD on/off\n'
    p00000010   db  '\n+ Jump loop\n'
    p00000011   db  '\n+ Kill the Bit\n'
    p00000100   db  '\n+ Program List\n'
    p00000101   db  '\n+ Add 2 numbers\n'
    p01000000   db  '\n+ Sound On/Off\n'
    p10000000   db  '\nCurrent test 1a\n'
    p10000001   db  '\nCurrent test 1b\n'
                      ;1234567890123456
                                        ;
                                        ; ------------------------------------------
    Start:
                mvi a,1                 ; Turn LCD backlight on and clear the screen display.
                out LCD_PORT
                mvi a,CLEAR_SCR
                out LCD_PORT
                                        ; ------------------------------------------
                                        ; Get the sense switch data byte.
    GetByte:
                lxi h,prompt            ; Prompt for input.
                call print
                hlt                     ; Halt to get the data byte.
                                        ; Use RUN to restart the program.
                                        ; ------------------------------------------
                in SENSE_SW             ; Get toggle sense switch byte value into register A.
                cpi 0                   ; If input byte value,
                jz s00000000            ;   jump to label.
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
                                        ;
                cpi 128                 ; Test descriptions
                jz s10000000
                cpi 129
                jz s10000001
                                        ; Else, unknown.
                mvi a,'?'
                out LCD_PORT
                mvi a,'\n'
                out LCD_PORT
                jmp GetByte
                                        ; ------------------------------------------
                                        ; Print the program name.
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
    s10000000:
                lxi h,p10000000
                call print
                jmp GetByte
    s10000001:
                lxi h,p10000001
                call print
                jmp GetByte
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
                end

