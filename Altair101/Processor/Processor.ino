// -----------------------------------------------------------------------------
/*
  Altair 101 software microprocessor

  Next:
  + Control status LED lights wired to use a SN74HC595N. Then update this program.
  + Get input opcode, IN, to work.
  + Test program, Kill the Bit.
  + Next opcodes to program, to run Pong:
  RET       11001001          -       Unconditional return from subroutine
  ++ RET requires CALL which requires PUSH and POP which requires stack memory.

  ---------------------------------------------
  Program sections,
    Sample programs.
    Definitions: Memory.
    Memory Functions.
    Front Panel Status LEDs.
    Output: Front Panel LED lights and serial log messages.
    ----------------------------
    Process opcodes instructions:
    + Instruction Set control.
    + Process opcode instruction: fetch opcode and process instruction cycle 1.
    + Process opcode instruction cycles greater than 1.
    ----------------------------
    Input: Front Panel infrared switch events
    ----------------------------
    setup() Computer initialization.
    loop()  Clock cycling through memory.
  ---------------------------------------------

  Altair 8800 Operator's Manual.pdf has a description of each opcode.

  Reference document, Intel 8080 Assembly Language Programming Manual:
    https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf
  This section is base on section 26: 8080 Instruction Set
    https://www.altairduino.com/wp-content/uploads/2017/10/Documentation.pdf
  Text listing of 8080 opcodes:
    https://github.com/tigerfarm/arduino/blob/master/Altair101/Processor/ProcessorOpcodes.txt
    https://github.com/tigerfarm/arduino/blob/master/Altair101/documents/8080opcodesBinaryList.txt
    
  Altair programming video, starting about 6 minutes in:
    https://www.youtube.com/watch?v=EV1ki6LiEmg

  Binary calculator:
    https://www.calculator.net/binary-calculator.html

  Bitwise operators:
    https://www.arduino.cc/reference/en/language/structure/bitwise-operators/bitwiseand/
  Extract highByte()
    https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/highbyte/
  Extract lowByte()
    https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/lowbyte/
*/
// -----------------------------------------------------------------------------
// Code compilation options.

// #define INCLUDE_LCD 1
// #define RUN_DELAY 1
#define RUN_NOW 1
// #define LOG_MESSAGES 1

// -----------------------------------------------------------------------------
// Kill the Bit program.

byte theProgram[] = {
  // ------------------------------------------------------------------
  // Kill the Bit program.
  // Before starting, make sure all the sense switches are in the down position.
  //
  //                    //            ; Start program.
  0,                    // org 0      ; An assembler directive, I guess.
  0041, 0, 0,           // LXI H,0    ; Move the lb hb data values into the register pair H(hb):L(lb). Initialize counter
  0026, 128,            // mvi D,80h ; Move db to register D. Set initial display bit.  080h = 128 = regD = 10 000 000
  0001, 0, 3,           // LXI B,0eh  ; Load a(lb:hb) into register B:C. Higher value = faster. Default: 0014 = B:C  = 00 010 000
  //
  //  ; Display bit pattern on upper 8 address lights.
  //                    // BEG:
  // 0343, 38,
  // 0166,                 // HLT
  0032,                 // ldax d     ; Move data from address D:E, to register A.
  0032,                 // ldax d     ; Move data from address D:E, to register A.
  0032,                 // ldax d     ; Move data from address D:E, to register A.
  0032,                 // ldax d     ; Move data from address D:E, to register A.
  //
  0011,                 // DAD B      ; Add B:C to H:L. Set carry bit. Increments the display counter
  0322, 9, 0,           // JNC BEG    ; If carry bit false, jump to BEG, LDAX instruction start.
  //
  0333, 0377,           // IN 0ffh    ; Input into A. Check for toggled input, at port 377 (toggle sense switches), that can kill the bit.
  0252,                 // XRA D      ; Exclusive OR register with A
  0017,                 // RRC        ; Rotate A right (shift byte right 1 bit). Set carry bit. Rotate display right one bit
  0127,                 // MOV D,A    ; Move register A to register D. Move data to display reg
  //
  0303, 9, 0,           // JMP BEG    ; Jump to BEG, halt before LDAX instructions restart.
  // ------------------------------------------------------------------
  0000, 0000, 0000      //             ; end
};

// -----------------------------------------------------------------------------
// Program to test opcodes.

