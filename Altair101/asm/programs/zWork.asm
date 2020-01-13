ant -f /Users/dthurston/Projects/arduino/Altair101/development/asm -Djavac.includes=asm/asmProcessor.java -Dnb.internal.action.name=run.single -Drun.class=asm.asmProcessor run-single
init:
Deleting: /Users/dthurston/Projects/arduino/Altair101/development/asm/build/built-jar.properties
deps-jar:
Updating property file: /Users/dthurston/Projects/arduino/Altair101/development/asm/build/built-jar.properties
Compiling 1 source file to /Users/dthurston/Projects/arduino/Altair101/development/asm/build/classes
Note: /Users/dthurston/Projects/arduino/Altair101/development/asm/src/asm/asmProcessor.java uses or overrides a deprecated API.
Note: Recompile with -Xlint:deprecation for details.
compile-single:
run-single:
++ Start.
+ Set opcode data.
++ Number opcode values = 130

+ Parse file lines.
++ parseLine, org directive. For now, ignore org line: org 0       ; An assembler directive on where to load this program.
++ Org address value: 0
++ Label Name: Start, Address: 0
++ parseLine, Opcode|lxi| p1|H| p2|0|
++ Opcode: lxi 00100001 p1|h| p2|0|
++ parseLine, Opcode|mvi| p1|D| p2|080h|
++ Opcode: mvi 00010110 p1|d| p2|080h|
++ parseLine, Opcode|lxi| p1|B| p2|5|
++ Opcode: lxi 00000001 p1|b| p2|5|
++ Label Name: Begin, Address: 8
++ parseLine, Opcode|ldax| theRest|D|
++ Opcode: ldax 00011010 p1|d|
++ parseLine, Opcode|ldax| theRest|D|
++ Opcode: ldax 00011010 p1|d|
++ parseLine, Opcode|ldax| theRest|D|
++ Opcode: ldax 00011010 p1|d|
++ parseLine, Opcode|ldax| theRest|D|
++ Opcode: ldax 00011010 p1|d|
++ parseLine, Opcode|dad| theRest|B|
++ Opcode: dad 00001001 p1|b|
++ parseLine, Opcode|jnc| theRest|Begin|
++ Opcode: jnc 11010010 p1|Begin|
++ parseLine, Opcode|in| theRest|0ffh|
++ Opcode: in 11011011 p1|0ffh|
++ parseLine, Opcode|xra| theRest|D|
++ Opcode: xra 10101010 p1|d|
++ parseLine, Opcode|rrc|
++ Opcode: rrc 00001111
++ parseLine, Opcode|mov| p1|D| p2|A|
++ Opcode: mov 01010111 p1|d| p2|a|
++ parseLine, Opcode|jmp| theRest|Begin|
++ Opcode: jmp 11000011 p1|Begin|


+ List label Addresses:
++ Start: 0
++ Begin: 8
+ End of list.

+ List immediate values...
+ End of list.

+ Set Program Label address values...
+ findName: 0
++ Label, lb:0:0
++ Label, hb:0
+ findName: 5
++ Label, lb:5:5
++ Label, hb:0
+ findName: Begin
+ Found theAddress: 8
++ Label, lb:Begin:8
++ Label, hb:0
+ findName: Begin
+ Found theAddress: 8
++ Label, lb:Begin:8
++ Label, hb:0
+ Label address values, set.

+ Set program immediate values...
++ immediate:080h:128
++ immediate:0ffh:255
+ Program immediate values, set.

+ Print a program array from the program data:
  B00100001, 0, 0,     //   0: lxi h,0
  B00010110, 128,      //   3: mvi 00010110 (080h)
  B00000001, 5, 0,     //   5: lxi b,5
  B00011010,           //   8: ldax d
  B00011010,           //   9: ldax d
  B00011010,           //  10: ldax d
  B00011010,           //  11: ldax d
  B00001001,           //  12: dad b
  B11010010, 8, 0,     //  13: jnc Begin
  B11011011, 255,      //  16: in 255 (0ffh)
  B10101010,           //  18: xra d
  B00001111,           //  19: rrc
  B01010111,           //  20: mov d,a
  B11000011, 8, 0,     //  21: jmp Begin
  0                    //  24: End of program

+ End of array.
++ Exit.
BUILD SUCCESSFUL (total time: 0 seconds)
