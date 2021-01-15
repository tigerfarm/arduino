# Galaxy Program Notes
````
; +----------------------------------+
; |           GALAXY  GAME           |
; |           FOR THE 8080           |
; +----------------------------------+
````
My challenge,
+ Update my assembler so that I can assemble GALAXY.
+ Run using Altair101a.
+ Make program enhancements.

Site I downloaded from:
   https://github.com/deltecent/scelbi-galaxy

--------------------------------------------------------------------------------
### To do next:

Create a new message buffer for the following:
````
You must destroy 16 TIE ships,  in 21 stardates with 6 space stations
++ New:
TIE fighters to destroy: 16, within 21 stardates utilizing 6 space stations.
````
Randomize the start number (in START:). Currently, it's always the same random number.

Continue to improve messages.

Check, if no Sith ships in the sector, don't say: The Sith retaliates.
````
Command?6
Torpedo trajectory(1-8.5) : 3.0
Tracking: 5 6
Tracking: 4 6
Tracking: 3 6
Tracking: 2 6
Tracking: 1 6
YOU MISSED! The Sith retaliates. 
LOSS OF ENERGY 0200
DANGER-SHIELD ENERGY 000
````
--------------------------------------------------------------------------------
````
    YOU MUST DESTROY 24 ALIEN SHIPS IN 29 STARDATES WITH 3 SPACE STATIONS
DNSST:	DB	000 ;Num. space stations	//5B 91 01011011 Data= 6  6
DNALS:	DB	000 ;Num. alien ships		//5C 92 01011100 Data=17 21
DNSTD:	DB	000 ;Num. stardates		//5D 93 01011101 Data=22 26

 -1--2--3--4--5--6--7--8-
1                         STARDATE  3028
2      <*>                CONDITION GREEN
3                         QUADRANT  7 8
4                         SECTOR    2 3
5          *     *        ENERGY    2617
6             *           TORPEDOES 09
7                         SHIELDS   0000
8             *        * 
 -1--2--3--4--5--6--7--8-
Command? 3
GALAXY DISPLAY
-------------------------------------------------
1 203 1 003 1 000 1 011 1 105 1 000 1 000 1 304 1#
-------------------------------------------------
1 204 1 004 1 000 1 000 1 105 1 000 1 003 1 000 1#
-------------------------------------------------
1 001 1 000 1 000 1 000 1 005 1 007 1 014 1 000 1#
-------------------------------------------------
1 005 1 115 1 002 1 203 1 000 1 103 1 000 1 000 1#
-------------------------------------------------
1 005 1 000 1 000 1 004 1 000 1 001 1 004 1 004 1#
-------------------------------------------------
1 000 1 102 1 014 1 000 1 000 1 006 1 006 1 012 1#
-------------------------------------------------
1 003 1 000 1 106 1 207 1 006 1 011 1 002 1 005 1#
-------------------------------------------------
1 000 1 000 1 002 1 000 1 004 1 000 1 000 1 000 1#
-------------------------------------------------
````
--------------------------------------------------------------------------------
#### Program is stored on the SD card:
````
?- + n, SD card directory lising.
++ File:      00001001.BIN  4608
?-

; Start byte, set switches: 
++    2470:00001001 10100110: 00110001 : 31:061 > opcode: lxi sp,STACK
;
; Enter the following to get to the start byte: 12578bx
; Then, hit Enter to show the following. The data byte is correct: 00110001.
;
?- + x, EXAMINE: 2470
?- 
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    *   *   .    .   .   *
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   *   .   .   *    *   .    *   .   .    *   *   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   ^   v   v   ^    ^   v    ^   v   v    ^   ^   v
````
To run,
+ Start Altair 101a and connect.
+ Send the start byte string which sets the switches: 12578bx.
+ "m" and Confirm the loading of SD file: 00001001.BIN (S8 and S11 which is 8b).
+ "x" to EXAMINE the address: 00001001:10100110 (S11 S8 S7 S5 S2 S1 which is 12578b).
+ "r" to run the program.
+ "3" to view the galaxy quadrants.