byte theProgramOra[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Test opcode ora.
  //                //            ; OR source register, with register A.
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize registers to starter values.
  //
  B00111110, 6,     // mvi a,6    ; Move # to registers.
  B00000110, 0,     // mvi b,0
  B00001110, 1,     // mvi c,1
  B00010110, 2,     // mvi d,2
  B00011110, 3,     // mvi e,3
  B00100110, 4,     // mvi h,4
  B00101110, 5,     // mvi l,5
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  // ORA S     10110SSS          ZSPCA   OR source register with A
  //
  //1RRR110 mvi
  B00111110, 73,    // mvi a,73   ; Move # to register A:    01 001 001 = 73
  B00000110, 70,    // mvi b,70   ; Move # to register B:    01 000 110 = 70
  //0110SSS ora
  B10110000,        // ora b      ; OR register B, with register A.
  0343, 37,         // out 37     ; Print register A answer: 00 100 1111.
  //
  B00111110, 73,    // mvi a,73
  B00001110, 70,    // mvi c,70
  B10110001,        // ora c
  0343, 37,         // out 37
  //
  B00111110, 73,    // mvi a,73
  B00001110, 70,    // mvi c,70
  B10110001,        // ora c
  0343, 37,         // out 37
  //
  B00111110, 73,    // mvi a,73
  B00010110, 70,    // mvi d,70
  B10110010,        // ora d
  0343, 37,         // out 37
  //
  B00111110, 73,    // mvi a,73
  B00011110, 70,    // mvi e,3
  B10110011,        // ora e
  0343, 37,         // out 37
  //
  B00111110, 73,    // mvi a,73
  B00100110, 70,    // mvi h,70
  B10110100,        // ora h
  0343, 37,         // out 37
  //
  B00111110, 73,    // mvi a,73
  B00101110, 5,     // mvi l,5
  B10110101,        // ora l
  0343, 37,         // out 37
  //
  0343, 38,         // out 38     ; Print the register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};

// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 512;
byte memoryData[memoryBytes];

// -----------------------------------------------------------------------------
// Memory Functions

char charBuffer[17];
byte zeroByte = B00000000;

void initMemoryToZero() {
  Serial.println(F("+ Initialize all memory bytes to zero."));
  for (int i = 0; i < memoryBytes; i++) {
    memoryData[i] = zeroByte;
  }
}

void copyByteArrayToMemory(byte btyeArray[], int arraySize) {
  Serial.println(F("+ Copy the program into the computer's memory array."));
  for (int i = 0; i < arraySize; i++) {
    memoryData[i] = btyeArray[i];
  }
  Serial.println(F("+ Copied."));
}

void listByteArray(byte btyeArray[], int arraySize) {
  Serial.println(F("+ List the program."));
  Serial.println(F("++   Address:       Data value"));
  for (int i = 0; i < arraySize; i++) {
    Serial.print(F("+ Addr: "));
    sprintf(charBuffer, "%4d:", i);
    Serial.print(charBuffer);
    Serial.print(F(" Data: "));
    printData(btyeArray[i]);
    Serial.println("");
  }
  Serial.println(F("+ End of listing."));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Status LEDs

// Video demonstrating status lights:
//    https://www.youtube.com/watch?v=3_73NwB6toY
// MEMR & M1 & WO are on when fetching an op code, example: jmp(303) or lda(072).
// MEMR & WO are on when fetching a low or high byte of an address.
// MEMR & WO are on when fetching data from an address.
// All status LEDs are off when storing a value to a memory address.
// When doing a data write, data LEDs are all on.
//
// Halt: MEMR & hltA & WO are on. All address and data lights are on.
//  System is locked. To get going again, hard reset: flip stop and reset same time.
//
// STACK is the only status LED on when making a stack push (write to the stack).
// MEMR & STACK & WO are on when reading from the stack.
//
// INP & WO are on when reading from an input port.
// out is on when send an output to a port.
//
// INTE is on when interrupts are enabled.
// INTE is off when interrupts are disabled.

// For STA and LDa,
// See the video: https://www.youtube.com/watch?v=3_73NwB6toY : 1:30 to 6:05 time.
// ---
// Process instruction: lda 40Q
// First:
// Opcode fetch: on: MEMR MI WO
// MEMR : Memory instruction
// MI : opcode fetch.
// WO : inverse logic for write output.
// Next
// Memory read: on: MEMR WO
// Fetch lb.
// Next
// Memory read: on: MEMR WO
// Fetch hb.
// Next, loading from memory.
// Memory read: on: MEMR WO
// Fetching the data, shows the address (40Q) and the data (at 40Q) on the LED lights.
// ---
// Next, to process next instruction: sta 41Q
// Opcode fetch: on: MEMR MI WO
// MI : opcode fetch.
// WO : inverse logic for write output.
// Next
// Memory read: on: MEMR WO
// Fetch lb.
// Next
// Memory read: on: MEMR WO
// Fetch hb.
// Next, writting to memory.
// Memory read: all status lights are off. Since it is a write output, and WO is reverse logic, WO is off.
// Address LED lights: 41Q.
// Data LED lights are all on because the data lights are tied to the data input bus, which is floating.

// ------------------------------
// Not in use:
// const int INTE_PIN = 42;    // On, interrupts enabled.
// const int PROT_PIN = 42;    // Useful only if RAM has page protection impliemented.
//
// const int HLDA_PIN = 42;    // 8080 processor go into a hold state because of other hardware.
//
// B00000001 : INT    ?- Interupts enabled/disabled

// ------------------------------
// Status LEDs
//
// Info: page 33 of the Altair 8800 oprator's manaul.
// Bit pattern for the status shift register (SN74HC595N):
// B10000000 : MEMR   The memory bus will be used for memory read data.
// B01000000 : INP    The address bus containing the address of an input device. The input data should be placed on the data bus when the data bus is in the input mode
// B00100000 : M1     Machine cycle 1, fetch opcode.
// B00010000 : OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
// B00001000 : HLTA   Machine opcode hlt, has halted the machine.
// B00000100 : STACK  Stack process
// B00000010 : WO     Write out (inverse logic)
// B00000001 : WAIT   For now, use this one for WAIT light status
//             INT    An interrupt request has been acknowledged.
//
const int MEMR_PIN = 42;    // Memory read such as fetching an op code (data instruction)
const int INP_PIN = 42;     // Input
const int M1_PIN = A3;      // On, when current address is an opcode, which is Machine cycle 1. Off when getting an opcodes data bytes.
const int OUT_PIN = 42;     // Write output.
const int HLTA_PIN = A2;    // Halt acknowledge, halt instruction executed.
const int STACK_PIN = 42;   // On, reading or writing to the stack.
const int WO_PIN = 10;      // Write Output uses inverse logic. On, not writing output.
//
const int WAIT_PIN = A0;    // On, program not running. Off, programrunning.

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Definitions: Instruction Set, Opcodes, Registers

// ------------------------------------
// Processing opcodes and opcode cycles

// Instruction parameters:
byte lowOrder = 0;           // lb: Low order byte of 16 bit value.
byte highOrder = 0;          // hb: High order byte of 16 bit value.
//                             Example: hb + lb = 16 bit memory address.
byte dataByte = 0;           // db = Data byte (8 bit)

// -----------------------------------------------------------------------------
// Opcodes that are programmed and tested:

// cpi #    11 111 110  Compare db with a > compareResult.
// dad RP   00 RP1 001  Add register pair(RP) to H:L (16 bit add). And set carry bit if the addition causes a carry out.
// hlt      01 110 110  Halt processor
// inx RP   00 RP0 011  Increment a register pair, example: H:L (a 16 bit value). To do: stack pointer INX.
// jmp a    11 000 011  Unconditional jump
// jz  a    11 001 010  If compareResult is true, jump to lb hb.
// jnc a    11 010 010  Jump if carry bit is 0 (false).
// ldax RP  00 RP1 010  Load indirect through BC(RP=00) or DE(RP=01)
//        ldax d : Load the accumulator from memory location 938Bh, where register d contains 93H and register e contains 8BH.
// lxi RP,a 00 RP0 001  RP=10  which matches "10=HL".
// out p    11 010 011  Write a to output port a.
// mvi R,#  00 RRR 110  Move a number (#), which is the next db, to register RRR.
// mov d,S  01 DDD SSS  Move register to a register.
// nop      00 000 000  No operation
// rlc      00 000 111  Rotate a left. Need to handle carry bit.
// rrc      00 001 111  Rotate a right (shift byte right 1 bit). Need to handle carry bit.
// shld a   00 100 010  Store L value to memory location: a(hb:lb). Store H value at: a + 1.
// xra R    10 101 SSS  Register exclusive OR with register with A.

//        Code   Octal       Inst Param  Encoding Flags  Description
const byte cpi    = 0376; // cpi db    11 111 110        Compare db with a > compareResult.
const byte hlt    = 0166; // hlt         01110110        Halt processor
const byte jmp    = 0303; // jmp  a      11000011        Unconditional jump
const byte jnc    = 0322; // jnc  lb hb  11010010        Jump if carry bit is 0 (false).
const byte jz     = 0312; // jz   lb hb  11001010        If compareResult is true, jump to lb hb.
const byte nop    = 0000; // nop         00000000        No operation
const byte out    = 0343; // out pa      11010011        Write a to output port
const byte rrc    = 0017; // rrc       00 001 111   c    Rotate a right (shift byte right 1 bit). Note, carry bit not handled at this time.

// -----------------------------------------------------------------------------
// Next opcodes to do:
//
// Pong opcodes:
// Code      Binary   Param  Flags     Description
// CALL a    11001101 lb hb    -       Unconditional subroutine call
// RET       11001001          -       Unconditional return from subroutine
// PUSH RP   11RP0101 *2       -       Push register pair on the stack
// POP RP    11RP0001 *2       *2      Pop  register pair from the stack
// Kill the Bit opcodes:
const byte IN     = 0333;
// IN p      11011011 pa       -       Read input for port a, into A

// -----------------------------------------------------------
// Destination and Source registers and register pairs.
byte regA = 0;   // 111=A  a  register A, or Accumulator
//                            --------------------------------
//                            Register pair 'RP' fields:
byte regB = 0;   // 000=B  b  00=BC   (B:C as 16 bit register)
byte regC = 0;   // 001=C  c
byte regD = 0;   // 010=D  d  01=DE   (D:E as 16 bit register)
byte regE = 0;   // 011=E  e
byte regH = 0;   // 100=H  h  10=HL   (H:L as 16 bit register)
byte regL = 0;   // 101=L  l
//                            11=SP   (Stack pointer, refers to PSW (FLAGS:A) for PUSH/POP)
byte regM = 0;   // 110=M  m  Memory reference for address in H:L

// -----------------------------------------------------------------------------
// Output: log messages and Front Panel LED data lights.

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}
void printWord(int theValue) {
  String sValue = String(theValue, BIN);
  for (int i = 1; i <= 16 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}

void printOctal(byte b) {
  String sOctal = String(b, OCT);
  for (int i = 1; i <= 3 - sOctal.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sOctal);
}

void printData(byte theByte) {
  sprintf(charBuffer, "%3d = ", theByte);
  Serial.print(charBuffer);
  printOctal(theByte);
  Serial.print(F(" = "));
  printByte(theByte);
}

// --------------------
// Data LED lights displayed using a shift register chips: SN74HC595N.
const int dataPin = 4;            // 74HC595 Data  pin 14 is connected to Digital pin 4
const int latchPin = 5;           // 74HC595 Latch pin 12 is connected to Digital pin 5
const int clockPin = 6;           // 74HC595 Clock pin 11 is connected to Digital pin 6

void displayLedData(byte dataByte) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
  digitalWrite(latchPin, HIGH);
}

void displayLedAddressData(unsigned int data16bits, byte data8bits) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, data8bits);
  shiftOut(dataPin, clockPin, LSBFIRST, lowByte(data16bits));
  shiftOut(dataPin, clockPin, LSBFIRST, highByte(data16bits));
  digitalWrite(latchPin, HIGH);
}

