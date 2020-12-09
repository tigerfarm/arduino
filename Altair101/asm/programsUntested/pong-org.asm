                ;---------------------------------------------------------------
                ;  PONG for Altair front panel.
                ;     May 2014, Mike Douglas
                ; 
                ;  Left player quickly toggles A15 to hit the "ball."
                ;  Right player toggles A8.
                ;  Score is kept in memory locations
                ;  80h and 81h (left and right).
                ;  Score is missed balls, so the lower number wins.
                ;---------------------------------------------------------------
                ; Parameters:
                ;   SPEED determines how fast the ball moves. Higher values
                ;       move faster. Speed can easily be patched at address 1.
                ;
                ;   HITMSK, HITEDG determines how easy it is to hit the ball.
                ;       These are best changed by re-assembling the program.
                ;       Frankly, even the medium setting is too easy. Probably
                ;       best to stick with "hard" and change difficulty by
                ;       adjusting SPEED.
                ;
                ;   DEMO mode can be set by patching 35h and 65h to zero
                ;       and raising A15 and A8.
                ;
000E =          SPEED   equ     0eh         ;higher value is faster

0001 =          HITMSKR equ     01h         ;01h=hard, 03h=med, 07h=easy 
0002 =          HITEDGR equ     02h         ;02h=hard, 04h=med, 08h=easy
                                            ;00h=demo with A15,A8 up

0010 =          HITMSKL equ     10h         ;10h=hard, 18h=med, 1ch=easy 
0008 =          HITEDGL equ     08h         ;08h=hard, 04h=med, 02h=easy
                                            ;00h=demo with A15,A8 up

                ;---------------------------------------------------------------- 
                ; Initialize
0000                    org     0
0000 010E00             lxi     b,SPEED     ;BC=adder for speed 
0003 317D00             lxi     sp,stack    ;init stack pointer 
0006 210000             lxi     h,0         ;zero the score 
0009 228000             shld    scoreL 
000C 110080             lxi     d,8000h     ;D=ball bit, E=switch status 
000F C31E00             jmp     rLoop       ;begin moving right

                ;------------------------------------------------------------------
                ; ledOut - Write D to LEDs A15-A8.
                ;   Loop accessing the address in DE which causes the proper LED
                ;   to light on the address lights. This routine is placed low
                ;   in memory so that address light A7-A5 remain off to make
                ;   A15-A8 look better.
                ;------------------------------------------------------------------ 
0012 210000     ledOut  lxi     h,0         ;HL=16 bit counter 
0015 1A         ledLoop ldax    d           ;display bit pattern on 
0016 1A                 ldax    d           ;...upper 8 address lights 
0017 1A                 ldax    d 
0018 1A                 ldax    d 
0019 09                 dad     b           ;increment display counter 
001A D21500             jnc     ledLoop 
001D C9                 ret

                ;----------------------------------------------------------------
                ;  Moving Right
                ;---------------------------------------------------------------- 
001E CD1200     rLoop   call    ledOut      ;output to LEDs A15-A8 from D
                ; Record the current right paddle state (A8) in the bit position
                ;   in E corresponding to the present ball position. 
0021 DBFF               in      0ffh        ;A=front panel switches 
0023 E601               ani     01h         ;get A8 alone 
0025 CA2D00             jz      chkRt       ;switch not up, bit already zero 
0028 7A                 mov     a,d         ;set bit in E corresponding to... 
0029 B3                 ora     e           ;   the current ball position 
002A E61F               ani     1fh         ;keep b7-b5 zero
002C 5F                 mov     e,a
                ; See if at the right edge. If so, see if A8 "paddle" has a hit 
002D 7A         chkRt   mov     a,d         ;is ball at right edge? 
002E E601               ani     1 
0030 CA4500             jz      moveRt      ;no, continue moving right 
0033 7B                 mov     a,e         ;switch state for each ball position 
0034 E602               ani     HITEDGR     ;test edge for switch too early 
0036 C23F00             jnz     missRt      ;hit too early 
0039 7B                 mov     a,e         ;test for hit 
003A E601               ani     HITMSKR 
003C C27300             jnz     moveLfR     ;have a hit, switch direction
                ; missRt - right player missed the ball. Increment count 
003F 218100     missRt  lxi     h,scoreR    ;increment right misses 
0042 34                 inr     m
                ; moveRt - Move the ball right again. 
0043 1E00       moveRtR mvi     e,0         ;reset switch state 
0045 7A         moveRt  mov     a,d         ;move right again 
0046 0F                 rrc 
0047 57                 mov     d,a
0048 C31E00             jmp     rLoop

                ;----------------------------------------------------------------
                ;  Moving left
                ;---------------------------------------------------------------- 
04B CD1200      lLoop   call    ledOut      ;output to LEDs A15-A8 from D
                ; Record the current left paddle state (A15) in the bit position
                ;   in E corresponding to the present ball position. 
004E DBFF               in      0ffh        ;A=front panel switches 
0050 E680               ani     80h         ;get A15 alone 
0052 CA5D00             jz      chkLft      ;switch not up, bit already zero 
0055 7A                 mov     a,d         ;A=ball position 
0056 0F                 rrc                 ;move b7..b3 to b4..b0 
0057 0F                 rrc                 ;   so LEDs A7-A5 stay off 
0058 0F                 rrc
0059 B3                 ora     e           ;form switch state in E 
005A E61F               ani     1fh         ;keep b7-b5 zero 
005C 5F                 mov     e,a
                ; See if at the left edge. If so, see if A15 "paddle" has a hit 
005D 7A         chkLft  mov     a,d         ;is ball at left edge? 
005E E680               ani     80h 
0060 CA7500             jz      moveLf      ;no, continue moving left 
0063 7B                 mov     a,e         ;switch state for each ball position 
0064 E608               ani     HITEDGL     ;test edge for switch too early 
0066 C26F00             jnz     missLf      ;hit too early 
0069 7B                 mov     a,e         ;test for hit 
006A E610               ani     HITMSKL 
006C C24300             jnz     moveRtR     ;have a hit, switch direction
                ; missLf - left player missed the ball. Increment count 
006F 218000     missLf  lxi     h,scoreL    ;increment left misses 
0072 34                 inr     m
                ; moveLf - Move the ball left again. 
0073 1E00      moveLfR: mvi     e,0         ;reset switch state 
0075 7A         moveLf  mov     a,d         ;move right again 
0076 07                 rlc
0077 57                 mov     d,a 
0078 C34B00             jmp     lLoop

                ;------------------------------------------------------------------
                ; Data Storage
                ;------------------------------------------------------------------ 
007B                    ds      2           ;stack space
007D =         stack    equ     $ 
0080                    org     80h         ;put at 80h and 81h 
0080           scoreL   ds      1           ;score for left paddle 
0081           scoreR   ds      1           ;score for right paddle
0082                    end