Start playing.

--------------------------------------------------------------------------------
## Running the program.
````
+ Ready to receive command.
?- + r, RUN.
?- + runProcessor()

DO YOU WANT TO GO ON A SPACE VOYAGE? Y
YOU MUST DESTROY 24 ALIEN SHIPS IN 29 STARDATES WITH 3 SPACE STATIONS
 -1--2--3--4--5--6--7--8-
1          *    <*>       STARDATE  3021
2                *        CONDITION GREEN
3    *                 *  QUADRANT  7 1
4                         SECTOR    1 6
5                         ENERGY    2952
6                         TORPEDOES 10
7 *                       SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-
````
Note, there are 24 ALIEN SHIPS in the galaxy grid below.
Which means that the requirement is to destroy all of the ALIEN SHIPS.
````
COMMAND? 3
GALAXY DISPLAY
-------------------------------------------------
1 105 1 001 1 000 1 203 1 003 1 000 1 004 1 112 1
-------------------------------------------------
1 105 1 000 1 000 1 012 1 105 1 105 1 000 1 000 1
-------------------------------------------------
1 003 1 104 1 000 1 006 1 003 1 003 1 000 1 004 1
-------------------------------------------------
1 103 1 006 1 004 1 000 1 002 1 105 1 003 1 000 1
-------------------------------------------------
1 000 1 003 1 000 1 000 1 103 1 106 1 004 1 003 1
-------------------------------------------------
1 000 1 203 1 007 1 000 1 006 1 001 1 004 1 204 1
-------------------------------------------------
1 005 1 002 1 000 1 105 1 000 1 000 1 007 1 304 1
-------------------------------------------------
1 000 1 007 1 104 1 000 1 000 1 112 1 105 1 103 1
-------------------------------------------------
````

--------------------------------------------------------------------------------
### COMMAND options:
````
O. SPACE SHIP movement
1. Short range scanner
2. Long  range scanner
3. Galaxy display 
4. Shields
5. Phasors
6. TORPEDO
````
------------------------------------------
#### O. SPACE SHIP movement
````
Directions:
   3
 4   2
5     1
 6   8
   7
````
Changing quadrants causes the STARDATE to increment.
````
COMMAND?0
COURSE (1-8.5)? 5.0
WARP FACTOR (0.1-7.7)? 0.1
 -1--2--3--4--5--6--7--8-
1 *       >1<             STARDATE  3019
2                         CONDITION GREEN
3                         QUADRANT  2 4
4                         SECTOR    8 3
5                         ENERGY    2892
6                         TORPEDOES 10
7          *        *     SHIELDS   0000
8      <*>          *    
 -1--2--3--4--5--6--7--8-
COMMAND?0
COURSE (1-8.5)? 3.0
WARP FACTOR (0.1-7.7)? 0.2
 -1--2--3--4--5--6--7--8-
1 *       >1<             STARDATE  3019
2                         CONDITION GREEN
3                         QUADRANT  2 4
4                         SECTOR    6 3
5                         ENERGY    2882
6      <*>                TORPEDOES 10
7          *        *     SHIELDS   0000
8                   *    
 -1--2--3--4--5--6--7--8-
````
---------------
Sample of using a fraction of trajectory.
````
6                         TORPEDOES 06
7 *       x!x             SHIELDS   0000
8   |o|                  
 -1--2--3--4--5--6--7--8-
Command > 6
Torpedo trajectory(1-8.5) : 5.5
Tracking: 7 3
Tracking: 8 2
TIE fighter destroyed.
````
---------------
Fails
````
Can crash into stars when warping into quadrants.
Command? 0
COURSE (1-8.5)? 6.0
WARP FACTOR (0.1-7.7)? 3.0
KA-BOOM, YOU CRASHED INTO A STAR. YOUR SHIP IS DESTROYED
DO YOU WANT TO GO ON A SPACE VOYAGE?

