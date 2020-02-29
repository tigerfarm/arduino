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
    ; ECHO        db      '+ Input data: '
    ; NO_INPUT    db      '+ No input.'
                                        ; -------------------
                    ;1234567890123456
    p00000000   db  '+ All NOPs'
    p00000001   db  '+ LCD on/off'
    p00000010   db  '+ Jump loop'
    p00000011   db  '+ Kill the Bit'
    p00000100   db  '+ Program List'
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
                mvi a,'a'
                out LCD_PORT
                call printNL
                mvi a,'b'
                out LCD_PORT
                call printNL
                mvi a,'c'
                out LCD_PORT
                call printNL
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
                                        ; Else, unknown.
                mvi a,'?'
                out LCD_PORT
                call printNL
                jmp GetByte
                                        ; ------------------------------------------
    s00000000:
                call printNL
                lxi h,p00000000
                call print
                call printNL
                jmp GetByte
    s00000001:
                call printNL
                lxi h,p00000001
                call print
                call printNL
                jmp GetByte
    s00000010:
                call printNL
                lxi h,p00000010
                call print
                call printNL
                jmp GetByte
    s00000011:
                call printNL
                lxi h,p00000011
                call print
                call printNL
                jmp GetByte
    s00000100:
                call printNL          ; Causes printPrompt to no longer print the prompt.
                lxi h,p00000100
                call print
                call printNL
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
                mov a,m             ; Move the data from H:L address to register A. (HL) -> A. 
                cpi TERMB           ; Compare to see if it's the string terminate byte.
                jz printed
                out LCD_PORT        ; Out register A to the print port.
                inr m               ; Increment H:L register pair.
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

