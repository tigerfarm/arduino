                                        ; --------------------------------------
                                        ; List program information to the LCD.
                                        ; --------------------------------------
                jmp Start
                                        ; --------------------------------------
                                        ;
    LCD_PORT    equ     1               ; Output port address: LCD.
    SENSE_SW    equ     255             ; Input port address: toggle sense switches.
                    ;1234567890123456   ; At most, 16 characters which is the max line length.
    prompt      db  'Program num >\n'
                    ;1234567890123456   ; At most, 16 characters which is the max line length.
    NO_INPUT    db  '+ No input.'
    Echo        db      'Echo: '
    TERMB       equ     0ffh            ; String terminator.
    NL          equ     10              ; 10 is new line, '\n'.
                                        ; --------------------------------------
    Start:
                mvi a,1                 ; Turn LCD backlight on.
                out LCD_PORT
                mvi a,'a'
                out LCD_PORT
                hlt
                                        ; --------------------------------------
    GetInput:
                call printPrompt        ; Print prompt.
                hlt                     ; Halt to get the data byte.
                                        ; Use RUN to restart the program.
                in SENSE_SW             ; Get toggle sense switch byte value into register A.
                out 37                  ; Print register A to the serial port.
                                        ; ----------------------
                cpi 0                   ; If input byte is 0,
                jz NoInput              ;   Ask again for a value.
                                        ;
                                        ; ------------------------------------------
                                        ; Process the input byte value.
                mov b,a                 ; Move the byte to register B.
                lxi h,Echo
                call lcdPrint
                mov a,b                 ; Move the input data byte back to register A.
                out LCD_PORT
                hlt
                jmp GetInput
                                        ; ------------------------------------------
    NoInput:
                lxi h,NO_INPUT
                call lcdPrint
                hlt
                jmp GetInput
                                        ; ------------------------------------------
                                        ; Clear the LCD screen and print the prompt.
    printPrompt:
                mvi a,2
                out LCD_PORT
                lxi h,prompt
                call lcdPrint
                ret
                                        ; ------------------------------------------
                                        ; Print a DB string which starts at address, M.
    lcdPrint:
                mov a,m                 ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB               ; Compare to see if it's the string terminate byte.
                jz lcdPrinted
                out LCD_PORT          ; Out register A to the print port.
                inr m                   ; Increment H:L register pair.
                jmp lcdPrint
    lcdPrinted:
                ret
                                        ; --------------------------------------
                end