Can run out of STARDATES.
Command? 0
COURSE (1-8.5)? 8.0
WARP FACTOR (0.1-7.7)? 0.1
MISSION FAILED, YOU HAVE RUN OUT OF STARDATES
DO YOU WANT TO GO ON A SPACE VOYAGE? 
````
------------------------------------------
#### 1. Short range scanner
````
COMMAND?1
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3092
2                         CONDITION GREEN
3                         QUADRANT  5 5
4                         SECTOR    1 8
5                         ENERGY    l=08
6                         TORPEDOES 08
7                         SHIELDS   g808
8                        
 -1--2--3--4--5--6--7--8-
````
------------------------------------------
#### 2. Long  range scanner
````
COMMAND?2
L.R. SCAN FOR QUADRANT  8 7
-------------------
1 011 1 002 1 105 1
-------------------
1 000 1 000 1 000 1
-------------------
1 000 1 000 1 000 1
-------------------
````
Key, 105 : 1 alien ship, 0 space stations, 5 stars

------------------------------------------
#### 3. Galaxy display 

````
COMMAND?3
GALAXY DISPLAY
-------------------------------------------------
1 203 1 003 1 000 1 011 1 105 1 000 1 000 1 304 1#  1
-------------------------------------------------
1 204 1 004 1 000 1 000 1 105 1 000 1 003 1 000 1#  2
-------------------------------------------------
1 001 1 000 1 000 1 000 1 005 1 007 1 014 1 000 1#  3
-------------------------------------------------
1 005 1 115 1 002 1 203 1 000 1 103 1 000 1 000 1#  4
-------------------------------------------------
1 005 1 000 1 000 1 004 1 000 1 001 1 004 1 004 1#  5
-------------------------------------------------
1 000 1 102 1 014 1 000 1 000 1 106 1 106 1 112 1#  6
-------------------------------------------------
1 003 1 000 1 106 1 207 1 006 1 011 1 002 1 105 1#  7
-------------------------------------------------
1 000 1 000 1 002 1 000 1 004 1 000 1 000 1 000 1#  8
-------------------------------------------------
1  1     2     3     4     5     6     7     8
````
Key, 105 : 1 alien ship, 0 space stations, 5 stars

--------------------------------------------------------------------------------
#### 4. SHIELD ENERGY TRANSFER
````
Enter a 4 digit value to transfer from ENERGY to SHIELDS.
Enter "-", then a 4 digit value to transfer from SHIELDS to ENERGY.

 -1--2--3--4--5--6--7--8-
1                         STARDATE  3024
2                         CONDITION GREEN
3                         QUADRANT  8 7
4                         SECTOR    6 6
5                         ENERGY    2932
6               <*>       TORPEDOES 10
7                         SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-
COMMAND?4
SHIELD ENERGY TRANSFER = 0300
COMMAND?1
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3024
2                         CONDITION GREEN
3                         QUADRANT  8 7
4                         SECTOR    6 6
5                         ENERGY    2612
6               <*>       TORPEDOES 10
7                         SHIELDS   0300
8                        
 -1--2--3--4--5--6--7--8-
COMMAND?

COMMAND?4
SHIELD ENERGY TRANSFER = -0030
COMMAND?1
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3024
2                         CONDITION GREEN
3                         QUADRANT  8 7
4                         SECTOR    6 6
5                         ENERGY    2622
6               <*>       TORPEDOES 10
7                         SHIELDS   0270
8                        
 -1--2--3--4--5--6--7--8-
````

--------------------------------------------------------------------------------
#### 5. Using Phasors
````
COURSE (1-8.5)? 5.0
WARP FACTOR (0.1-7.7)? 3.0
LOSS OF ENERGY 0600
DANGER-SHIELD ENERGY 000
 -1--2--3--4--5--6--7--8-
1    *  *     *           STARDATE  3027
2               <*>+++    CONDITION RED
3                         QUADRANT  7 3
4       *                 SECTOR    2 6
5 *                       ENERGY    2107
6                         TORPEDOES 10
7    *                    SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-

Enter a four digit number for the phasor shots to be fired at the alien ships in the quadrant.
0250 ENERGY units is enough to destroy an alien ship.
However, they may have shields, so use more, example 0280.
If there are 2 ships, PHASOR ENERGY TO FIRE = 0500, if they were right beside your ship.
Will need to do calculations when using phasors.