// -----------------------------------------------------------------------------
// Processor: Processing opcode instructions

int programCounter = 0;     // Program address value

byte opcode = 0;            // Opcode being processed
int instructionCycle = 0;   // Opcode process cycle

void printAddressData() {
  //
  displayLedAddressData(programCounter, dataByte);
  //
#ifdef LOG_MESSAGES
  Serial.print(F("Addr: "));
  sprintf(charBuffer, "%4d:", programCounter);
  Serial.print(charBuffer);
  Serial.print(F(" Data: "));
  dataByte = memoryData[programCounter];
  printData(dataByte);
#else
/*
  Serial.print(" > ");
  printByte(highByte(programCounter));
  Serial.print(":");
  printByte(lowByte(programCounter));
  Serial.print(":");
  printByte(lowByte(regA));
  */
#endif
}

void processData() {
  if (opcode == 0) {
    digitalWrite(M1_PIN, HIGH); // Machine cycle 1, get an opcode.
#ifdef LOG_MESSAGES
    Serial.print("> ");
#endif
    printAddressData();
#ifdef LOG_MESSAGES
    Serial.print(" ");
#endif
    processOpcode();
    programCounter++;
    instructionCycle = 0;
  } else {
    digitalWrite(M1_PIN, LOW); // Machine cycle 1+x, getting opcode data.
#ifdef LOG_MESSAGES
    Serial.print("+ ");
#endif
    printAddressData();
#ifdef LOG_MESSAGES
    Serial.print(" ");
#endif
    instructionCycle++;
    processOpcodeData();      // Example: sta 42: 0. fetch the opcode, 1. fetch lb, 2. fetch hb, 3. store the data.
  }
#ifdef LOG_MESSAGES
  Serial.println("");
#endif
}

// -----------------------------------
// Process flags and values.
boolean runProgram = false;
boolean halted = false;       // Set true for an hlt opcode.
boolean carryBit = false;     // Set by dad. Used jnc.
boolean compareResult = true; // Set by cpi. Used by jz.

