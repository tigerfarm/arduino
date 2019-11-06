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
*/
// -----------------------------------------------------------------------------
// Program to test opcodes.

byte theProgram[] = {
  // ------------------------------------------------------------------
  // Kill the Bit program.
  // Before starting, make sure all the sense switches are in the down position.
  //
  //                Start program.
  0041, 0000, 0000, // LXI H,0    ; Move the lb hb data values into the register pair H(hb):L(lb). Initialize counter
  0026, 0200,       // MVI D,080h ; Move db to register D. Set initial display bit.  080h = 0200 = regD = 10 000 000
  0001, 0036, 0000, // LXI B,0eh  ; Load a(lb:hb) into register B:C. Higher value = faster. Default: 0016 = B:C  = 00 010 000
  //
  //  ; Display bit pattern on upper 8 address lights.
  //                // BEG:
  0166,             // HLT
  0032,             // LDAX D     ; Move data from address D:E, to register A.
  0032,             // LDAX D     ; Move data from address D:E, to register A.
  0032,             // LDAX D     ; Move data from address D:E, to register A.
  0032,             // LDAX D     ; Move data from address D:E, to register A.
  //
  0011,             // DAD B      ; Add B:C to H:L. Set carry bit. Increments the display counter 
  0322, 0010, 0000, // JNC BEG    ; If carry bit false, jump to lb hb, LDAX instruction start.
  //
  0333, 0377,       // IN 0ffh    ; Check for the toggled input, at port 377 (toggle sense switches), that can kill the bit.
  0252,             // XRA D      ; Exclusive OR register with A
  0017,             // RRC        ; Rotate A right (shift byte right 1 bit). Set carry bit. Rotate display right one bit 
  0127,             // MOV D,A    ; Move register A to register D. Move data to display reg
  // 
  0303, 0010, 0000, // JMP BEG    ; Jump to lb hb, LDAX instruction start.
  // 0000,             // NOP
  // 0166,             // HLT
  // ------------------------------------------------------------------
  0000, 0000, 0000  //       end
};

//  Kill the Bit program:
/*
  Addr Data toggles  Octal Value
  00   00 100 001     041 lxi  : Move the data at lb hb address, into register pair H(hb):L(lb)
  01   00 000 000     000      : lb
  02   00 000 000     000      : hb
  03   00 010 110     026 mvi  : Move db to register D.
  04   10 000 000     200      : db
  05   00 000 001     001 lxi  : Move the lb hb data, into register pair > B:C = hb:lb.
  06   00 001 110     016      : lb = 016
  07   00 000 000     000      : hb = 000
  //                         Make the bit that move across the hb address LED lights.
  08   00 011 010 beg:032 ldax : Load register A with the data at address D:E.
  09   00 011 010     032 ldax
  10   00 011 010     032 ldax
  11   00 011 010     032 ldax
  12   00 001 001     011 dad  : Add B:C to H:L. Set carry bit.
  13   11 010 010     322 jnc  : Jump to address 8(000:010)
  14   00 001 000     010
  15   00 000 000     000
  //
  16   11 011 011     333 in   : Check for the toggled input that can kill the bit.
  17   11 111 111     377
  18   10 101 010     252 xra
  19   00 001 111     017 rrc
  20   01 010 111     127 mov  : Move register A to register D.
  21   11 000 011     303 jmp  : Jump instruction: jmp beg
  22   00 001 000     010      : lb = 8. 00 001 000 = 8. Low order address bits.
  23   00 000 000     000      : hb = 0, to get a 16 bit address(hb:lb): 00 000 000 : 00 001 000 = 8.
  24                      end
*/