COMMAND?5
PHASOR ENERGY TO FIRE = 0250
GALAXY DISPLAY DESTROYED
COMMAND?1
 -1--2--3--4--5--6--7--8-
1    *  *     *           STARDATE  3027
2               <*>       CONDITION GREEN
3                         QUADRANT  7 3
4       *                 SECTOR    2 6
5 *                       ENERGY    1837
6                         TORPEDOES 10
7    *                    SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-

The distance is the number of sectors in the horizontal and vertical direction
   between the space ship and the alien ship.
 -1--2--3--4--5--6--7--8-
1+++                      STARDATE  3026
2                         CONDITION RED
3    *                    QUADRANT  5 2
4       *       +++       SECTOR    5 5
5            <*>          ENERGY    1602
6 *                       TORPEDOES 08
7                         SHIELDS   4976
8    *                   
 -1--2--3--4--5--6--7--8-

5,5 6,4
6-5=1
5-4=1
2/4 = 0500.
First time, I had energy loss, so 500 wasn't enough.
PHASOR ENERGY TO FIRE = 0500
GALAXY DISPLAYE1ERGY = 0477
LOSS OF ENERGY 0119
GALAXY DISPLAYE6ERGY = 0090
LOSS OF ENERGY 0022
---
Second time, I use 0600 and it was enough.
COMMAND?5
PHASOR ENERGY TO FIRE = 0600
GALAXY DISPLAYE1ERGY = 0402
LOSS OF ENERGY 0100
GALAXY DISPLAYDESTROYED
````
--------------------------------------------------------------------------------
````
YOU MUST DESTROY 19 ALIEN SHIPS IN 24 STARDATES WITH 3 SPACE STATIONS
 -1--2--3--4--5--6--7--8-
1+++                      STARDATE  3026
2                         CONDITION RED
3    *                    QUADRANT  5 2
4       *       +++       SECTOR    8 6
5               +++       ENERGY    2952
6 *                       TORPEDOES 10
7                         SHIELDS   0000
8    *          <*>      
 -1--2--3--4--5--6--7--8-

Directions:
   3
 4   2
5     1
 6   8
   7

 -1--2--3--4--5--6--7--8-
1+++                      STARDATE  3026
2                         CONDITION RED
3    *                    QUADRANT  5 2
4       *       +++       SECTOR    8 6
5               +++       ENERGY    2682
6 *                       TORPEDOES 09
7                         SHIELDS   5336
8    *          <*>      
 -1--2--3--4--5--6--7--8-
COMMAND?6
TORPEDO TRAJECTORY(1-8.5) : 3.0HIP RETALIATE SHIP RETALIATE SHIP RETALIATES
ALIEN SHIP DESTROYED
````
--------------------------------------------------------------------------------
Dock to a space station to get energy and torpedoes.
````
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3027
2                         CONDITION GREEN
3                         QUADRANT  6 2
4                         SECTOR    7 2
5                         ENERGY    2672
6                         TORPEDOES 09
7 * <*>         >1<       SHIELDS   0000
8 *                      
 -1--2--3--4--5--6--7--8-
COMMAND?0
COURSE (1-8.5)? 1.0
WARP FACTOR (0.1-7.7)? 0.3
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3027
2                         CONDITION GREEN
3                         QUADRANT  6 2
4                         SECTOR    7 5
5                         ENERGY    2952
6                         TORPEDOES 10
7 *          <*>>1<       SHIELDS   0000
8 *                      
 -1--2--3--4--5--6--7--8-
````
--------------------------------------------------------------------------------
#### 6. Using torpedoes
````
 -1--2--3--4--5--6--7--8-
