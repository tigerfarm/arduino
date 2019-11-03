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
*/
// -----------------------------------------------------------------------------
// Memory definitions

// Should move the memory into a function?
// Test if that will move memoryData from Global memory (2K) to program memory (30K).

int programCounter = 0;     // Program address value
const int memoryBytes = 512;
byte memoryData[memoryBytes];

// Define a jump loop program byte array.
byte jumpLoopProgram[] = {
  0303, 0006, 0000, // 0 1 2
  0000, 0000, 0000, // 3 4 5
  0303, 0000, 0000  // 6 7 8
};
// Needs to work like in the Altair in the following video, starting a 6 minutes in:
//    https://www.youtube.com/watch?v=EV1ki6LiEmg
// When single stepping,
// + MEMR and WO are on.
// + Flip single step,
// ++ The new address is displayed and data value retreived and LED displayed.
// ++ The data value is the jump instruction code.
// + MEMR and WO are on. Status LED MI is off.
// + Flip single step, status LED MI goes on.
// + And Low order address byte is retrieved and shown. Status LED MI goes off.
// + Flip single step, high order address byte is retrieved and shown.
// + Status LED MI goes off.
// + Flip single step,
// ++ Program counter is to that address.
// ++ The new address and data values are LED displayed.
// ++ Status LED MI goes on.
// Note, the simulator works like my code, which is different than the Altair in the video.

// Define a jump loop program with NOP instructions.
byte jumpLoopNopProgram[] = {
  0303, 0004, 0000, // 0 1 2
  0000, 0000, 0000, // 3 4 5
  0303, 0000, 0000  // 6 7 8
};

byte TestProgram[] = {
  0303, 0003, 0000, // 0 1 2 JMP to address: 4 
  0041, 0006, 0000, // 3 4 5 LXI_HL lb hb. Load 0000:0006 into register H:L.
  0303, 0000, 0000  // 6 7 8
};

// Define a jump loop program
//    with a halt(HLT binary 01 110 110 = octal 166) instruction,
//    and NOP instructions.
byte jumpHaltLoopProgram[] = {
  0303, 0006, 0000,
  0000, 0000, 0000,
  0166, 0000, 0000,
  0303, 0000, 0000
};

// Sample to test:
//  LXI_HL : Intialize address value in H:L.
//  MOV_HL : Move the data at address H:L, to register A.
//  INX_HL : Increment the address H:L.
//  HLT    : Halt the program.
//  JMP    : Jump back to the move, and loop.
byte lxiNopLoopProgram[] = {
  //                         Start program.
  0000,                   // NOP
  0041, 14, 0000,         // LXI_HL lb hb. Load 0000:14 into register H:L.
  //
  0176,                   // MOV M:address(H:L):data > register A
  0000,                   // NOP
  0043,                   // INX > Increment H:L
  0000,                   // NOP
  0166,                   // HLT
  0000,                   // NOP
  0303, 0006, 0000,       // JMP to 6, jump to the MOV operation.
  //                         Data
  0000, 0101, 0110,       // 3 4 5
  0111, 0000, 0000,       // 6 7 8
  //
  0000, 0000, 0000  //       end
};
// byte NopLxiMovInxHltJmpCpiProgram[] = {
byte theProgram[] = {
  // ------------------------------------------------------------------
  // Initialize memory read location to memory start.
  // Move the first data byte to the accumulator, which is register A.
  // While not hit the end character (0111)
  //    Increment the memory pointer (H:L).
  //    Move the next data byte to the accumulator.
  //    Halt the process.
  //    Restart the while loop.
  // End while.
  // Restart the program from memory initialization.
  //
  //                Start program.
  0000,             // NOP
  0041, 24, 0000,   // LXI_HL lb hb. Load hb:lb into registers H(hb):L(lb).
  0176,             // MOV M:address(H:L):data > register A
  //
  //                While address:data != 0111
  0376, 0111,             // CPI Compare A with 0111.
  0312, 19, 0000,         // JZ lb hb. If it matches, jump to lb hb (end while)
  0000,                   // NOP
  0166,                   // HLT
  0043,                   // INX > Increment H:L
  0176,                   // MOV M:address(H:L):data > register A
  0000,                   // NOP
  0303, 0005, 0000,       // JMP to the start of the while loop.
  //                End while.
  0000,             // NOP
  0166,             // HLT
  0000,             // NOP
  0303, 1, 0000,    // JMP to 1. Restart: jump to program start.
  // ------------------------------------------------------------------
  //                Data, starts at address 24.
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
  Serial.println("+ Initialize all memory bytes to zero.");
  for (int i = 0; i < memoryBytes; i++) {
    memoryData[i] = zeroByte;
  }
}

