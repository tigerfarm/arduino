// -----------------------------------------------------------------------------
/*
  Altair 101 software microprocessor

  Program sections,
    Definitions: Memory and sample program.
    Memory Functions.
    Front Panel Status LEDs.
    Definitions: Instruction Set, Opcodes, Registers.
    Output: Front Panel LED lights and serial log messages.
    Processor: Processing opcodes instructions
    Input: Front Panel infrared switch events
    setup() Computer initialization.
    loop()  Clock cycling through memory.

  Reference document, Intel 8080 Assembly Language Programming Manual:
    https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf
  Text listing of 8080 opcodes:
    http://www.classiccmp.org/dunfield/r/8080.txt

  Altair programming video, starting about 6 minutes in:
    https://www.youtube.com/watch?v=EV1ki6LiEmg

  Binary calculator:
    https://www.calculator.net/binary-calculator.html

  Extract highBtye()
    https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/highbyte/
  lowByte()
    https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/lowbyte/
*/
// -----------------------------------------------------------------------------
// Code compilation options.

// #define RUN_DELAY 1
// #define INCLUDE_LCD 1
#define LOG_MESSAGES 1

// -----------------------------------------------------------------------------
// Program to test opcodes.

byte theProgram[] = {
  //                //            ; --------------------------------------
  //                // Start:     ; Start
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //1RRR110
  B00111110, 6,     // mvi a,0    ; Move # to register A.
  //
  B00100110, 0,     // mvi h,0    ; Address for memory address testing.
  B00101110, 60,    // mvi l,60
  //
  0343, 38,         // out 38     ; Print the intial register values.
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  //
  B00111110, 6,     // mvi a,0    ; Move # to register A.
  0343, 37,         // out 37     ; Print register A.
  //
  B00110010, 60, 0, // sta 60     ; Store register A's data to the address(hb:lb).
  0343, 36,         // out 36     ; Print memory address data value. "36" prints the register pair H:L and data at the address.
  //
  B00111010, 60, 0, // lda 60     ; Load register A from the address(hb:lb).
  0343, 37,         // out 37     ; Print register A.
  //
  0166,             // hlt
  //
  // -----------------------------------------------------------------------------
  0303, 0000, 0000, // jmp Start    ; Jump back to beginning to avoid endless nops.
  0000              //            ; End.
};

// -----------------------------------------------------------------------------
// Loop Program.