/*
  00 000 000 = 000 =   0
  00 000 001 = 002 =   1 2^0
  00 000 010 = 002 =   2 2^1
  00 000 100 = 040 =   4 2^2
  00 001 000 = 010 =   8 2^3
  00 010 000 = 020 =  16 2^4
  00 100 000 = 014 =  32 2^5
  01 000 000 = 014 =  64 2^6
  10 000 000 = 014 = 128 2^7 8 bits store valuse: 0 ... 255
                     256 2^8
                     512 2^9
                    1024 2^10  1K
                    2048 2^11  2K
                    4096 2^12  4K
                    8192 2^13  8K
                   16384 2^14 16K
                   32768 2^15 32k
                   65535 2^16 64k
*/
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
const int HLTA_PIN = A2;    // Halt acknowledge, halt instruction executed.
const int STACK_PIN = 42;   // On, reading or writing to the stack.
const int WO_PIN = 42;      // Write Output uses inverse logic. On, not writing output.
const int INT_PIN = 42;     // On when executing an interrupt step.
//
const int WAIT_PIN = A0;    // On, program not running. Off, programrunning.
const int HLDA_PIN = 42;    // 8080 processor go into a hold state because of other hardware.

// MEMR & MI & WO are on when fetching an op code, example: JMP(303) or lda(072).
// MEMR & WO are on when fetching a low or high byte of an address.
// MEMR & WO are on when fetching data from an address.
// All status LEDs are off when storing a value to a memory address.
// When doing a data write, data LEDs are all on.
//
// Halt: MEMR & HLTA & WO are on. All address and data lights are on.
//  System is locked. To get going again, hard reset: flip stop and reset same time.
//
// STACK is the only status LED on when making a stack push (write to the stack).
// MEMR & STACK & WO are on when reading from the stack.
//
// INP & WO are on when reading from an input port.
// OUT is on when send an output to a port.
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
//         LXI_RP = 0041; // LXI RP,#  00RP0001 lb hb    -    Load lb and hb into the register pair (RP)
const byte LXI_DE = 0021; //           00 010 001 RP = 10 which matches "01=DE".
const byte LXI_SP = 0061; //           00 110 001 RP = 10 which matches "11=SP".

// For STA and LDA, see the video: https://www.youtube.com/watch?v=3_73NwB6toY
const byte STA =    0062; // STA a     00110010 lb hb    -    Store register A to memory address: lb hb
const byte LDA =    0062; // LDA is for copying data from memory location to accumulator
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
byte regA = 0;   // 111=A  A, register A, or Accumulator
byte regB = 0;   // 000=B  B, register B
byte regC = 0;   // 001=C  C
byte regD = 0;   // 010=D  D
byte regE = 0;   // 011=E  E
byte regH = 0;   // 100=H  H
byte regL = 0;   // 101=L  L
byte regM = 0;   // 110=M  Memory reference through address in H:L

// -----------------------------------------------------------------------------
// Opcodes that are programmed and tested:

//        Code   Octal       Inst Param  Encoding Flags  Description
const byte CPI    = 0376; // CPI db    11 111 110   -    Compare db with A > compareResult.
const byte DAD_BC = 0011; // DAD       00 001 001   C    Add B:C to H:L. Set carry bit.
const byte HLT    = 0166; // HLT         01110110   -    Halt processor
const byte INX_HL = 0043; // INX  HL   00 100 011        Increment H:L (can be a 16 bit address)
const byte JMP    = 0303; // JMP  a      11000011   -    Unconditional jump
const byte JNC    = 0322; // JNC  lb hb  11010010        Jump if carry bit is 0 (false).
const byte JZ     = 0312; // JZ   lb hb  11001010   -    If compareResult is true, jump to lb hb.
const byte LXI_BC = 0001; // LXI  BC,a 00 000 001        Move a(lb hb) into register pair, B:C = hb:lb.
const byte LXI_HL = 0041; // LXI  RP,a 00 100 001        Move a(lb hb) into register pair, H:L = hb:lb.
const byte MOV_AM = 0176; // MOV  A,M  01 111 110        Move data from M(address in H:L) to register A.
const byte MOV_DA = 0127; // MOV  D,A  01 010 111        Move register A content to register D.
const byte MVI_B  = 0006; // MVI  B,db   00 000 110 -    Move db to register B.
const byte MVI_C  = 0016; // MVI  C,db   00 001 110 -    Move db to register C.
const byte MVI_D  = 0026; // MVI  D,db 00 010 110 -      Move db to register D.
const byte MVI_E  = 0036; // MVI  E,db 00 011 110 -      Move db to register E.
const byte NOP    = 0000; // NOP         00000000   -    No operation
//
// Opcode notes, more details:
// --------------------------
//        Code   Octal       Inst Param  Encoding Flags  Description
//                           DAD  RP     00RP1001 C      Add register pair(RP) to H:L (16 bit add)
//                           INX  RP     00RP0011        Increment H:L (can be a 16 bit address)
//                           LDAX RP     00RP1010 -      Load indirect through BC(RP=00) or DE(RP=01)
//                           LXI  RP,a   00RP0001        RP=10  which matches "10=HL".
//                           MVI  R,db   00RRR110 db     Move db to register R.
//
//        DAD                Set carry bit if the addition causes a carry out.
//        LDAX D : Load the accumulator from memory location 938BH, where register D contains 93H and register E contains 8BH.
//
// MOV D,S   01DDDSSS          -       Move register to a register.
// MOV D,M   01DDD110          -    Or Move data from M(H:L) address, to register DDD.
// Example, MOV A,M 176 =    01 111 110  Move the DATA at address H/L to register A.
//
// --------------------------------------
// In progress, Kill the Bit opcodes:
//         Code     Octal    Inst Param  Encoding Flags  Description
const byte LDAX_D = 0032; // LDAX D    00 011 010 -      Load register A with the data at address D:E.
const byte XRA    = 0252; // XRA S     10101SSS          ZSPCA   Exclusive OR register with A
const byte RRC    = 0017; // RRC       00001111          C       Rotate A right (shift byte right 1 bit)
const byte IN     = 0333; // IN p      11011011 pa       -       Read input port into A

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
    processOpcode();
    programCounter++;
    instructionCycle = 0;
  } else {
    digitalWrite(M1_PIN, LOW); // Machine cycle 1+x, getting opcode data.
    Serial.print("+ ");
    printAddressData();
    processOpcodeData();
  }
  Serial.println("");
}

// -----------------------------------
// Process flags and values.
boolean compareResult = true; // Set by CPI. Used by JZ.
boolean carryBit = false;     // Set by DAD. Used JNC.
boolean halted = false;       // Set true for an HLT opcode.
boolean runProgram = false;