void copyByteArrayToMemory(byte btyeArray[], int arraySize) {
  Serial.println("+ Copy the program into the computer's memory array.");
  for (int i = 0; i < arraySize; i++) {
    memoryData[i] = btyeArray[i];
  }
  Serial.println("+ Copied.");
}

void listByteArray(byte btyeArray[], int arraySize) {
  Serial.println("+ List the program.");
  Serial.println("++   Address:       Data value");
  for (int i = 0; i < arraySize; i++) {
    Serial.print("+ Addr: ");
    sprintf(charBuffer, "%4d:", i);
    Serial.print(charBuffer);
    Serial.print(" Data: ");
    printData(btyeArray[i]);
    Serial.println("");
  }
  Serial.println("+ End of listing.");
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
// Front Panel toggle/button switch events

const int STOP_BUTTON_PIN = 4;        // Nano pin D4
const int RUN_BUTTON_PIN = 5;
const int STEP_BUTTON_PIN = 6;
const int EXAMINE_BUTTON_PIN = 7;     // Display the address and data that is next to be processed.
const int EXAMINENEXT_BUTTON_PIN = 8; // Increment the program counter and display the address and data

boolean runProgram = false;

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.
boolean stopButtonState = true;
boolean runButtonState = true;
boolean stepButtonState = true;
boolean examineButtonState = true;
boolean examineNextButtonState = true;

boolean buttonWentHigh = false;

void checkStopButton() {
  if (digitalRead(STOP_BUTTON_PIN) == HIGH) {
    if (!stopButtonState) {
      Serial.println("+ Stop process.");
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      stopButtonState = false;
      buttonWentHigh = true;
    }
    stopButtonState = true;
  } else {
    if (stopButtonState) {
      stopButtonState = false;
    }
  }
}

void checkRunButton() {
  if (digitalRead(RUN_BUTTON_PIN) == HIGH) {
    if (!runButtonState) {
      Serial.println("+ Run process.");
      runProgram = true;
      digitalWrite(WAIT_PIN, LOW);
      runButtonState = false;
      buttonWentHigh = true;
    }
    runButtonState = true;
  } else {
    if (runButtonState) {
      runButtonState = false;
    }
  }
}

void checkStepButton() {
  // If the button is pressed (circuit closed), the button status is HIGH.
  if (digitalRead(STEP_BUTTON_PIN) == HIGH) {
    if (!stepButtonState) {
      // digitalWrite(STEP_BUTTON_PIN, HIGH);
      // Serial.println("+ checkButton(), turn LED on.");
      processData();
      stepButtonState = false;
      buttonWentHigh = true;
    }
    stepButtonState = true;
  } else {
    if (stepButtonState) {
      // digitalWrite(STEP_BUTTON_PIN, LOW);
      // Serial.println("+ checkButton(), turn LED off.");
      stepButtonState = false;
    }
  }
}

void checkExamineButton() {
  // If the button is pressed (circuit closed), the button status is HIGH.
  if (digitalRead(EXAMINE_BUTTON_PIN) == HIGH) {
    if (!examineButtonState) {
      // Serial.println("+ Examine switch clicked on.");
      Serial.print("? ");
      printAddressData();
      Serial.println("");
      // processData();
      examineButtonState = false;
      buttonWentHigh = true;
    }
    examineButtonState = true;
  } else {
    if (examineButtonState) {
      examineButtonState = false;
    }
  }
}

void checkExamineNextButton() {
  // If the button is pressed (circuit closed), the button status is HIGH.
  if (digitalRead(EXAMINENEXT_BUTTON_PIN) == HIGH) {
    if (!examineNextButtonState) {
      // Serial.println("+ Examine Next switch clicked on.");
      programCounter++;
      Serial.print("? ");
      printAddressData();
      Serial.println("");
      examineNextButtonState = false;
      buttonWentHigh = true;
    }
    examineNextButtonState = true;
  } else {
    if (examineNextButtonState) {
      examineNextButtonState = false;
    }
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Instruction Set, Opcodes, Registers

// This section is base on section 26: 8080 Instruction Set
//    https://www.altairduino.com/wp-content/uploads/2017/10/Documentation.pdf

// -----------------------------------------------------------------------------
// Processing opcodes and opcode cycles

byte opcode = 0;            // Opcode being processed
int instructionCycle = 0;   // Opcode process cycle

void printAddressData() {
  Serial.print("Addr: ");
  sprintf(charBuffer, "%4d:", programCounter);
  Serial.print(charBuffer);
  Serial.print(" Data: ");
  printData(memoryData[programCounter]);
}

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
const byte MVI =    0026; // MVI D,#   00DDD110 db       -       Move immediate to register*
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
const byte DAD_BC = 0011; // DAD         00001001          C    Add B:C to H:L. Set carry bit.
//                           DAD  RP     00RP1001          C    Add register pair to HL (16 bit add)
//                           Set carry bit if the addition causes a carry out.
const byte JNC =    0322; // JNC  lb hb  11010010               Jump if carry bit is 0 (false).

// -------------------
// Process flags and values.
boolean compareResult = true; // Set by CPI. Used by JZ.
boolean carryBit = false;     // Set by DAD. Used JNC.
boolean halted = false;       // Set true for an HLT opcode.

void processData() {
  if (opcode == 0) {
    Serial.print("> ");
    printAddressData();
    digitalWrite(M1_PIN, HIGH); // Machine cycle 1, get an opcode.
    processOpcode();
    programCounter++;
    instructionCycle = 0;
  } else {
    Serial.print("+ ");
    printAddressData();
    digitalWrite(M1_PIN, LOW); // Machine cycle 1+x, getting opcode data.
    processOpcodeData();
  }
  Serial.println("");
}

void processOpcode() {
  int aValue = 0;       // For calculating if there is a carry over.
  int anAddress = 0;
  byte dataByte = memoryData[programCounter];
  switch (dataByte) {
    case CPI:
      opcode = CPI;
      Serial.print(" > CPI, compare next data byte to A. Store true or false into compareResult.");
      break;
    case DAD_BC:
      Serial.print(" > DAD_BC, Add B:C to H:L. Set carry bit. ");
      aValue = (int)word(regB, regC) + (int)word(regH, regL);
      // 2^16 = 65535 = 64k = 11 111 111 : 11 111 111
      if (aValue > 65535) {
        carryBit = true;
      } else {
        carryBit = false;
      }
      Serial.print(", sum = ");
      Serial.print(aValue);
      break;
    case JNC:
      opcode = JNC;
      Serial.print(" > JNC, Jump if carry bit is 0 (false), not set.");
      break;
    case JZ:
      opcode = JZ;
      Serial.print(" > JZ, if compareResult, jump to the following address (lh hb).");
      break;
    case JMP:
      opcode = JMP;
      Serial.print(" > JMP, get address low and high order bytes.");
      break;
    case LXI_HL:
      opcode = LXI_HL;
      Serial.print(" > LXI, move lb hb address into register pair H(hb):L(lb).");
      break;
    case INX_HL:
      regL++;       // Update this later because need to update the pair.
      Serial.print(" > INX, increment the 16 bit register pair H:L.");
      Serial.print(", regL = ");
      Serial.print(regL);
      // printOctal(regL);
      break;
    case MOV_AM:
      Serial.print(" > MOV");
      anAddress = word(regH, regL);
      regA = memoryData[anAddress];
      Serial.print(" data from Addr: ");
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(", to Accumulator = ");
      printOctal(regA);
      Serial.print(" = ");
      Serial.print(regA);
      break;
    case HLT:
      Serial.print(" > HLT, halt the processor.");
      runProgram = false;
      halted = true;
      // digitalWrite(HLTA_PIN, HIGH);
      // Set all the address and data lights on.
      digitalWrite(WAIT_PIN, HIGH);
      break;
    case NOP:
      Serial.print(" > NOP ---");
      // delay(100);
      break;
    default:
      Serial.print(" - Error, unknown instruction.");
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
      Serial.print(" > CPI, compareResult db: ");
      Serial.print(dataByte);
      if (compareResult) {
        Serial.print(" == ");
      } else {
        Serial.print(" != ");
      }
      Serial.print(" A: ");
      Serial.print(regA);
      programCounter++;
      break;
    case JNC:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
        Serial.print(" < JNC, lb: ");
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
      Serial.print(" < JNC, hb: ");
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
      if (!carryBit) {
        Serial.print(" > JNC, jump to:");
        programCounter = word(highOrder, lowOrder);
        Serial.print(programCounter);
      } else {
        Serial.print(" - carryBit is true, don't jump.");
        programCounter++;
      }
      break;
    case JZ:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
        Serial.print(" < JZ, lb: ");
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
      Serial.print(" < JZ, hb: ");
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
      if (compareResult) {
        Serial.print(" > JZ, jump to:");
        programCounter = word(highOrder, lowOrder);
        Serial.print(programCounter);
      } else {
        Serial.print(" - compareResult is false, don't jump.");
        programCounter++;
      }
      break;
    case JMP:
      if (instructionCycle == 1) {
        lowOrder = programCounter;
        Serial.print(" > JMP, lb: ");
        Serial.print(lowOrder);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      programCounter = word(memoryData[programCounter], memoryData[lowOrder]);
      Serial.print(" > JMP, jump to:");
      sprintf(charBuffer, "%4d = ", programCounter);
      Serial.print(charBuffer);
      printByte((byte)programCounter);
      break;
    case LXI_HL:
      if (instructionCycle == 1) {
        regL = memoryData[programCounter];
        Serial.print(" < LXI, lb: ");
        sprintf(charBuffer, "%4d:", regL);
        Serial.print(charBuffer);
        programCounter++;
        return;
      }
      // instructionCycle == 2
      programCounter;
      regH = memoryData[programCounter];
      Serial.print(" < LXI, hb: ");
      sprintf(charBuffer, "%4d:", regH);
      Serial.print(charBuffer);
      Serial.print(" > H:L address: ");
      sprintf(charBuffer, "%8d:", word(regH, regL));
      Serial.print(charBuffer);
      printOctal(regH);
      Serial.print(" = ");
      printOctal(regL);
      programCounter++;
      break;
    default:
      Serial.print(" - Error, unknow instruction.");
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
  }
  // The opcode cycles are complete.
  opcode = 0;
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  pinMode(STOP_BUTTON_PIN, INPUT);
  pinMode(RUN_BUTTON_PIN, INPUT);
  pinMode(STEP_BUTTON_PIN, INPUT);
  pinMode(EXAMINE_BUTTON_PIN, INPUT);
  pinMode(EXAMINENEXT_BUTTON_PIN, INPUT);
  // pinMode(RESET_BUTTON_PIN, INPUT);
  Serial.println("+ Toggle/button switches are configured for input.");

  pinMode(WAIT_PIN, OUTPUT);
  digitalWrite(WAIT_PIN, HIGH);
  pinMode(M1_PIN, OUTPUT);
  digitalWrite(M1_PIN, HIGH);
  // digitalWrite(MEMR_PIN, HIGH);
  // digitalWrite(HLTA_PIN, LOW);
  // digitalWrite(WO_PIN, HIGH);
  Serial.println("+ LED lights configured and initialized.");

  // List a program. Available programs:
  //    jumpLoopProgram
  //    jumpLoopNopProgram
  //    jumpHaltLoopProgram
  //    lxiNopLoopProgram
  int programSize = sizeof(theProgram);
  listByteArray(theProgram, programSize);
  // Load a program.
  copyByteArrayToMemory(theProgram, programSize);

  Serial.println("+++ Start the processor loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing each byte of machine code.

static unsigned long timer = millis();
void loop() {

  if (runProgram) {
    // When testing, can add a cycle delay.
    // Clock process timing is controlled by the timer.
    // Example, 50000 : once every 1/2 second.
    // if (millis() - timer >= 500) {
      processData();
      // timer = millis();
    // }
    checkStopButton();
  } else {
    if (buttonWentHigh && halted) {
      halted = false;
      buttonWentHigh = false;
      // digitalWrite(HLTA_PIN, LOW); // Not halted anymore.
    }
    checkRunButton();
    checkStepButton();
    checkExamineButton();
    checkExamineNextButton();
    delay(60);
  }

}
// -----------------------------------------------------------------------------