byte theProgramLoop[] = {
  //                //            ; --------------------------------------
  //                // Start:       ; Start
  //
  0343, 38,         // out 38     ; Print the intial register values.
  0166,             // hlt
  //
  //                //            ; --------------------------------------
  //                //            ; Intialize register values.
  //
  0041, 0000, 0000, // lxi h,Start  ; lxi_HL lb hb. Load into register H:L = 0000:0000.
  0176,             // mov M      ; Move the data in register M(register address H:L), to register A.
  0006, 0001,       // mvi b,1    ; Move db to register B.
  0016, 0002,       // mvi c,2    ; Move db to register C.
  0127,             // mov d,a    ; Move register a to register D.
  0036, 0003,       // mvi e,3    ; Move db to register E.
  //
  0343, 38,         // out 38     ; Print the Intialized register values.
  0166,             // hlt
  //
  //                //            ; --------------------------------------
  //                // LOOP:      ; Change values and loop.
  0043,             // inx M      ; Increment register M(register address H:L).
  0176,             // mov M      ; Move the data in register M(register address H:L), to register A.
  0343, 38,         // out 38     ; Print the registers.
  0166,             // hlt
  0303, 14, 0000,   // jmp        ; Jump to LOOP.
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
// Front Panel Status LEDs

// Video demonstrating status lights:
//    https://www.youtube.com/watch?v=3_73NwB6toY

const int INTE_PIN = 42;    // On, interrupts enabled.
const int PROT_PIN = 42;    // Useful only if RAM has page protection impliemented.
// Status LEDs
const int MEMR_PIN = 42;    // Memory read such as fetching an op code (data instruction)
const int INP_PIN = 42;     // Input
const int M1_PIN = A3;      // On, when current address is an opcode, which is Machine cycle 1. Off when getting an opcodes data bytes.
const int OUT_PIN = 42;     // Write output.
const int hltA_PIN = A2;    // Halt acknowledge, halt instruction executed.
const int STACK_PIN = 42;   // On, reading or writing to the stack.
const int WO_PIN = 42;      // Write Output uses inverse logic. On, not writing output.
const int INT_PIN = 42;     // On when executing an interrupt step.
//
const int WAIT_PIN = A0;    // On, program not running. Off, programrunning.
const int HLDA_PIN = 42;    // 8080 processor go into a hold state because of other hardware.

// MEMR & MI & WO are on when fetching an op code, example: jmp(303) or lda(072).
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Definitions: Instruction Set, Opcodes, Registers

// This section is base on section 26: 8080 Instruction Set
//    https://www.altairduino.com/wp-content/uploads/2017/10/Documentation.pdf

// ------------------------------------
// Processing opcodes and opcode cycles

// Instruction parameters:
byte lowOrder = 0;           // lb: Low order byte of 16 bit value.
byte highOrder = 0;          // hb: High order byte of 16 bit value.
//                             Example: hb + lb = 16 bit memory address.
byte dataByte = 0;           // db = Data byte (8 bit)

// To do:
//
//         lxi_RP = 0041; // lxi RP,#  00RP0001 lb hb    -    Load lb and hb into the register pair (RP)
const byte lxi_DE = 0021; //           00 010 001 RP = 10 which matches "01=DE".
const byte lxi_SP = 0061; //           00 110 001 RP = 10 which matches "11=SP".
//

/*
    a  = hb + lb (16 bit value)
    d  = 8 bit data, such as data from an address
    pa = Port address (8 bit)
    p  = 8 bit port address

    Register pair 'RP' fields:
    00=BC   (B:C as 16 bit register)
    01=DE   (D:E as 16 bit register)
    10=HL   (H:L as 16 bit register)
    11=SP   (Stack pointer, refers to PSW (FLAGS:A) for PUSH/POP)
*/

//   Destination and Source register fields.
byte regA = 0;   // 111=A  a, register a, or Accumulator
byte regB = 0;   // 000=B  b, register B
byte regC = 0;   // 001=C  C
byte regD = 0;   // 010=D  D
byte regE = 0;   // 011=E  E
byte regH = 0;   // 100=H  H
byte regL = 0;   // 101=L  L
byte regM = 0;   // 110=M  Memory reference for address in H:L

// mvi R,#  00 RRR 110  Move a number (#), which is the next db, to register RRR.
// mvi a,#  00 111 110  0036
// mvi b,#  00 000 110  0006
// mvi c,#  00 001 110  0016
// mvi d,#  00 010 110  0026
// mvi e,#  00 011 110  0036
// mvi h,#  00 100 110  0046
// mvi l,#  00 101 110  0056

// mov d,S  01 DDD SSS   Move register to a register.
// mov b,a  01 000 111  0103
// mov c,a  01 001 111  0113
// mov d,a  01 010 111  0123  * Done
// mov e,a  01 011 111  0133
// mov h,a  01 100 111  0143
// mov l,a  01 101 111  0153

// -----------------------------------------------------------------------------
// Opcodes ordering

// Without parameters in the opcode:
// nop      00 000 000  No operation
// RLC      00 000 111  Rotate a left. Need to handle carry bit.
// rrc      00 001 111  Rotate a right (shift byte right 1 bit). Need to handle carry bit.
// hlt      01 110 110  Halt processor
// jmp a    11 000 011  Unconditional jump
// jz  a    11 001 010  If compareResult is true, jump to lb hb.
// jnc a    11 010 010  Jump if carry bit is 0 (false).
// out p    11 010 011  Write a to output port a.
// IN  p    11 011 011  Read input for port a, into A
// cpi #    11 111 110  Compare db with a > compareResult.

// With parameters in the opcode:
// mov d,S  01 DDD SSS  Move register to a register.
// xra R    10 101 SSS  Register exclusive OR with register with A.

// ldax RP  00 RP1 010  Load indirect through BC(RP=00) or DE(RP=01)
// inx RP   00 RP0 011  Increment a register pair, example: H:L (a 16 bit value)
// mvi R,#  00 RRR 110  Move a number (#), which is the next db, to register RRR.

// lxi RP,a 00 RP0 001  RP=10  which matches "10=HL".
// dad RP   00 RP1 001  Add register pair(RP) to H:L (16 bit add). And set carry bit.

// -----------------------------------------------------------------------------
// Opcodes that are programmed and tested:

//        Code   Octal       Inst Param  Encoding Flags  Description
const byte cpi    = 0376; // cpi db    11 111 110        Compare db with a > compareResult.
const byte hlt    = 0166; // hlt         01110110        Halt processor
const byte jmp    = 0303; // jmp  a      11000011        Unconditional jump
const byte jnc    = 0322; // jnc  lb hb  11010010        Jump if carry bit is 0 (false).
const byte jz     = 0312; // jz   lb hb  11001010        If compareResult is true, jump to lb hb.
const byte nop    = 0000; // nop         00000000        No operation
const byte out    = 0343; // out pa      11010011        Write a to output port
const byte rrc    = 0017; // rrc       00 001 111   c    Rotate a right (shift byte right 1 bit). Note, carry bit not handled at this time.

const byte dad_BC = 0011; // dad       00 001 001   c    Add B:C to H:L. Set carry bit.
const byte ldax_D = 0032; // ldax d    00 011 010        Load register a with the data at address D:E.
const byte lxi_BC = 0001; // lxi  Bc,a 00 000 001        Move a(lb hb) into register pair, B:C = hb:lb.
const byte lxi_HL = 0041; // lxi  RP,a 00 100 001        Move a(lb hb) into register pair, H:L = hb:lb.
const byte mov_AM = 0176; // mov  a,M  01 111 110        Move data from M(address in H:L) to register A.
const byte mov_DA = 0127; // mov  d,a  01 010 111        Move register a content to register D.
const byte mvi_B  = 0006; // mvi  b,db 00 000 110        Move db to register B.
const byte mvi_C  = 0016; // mvi  c,db 00 001 110        Move db to register C.
const byte mvi_D  = 0026; // mvi  d,db 00 010 110        Move db to register D.
const byte mvi_E  = 0036; // mvi  e,db 00 011 110        Move db to register E.
const byte xra_D  = 0252; // xra RP    10 101 010  ZSPCA Register d, exclusive OR with register with A.
//
// Opcode notes, more details:
// --------------------------
//        Code   Octal       Inst Param  Encoding Flags  Description
//                           dad  RP     00RP1001 c      Add register pair(RP) to H:L (16 bit add)
//                           inx  RP     00RP0011        Increment H:L (can be a 16 bit address)
//                           ldax RP     00RP1010 -      Load indirect through BC(RP=00) or DE(RP=01)
//                           lxi  RP,a   00RP0001        RP=10  which matches "10=HL".
//                           mvi  R,db   00RRR110        Move db to register R.
//                           xra  R      10101SSS        Register exclusive OR with register with A.
//
//        dad                Set carry bit if the addition causes a carry out.
//        inx                Not done for the stack pointer.
//        ldax d : Load the accumulator from memory location 938Bh, where register d contains 93H and register e contains 8BH.
//
// mov d,S   01DDDSSS          -       Move register to a register.
// mov d,M   01DDD110          -    Or Move data from M(H:L) address, to register DDD.
// Example, mov a,M 176 =    01 111 110  Move the DATA at address H/L to register A.
//
// --------------------------------------
// In progress, Kill the Bit opcodes:
//         Code     Octal    Inst Param  Encoding Flags  Description
const byte IN     = 0333; // IN p      11011011 pa       -       Read input port into A
//
// For STA and LDa, see the video: https://www.youtube.com/watch?v=3_73NwB6toY
const byte STA =    0062; // STA a     00110010 lb hb    -    Store register a to memory address: lb hb
const byte LDA =    0062; // LDA is for copying data from memory location to accumulator
//
// inr d     00DDD100    Flags:ZSPA    Increment register
// dcr d     00DDD101          ZSPA    Decrement register

// -----------------------------------------------------------------------------
// Output: Front Panel Output and log messages

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

// -----------------------------------------------------------------------------
// Processor: Processing opcode instructions

int programCounter = 0;     // Program address value

byte opcode = 0;            // Opcode being processed
int instructionCycle = 0;   // Opcode process cycle

void printAddressData() {
  Serial.print(F("Addr: "));
  sprintf(charBuffer, "%4d:", programCounter);
  Serial.print(charBuffer);
  Serial.print(F(" Data: "));
  printData(memoryData[programCounter]);
}

void processData() {
  if (opcode == 0) {
    digitalWrite(M1_PIN, HIGH); // Machine cycle 1, get an opcode.
    Serial.print("> ");
    printAddressData();
    Serial.print(" ");
    processOpcode();
    programCounter++;
    instructionCycle = 0;
  } else {
    digitalWrite(M1_PIN, LOW); // Machine cycle 1+x, getting opcode data.
    Serial.print("+ ");
    printAddressData();
    Serial.print(" ");
    processOpcodeData();
  }
  Serial.println("");
}

// -----------------------------------
// Process flags and values.
boolean compareResult = true; // Set by cpi. Used by jz.
boolean carryBit = false;     // Set by dad. Used jnc.
boolean halted = false;       // Set true for an hlt opcode.
boolean runProgram = false;

// For calculating 16 values.
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

void processOpcode() {

  unsigned int anAddress = 0;
  byte dataByte = memoryData[programCounter];
  switch (dataByte) {
    case cpi:
      opcode = cpi;
#ifdef LOG_MESSAGES
      Serial.print(F("> cpi, compare next data byte to A. Store true or false into compareResult."));
#endif
      break;
    // ---------------------------------------------------------------------
    case dad_BC:
      bValue = regB;
      cValue = regC;
      hValue = regH;
      lValue = regL;
      bcValue = bValue * 256 + cValue;
      hlValue = hValue * 256 + lValue;
      hlValueNew = (hValue * 256 + lValue) + (bValue * 256 + cValue);
      // 2^16 = 65535 = 64k = 11 111 111 : 11 111 111
      // B:C 1 + H:L 65533 = 65534
      // B:C 1 + H:L 65534 = 65535
      // B:C 1 + H:L 65535 = 0
      if (hlValueNew < bcValue || hlValueNew < hlValue) {
        carryBit = true;
      } else {
        carryBit = false;
      }
      regH = regB + regH;
      lValue = regL;
      regL = regC + regL;
      if (regL < regC || regL < lValue) {
        // If the sum is less the either part, then add 1 to the higher byte.
        regH++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F("> dad_BC"));
      Serial.print(F(", Add B:C "));
      Serial.print(bcValue);
      Serial.print("(");
      Serial.print(bValue);
      Serial.print(":");
      Serial.print(cValue);
      Serial.print(")");
      Serial.print(F(" + H:L "));
      Serial.print(hlValue);
      Serial.print("(");
      Serial.print(hValue);
      Serial.print(":");
      Serial.print(lValue);
      Serial.print(")");
      Serial.print(F(" = "));
      Serial.print(hlValueNew);
      Serial.print("(");
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
      Serial.print(")");
      Serial.print(F(", Carry bit set: "));
      if (carryBit) {
        Serial.print(F("true. Over: 65535."));
      } else {
        Serial.print(F("false. Not over: 65535."));
      }
#endif
      /*
        void _I8080_dad(uint8_t rp) {
        uint16_t d16_1;
        uint16_t d16_2;
        uint16_t d16_3;
        d16_1 = _rH * 256 + _rL;
        switch (rp) {
        case _RP_BC:
        d16_2 = _rB * 256 + _rC;
        break;
        case _RP_DE:
        d16_2 = _rD * 256 + _rE;
        break;
        case _RP_HL:
        d16_2 = _rH * 256 + _rL;
        break;
        case _RP_SP:
        d16_2 = _SP;
        break;
        }
        d16_3 = d16_1 + d16_2;
        _rH = highByte(d16_3);
        _rL = lowByte(d16_3);
        if ((d16_3 < d16_1) || (d16_3 < d16_2)) {
        _setFlags_C(1);
        }
        else {
        _setFlags_C(0);
        }
        _PC++;
        }
      */
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
    case hlt:
      runProgram = false;
      halted = true;
#ifdef LOG_MESSAGES
      Serial.println(F("> hlt, halt the processor."));
      digitalWrite(WAIT_PIN, HIGH);
      digitalWrite(M1_PIN, LOW);
      digitalWrite(hltA_PIN, HIGH);
#else
      Serial.println("");
#endif
      Serial.print(F("+ Process halted."));
      // To do: set all the address and data lights on.
      break;
      opcode = IN;
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
    case B00000011:
      regC++;
      if (regC == 0) {
        regB++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F("> inx, increment the 16 bit register pair B:C."));
      Serial.print(F(", regB:regC = "));
      Serial.print(regB);
      Serial.print(":");
      Serial.print(regC);
      Serial.print(" = ");
      printOctal(regB);
      Serial.print(":");
      printOctal(regC);
#endif
      break;
    case B00010011:
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
    case B00100011:
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
    case ldax_D:
      anAddress = word(regD, regE);
      dValue = regD;
      eValue = regE;
      deValue = dValue * 256 + eValue;
      if (deValue < memoryBytes) {
        regA = memoryData[deValue];
      } else {
        regA = 0;
      }
#ifdef LOG_MESSAGES
      Serial.print(F("> ldax"));
      Serial.print(F(", Load data from Address D:E = "));
      printOctal(regD);
      Serial.print(F(" : "));
      printOctal(regE);
      Serial.print(F(" = "));
      Serial.print(deValue);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    // ---------------------------------------------------------------------
    case lxi_BC:
      opcode = lxi_BC;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Move the lb hb data, into register pair B:C = hb:lb."));
#endif
      break;
    case lxi_HL:
      opcode = lxi_HL;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Move the lb hb data, into register pair H:L = hb:lb."));
#endif
      break;
    // ---------------------------------------------------------------------
    case mov_AM:
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
    case mvi_B:
      opcode = mvi_B;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register B."));
#endif
      break;
    case mvi_C:
      opcode = mvi_C;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register C."));
#endif
      break;
    case mvi_D:
      opcode = mvi_D;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register D."));
#endif
      break;
    case mvi_E:
      opcode = mvi_E;
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
      Serial.print(F("> nop ---"));
#endif
      delay(100);
      break;
    // ------------------------------------------------------------------------------------------
    case out:
      opcode = out;
#ifdef LOG_MESSAGES
#else
      Serial.println("");
#endif
      Serial.print(F("> OUT, Write output to the port address(following db)."));
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
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    // stacy
    case B00110010:
      opcode = B00110010;
#ifdef LOG_MESSAGES
      Serial.print(F("> sta, Store register A to the hb:lb address."));
#endif
      break;
    case B00111010:
      opcode = B00110010;
#ifdef LOG_MESSAGES
      Serial.print(F("> lda, Load register A with data from the hb:lb address."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case xra_D:
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
    // ------------------------------------------------------------------------------------------
    default:
      Serial.print(F(" - Error, unknown instruction."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      // ------------------------------------------------------------------------------------------
  }
}

void processOpcodeData() {
  // For calculating hb:lb as a 16 value.
  unsigned int hbLb16BitValue = 0;
  unsigned int hbLb16BitValue1 = 0;
  unsigned int hbLb16BitValue2 = 0;

  // Note,
  //    if not jumping, increment programCounter.
  //    if jumping, don't increment programCounter.

  instructionCycle++;
  switch (opcode) {
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
    case IN:
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
      Serial.print(F("< IN, input port: "));
      Serial.print(dataByte);
      Serial.print(F(". Not implemented, yet."));
      programCounter++;
      break;
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
    case lxi_BC:
      if (instructionCycle == 1) {
        regB = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regB);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regC = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regH);
      Serial.print(charBuffer);
      Serial.print(F("> B:C = "));
      printOctal(regB);
      Serial.print(F(":"));
      printOctal(regC);
#endif
      programCounter++;
      break;
    case lxi_HL:
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
    case mvi_B:
      regB = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register B: "));
      printData(regB);
#endif
      programCounter++;
      break;
    case mvi_C:
      regC = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register C: "));
      printData(regC);
#endif
      programCounter++;
      break;
    case mvi_D:
      regD = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register D: "));
      printData(regD);
#endif
      programCounter++;
      break;
    case mvi_E:
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
#else
      Serial.println("");
#endif
      Serial.print(F("< OUT, input port: "));
      Serial.print(dataByte);
      switch (dataByte) {
        case 1:
          Serial.print(F(", terminal output to be implemented on LCD."));
          break;
        case 38:
          Serial.println("");
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
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    // stacy
    case B00110010:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< sta, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
      hlValue = highOrder * 256 + lowOrder;
      memoryData[hlValue] = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("< sta, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
      Serial.print(F(", register A: "));
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
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
      hlValue = highOrder * 256 + lowOrder;
      regA = memoryData[hlValue];
#ifdef LOG_MESSAGES
      Serial.print(F("< lda, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
      Serial.print(F(" hb:lb address data move to register A: "));
      Serial.print(regA);
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
// Front Panel toggle/button switch events
// Using an infrared receiver for simplier hardware setup.

boolean buttonWentHigh = false;

// -----------------------------------------------------------------------
#include <IRremote.h>

int IR_PIN = 9;
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
      digitalWrite(hltA_PIN, LOW);
      processData();
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      // Serial.println(F("+ Key up"));
      Serial.println(F("+ Run process."));
      runProgram = true;
      digitalWrite(WAIT_PIN, LOW);
      digitalWrite(hltA_PIN, LOW);
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      // Already stopped.
      // Serial.println(F("+ Key down"));
      // Serial.println(F("+ Stop process."));
      // runProgram = false;
      // digitalWrite(WAIT_PIN, HIGH);
      // digitalWrite(hltA_PIN, LOW);
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Run process."));
      runProgram = true;
      digitalWrite(WAIT_PIN, LOW);
      digitalWrite(hltA_PIN, LOW);
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
      digitalWrite(hltA_PIN, LOW);
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Stop process."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      digitalWrite(hltA_PIN, LOW);
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

  pinMode(WAIT_PIN, OUTPUT);
  digitalWrite(WAIT_PIN, HIGH);
  // pinMode(MEMR_PIN, OUTPUT);
  // digitalWrite(MEMR_PIN, HIGH);
  pinMode(M1_PIN, OUTPUT);
  digitalWrite(M1_PIN, HIGH);
  pinMode(hltA_PIN, OUTPUT);
  digitalWrite(hltA_PIN, LOW);
  // pinMode(WO_PIN, OUTPUT);
  // digitalWrite(WO_PIN, HIGH);
  Serial.println(F("+ LED lights configured and initialized."));

  int programSize = sizeof(theProgram);
  // List a program.
  listByteArray(theProgram, programSize);
  // Load a program.
  copyByteArrayToMemory(theProgram, programSize);

  Serial.println(F("+++ Start the processor loop."));
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
