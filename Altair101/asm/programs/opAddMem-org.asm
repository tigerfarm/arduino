                            ; --------------------------------------
                            ; Test ADD, and moving data from addresses to registers and back.
                            ;
                            ; 8085 program to add two 8 bit numbers
                            ; https://www.geeksforgeeks.org/assembly-language-program-8085-microprocessor-add-two-8-bit-numbers/
                            ;
                            ; --------------------------------------
            jmp Start       ; Jump to start of the test program.
                            ;
    Addr1   equ     128
    Addr2   ds      2
                            ;
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
            LDA 2050        ; A<-[2050]
            MOV H, A        ; H<-A
            LDA 2051        ; A<-[2051]
            ADD H           ; A<-A+H
            MOV L, A        ; L<-A
            MVI A,0         ; A<-0
            ADC A           ; A<-A+A+carry
            MOV H, A        ; H<-A
            SHLD 3050       ; H->3051, L->3050
                            ; --------------------------------------
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ; --------------------------------------
            end