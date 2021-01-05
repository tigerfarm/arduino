                            ; --------------------------------------
                            ; Test JP.
                            ;
                            ; --------------------------------------
    Start:
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            mvi a,'1'       ; '1' first jump.
            out 3
                            ; --------------------------------------
            mvi a,1100001b
            ;mvi a,0
            ANI SIORDR
            ANI	0FFH        ;Non-zero?
            JZ	INPCK1
            ORI	0FFH        ;Set sign flag
    INPCK1:
            JP  Jumped      ; if P=1, jump (PC <- adr)
                            ; --------------------------------------
            mov b,a
            mvi a,'N'       ; Not jumped.
            out 3
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            hlt
                            ; --------------------------------------
            jmp Start
                            ; --------------------------------------
    Jumped:
            mvi a,'J'       ; 'S' for Success. 
            out 3
            mvi a,'\n'
            out 3
            mvi a,'\r'
            out 3
            hlt             ; Program output before the HLT, is: "123S".
                            ; --------------------------------------
            jmp Start
                            ; --------------------------------------
    SIORDR  EQU 01H         ;RCV READY MASK
            end
                            ; --------------------------------------
                            ; Successful run:

Needs work:

- Number of errors = 1
- List Error Messages:
-- 12:             mvi a, 1100001b
-- -- Error3, INVALID, Opcode: a, 11010011  + p1|1100001b|
+ End of list.

                            ;
                            ; --------------------------------------
