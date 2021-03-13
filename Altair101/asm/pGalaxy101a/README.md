# Galaxy Program Notes
````
; +----------------------------------+
; |           GALAXY  GAME           |
; |           FOR THE 8080           |
; +----------------------------------+
````
[Galaxy101a.asm](Galaxy101a.asm) is my updated version of the Galaxy80 game.
Galaxy80 is a variation on the Star Trek game that is popular is Altair 8800 users.

A major change I made to the game, was to change from a Star Trek theme to a Star Wars theme.
The reason is that the Star Trek theme is exploration and to boldly go where no one has gone before.
Star Wars has a warrior theme.
Since the game is for you to destroy enemies, a Star Wars theme make sense.

This document has notes to load, run, and play the game.
This document also includes my developer notes.

--------------------------------------------------------------------------------

My challenge was to,
+ Update my assembler program so that I could assemble the program.
+ Make program changes to get the program to run on my Altair101a.
+ Run and test the program.
+ Make enhancements.

I can assemble and run it. I even fixed a usage flow option.
The game is enhanced as far as I can go on my Mega 2560 Altair101a because of the memory limitations.
The enhanced program has very little memory left for me to upgrade the playing experience.

[Galaxy101.asm](Galaxy101.asm) is the current tested program.
[GALAXY80.asm](GALAXY80.asm) is the original that I downloaded from a GitHub
[project](https://github.com/deltecent/scelbi-galaxy).

--------------------------------------------------------------------------------
### To do next:

Modify the display of the sector map and messages,
to work similarly to the Altair101a virtual front panel display and log messages.
+ Use VT100 escape sequences to print the sector map in one location, at the top.
+ Messages display below.

Switch over to use 88-2SIO CHANNEL A SERIAL INTERFACE so that this program can run an actual Altair 8800.
Or, compile as second version with the 88-2SIO CHANNEL A SERIAL INTERFACE from the original program.

#### Game Enhancements

Option for quick game or longer game based on the number of enemy ships.

Add torpedo animation from the X-wing to the TIE fighter.
Each tracking message would display the torpedo moving toward the TIE fighter, hit or miss.
```
Command > d
Directions
    3
  4 | 2
5 - + - 1
  6 | 8
    7
Command > 0
Course direction (1-8.5): 5.0
Parsecs (0.1-7.7): 6.7
 -1--2--3--4--5--6--7--8-
1          *              STARDATE  3039
2       *    >1<          CONDITION RED
3                  |o|    REGION    8 1
4 *                       SECTOR    8 7
5                         ENERGY    2757
6 *        *              TORPEDOES 10
7                         SHIELDS   0000
8                  x!x   
 -1--2--3--4--5--6--7--8-
Command > 6
Torpedo trajectory(1-8.5) : 3.0
Tracking: 7 7
Tracking: 6 7
Tracking: 5 7
Tracking: 4 7
Tracking: 3 7
TIE fighter destroyed.
Command > 
```

Should say, "TIE fighter destroyed."
````
Command > 6
Torpedo trajectory(1-8.5) : 5.0
Tracking: 4 7
Tracking: 4 6

CONGRATULATIONS! You eliminated all the TIE fighters. Rebels are safe...for now.
````

Shoudl say: CONDITION DOCKED
````
2            x!x>1<       CONDITION GREEN
````

If warp into a base(DOCKED), by coincidence,
+ Resources are not update for the ship, x!x>1<
+ Should update the ship.

Check, if no alien ships in the sector, don't say: The "alien" retaliates.
+ There is sample code for Laser cannons, search for, "Any alien ships?"
+ Likely add the same code for torpedoes.
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
#### Program is stored on the SD card:

From Altair101a command line, you can list, load, and run program files.

To run,
+ Start Altair101a and connect using a serial program.
A VT100 terminal is recommended.
+ If using a VT100 terminal, press "T" to use terminal escape characters.
+ Set the sense switches to load the Galaxy101a program: S8 and S10 which is 8 and a.
+ "m" and then confirm the loading of SD file: 00000101.BIN.
+ "x" to EXAMINE the address: 00000101:00000000
+ "r" to run the program.
+ "3" to view the galaxy regional sectors.

Start playing.

##### Program notes

List,
````
?- + n, SD card directory listing.
++ File:      00000101.BIN  5120
?-
````
Load a program.
````
; Start byte, set switches: 
++    2470:00000101 10100110: 00110001 : 31:061 > opcode: lxi sp,STACK
;
; From command line, enter the following to get to the start byte: 8ax.
?- + x, EXAMINE: 2470
?- <Hit Enter key>
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    *   *   .    .   .   *
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   *   .   *    .   .    .   .   .    .   .   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   ^   v   ^    v   v    v   v   v    v   v   v
````
--------------------------------------------------------------------------------
## Running the program.

From command line, press "r".
````
+ Ready to receive command.
?- + r, RUN.
?- + runProcessor()

Ready to start a Star Wars X-wing starfighter mission? (Y/N)
````
Press "y" to play.
````
Preparations are being made...

You must destroy 09 TIE fighters in 14 stardates.
Supplies are available at any of the 6 rebel outposts.
 -1--2--3--4--5--6--7--8-
1             *           STARDATE  3036
2               x!x   |o| CONDITION RED
3                         REGION    2 1
4                         SECTOR    2 6
5      |o|                ENERGY    2912
6                         TORPEDOES 10
7                         SHIELDS   0000
8       *              * 
 -1--2--3--4--5--6--7--8-
Command > 
````
The above regional map is located in row 2, column 1
of the below Regional Sector map.
In the Regional Sector map, it has the number 203.
203 implies there are:
+ 2 TIE fighters
+ 0 rebel outposts
+ 3 stars

Press "g" to get your current game stats.
````
Command > g
You must destroy 09 TIE fighters in 14 stardates.
Supplies are available at any of the 6 rebel outposts
````
There are 09 TIE fighters in the Regional Sector grid below.
Which means that the requirement is to destroy all of the fighters.

Press "3" to view the Regional Sector Display.
````
Command > 3
Regional Sector Display
-------------------------------------------------
| 000 | 005 | 005 | 002 | 004 | 000 | 000 | 201 |
-------------------------------------------------
| 203 | 007 | 011 | 000 | 002 | 006 | 000 | 000 |
-------------------------------------------------
| 003 | 002 | 006 | 015 | 013 | 000 | 011 | 001 |
-------------------------------------------------
| 000 | 004 | 000 | 015 | 000 | 007 | 006 | 005 |
-------------------------------------------------
| 000 | 003 | 002 | 004 | 017 | 006 | 004 | 005 |
-------------------------------------------------
| 006 | 206 | 004 | 000 | 000 | 005 | 000 | 103 |
-------------------------------------------------
| 203 | 007 | 000 | 000 | 000 | 004 | 000 | 000 |
-------------------------------------------------
| 000 | 003 | 000 | 000 | 000 | 002 | 003 | 000 |
-------------------------------------------------
Command > 
````

--------------------------------------------------------------------------------
### COMMAND Options

View options,
````
Command > h
0. Set X-wing course setting, and Fly
1. Sector range scanner
2. Sector wide area scanner
3. Regional sector display
4. Shields
5. Laser cannons
6. Proton torpedoes
g. Game stats
d. Directions key
X/x. Exit, exit command option or end game
Command > 
````
------------------------------------------
#### 0. Set X-wing course setting, and Fly

Press "d" to get the direction headings.
````
Command > d
Directions
    3
  4 | 2
5 - + - 1
  6 | 8
    7
````

View your current regional sector and sector location of your ship, press "1".
````
Command > 1
 -1--2--3--4--5--6--7--8-
1             *           STARDATE  3036
2               x!x   |o| CONDITION RED
3                         REGION    2 1
4                         SECTOR    2 6
5      |o|                ENERGY    2922
6                         TORPEDOES 10
7                         SHIELDS   0000
8       *              * 
 -1--2--3--4--5--6--7--8-
````

Changing sectors causes the STARDATE to increment.
Press "g" to get the number stardates left before time runs out.
````
Command > g
You must destroy 09 TIE fighters in 14 stardates.
Supplies are available at any of the 6 rebel outposts
````

Use a torpedo to destroy TIE fighters.
````
Command > 6
Torpedo trajectory(1-8.5) : 1.0
Tracking: 2 7
Tracking: 2 8
TIE fighter destroyed.
Command > 
````

Now that the TIE fighter is out of the way, fly over to the next sector.

````
Command > 0
Course direction (1-8.5): 1.0
Parsecs (0.1-7.7): 0.3
 -1--2--3--4--5--6--7--8-
1                *     *  STARDATE  3037
2x!x                      CONDITION GREEN
3       *                 REGION    2 2
4          *              SECTOR    2 1
5                   *     ENERGY    2607
6                         TORPEDOES 09
7    *                 *  SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-
````
Since I flew to a new sector, I have one less stardate to finish the game.
````
Command > g
You must destroy 08 TIE fighters in 13 stardates.
Supplies are available at any of the 6 rebel outposts.
Command > 

````

---------------
Dock to a space station to get energy and torpedoes.
````
Command > 0
Course direction (1-8.5): 1.0
Parsecs (0.1-7.7): 0.3
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3038
2                         CONDITION GREEN
3             *           REGION    2 3
4x!x            >1<       SECTOR    4 1
5                         ENERGY    2492
6                         TORPEDOES 09
7                         SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-
Command > 0
Course direction (1-8.5): 1.0
Parsecs (0.1-7.7): 0.4
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3038
2                         CONDITION GREEN
3             *           REGION    2 3
4            x!x>1<       SECTOR    4 5
5                         ENERGY    2952
6                         TORPEDOES 10
7                         SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-
Command > 

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
#### Fails

You cannot fly into or through a star. If you do, you will crash and burn.

Can run out of STARDATES.

------------------------------------------
#### 1. Sector range scanner
````
Command > 1
 -1--2--3--4--5--6--7--8-
1                *     *  STARDATE  3037
2                         CONDITION GREEN
3       *                 REGION    2 2
4          *              SECTOR    5 5
5            x!x    *     ENERGY    2567
6                         TORPEDOES 09
7    *                 *  SHIELDS   0000
8                        
 -1--2--3--4--5--6--7--8-
Command >
````
------------------------------------------
#### 2. Long  range scanner
````
Command > 2
L.R. SCAN FOR REGION    2 2
-------------------
1 000 1 005 1 005 1
-------------------
1 103 1 007 1 011 1
-------------------
1 003 1 002 1 006 1
-------------------
Command > 
````
Key, 103 : 1 TIE fighter, 0 rebel bases, 3 stars.

------------------------------------------
#### 3. Regional Sector Display

````
Command > 3
Regional Sector Display                            Rows
-------------------------------------------------
| 000 | 005 | 005 | 002 | 004 | 000 | 000 | 201 |   1
-------------------------------------------------
| 103 | 007 | 011 | 000 | 002 | 006 | 000 | 000 |   2
-------------------------------------------------
| 003 | 002 | 006 | 015 | 013 | 000 | 011 | 001 |   3
-------------------------------------------------
| 000 | 004 | 000 | 015 | 000 | 007 | 006 | 005 |   4
-------------------------------------------------
| 000 | 003 | 002 | 004 | 017 | 006 | 004 | 005 |   5
-------------------------------------------------
| 006 | 206 | 004 | 000 | 000 | 005 | 000 | 103 |   6
-------------------------------------------------
| 203 | 007 | 000 | 000 | 000 | 004 | 000 | 000 |   7
-------------------------------------------------
| 000 | 003 | 000 | 000 | 000 | 002 | 003 | 000 |   8
-------------------------------------------------
Command >

Columns:
   1     2     3     4     5     6     7     8
````
Key, 103 : 1 TIE fighter, 0 rebel bases, 3 stars.

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
#### 5. Laser cannons
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
#### 6. Using torpedoes
````
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3044
2                         CONDITION RED
3      |o|         x!x    REGION    2 1
4                         SECTOR    3 7
5    *                    ENERGY    2712
6       *                 TORPEDOES 10
7                         SHIELDS   0000
8                   *    
 -1--2--3--4--5--6--7--8-
Command > d
Directions
    3
  4 | 2
5 - + - 1
  6 | 8
    7
Command > 6
Torpedo trajectory(1-8.5) : 5.0
Tracking: 3 6
Tracking: 3 5
Tracking: 3 4
Tracking: 3 3
TIE fighter destroyed.
Command > 1
 -1--2--3--4--5--6--7--8-
1                         STARDATE  3044
2                         CONDITION GREEN
3                  x!x    REGION    2 1
4                         SECTOR    3 7
5    *                    ENERGY    2442
6       *                 TORPEDOES 09
7                         SHIELDS   0000
8                   *    
 -1--2--3--4--5--6--7--8-
Command >
````

--------------------------------------------------------------------------------
### How to Save a Game

Games can be saved and then later restored for continued playing.

Save a game.
````
+ "g" to see the game status before saving the game. For example:
Command > g
You must destroy 07 TIE fighters in 06 stardates.
Supplies are available at any of the 6 rebel outposts.
Command >

+ "s" or Ctrl+d to STOP the program, to go into WAIT mode.
+ Set Sense switches to the file number to save to. For example set to: "c".
+ "M" for memory upload (save). Confirm, "y".
+ "r" to RUN the program, to go into RUN mode.
+ "g" to confirm the same game status. For example:
Command > g
You must destroy 07 TIE fighters in 06 stardates.
Supplies are available at any of the 6 rebel outposts.
Command >
````

Clear the memory and restore the game.

Clear the memory.
````
+ "s" or Ctrl+d to STOP the program, to go into WAIT mode.
+ Press "C" and confirm to clear the memory.
?- + C, CLR: Clear memory, set registers to zero, set data and address to zero.
+ Confirm CLR, y/n: + CLR confirmed.
+ Clear memory: 5376
+ CLR registers.
+ CLR data, address, switches, and reset status lights.
````

Restore the game.

Load from SD file.
````
+ m, Read file into program memory.
++ Program filename: 00010000.bin
++ Confirm, y/n: 
+ Confirmed.
+ Program file loaded and ready to use.
````
Start the program from the restart address: 1683 = 00000110:10010011
````
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    *   *   .    .   .   *
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   *   *   .    *   .    .   *   .    .   *   *
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   ^   ^   v    ^   v    v   ^   v    v   ^   ^
 ------ 
+ Ready to receive command.
+ x, EXAMINE: 1683

?- + r, RUN.
?- + runProcessor()

Command > g

You must destroy 07 TIE fighters in 06 stardates.
Supplies are available at any of the 6 rebel outposts.
Command >
````
The game is ready to continue.

----------------------------------------------------------------------------------
Common file names I use.
````
#### Saved memory files:
01000000.bin e : Current test
00100000.bin d : Alternate test
00010000.bin c : Saved game
````
----------------------------------------------------------------------------------