1            <*>   +++    STARDATE  3025
2                *        CONDITION RED
3    *              *     QUADRANT  6 7
4                         SECTOR    1 5
5 *                       ENERGY    2862
6          *        *     TORPEDOES 10
7                         SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-
COMMAND?6
TORPEDO TRAJECTORY(1-8.5) : 3.0
YOU MISSED! ALIEN SHIP RETALIATES
LOSS OF ENERGY 0200
COMMAND?6
TORPEDO TRAJECTORY(1-8.5) : 1.0
TRACKING: 1 6
TRACKING: 1 7
ALIEN SHIP DESTROYED
COMMAND?
````
--------------------------------------------------------------------------------
### How to Save a Game
````
+ "g" to see the game status before saving the game. For example:
You must destroy 10 Sith ships in  15 stardates with 6 space stations
+ "s" to STOP the program, to go into WAIT mode.
+ Set Sense switches to the file number to save to. For example set to: "c".
+ "M" for memory upload (save). Confirm, "y".
+ "r" to RUN the program, to go into RUN mode.
+ "g" to confirm the same game status. For example:
You must destroy 10 Sith ships in  15 stardates with 6 space stations
````
Following is an example save and continue the game.
````
Command?g
You must destroy 10 Sith ships in  15 stardates with 6 space stations
Command?1
 -1--2--3--4--5--6--7--8-
1         x!x             STARDATE  3035
2                         CONDITION GREEN
3                         QUADRANT  1 4
4                         SECTOR    1 4
5            >1<          ENERGY    2942
6                         TORPEDOES 10
7                         SHIELDS   0000
8                   *    
 -1--2--3--4--5--6--7--8-
