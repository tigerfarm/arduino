                            ; --------------------------------------
                            ; Test ADD, and moving data from addresses to registers and back.
                            ;
                            ; 8085 program to add two 8 bit numbers
                            ; https://www.geeksforgeeks.org/assembly-language-program-8085-microprocessor-add-two-8-bit-numbers/
                            ;
                            ;
                            ; STA a: Store register A data to an address a(hb:lb).
                            ; LDA a: Load register A with the data at the address a(hb:lb).
                            ;
                           ; --------------------------------------
            lxi sp,512      ; Set stack pointer which is used with CALL and RET.
    Start:
            mvi a,32
            out 37          ; Print register A
            sta Addr1       ; Addr1<-A
            mvi a,33
            out 37
            sta Addr2       ; Addr2<-A
                            ;
            mvi a,0
            out 37
            lda Addr1       ; A<-(Addr1)
            out 37
            MOV H, A        ; H<-A
            out 34          ; Print register H
            lda Addr2       ; A<-(Addr2)
            out 37
                            ;
            ADD H           ; A<-A+H
            out 37          ; Print register A
                            ;
            MOV L, A        ; L<-A
            MVI A,0         ; A<-0
            ADC A           ; A<-A+A+carry
            MOV H, A        ; H<-A
            SHLD Addr2      ; H->Addr2+1, L->Addr2
            out 36          ; Print the register values for H:L, and the content at that address.
                            ; --------------------------------------
            mvi a,'\r'
            out 3
            mvi a,'\n'
            out 3
            hlt             ; The program will halt at each iteration, after the first.
            jmp Start       ; Jump to start of the test program.
                            ; --------------------------------------
    Addr1   ds     1
    Addr2   ds     1
                            ; --------------------------------------
                end
                            ; --------------------------------------
