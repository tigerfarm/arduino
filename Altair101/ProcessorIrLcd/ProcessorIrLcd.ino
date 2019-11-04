// -----------------------------------------------------------------------------
/*
  Altair 101 software microprocessor

  Program sections,
    Memory definitions and sample program definitions.
    Output: Front Panel LED lights and serial log messages.
    Memory Management.
    Front Panel Status LEDs.
    Process Front Panel toggle events.
    Instruction Set, Opcodes, Registers.
    Processing opcodes and opcode cycles
    setup() Computer initialization.
    loop()  Clock cycling through memory.

  Reference document, Intel 8080 Assembly Language Programming Manual:
    https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf

  Altair programming video, starting a 6 minutes in:
    https://www.youtube.com/watch?v=EV1ki6LiEmg

*/
// -----------------------------------------------------------------------------
// Memory definitions

int programCounter = 0;     // Program address value
const int memoryBytes = 512;
byte memoryData[memoryBytes];

byte theProgram[] = {
  /*
    //         Code     Octal    Inst Param  Encoding Param  Flags  Description
    const byte DAD_BC = 0011; // DAD         00001001          C    Add B:C to H:L. Set carry bit.
    //                           DAD  RP     00RP1001          C    Add register pair to HL (16 bit add)
    //                           Set carry bit if the addition causes a carry out.
    const byte JNC =    0322; // JNC  lb hb  11010010               Jump if carry bit is 0 (false).

  */
  // ------------------------------------------------------------------
  // Test: MVI, DAD, JNC
  //                Start program.
  0000,             // NOP
  0006, 0000,       // MVI  B,db : Move db to register B.
  0016, 0001,       // MVI  C,db : Move db to register B.
  0041, 0773, 0777, // LXI_HL lb hb. Load 0000:1 into register H:L.
  //                While carry bit {
  0000,               // NOP
  0166,               // HLT
  0011,               // DAD Add B:C to H:L. Set carry bit.
  0322,  8, 0000,     // JNC lb hb. If carry bit false, jump to lb hb (end while)
  0303, 17, 0000,     // JMP to the start of the while loop.
  //                }
  0166,             // HLT
  0000,             // NOP
  0303, 1, 0000,    // JMP to 1. Restart: jump to program start.
  // ------------------------------------------------------------------
  //                Data, starts at address 27.
  0000, 0101, 0001, // 5 6 7
  0010, 0110, 0111, // 8 9 0
  //
  0000, 0000, 0000  //       end
};