Command?+ s, STOP
?- (I hit the enter key to show the switch status.
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    .   .   .    .   .   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   *   .   .   .   .    .   .    .   *   .    .   .   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   ^   v   v   v   v    v   v    v   v   v    v   v   v
 ------ 
+ Ready to receive command.
?- + M, Write program Memory into a file.
++ Write filename: 00010000.bin
++ Confirm, y/n: 
+ Confirmed.
+ Write completed, file closed.
?- + r, RUN.
?- + runProcessor()
[Command?]1
 -1--2--3--4--5--6--7--8-
1         x!x             STARDATE  3035
2                         CONDITION GREEN
3                         QUADRANT  1 4
4                         SECTOR    1 4
5            >1<          ENERGY    2932
6                         TORPEDOES 10
7                         SHIELDS   0000
8                   *    
 -1--2--3--4--5--6--7--8-
Command?g
You must destroy 10 Sith ships in  15 stardates with 6 space stations
````
The game is ready to continue.

--------------------------------------------------------------------------------
### The Win!
````
Command?0
COURSE (1-8.5)? 5.0
WARP FACTOR (0.1-7.7)? 0.4
 -1--2--3--4--5--6--7--8-
1      x!x                STARDATE  3042
2                      *  CONDITION RED
3                      *  QUADRANT  4 4
4             *           SECTOR    1 3
5                         ENERGY    1312
6                         TORPEDOES 05
7                         SHIELDS   0000
8      |o|               
 -1--2--3--4--5--6--7--8-
Command?6
Torpedo trajectory(1-8.5) : 7.0
Tracking: 2 3
Tracking: 3 3
Tracking: 4 3
Tracking: 5 3
Tracking: 6 3
Tracking: 7 3
Tracking: 8 3
CONGRATULATIONS! You eliminated all the Sith star ships.   
Ready for a Star Wars space mission? 
````
--------------------------------------------------------------------------------
### Program addresses
````
12578bx Galaxy program start address, label: GALAXY.
        First instruction is to set the stack pointer.
D7  D6   D5  D4  D3   D2  D1  D0
.   .    *   *   .    .   .   *

2589bx to get to the Command Menu. First instruction is to set the stack pointer.
; ++    2852:00001011 00100100: 00110001 : 31:061 > opcode: lxi sp,STACK
 D7  D6   D5  D4  D3   D2  D1  D0
 .   .    *   *   .    .   .   *

YOU MUST DESTROY 21 ALIEN SHIPS IN 26 STARDATES WITH 6 SPACE STATIONS
DNSST:	DB	000 ;Num. space stations	//5B 91 01011011 Data= 6
DNALS:	DB	000 ;Num. alien ships		//5C 92 01011100 Data=21
DNSTD:	DB	000 ;Num. stardates		//5D 93 01011101 Data=26

Key:
S.S. : space stations
A.S. : alien ships

	ANA	A		;Quadrant crossing occurred?
	JZ	NOX		;No, complete move
	MVI	L,05DH		;Yes, fetch stardate value.
	MOV	B,M
	DCR	B		;Decrement stardate counter
	JZ	TIME		;If 0, end of game
	MOV	M,B		;Else save new date
NOX:
````
--------------------------------------------------------------------------------
````
Original version
-------------------------------------------------
1 203 1 003 1 000 1 011 1 105 1 000 1 000 1 304 1#
-------------------------------------------------
1 204 1 004 1 000 1 000 1 105 1 000 1 003 1 000 1#
-------------------------------------------------
1 001 1 000 1 000 1 000 1 005 1 007 1 014 1 000 1#
-------------------------------------------------
1 005 1 115 1 002 1 203 1 000 1 103 1 000 1 000 1#
-------------------------------------------------
1 005 1 000 1 000 1 004 1 000 1 001 1 004 1 004 1#
-------------------------------------------------
1 000 1 102 1 014 1 000 1 000 1 106 1 106 1 112 1#
-------------------------------------------------
1 003 1 000 1 106 1 207 1 006 1 011 1 002 1 105 1#
-------------------------------------------------
1 000 1 000 1 002 1 000 1 004 1 000 1 000 1 000 1#

-------------------------------------------------
Current version saved in "c", file: 00010000.BIN  5120.

You must destroy 21 Sith ships in  26 stardates with 6 space stations
-------------------------------------------------
1 203 1 003 1 000 1 011 1 105 1 000 1 000 1 304 |
-------------------------------------------------
1 204 1 004 1 000 1 000 1 105 1 000 1 003 1 000 |
-------------------------------------------------
1 001 1 000 1 000 1 000 1 005 1 007 1 014 1 000 |
-------------------------------------------------
1 005 1 115 1 002 1 203 1 000 1 103 1 000 1 000 |
-------------------------------------------------
1 005 1 000 1 000 1 004 1 000 1 001 1 004 1 004 |
-------------------------------------------------
1 000 1 102 1 014 1 000 1 000 1 106 1 106 1 112 |
-------------------------------------------------
1 003 1 000 1 106 1 207 1 006 1 011 1 002 1 105 |
-------------------------------------------------
1 000 1 000 1 002 1 000 1 004 1 000 1 000 1 000 |
-------------------------------------------------

Work version
You must destroy 21 Sith ships in  26 stardates with 6 space stations
-------------------------------------------------
1 003 1 003 1 000 1 011 1 005 1 000 1 000 1 004 |
-------------------------------------------------
1 004 1 004 1 000 1 000 1 105 1 000 1 003 1 000 |
-------------------------------------------------
1 001 1 000 1 000 1 000 1 005 1 007 1 014 1 000 |
-------------------------------------------------
1 005 1 015 1 002 1 003 1 000 1 003 1 000 1 000 |
-------------------------------------------------
1 005 1 000 1 000 1 004 1 000 1 001 1 004 1 004 |
-------------------------------------------------
1 000 1 002 1 014 1 000 1 000 1 006 1 006 1 012 |
-------------------------------------------------
1 003 1 000 1 006 1 007 1 006 1 011 1 002 1 005 |
-------------------------------------------------
1 000 1 000 1 002 1 000 1 004 1 000 1 000 1 000 |
-------------------------------------------------
You must destroy 06 TIE ships,  in 12 stardates with 6 space stations
````
; ------------------------------------------------------------------------------
````
#### Saved memory files:
01000000.bin e : Current test
00100000.bin d : Alternate test
00010000.bin c : Save game, near completion
00010001.bin   : Save game, final sector
````
; ------------------------------------------------------------------------------
````
12578bx Galaxy program start address, label: GALAXY.
2589bx Command Menu start address. First instruction is to set the stack pointer.
cmyc12578bxr for the Arduino IDE monitor.
````