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
    prompt:
                mvi a,'>'
                out SERIAL_PORT
    GetByte:
                                    ; ----------------------
    Halt:
    Stop:
                hlt
                jmp GetByte

    WaitTermReady:
                IN 00               ; Serial input into register A.
                ANI 80h             ; AND 1000:0000 with register A.
                JNZ WaitTermReady
                POP PSW
                OUT 01
                RET
    InputChar:
                IN 00               ; Serial input into register A.
                ANI 01              ; AND 1 with register A.
                JNZ InputChar
                IN 01
                ANI 7Fh
                RET

    TestBreakKey:
                IN 00               ;Exit if no key pressed.
                ANI 01
                RNZ
                CALL InputChar
                CPI 0x03            ;Break key?
                JMP Stop	


                                    ; --------------------------------------------
                end
                                    ; --------------------------------------------

