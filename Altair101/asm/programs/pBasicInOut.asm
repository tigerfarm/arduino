; --------------------------------------------------------------------------------
; Basic 4K I/O
;
; Program to test modifications to Altair101a.ino to handle 4K Basic I/0.
;   Effectively, simulating the serial interface card, 88-SIO, using I/O addresses 0 and 1.
;
; --------------------------------------------------------------------------------
; Following is from the documentation section: Interacting with software via a terminal.
;
; Early terminals used only 7-bit characters.
;   Terminal would ignore the 8th bit.
; 
; Output:
;   Filter out the 8th bit by enabling the “Use 7 bits” setting.
; In Altair101a.ino, remove(set to zero) the output 8th bit.
;   regAdata = regAdata & B01111111;
;
; Input:
;   Early terminals only had upper case characters.
;   Use only uppercase letters in 4k BASIC.
;   4k BASIC uses the 8th bit of a character to define end-of-string.
;   4k BASIC backspace: underscore (“_”) will print, and delete the last character from the input buffer.
;   Copy and paste doesn't work for input when using a Mega, it's too slow.
;
; Simulating 88-SIO using I/O addresses 0 and 1.
; 88- 2SIO has two serial ports via jumpers: addresses 16/17 first port, and 18/19 second port.
;
; The simulator has a serial setting to automatically covert incoming lower-case characters to upper case.
;
; --------------------------------------------------------------------------------
                                    ;
    Start:
                lxi sp,1024         ; Set stack pointer.
    prompt:
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
                mvi a,'>'
                out PRINT_PORT
                mvi a,' '
                out PRINT_PORT
                                    ; ----------------------
    InputNext:                      ;  File: basic4k.lst, line# 0382
                call InputChar
                CPI '\r'            ; CR
                JZ TerminateInput
                CPI '\n'            ; LF
                JZ TerminateInput
                CPI 3               ; Crtl+c
                JZ TestBreakKey
                                    ;
                CPI ' '             ;   If < ' '
                JC InputNext        ;or
                CPI 7Dh            ;   > '}' ... was 0x7D
                JNC InputNext       ;then loop back.
                                    ;
                mov b,a
                mvi a,':'
                out PRINT_PORT
                mov a,b
                out PRINT_PORT
                mvi a,':'
                out PRINT_PORT
                                    ;
                jmp InputNext
                                    ; ----------------------
    Stop:
                mov b,a
                mvi a,'O'
                out PRINT_PORT
                mov b,a
                mvi a,'K'
                out PRINT_PORT
                mvi a,'\r'
                out PRINT_PORT
                mvi a,'\n'
                out PRINT_PORT
    TerminateInput:
    Halt:
                hlt
                jmp prompt
                                    ; --------------------------------------------
                                    ; Get an input character. File: basic4k.lst, line# 0341
    InputChar:
                IN 00               ; Indicator on an input. Serial input into register A.
                ANI 01              ; AND 1 with register A.
                JNZ InputChar       ; 1 indicates an input character in serial input 01.
                IN 01               ; Serial input character into register A.
                ANI 7Fh             ; Remove the 8th bit.
                RET
                                    ; --------------------------------------------
                                    ; Ctrl+c break from running a program.
                                    ; Currently,
                                    ;   First Ctrl+c, stops the program
                                    ;   Second Ctrl+c, brings back the OK prompt.
    TestBreakKey:
                IN 00               ;Exit if no key pressed.
                ANI 01
                RNZ
                CALL InputChar
                CPI 03              ;Break key? (Ctrl+c)
                JZ Stop
                JMP InputNext
                                    ; --------------------------------------------
                                    ; --------------------------------------------
                                    ; Test for an input character with the 8th bit set.
    WaitTermReady:
                IN 00               ; Serial input into register A.
                ANI 80h             ; AND 1000:0000 with register A.
                JNZ WaitTermReady
                ; POP PSW
                OUT 01
                RET
                                    ; Declarations.
    SENSE_SW    equ     255         ; Input port address: toggle sense switch byte, into register A.
    PRINT_PORT  EQU 2               ; USB Serial2 port#.

                                    ; --------------------------------------------
                end
                                    ; --------------------------------------------