// For calculating 16 bit values.
// uint16_t bValue;           // Test using uint16_t instead of "unsigned int".
unsigned int bValue = 0;
unsigned int cValue = 0;
unsigned int dValue = 0;
unsigned int eValue = 0;
unsigned int hValue = 0;
unsigned int lValue = 0;
unsigned int bcValue = 0;
unsigned int deValue = 0;
unsigned int hlValue = 0;
unsigned int hlValueNew = 0;

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
void processOpcode() {

  unsigned int anAddress = 0;
  byte dataByte = memoryData[programCounter];
  switch (dataByte) {
    // ---------------------------------------------------------------------
    //ANI #     11100110 db       ZSPCA   AND immediate with A
    case B11100110:
      opcode = B11100110;
#ifdef LOG_MESSAGES
      Serial.print(F("> ani, AND db with register A."));
#endif
      break;
    case cpi:
      opcode = cpi;
#ifdef LOG_MESSAGES
      Serial.print(F("> cpi, compare next data byte to A. Store true or false into compareResult."));
#endif
      break;
    // ---------------------------------------------------------------------
    // dad RP   00RP1001  Add register pair(RP) to H:L (16 bit add). And set carry bit.
    // ----------
    //    00RP1001
    case B00001001:
      // dad b  : Add B:C to H:L (16 bit add). Set carry bit.
      bValue = regB;
      cValue = regC;
      hValue = regH;
      lValue = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> dad, 16 bit add: B:C("));
      Serial.print(bValue);
      Serial.print(":");
      Serial.print(cValue);
      Serial.print(")");
      Serial.print(F(" + H:L("));
      Serial.print(hValue);
      Serial.print(":");
      Serial.print(lValue);
      Serial.print(")");
#endif
      bcValue = bValue * 256 + cValue;
      hlValue = hValue * 256 + lValue;
      hlValueNew = bcValue + hlValue;
      if (hlValueNew < bcValue || hlValueNew < hlValue) {
        carryBit = true;
      } else {
        carryBit = false;
      }
      regH = highByte(hlValueNew);
      regL = lowByte(hlValueNew);
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      Serial.print(bcValue);
      Serial.print(F(" + "));
      Serial.print(hlValue);
      Serial.print(F(" = "));
      Serial.print(hlValueNew);
      Serial.print("(");
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
      Serial.print(")");
      Serial.print(F(", Carry bit: "));
      if (carryBit) {
        Serial.print(F("true. Sum over: 65535."));
      } else {
        Serial.print(F("false. Sum <= 65535."));
      }
#endif
      break;
    // ----------
    //    00RP1001
    case B00011001:
      // dad d  : Add D:E to H:L (16 bit add). Set carry bit.
      dValue = regD;
      eValue = regE;
      hValue = regH;
      lValue = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> dad, 16 bit add: D:E"));
      Serial.print("(");
      Serial.print(dValue);
      Serial.print(":");
      Serial.print(eValue);
      Serial.print(")");
      Serial.print(F(" + H:L "));
      Serial.print("(");
      Serial.print(hValue);
      Serial.print(":");
      Serial.print(lValue);
      Serial.print(")");
#endif
      deValue = dValue * 256 + eValue;
      hlValue = hValue * 256 + lValue;
      hlValueNew = deValue + hlValue;
      if (hlValueNew < deValue || hlValueNew < hlValue) {
        carryBit = true;
      } else {
        carryBit = false;
      }
      regH = highByte(hlValueNew);
      regL = lowByte(hlValueNew);
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      Serial.print(deValue);
      Serial.print(F(" + "));
      Serial.print(hlValue);
      Serial.print(F(" = "));
      Serial.print(hlValueNew);
      Serial.print("(");
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
      Serial.print(")");
      if (carryBit) {
        Serial.print(F("true. Sum over: 65535."));
      } else {
        Serial.print(F("false. Sum <= 65535."));
      }
#endif
      break;
    // ---------------------------------------------------------------------
    // dcr d : decrement a register. To do: update the flags.
    //    00DDD101    Flags:ZSPA
    case B00111101:
#ifdef LOG_MESSAGES
      Serial.print(F("> dcr register A: "));
      Serial.print(regA);
#endif
      regA--;
      if (regA == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regA);
#endif
      break;
    case B00000101:
#ifdef LOG_MESSAGES
      Serial.print(F("> dcr register B: "));
      Serial.print(regB);
#endif
      regB--;
      if (regB == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regB);
#endif
      break;
    case B00001101:
#ifdef LOG_MESSAGES
      Serial.print(F("> dcr register C: "));
      Serial.print(regC);
#endif
      regC--;
      if (regC == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regC);
#endif
      break;
    case B00010101:
#ifdef LOG_MESSAGES
      Serial.print(F("> dcr register D: "));
      Serial.print(regD);
#endif
      regD--;
      if (regD == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regD);
#endif
      break;
    case B00011101:
#ifdef LOG_MESSAGES
      Serial.print(F("> dcr register E: "));
      Serial.print(regE);
#endif
      regE--;
      if (regE == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regE);
#endif
      break;
    case B00100101:
#ifdef LOG_MESSAGES
      Serial.print(F("> dcr register H: "));
      Serial.print(regH);
#endif
      regH--;
      if (regH == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regH);
#endif
      break;
    case B00101101:
#ifdef LOG_MESSAGES
      Serial.print(F("> dcr register L: "));
      Serial.print(regL);
#endif
      regL--;
      if (regL == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    case hlt:
      runProgram = false;
      halted = true;
#ifdef LOG_MESSAGES
      Serial.println(F("> hlt, halt the processor."));
      digitalWrite(WAIT_PIN, HIGH);
      digitalWrite(M1_PIN, LOW);
      digitalWrite(HLTA_PIN, HIGH);
#else
      Serial.println("");
#endif
      Serial.print(F("+ Process halted."));
      // To do: set all the address and data lights on.
      break;
    case B11011011:
      opcode = B11011011;
      // INP status light is on when reading from an input port.
#ifdef LOG_MESSAGES
      Serial.print(F("> IN, If input value is available, get the input byte."));
#endif
      break;
    // ---------------------------------------------------------------------
    // inr d : increment a register. To do: update the flags.
    //    00DDD100    Flags:ZSPA
    case B00111100:
#ifdef LOG_MESSAGES
      Serial.print(F("> inr register A: "));
      Serial.print(regA);
#endif
      regA++;
      if (regA == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regA);
#endif
      break;
    case B00000100:
#ifdef LOG_MESSAGES
      Serial.print(F("> inr register B: "));
      Serial.print(regB);
#endif
      regB++;
      if (regB == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regB);
#endif
      break;
    case B00001100:
#ifdef LOG_MESSAGES
      Serial.print(F("> inr register C: "));
      Serial.print(regC);
#endif
      regC++;
      if (regC == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regC);
#endif
      break;
    case B00010100:
#ifdef LOG_MESSAGES
      Serial.print(F("> inr register D: "));
      Serial.print(regD);
#endif
      regD++;
      if (regD == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regD);
#endif
      break;
    case B00011100:
#ifdef LOG_MESSAGES
      Serial.print(F("> inr register E: "));
      Serial.print(regE);
#endif
      regE++;
      if (regE == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regE);
#endif
      break;
    case B00100100:
#ifdef LOG_MESSAGES
      Serial.print(F("> inr register H: "));
      Serial.print(regH);
#endif
      regH++;
      if (regH == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regH);
#endif
      break;
    case B00101100:
#ifdef LOG_MESSAGES
      Serial.print(F("> inr register L: "));
      Serial.print(regL);
#endif
      regL++;
      if (regL == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    // inx : increment a register pair.
    // ------------
    case B00000011:
      // inb b
      regC++;
      if (regC == 0) {
        regB++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F("> inx, increment the 16 bit register pair B:C."));
      Serial.print(F(", B:C = "));
      Serial.print(regB);
      Serial.print(":");
      Serial.print(regC);
      Serial.print(" = ");
      printOctal(regB);
      Serial.print(":");
      printOctal(regC);
#endif
      break;
    // ------------
    case B00010011:
      // inx d
      regE++;
      if (regE == 0) {
        regD++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F("> inx, increment the 16 bit register pair D:E."));
      Serial.print(F(", regD:regE = "));
      Serial.print(regD);
      Serial.print(":");
      Serial.print(regE);
      Serial.print(" = ");
      printOctal(regD);
      Serial.print(":");
      printOctal(regE);
#endif
      break;
    // ------------
    case B00100011:
      // inx m (h:l)
      regL++;
      if (regL == 0) {
        regH++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F("> inx, increment the 16 bit register pair H:L."));
      Serial.print(F(", regH:regL = "));
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
      Serial.print(" = ");
      printOctal(regH);
      Serial.print(":");
      printOctal(regL);
#endif
      break;
    // 00RP0011 -------------------------------------------------
    /*   00110011 Not done for the stack pointer.
      // inx
      void _I8080_inx(uint8_t rp) {
      ...
      case _RP_SP:
      _SP++;
      break;
      }
      _PC++;
      }
    */
    // ---------------------------------------------------------------------
    case jnc:
      opcode = jnc;
#ifdef LOG_MESSAGES
      Serial.print(F("> jnc, Jump if carry bit is false (0, not set)."));
#endif
      break;
    case jz:
      opcode = jz;
#ifdef LOG_MESSAGES
      Serial.print(F("> jz, if compareResult, jump to the following address (lh hb)."));
#endif
      break;
    case jmp:
      opcode = jmp;
#ifdef LOG_MESSAGES
      Serial.print(F("> jmp, get address low and high order bytes."));
#endif
      break;
    // ---------------------------------------------------------------------
    // ldax RP  00RP1010 - Load indirect through BC(RP=00) or DE(RP=01)
    // ---------------
    case B00001010:
      opcode = B00001010;
#ifdef LOG_MESSAGES
      Serial.print(F("> ldax, Into register A, load data from B:C address: "));
      printOctal(regB);
      Serial.print(F(" : "));
      printOctal(regC);
#endif
      break;
    // ---------------
    case B00011010:
      opcode = B00011010;
#ifdef LOG_MESSAGES
      Serial.print(F("> ldax, Into register A, load data from D:E address: "));
      printOctal(regD);
      Serial.print(F(" : "));
      printOctal(regE);
#endif
      break;
    // ---------------------------------------------------------------------
    // lxi                                Load register pair immediate
    // lxi RP,a 00RP0001                  Move a(hb:lb) into register pair RP, example, B:C = hb:lb.
    case B00000001:
      opcode = B00000001;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Move the lb hb data, into register pair B:C = hb:lb."));
#endif
      break;
    case B00010001:
      opcode = B00010001;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Move the lb hb data, into register pair D:E = hb:lb."));
#endif
      break;
    case B00100001:
      opcode = B00100001;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Move the lb hb data, into register pair H:L = hb:lb."));
#endif
      break;
    case B00110001:
      // stacy
      // opcode = B00110001;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Stacy, to do: move the lb hb data, to the stack pointer."));
#endif
      Serial.print(F(" - Error, unhandled instruction."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      break;
    // ---------------------------------------------------------------------
    case B01111110:
      anAddress = word(regH, regL);
      regA = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F("> mov"));
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    /*
      B01DDDSSS         // mov d,S  ; Move from one register to another.
    */
    // ---------------------------------------------------------------------
    case B01000111:
      regB = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to b = "));
      printData(regB);
#endif
      break;
    case B01001111:
      regC = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to c = "));
      printData(regC);
#endif
      break;
    case B01010111:
      regD = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to d = "));
      printData(regD);
#endif
      break;
    case B01011111:
      regE = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to e = "));
      printData(regE);
#endif
      break;
    case B01100111:
      regH = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to h = "));
      printData(regH);
#endif
      break;
    case B01101111:
      regL = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to l = "));
      printData(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    case B01111000:
      regA = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to a = "));
      printData(regL);
#endif
      break;
    case B01001000:
      regC = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to c = "));
      printData(regL);
#endif
      break;
    case B01010000:
      regD = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to d = "));
      printData(regD);
#endif
      break;
    case B01011000:
      regE = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to e = "));
      printData(regE);
#endif
      break;
    case B01100000:
      regH = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to h = "));
      printData(regH);
#endif
      break;
    case B01101000:
      regL = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to l = "));
      printData(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    case B01111001:
      regA = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to a = "));
      printData(regA);
#endif
      break;
    case B01000001:
      regB = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to b = "));
      printData(regB);
#endif
      break;
    case B01010001:
      regD = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to d = "));
      printData(regD);
#endif
      break;
    case B01011001:
      regE = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to e = "));
      printData(regE);
#endif
      break;
    case B01100001:
      regH = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to h = "));
      printData(regH);
#endif
      break;
    case B01101001:
      regL = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111010:
      regA = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to a = "));
      printData(regA);
#endif
      break;
    case B01000010:
      regB = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to b = "));
      printData(regB);
#endif
      break;
    case B01001010:
      regC = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to c = "));
      printData(regC);
#endif
      break;
    case B01011010:
      regE = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to e = "));
      printData(regE);
#endif
      break;
    case B01100010:
      regH = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to h = "));
      printData(regH);
#endif
      break;
    case B01101010:
      regL = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111011:
      regA = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to a = "));
      printData(regA);
#endif
      break;
    case B01000011:
      regB = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to b = "));
      printData(regB);
#endif
      break;
    case B01001011:
      regC = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to c = "));
      printData(regC);
#endif
      break;
    case B01010011:
      regD = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to d = "));
      printData(regD);
#endif
      break;
    case B01100011:
      regH = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to h = "));
      printData(regH);
#endif
      break;
    case B01101011:
      regL = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111100:
      regA = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to a = "));
      printData(regA);
#endif
      break;
    case B01000100:
      regB = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to b = "));
      printData(regB);
#endif
      break;
    case B01001100:
      regC = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to c = "));
      printData(regC);
#endif
      break;
    case B01010100:
      regD = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to d = "));
      printData(regD);
#endif
      break;
    case B01011100:
      regE = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to e = "));
      printData(regE);
#endif
      break;
    case B01101100:
      regL = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111101:
      regA = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to a = "));
      printData(regA);
#endif
      break;
    case B01000101:
      regB = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to b = "));
      printData(regB);
#endif
      break;
    case B01001101:
      regC = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to c = "));
      printData(regC);
#endif
      break;
    case B01010101:
      regD = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to d = "));
      printData(regD);
#endif
      break;
    case B01011101:
      regE = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to e = "));
      printData(regE);
#endif
      break;
    case B01100101:
      regH = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to h = "));
      printData(regH);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    // mvi R,#  00 RRR 110  Move a number (#), which is the next db, to register RRR.
    // mvi a,#  00 111 110  0036
    // mvi b,#  00 000 110  0006
    // mvi c,#  00 001 110  0016
    // mvi d,#  00 010 110  0026
    // mvi e,#  00 011 110  0036
    // mvi h,#  00 100 110  0046
    // mvi l,#  00 101 110  0056
    case B00111110:
      opcode = B00111110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register A."));
#endif
      break;
    case B00000110:
      opcode = B00000110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register B."));
#endif
      break;
    case B00001110:
      opcode = B00001110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register C."));
#endif
      break;
    case B00010110:
      opcode = B00010110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register D."));
#endif
      break;
    case B00011110:
      opcode = B00011110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register E."));
#endif
      break;
    case B00100110:
      opcode = B00100110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register H."));
#endif
      break;
    case B00101110:
      opcode = B00101110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register L."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case nop:
#ifdef LOG_MESSAGES
      Serial.print(F("> nop ------------"));
#endif
      delay(100);
      break;
    // ------------------------------------------------------------------------------------------
    case out:
      opcode = out;
      digitalWrite(WO_PIN, LOW);  // Inverse logic: off writing out. On when not.
#ifdef LOG_MESSAGES
      Serial.print(F("> OUT, Write output to the port address(following db)."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case rrc:
#ifdef LOG_MESSAGES
      Serial.print(F("> rrc"));
      Serial.print(F(", Shift register A: "));
      printByte(regA);
      Serial.print(F(", right 1 bit: "));
#endif
      regA = regA >> 1;
      // https://github.com/knowncold/8080/blob/master/opcode
      // # 0x0f RRC 1 CY  A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0
      // Need to improve the if, i.e. wrap 1 or 0, not always use 1.
      if (regA == 0) {
        regA = B10000000;
      }
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    // shld a    00100010 lb hb    -  Store register L to memory address hb:lb. Store register H to hb:lb + 1.
    // The contents of register L, are stored in the memory address specified in bytes lb and hb (hb:lb).
    // The contents of register H, are stored in the memory at the next higher address (hb:lb + 1).
    case B00100010:
      opcode = B00100010;
#ifdef LOG_MESSAGES
      Serial.print(F("> shld, Store register L to memory address hb:lb. Store register H to hb:lb + 1."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B00110010:
      opcode = B00110010;
#ifdef LOG_MESSAGES
      Serial.print(F("> sta, Store register A to the hb:lb address."));
#endif
      break;
    case B00111010:
      opcode = B00111010;
#ifdef LOG_MESSAGES
      Serial.print(F("> lda, Load register A with data from the hb:lb address."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    // XRA S     10101SSS          ZSPCA   ExclusiveOR register with A.
    //                             ZSPCA   Stacy, need to set flags.
    // ---------------
    case B10101000:
      // xra b
#ifdef LOG_MESSAGES
      Serial.print(F("> xra"));
      Serial.print(F(", Register B: "));
      printByte(regB);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
      // If one but not both of the variables has a value of one, the bit will be one, else zero.
#endif
      regA = regB ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101001:
      // xra c
#ifdef LOG_MESSAGES
      Serial.print(F("> xra"));
      Serial.print(F(", Register C: "));
      printByte(regC);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regC ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101010:
      // xra d
#ifdef LOG_MESSAGES
      Serial.print(F("> xra"));
      Serial.print(F(", Register D: "));
      printByte(regD);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regD ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101011:
      // xra e
#ifdef LOG_MESSAGES
      Serial.print(F("> xra"));
      Serial.print(F(", Register E: "));
      printByte(regE);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regE ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101100:
      // xra h
#ifdef LOG_MESSAGES
      Serial.print(F("> xra"));
      Serial.print(F(", Register H: "));
      printByte(regH);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regH ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101101:
      // xra l
#ifdef LOG_MESSAGES
      Serial.print(F("> xra"));
      Serial.print(F(", Register L: "));
      printByte(regL);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regL ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // -------------------------------------------------------------------------------------
    default:
      Serial.print(F(" - Error, unknown instruction."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
  }
}

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
void processOpcodeData() {
  // Note,
  //    if not jumping, increment programCounter.
  //    if jumping, don't increment programCounter.

  switch (opcode) {
    // ---------------------------------------------------------------------
    // stacy
    //ANI #     11100110 db       ZSPCA   AND immediate with A
    case B11100110:
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("> ani, AND db:"));
      printByte(dataByte);
      Serial.print(F(" with register A:"));
      printByte(regA);
#endif
      regA = regA & dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case cpi:
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
      compareResult = dataByte == regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> cpi, compareResult db: "));
      Serial.print(dataByte);
      if (compareResult) {
        Serial.print(F(" == "));
      } else {
        Serial.print(F(" != "));
      }
      Serial.print(F(" A: "));
      Serial.print(regA);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case B11011011:
      // instructionCycle == 1
      // // INP & WO are on when reading from an input port.
      dataByte = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< IN, input port: "));
      Serial.print(dataByte);
      Serial.print(F(". Not implemented, yet."));
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case jnc:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< jnc, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< jnc, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (!carryBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F("> jnc, carryBit is false, jump to:"));
        Serial.print(programCounter);
#endif
      } else {
#ifdef LOG_MESSAGES
        Serial.print(F(" - carryBit is true, don't jump."));
#endif
        programCounter++;
      }
      break;
    // ---------------------------------------------------------------------
    case jz:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< jz, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< jz, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (compareResult) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F("> jz, jump to:"));
        Serial.print(programCounter);
#endif
      } else {
#ifdef LOG_MESSAGES
        Serial.print(F(" - compareResult is false, don't jump."));
#endif
        programCounter++;
      }
      break;
    // ---------------------------------------------------------------------
    case jmp:
      if (instructionCycle == 1) {
        lowOrder = programCounter;
#ifdef LOG_MESSAGES
        Serial.print(F("> jmp, lb: "));
        Serial.print(lowOrder);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      programCounter = word(memoryData[programCounter], memoryData[lowOrder]);
#ifdef LOG_MESSAGES
      Serial.print(F("> jmp, jump to:"));
      sprintf(charBuffer, "%4d = ", programCounter);
      Serial.print(charBuffer);
      printByte((byte)programCounter);
#endif
      break;
    // ---------------------------------------------------------------------
    // ldax RP  00RP1010 - Load indirect through BC(RP=00) or DE(RP=01)
    // ---------------
    case B00001010:
      bValue = regB;
      cValue = regC;
      bcValue = bValue * 256 + cValue;
      if (deValue < memoryBytes) {
        regA = memoryData[bcValue];
      } else {
        regA = 0;
      }
      displayLedAddressData(deValue, regA);
#ifdef LOG_MESSAGES
      Serial.print(F("< ldax, the data moved into Accumulator is "));
      printData(regA);
#endif
      break;
    // ---------------
    case B00011010:
      dValue = regD;
      eValue = regE;
      deValue = dValue * 256 + eValue;
      if (deValue < memoryBytes) {
        regA = memoryData[deValue];
      } else {
        regA = 0;
      }
      displayLedAddressData(deValue, regA);
#ifdef LOG_MESSAGES
      Serial.print(F("< ldax, the data moved into Accumulator is "));
      printData(regA);
#endif
      break;
    // ---------------------------------------------------------------------
    case B00000001:
      // lxi b,16-bit-address
      if (instructionCycle == 1) {
        regC = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regC);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regB = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regB);
      Serial.print(charBuffer);
      Serial.print(F("> B:C = "));
      printOctal(regB);
      Serial.print(F(":"));
      printOctal(regC);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case B00010001:
      // lxi d,16-bit-address
      if (instructionCycle == 1) {
        regE = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regE);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regD = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regD);
      Serial.print(charBuffer);
      Serial.print(F("> D:E = "));
      printOctal(regD);
      Serial.print(F(":"));
      printOctal(regE);
#endif
      programCounter++;
      break;
    // -------------------
    case B00100001:
      // lxi h,16-bit-address
      if (instructionCycle == 1) {
        regL = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regL);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regH = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regH);
      Serial.print(charBuffer);
      Serial.print(F("> H:L = "));
      printOctal(regH);
      Serial.print(F(":"));
      printOctal(regL);
#endif
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    // mvi R,#  00 RRR 110  Move a number (#), which is the next db, to register RRR.
    // mvi a,#  00 111 110  0036
    // mvi b,#  00 000 110  0006
    // mvi c,#  00 001 110  0016
    // mvi d,#  00 010 110  0026
    // mvi e,#  00 011 110  0036
    // mvi h,#  00 100 110  0046
    // mvi l,#  00 101 110  0056
    case B00111110:
      regA = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register A: "));
      printData(regA);
#endif
      programCounter++;
      break;
    case B00000110:
      regB = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register B: "));
      printData(regB);
#endif
      programCounter++;
      break;
    case B00001110:
      regC = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register C: "));
      printData(regC);
#endif
      programCounter++;
      break;
    case B00010110:
      regD = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register D: "));
      printData(regD);
#endif
      programCounter++;
      break;
    case B00011110:
      regE = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register E: "));
      printData(regE);
#endif
      programCounter++;
      break;
    case B00100110:
      regH = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register H: "));
      printData(regH);
#endif
      programCounter++;
      break;
    case B00101110:
      regL = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register L: "));
      printData(regL);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case out:
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< OUT, input port: "));
      Serial.print(dataByte);
#else
      Serial.println("");
#endif
      switch (dataByte) {
        case 1:
          Serial.print(F(", terminal output to be implemented on LCD."));
          break;
        case 38:
#ifdef LOG_MESSAGES
          Serial.println("");
#endif
          Serial.println(F("------------"));
          printRegisters();
          Serial.print(F("------------"));
          break;
        case 37:
          Serial.print(F(" > Register A = "));
          printData(regA);
          break;
        case 30:
          Serial.print(F(" > Register B = "));
          printData(regB);
          break;
        case 31:
          Serial.print(F(" > Register C = "));
          printData(regC);
          break;
        case 32:
          Serial.print(F(" > Register D = "));
          printData(regD);
          break;
        case 33:
          Serial.print(F(" > Register E = "));
          printData(regE);
          break;
        case 34:
          Serial.print(F(" > Register H = "));
          printData(regH);
          break;
        case 35:
          Serial.print(F(" > Register L = "));
          printData(regL);
          break;
        case 36:
          Serial.print(F(" > Register H:L = "));
          Serial.print(regH);
          Serial.print(F(":"));
          Serial.print(regL);
          Serial.print(F(", Data: "));
          hlValue = regH * 256 + regL;
          Serial.print(memoryData[hlValue]);
          break;
        default:
          Serial.print(F("-- Error, unknow out port number: "));
      }
      digitalWrite(WO_PIN, HIGH);  // Inverse logic: off writing out. On when not.
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    case B00110010:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGE
        Serial.print(F("< sta, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 2) {
        highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< sta, hb: "));
        sprintf(charBuffer, "%4d:", highOrder);
        Serial.print(charBuffer);
#endif
        return;
      }
      // instructionCycle == 3
      hlValue = highOrder * 256 + lowOrder;
      memoryData[hlValue] = regA;
      // Stacy, to do:
      //  Set address lights to hb:lb. Set data lights to regA.
      //  Turn status light MEMR, MI, and WO off during this step.
#ifdef LOG_MESSAGES
      Serial.print(F("> sta, register A: "));
      Serial.print(regA);
      Serial.print(F(", stored to the hb:lb address."));
#endif
      programCounter++;
      break;
    // -----------------------------------------
    case B00111010:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lda, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 2) {
        highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lda, hb: "));
        sprintf(charBuffer, "%4d:", highOrder);
        Serial.print(charBuffer);
#endif
        return;
      }
      hlValue = highOrder * 256 + lowOrder;
      regA = memoryData[hlValue];
#ifdef LOG_MESSAGES
      Serial.print(F("> lda, hb:lb address data is moved to register A: "));
      Serial.print(regA);
#endif
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    case B00100010:
      // shld a
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< shld, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 2) {
        highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< shld, hb: "));
        sprintf(charBuffer, "%4d:", highOrder);
        Serial.print(charBuffer);
#endif
        return;
      }
      hlValue = highOrder * 256 + lowOrder;
      memoryData[hlValue] = regL;
      memoryData[hlValue + 1] = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> shld, Store register L:"));
      Serial.print(regL);
      Serial.print(F(" to memory address hb:lb :"));
      Serial.print(hlValue);
      Serial.print(F(". Store register H:"));
      Serial.print(regH);
      Serial.print(F(" to hb:lb + 1 = "));
      Serial.print(hlValue + 1);
#endif
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    default:
      Serial.print(F(" -- Error, unknow instruction."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
  }
  // The opcode cycles are complete.
  opcode = 0;
}

// -----------------------------------------------------------------------------
//  Output Functions

void printRegisters() {
  Serial.print(F("+ regA: "));
  printData(regA);
  Serial.println("");
  // ---
  Serial.print(F("+ regB: "));
  printData(regB);
  Serial.print(F("  regC: "));
  printData(regC);
  Serial.println("");
  // ---
  Serial.print(F("+ regD: "));
  printData(regD);
  Serial.print(F("  regE: "));
  printData(regE);
  Serial.println("");
  // ---
  Serial.print(F("+ regH: "));
  printData(regH);
  Serial.print(F("  regL: "));
  printData(regL);
  // ---
  Serial.println("");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel toggle/button switch events
// Using an infrared receiver for simplier hardware setup.

boolean buttonWentHigh = false;

// -----------------------------------------------------------------------
#include <IRremote.h>

int IR_PIN = A1;
IRrecv irrecv(IR_PIN);
decode_results results;

void infraredSwitchControl() {
  // Serial.println(F("+ infraredSwitch"));
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0xFF10EF:
    case 0xE0E0A659:
      // Serial.println(F("+ Key < - previous"));
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:
      // Serial.println(F("+ Key > - next: SINGLE STEP toggle/button switch."));
      digitalWrite(HLTA_PIN, LOW);
      processData();
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      // Serial.println(F("+ Key up"));
      Serial.println(F("+ Run process."));
      runProgram = true;
      digitalWrite(WAIT_PIN, LOW);
      digitalWrite(HLTA_PIN, LOW);
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      // Already stopped.
      // Serial.println(F("+ Key down"));
      // Serial.println(F("+ Stop process."));
      // runProgram = false;
      // digitalWrite(WAIT_PIN, HIGH);
      // digitalWrite(HLTA_PIN, LOW);
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Run process."));
      runProgram = true;
      digitalWrite(WAIT_PIN, LOW);
      digitalWrite(HLTA_PIN, LOW);
      break;
    // -----------------------------------
    case 0xFF9867:
    case 0xE0E08877:
      // Serial.print(F("+ Key 0:"));
      // Serial.println("");
      break;
    case 0xFFA25D:
      // Serial.println(F("+ Key 1: "));
      // Serial.println("+ Examine.");
      Serial.print("? ");
      printAddressData();
      Serial.println("");
      buttonWentHigh = true;
      break;
    case 0xFF629D:
      // Serial.println(F("+ Key 2: "));
      // Serial.println("+ Examine Next.");
      programCounter++;
      Serial.print("? ");
      printAddressData();
      Serial.println("");
      buttonWentHigh = true;
      break;
    case 0xFFE21D:
      // Serial.println(F("+ Key 3: "));
      // Serial.println("+ Examine Previous.");
      programCounter--;
      Serial.print("? ");
      printAddressData();
      Serial.println("");
      buttonWentHigh = true;
      break;
    case 0xFF22DD:
      // Serial.print(F("+ Key 4: "));
      // Serial.println("");
      break;
    case 0xFF02FD:
      // Serial.print(F("+ Key 5: "));
      // Serial.println("");
      break;
    case 0xFFC23D:
      // Serial.print(F("+ Key 6: "));
      // Serial.println("");
      break;
    case 0xFFE01F:
      // Serial.print(F("+ Key 7: "));
      // Serial.println("");
      break;
    case 0xFFA857:
      // Serial.print(F("+ Key 8: "));
      // Serial.println("");
      break;
    case 0xFF906F:
      // Serial.print(F("+ Key 9: "));
      // Serial.println("");
      break;
    // -----------------------------------
    case 0xFF6897:
    case 0xE0E01AE5:
      // Serial.println(F("+ Key * (Return)"));
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      // Serial.println(F("+ Key # (Exit)"));
      break;
    // -----------------------------------
    default:
      // Serial.print("+ Result value: ");
      // Serial.println(results.value, HEX);
      break;
      // -----------------------------------
  } // end switch

  irrecv.resume();

}

// -----------------------------------------------------------------------------
void infraredSwitchInput() {
  // Serial.println(F("+ infraredSwitch"));
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0xFF10EF:
    case 0xE0E0A659:
      // Serial.println(F("+ Key < - previous"));
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:
      // Serial.println(F("+ Key > - next."));
      processData();
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      // Serial.println(F("+ Key up"));
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      // Serial.println(F("+ Key down"));
      Serial.println(F("+ Stop process."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      digitalWrite(HLTA_PIN, LOW);
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Stop process."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      digitalWrite(HLTA_PIN, LOW);
      break;
    // -----------------------------------
    case 0xFF9867:
    case 0xE0E08877:
      // Serial.print(F("+ Key 0:"));
      // Serial.println("");
      break;
    case 0xFFA25D:
      // Serial.println(F("+ Key 1: "));
      break;
    case 0xFF629D:
      // Serial.println(F("+ Key 2: "));
      break;
    case 0xFFE21D:
      // Serial.println(F("+ Key 3: "));
      break;
    case 0xFF22DD:
      // Serial.print(F("+ Key 4: "));
      // Serial.println("");
      break;
    case 0xFF02FD:
      // Serial.print(F("+ Key 5: "));
      // Serial.println("");
      break;
    case 0xFFC23D:
      // Serial.print(F("+ Key 6: "));
      // Serial.println("");
      break;
    case 0xFFE01F:
      // Serial.print(F("+ Key 7: "));
      // Serial.println("");
      break;
    case 0xFFA857:
      // Serial.print(F("+ Key 8: "));
      // Serial.println("");
      break;
    case 0xFF906F:
      // Serial.print(F("+ Key 9: "));
      // Serial.println("");
      break;
    // -----------------------------------
    case 0xFF6897:
    case 0xE0E01AE5:
      // Serial.println(F("+ Key * (Return)"));
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      // Serial.println(F("+ Key # (Exit)"));
      break;
    // -----------------------------------
    default:
      // Serial.print("+ Result value: ");
      // Serial.println(results.value, HEX);
      break;
      // -----------------------------------
  } // end switch

  irrecv.resume();

}

// -----------------------------------------------------------------------------
// 1602 LCD
#ifdef INCLUDE_LCD

#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

String theLine = "";
int displayColumns = 16;
void displayPrintln(int theRow, String theString) {
  // To overwrite anything on the current line.
  String printString = theString;
  int theRest = displayColumns - theString.length();
  if (theRest < 0) {
    // Shorten to the display column length.
    printString = theString.substring(0, displayColumns);
  } else {
    // Buffer with spaces to the end of line.
    while (theRest < displayColumns) {
      printString = printString + " ";
      theRest++;
    }
  }
  lcd.setCursor(0, theRow);
  lcd.print(printString);
}

void readyLcd() {
  lcd.init();
  lcd.backlight();
  //                1234567890123456
  displayPrintln(0, "Altair 101");
  theLine = "LCD ready...";
  displayPrintln(1, theLine);
  // delay(3000);
  // lcd.clear();
}
#endif

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  irrecv.enableIRIn();
  Serial.println(F("+ infrared receiver ready for input."));

#ifdef INCLUDE_LCD
  readyLcd();
  Serial.println(F("+ LCD ready for output."));
#endif

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  delay(300);
  // Initial test pattern.
  displayLedData(B01010101);
  Serial.println(F("+ Data LED shift register set."));

  pinMode(WAIT_PIN, OUTPUT);
  // pinMode(MEMR_PIN, OUTPUT);
  pinMode(M1_PIN, OUTPUT);
  pinMode(HLTA_PIN, OUTPUT);
  pinMode(WO_PIN, OUTPUT);
  //
  digitalWrite(WAIT_PIN, HIGH);     // Wait: on.
  // digitalWrite(MEMR_PIN, HIGH);
  digitalWrite(M1_PIN, HIGH);       // MI: on.
  digitalWrite(HLTA_PIN, LOW);
  digitalWrite(WO_PIN, HIGH);       // WO: on, Inverse logic: off when writing out. On when not.
  Serial.println(F("+ LED lights configured and initialized."));

  int programSize = sizeof(theProgram);
  // List a program.
  listByteArray(theProgram, programSize);
  // Load a program.
  copyByteArrayToMemory(theProgram, programSize);

  Serial.println(F("+++ Start the processor loop."));
#ifdef RUN_NOW
  runProgram = true;
#endif
}

// -----------------------------------------------------------------------------
// Device Loop for processing each byte of machine code.

#ifdef RUN_DELAY
static unsigned long timer = millis();
#endif
void loop() {

  // Process infrared key presses.
  if (runProgram) {
#ifdef RUN_DELAY
    // When testing, can add a cycle delay.
    // Clock process timing is controlled by the timer.
    // Example, 50000 : once every 1/2 second.
    if (millis() - timer >= 500) {
#endif
      processData();
#ifdef RUN_DELAY
      timer = millis();
    }
#endif
    if (irrecv.decode(&results)) {
      // Key pressed:
      // + STOP program running, or
      // + Use the keypress value as input into the running program. Used by opcode: IN.
      infraredSwitchInput();
    }
  } else {
    if (irrecv.decode(&results)) {
      // Program control: RUN, SINGLE STEP, EXAMINe, EXAMINE NEXT, Examine previous.
      infraredSwitchControl();
    }
    delay(60);
  }

}
// -----------------------------------------------------------------------------