void processOpcode() {

  // For calculating if there is a carry over.
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

  unsigned int anAddress = 0;
  byte dataByte = memoryData[programCounter];
  switch (dataByte) {
    case CPI:
      opcode = CPI;
      Serial.print(F(" > CPI, compare next data byte to A. Store true or false into compareResult."));
      break;
    case DAD_BC:
      Serial.print(F(" > DAD_BC"));
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
      Serial.print(F(", Add B:C "));
      Serial.print(bcValue);
      Serial.print("(");
      Serial.print(regB);
      Serial.print(":");
      Serial.print(regC);
      Serial.print(")");
      Serial.print(F(" + H:L "));
      Serial.print(hlValue);
      Serial.print("(");
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
      Serial.print(")");
      Serial.print(F(" = "));
      Serial.print(hlValueNew);
      regH = regB + regH;   // Stacy, update for proper 16 bit addition.
      regL = regC + regL;
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
      /*
        Example of how to calculate the value of hb and hl:
        + Addr:    6: Data: 251 = 373 = 11111011 < LXI, lb:  251:
        + Addr:    7: Data: 255 = 377 = 11111111 < LXI, hb:  255: > H:L = 377:373 = ?
        11111111:11111111 = 65535
        11111111:11111011 = 65531
        11111111:00000000 = 65280
        11111111 = 255
        11111011 = 251
        65531 - 251 = 65280
        65280 / 255 = 256
        255 * 256 + 251 = 65531
        + Addr:    6: Data: 251 = 373 = 11111011 < LXI, lb:  251:
        + Addr:    7: Data: 255 = 377 = 11111111 < LXI, hb:  255: > H:L = 377:373 = 65531
      */
      break;
    case JNC:
      opcode = JNC;
      Serial.print(F(" > JNC, Jump if carry bit is false (0, not set)."));
      break;
    case JZ:
      opcode = JZ;
      Serial.print(F(" > JZ, if compareResult, jump to the following address (lh hb)."));
      break;
    case JMP:
      opcode = JMP;
      Serial.print(F(" > JMP, get address low and high order bytes."));
      break;
    case HLT:
      Serial.print(F(" > HLT, halt the processor."));
      runProgram = false;
      halted = true;
      digitalWrite(WAIT_PIN, HIGH);
      digitalWrite(M1_PIN, LOW);
      digitalWrite(HLTA_PIN, HIGH);
      // To do: set all the address and data lights on.
      break;
    case INX_HL:
      regL++;       // Update this later because need to update the pair.
      Serial.print(F(" > INX, increment the 16 bit register pair H:L."));
      Serial.print(F(", regL = "));
      Serial.print(regL);
      // printOctal(regL);
      break;
    case LDAX_D:
      Serial.print(F(" > LDAX"));
      anAddress = word(regD, regE);
      dValue = regD;
      eValue = regE;
      deValue = dValue * 256 + eValue;
      if (deValue < memoryBytes){
        regA = memoryData[deValue];
      } else {
        regA = 0;
      }
      Serial.print(F(", Load data from Address D:E = "));
      printOctal(regD);
      Serial.print(F(" : "));
      printOctal(regE);
      Serial.print(F(" = "));
      Serial.print(deValue);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
      break;
    case LXI_BC:
      opcode = LXI_BC;
      Serial.print(F(" > LXI, Move the lb hb data, into register pair B:C = hb:lb."));
      break;
    case LXI_HL:
      opcode = LXI_HL;
      Serial.print(F(" > LXI, Move the lb hb data, into register pair H:L = hb:lb."));
      break;
    case MOV_AM:
      Serial.print(F(" > MOV"));
      anAddress = word(regH, regL);
      regA = memoryData[anAddress];
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
      break;
    case MOV_DA:
      Serial.print(F(" > MOV"));
      regD = regA;
      Serial.print(F(", move data from register A "));
      Serial.print(F(" to register D = "));
      printData(regD);
      break;
    case MVI_B:
      opcode = MVI_B;
      Serial.print(F(" > MVI, move db address into register B."));
      break;
    case MVI_C:
      opcode = MVI_C;
      Serial.print(F(" > MVI, move db address into register C."));
      break;
    case MVI_D:
      opcode = MVI_D;
      Serial.print(F(" > MVI, move db address into register D."));
      break;
    case MVI_E:
      opcode = MVI_E;
      Serial.print(F(" > MVI, move db address into register E."));
      break;
    case NOP:
      Serial.print(F(" > NOP ---"));
      // delay(100);
      break;
    default:
      Serial.print(F(" - Error, unknown instruction."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
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
    case CPI:
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
      compareResult = dataByte == regA;
      Serial.print(F(" > CPI, compareResult db: "));
      Serial.print(dataByte);
      if (compareResult) {
        Serial.print(F(" == "));
      } else {
        Serial.print(F(" != "));
      }
      Serial.print(F(" A: "));
      Serial.print(regA);
      programCounter++;
      break;
    case JNC:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
        Serial.print(F(" < JNC, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
      Serial.print(F(" < JNC, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
      if (!carryBit) {
        Serial.print(F(" > JNC, carryBit is false, jump to:"));
        programCounter = word(highOrder, lowOrder);
        Serial.print(programCounter);
      } else {
        Serial.print(F(" - carryBit is true, don't jump."));
        programCounter++;
      }
      break;
    case JZ:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
        Serial.print(F(" < JZ, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
      Serial.print(F(" < JZ, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
      if (compareResult) {
        Serial.print(F(" > JZ, jump to:"));
        programCounter = word(highOrder, lowOrder);
        Serial.print(programCounter);
      } else {
        Serial.print(F(" - compareResult is false, don't jump."));
        programCounter++;
      }
      break;
    case JMP:
      if (instructionCycle == 1) {
        lowOrder = programCounter;
        Serial.print(F(" > JMP, lb: "));
        Serial.print(lowOrder);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      programCounter = word(memoryData[programCounter], memoryData[lowOrder]);
      Serial.print(F(" > JMP, jump to:"));
      sprintf(charBuffer, "%4d = ", programCounter);
      Serial.print(charBuffer);
      printByte((byte)programCounter);
      break;
    case LXI_BC:
      if (instructionCycle == 1) {
        regB = memoryData[programCounter];
        Serial.print(F(" < LXI, lb data: "));
        sprintf(charBuffer, "%4d:", regB);
        Serial.print(charBuffer);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regC = memoryData[programCounter];
      Serial.print(F(" < LXI, hb data: "));
      sprintf(charBuffer, "%4d:", regH);
      Serial.print(charBuffer);
      Serial.print(F(" > B:C = "));
      printOctal(regB);
      Serial.print(F(":"));
      printOctal(regC);
      programCounter++;
      break;
    case LXI_HL:
      if (instructionCycle == 1) {
        regL = memoryData[programCounter];
        Serial.print(F(" < LXI, lb data: "));
        sprintf(charBuffer, "%4d:", regL);
        Serial.print(charBuffer);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regH = memoryData[programCounter];
      Serial.print(F(" < LXI, hb data: "));
      sprintf(charBuffer, "%4d:", regH);
      Serial.print(charBuffer);
      Serial.print(F(" > H:L = "));
      printOctal(regH);
      Serial.print(F(":"));
      printOctal(regL);
      programCounter++;
      break;
    case MVI_B:
      regB = memoryData[programCounter];
      Serial.print(F(" < MVI, move db > register B: "));
      printData(regB);
      programCounter++;
      break;
    case MVI_C:
      regC = memoryData[programCounter];
      Serial.print(F(" < MVI, move db > register C: "));
      printData(regC);
      programCounter++;
      break;
    case MVI_D:
      regD = memoryData[programCounter];
      Serial.print(F(" < MVI, move db > register D: "));
      printData(regD);
      programCounter++;
      break;
    case MVI_E:
      regE = memoryData[programCounter];
      Serial.print(F(" < MVI, move db > register E: "));
      printData(regE);
      programCounter++;
      break;
    default:
      Serial.print(F(" - Error, unknow instruction."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
  }
  // The opcode cycles are complete.
  opcode = 0;
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

void infraredSwitch() {

  // If a program is running, only the STOP option is avaiable.
  // The OK (Return) button, becomes STOP when a program is running.
  if (runProgram) {
    if (results.value == 0xFF4AB5 || results.value == 0xE0E08679
        ||  results.value == 0xFF38C7 || results.value == 0xE0E016E9) {
      Serial.println(F("+ Stop process."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      digitalWrite(HLTA_PIN, LOW);
    }
    return;
  }

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

}

// -----------------------------------------------------------------------------
// 1602 LCD
/*
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
*/
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  irrecv.enableIRIn();
  Serial.println(F("+ infrared receiver ready for input."));

  // readyLcd();
  // Serial.println(F("+ LCD ready for output."));

  pinMode(WAIT_PIN, OUTPUT);
  digitalWrite(WAIT_PIN, HIGH);
  // pinMode(MEMR_PIN, OUTPUT);
  // digitalWrite(MEMR_PIN, HIGH);
  pinMode(M1_PIN, OUTPUT);
  digitalWrite(M1_PIN, HIGH);
  pinMode(HLTA_PIN, OUTPUT);
  digitalWrite(HLTA_PIN, LOW);
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

// static unsigned long timer = millis();
void loop() {

  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    infraredSwitch();
    irrecv.resume();
  }

  if (runProgram) {
    // When testing, can add a cycle delay.
    // Clock process timing is controlled by the timer.
    // Example, 50000 : once every 1/2 second.
    // if (millis() - timer >= 500) {
    processData();
    // timer = millis();
    // }
  } else {
    delay(60);
  }

}
// -----------------------------------------------------------------------------
