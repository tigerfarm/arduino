                            ; --------------------------------------
                            ; Add ADC, and Test ADC with a prompt and result on LCD.
                            ; Add register to A with carry (with ZSPCA).
                            ; Steampunk file# ?.
                            ;
                            ; --------------------------------------
    Start:
                            ; Add two numbers together:
                            ; Register A contents + ADI immediate value.
                            ;
            mvi a,2         ; Register A = 2.
            adi 3           ; Add immediate number to register A.
                            ;
            sta 64          ; Move result (register A) to address 64.
                            ;
                            ; Echo the result to the Arduino serial monitor.
            mvi h,0         ; Set H:L to address 64.
            mvi l,64
            out 36          ; Print the content at the H:L address.
                            ; --------------------------------------
            hlt
            jmp Start
            end
