                            ; --------------------------------------
                            ; Test ADD, and moving data from addresses to registers and back.
                            ;
                            ; 8085 program to add two 8 bit numbers
                            ; https://www.geeksforgeeks.org/assembly-language-program-8085-microprocessor-add-two-8-bit-numbers/
                            ;
                            ; --------------------------------------
            jmp Start       ; Jump to start of the test program.
                            ;
    Addr1   equ     32
    Addr2   equ     33
                            ;
    Halt:
            hlt             ; The program will halt at each iteration, after the first.
                            ; --------------------------------------
    Start:
            LDA Addr1       ; A<-(Addr1)
            MOV H, A        ; H<-A
            LDA Addr2       ; A<-(Addr1)
            ADD H           ; A<-A+H
            MOV L, A        ; L<-A
            MVI A,0         ; A<-0
            ADC A           ; A<-A+A+carry
            MOV H, A        ; H<-A
            SHLD Addr2      ; H->Addr2+1, L->Addr2
                            ; --------------------------------------
            jmp Halt        ; Jump back to the early halt command.
                            ; --------------------------------------
            end