/*
  00 000 000 = 000 =   0 2^0
  00 000 001 = 002 =   1 2^0
  00 000 010 = 002 =   2 2^1
  00 000 100 = 040 =   4 2^2
  00 001 000 = 010 =   8 2^3
  00 010 000 = 020 =  16 2^4
  00 100 000 = 014 =  32 2^5
  01 000 000 = 014 =  64 2^6
  10 000 000 = 014 = 128 2^7
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
// Output: Front Panel and log messages such as memory address and data valules.

byte zeroByte = B00000000;
char charBuffer[16];

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
  printOctal(theByte);
  Serial.print(":");
  printByte(theByte);
  //
  // printByte(theByte);
  // Serial.print(theByte, BIN);
  // Serial.print(":");
  //
  // printOctal(theByte);
  // Serial.print(theByte, OCT);
  // Serial.print(" : ");
  //
  // Serial.println(theByte, DEC);
  // sprintf(charBuffer, "%3d", theByte);
  // Serial.print(charBuffer);
}

// -----------------------------------------------------------------------------
// Memory Functions

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
const int M1_PIN = A1;      // On, when current address is an opcode, which is Machine cycle 1. Off when getting an opcodes data bytes.
const int OUT_PIN = 42;     // Write output.
const int HLTA_PIN = 42;    // Halt acknowledge, halt instruction executed.
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
// Instruction Set, Opcodes, Registers

// This section is base on section 26: 8080 Instruction Set
//    https://www.altairduino.com/wp-content/uploads/2017/10/Documentation.pdf

// -----------------------------------------------------------------------------
// Processing opcodes and opcode cycles

byte opcode = 0;            // Opcode being processed
int instructionCycle = 0;   // Opcode process cycle

// Instruction parameters:
byte lowOrder = 0;           // lb: Low order byte of 16 bit value.
byte highOrder = 0;          // hb: High order byte of 16 bit value.
//                             Example: hb + lb = 16 bit memory address.
byte dataByte = 0;           // db = Data byte (8 bit)

/*
    a  = hb + lb (16 bit value)
    pa = Port address (8 bit)
    p  = 8 bit port address
*/
/*
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

// To do:
//
//         LXI_RP = 0041; // LXI RP,#  00RP0001 lb hb    -    Load lb and hb into the register pair (RP)
const byte LXI_BC = 0001; //           00 000 001 RP = 00 which matches "00=BC".
const byte LXI_DE = 0021; //           00 010 001 RP = 10 which matches "01=DE".
const byte LXI_SP = 0061; //           00 110 001 RP = 10 which matches "11=SP".

// Others to code:
// For STA and LDA, see the video: https://www.youtube.com/watch?v=3_73NwB6toY
const byte STA =    0062; // STA a     00110010 lb hb    -    Store register A to memory address: lb hb
const byte LDA =    0062; // LDA is for copying data from memory location to accumulator
//

// More opcodes for Kill the Bit:
const byte LDAX =   0032; // LDAX RP   00RP1010 *1       -       Load indirect through BC or DE
//                    *1 = Only RP=00(BC) and 01(DE) are allowed for LDAX/STAX
const byte XRA =    0252; // XRA S     10101SSS          ZSPCA   Exclusive OR register with A
const byte RRC =    0017; // RRC       00001111          C       Rotate A right
const byte IN =     0333; // IN p      11011011 pa       -       Read input port into A

// -----------------------------------------------------------------------------
// Opcodes that are programmed and tested:

//        Code   Octal       Inst Param  Encoding Param  Flags  Description
const byte CPI = 0376;    // CPI db      00000000          -    Compare db with A > compareResult.
const byte HLT = 0166;    // HLT         01110110          -    Halt processor
const byte INX_HL = 0043; // INX HL    00 100 011 00RP0011      Increment H:L (can be a 16 bit address)
const byte JMP = 0303;    // JMP a       11000011 lb hb    -    Unconditional jump
const byte JZ  = 0312;    // JZ lb hb    00000000          -    If compareResult is true, jump to lb hb.
const byte LXI_HL = 0041; // LXI RP,a  00 100 001 00RP0001 RP=10 which matches "10=HL".
const byte MOV_AM = 0176; // MOV  A  M(H:L) Where A is register A and M is the address in H:L
const byte NOP = 0000;    // NOP         00000000          -    No operation
//
// MOV D,S   01DDDSSS          -       Move register to a register.
// MOV D,M   01DDD110          -    Or Move register to the register M's address in H:L.
// Example, MOV A,M 176 =    01 111 110  Move the DATA at address H/L to register A.
//
// -------------------
// In progress, Kill the Bit opcodes:
//         Code     Octal    Inst Param  Encoding Param  Flags  Description
const byte MVI_B =  0006; // MVI  B,db   00 000 110 db     -      Move db to register B.
const byte MVI_C =  0016; // MVI  C,db   00 001 110 db     -      Move db to register B.
//                           MVI  R,db   00 RRR 110 db     -      Move db to register R.
//
const byte DAD_BC = 0011; // DAD         00001001        C      Add B:C to H:L. Set carry bit.
//                           DAD  RP     00RP1001        C      Add register pair to HL (16 bit add)
//                           Set carry bit if the addition causes a carry out.
const byte JNC =    0322; // JNC  lb hb  11010010               Jump if carry bit is 0 (false).

// -------------------
// Process flags and values.
boolean compareResult = true; // Set by CPI. Used by JZ.
boolean carryBit = false;     // Set by DAD. Used JNC.
boolean halted = false;       // Set true for an HLT opcode.
boolean runProgram = false;

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

void processOpcode() {
  int aValue = 0;       // For calculating if there is a carry over.
  int bValue = 0;
  int cValue = 0;
  int anAddress = 0;
  byte dataByte = memoryData[programCounter];
  switch (dataByte) {
    case CPI:
      opcode = CPI;
      Serial.print(F(" > CPI, compare next data byte to A. Store true or false into compareResult."));
      break;
    case DAD_BC:
      Serial.print(F(" > DAD_BC, Add B:C to H:L. Set carry bit. "));
      aValue = (int)word(regB, regC);
      bValue = (int)word(regH, regL);
      cValue = aValue + bValue;
      // 2^16 = 65535 = 64k = 11 111 111 : 11 111 111
      if (cValue > 65535) {
        carryBit = true;
      } else {
        carryBit = false;
      }
      Serial.print(F(", B:C "));
      Serial.print(aValue);
      Serial.print(F(" +  H:L "));
      Serial.print(bValue);
      Serial.print(F(" = "));
      Serial.print(cValue);
      break;
    case JNC:
      opcode = JNC;
      Serial.print(F(" > JNC, Jump if carry bit is 0 (false), not set."));
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
      // digitalWrite(HLTA_PIN, HIGH);
      // Set all the address and data lights on.
      digitalWrite(WAIT_PIN, HIGH);
      break;
    case INX_HL:
      regL++;       // Update this later because need to update the pair.
      Serial.print(F(" > INX, increment the 16 bit register pair H:L."));
      Serial.print(F(", regL = "));
      Serial.print(regL);
      // printOctal(regL);
      break;
    case LXI_HL:
      opcode = LXI_HL;
      Serial.print(F(" > LXI, move lb hb address into register pair H(hb):L(lb)."));
      break;
    case MOV_AM:
      Serial.print(F(" > MOV"));
      anAddress = word(regH, regL);
      regA = memoryData[anAddress];
      Serial.print(F(" data from Addr: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printOctal(regA);
      Serial.print(" = ");
      Serial.print(regA);
      break;
    case MVI_B:
      opcode = MVI_B;
      Serial.print(F(" > MVI, move db address into register B."));
      break;
    case MVI_C:
      opcode = MVI_C;
      Serial.print(F(" > MVI, move db address into register C."));
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
        Serial.print(F(" > JNC, jump to:"));
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
    case LXI_HL:
      if (instructionCycle == 1) {
        regL = memoryData[programCounter];
        Serial.print(F(" < LXI, lb: "));
        sprintf(charBuffer, "%4d:", regL);
        Serial.print(charBuffer);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      programCounter;
      regH = memoryData[programCounter];
      Serial.print(F(" < LXI, hb: "));
      sprintf(charBuffer, "%4d:", regH);
      Serial.print(charBuffer);
      Serial.print(F(" > H:L address: "));
      sprintf(charBuffer, "%8d:", word(regH, regL));
      Serial.print(charBuffer);
      printOctal(regH);
      Serial.print(F(" = "));
      printOctal(regL);
      programCounter++;
      break;
    case MVI_B:
      regB = memoryData[programCounter];
      Serial.print(F(" > MVI, move db > register B: "));
      Serial.print(regB);
      Serial.print(F(" = "));
      printOctal(regB);
      Serial.print(F(" = "));
      printByte(regB);
      programCounter++;
      break;
    case MVI_C:
      regC = memoryData[programCounter];
      Serial.print(F(" > MVI, move db > register C: "));
      Serial.print(regC);
      Serial.print(F(" = "));
      printOctal(regC);
      Serial.print(F(" = "));
      printByte(regC);
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

boolean buttonWentHigh = false;

// -----------------------------------------------------------------------
#include <IRremote.h>

int IR_PIN = 9;
IRrecv irrecv(IR_PIN);
decode_results results;

void infraredSwitch() {
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
      // Serial.println(F("+ Key > - next"));
      // digitalWrite(STEP_BUTTON_PIN, HIGH);
      // Serial.println("+ checkButton(), turn LED on.");
      processData();
      buttonWentHigh = true;
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      // Serial.println(F("+ Key up"));
      Serial.println(F("+ Run process."));
      runProgram = true;
      digitalWrite(WAIT_PIN, LOW);
      buttonWentHigh = true;
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      // Serial.println(F("+ Key down"));
      Serial.println(F("+ Stop process."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      buttonWentHigh = true;
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      Serial.println(F("+ Key OK - Toggle"));
      //
      Serial.println(F("+ Stop process."));
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      buttonWentHigh = true;
      //
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
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  irrecv.enableIRIn();
  Serial.println(F("+ infrared receiver initialized for input."));

  pinMode(WAIT_PIN, OUTPUT);
  digitalWrite(WAIT_PIN, HIGH);
  pinMode(M1_PIN, OUTPUT);
  digitalWrite(M1_PIN, HIGH);
  // digitalWrite(MEMR_PIN, HIGH);
  // digitalWrite(HLTA_PIN, LOW);
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
    if (buttonWentHigh && halted) {
      halted = false;
      buttonWentHigh = false;
      // digitalWrite(HLTA_PIN, LOW); // Not halted anymore.
    }
    delay(60);
  }

}
// -----------------------------------------------------------------------------
