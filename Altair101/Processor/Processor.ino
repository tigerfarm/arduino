// -----------------------------------------------------------------------------
/*
  Altair 101 software microprocessor

    Definitions Memory and Memory Management.
    Control Front Panel LED lights.
    Process Front Panel toggle events.
    Instruction Set, Opcodes, Registers
*/
// -----------------------------------------------------------------------------
// Memory definitions

int memoryBytes = 512;
byte memoryData[512];

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
// + Status LED MI stays off.
// + Flip single step,
// ++ Program counter is to that address.
// ++ The new address and data values are LED displayed.
// ++ Status LED MI goes on.
// Note, the simulator works like my code, which is different than the Altair in the video.

// Define a jump loop program with NOP instructions.
byte jumpLoopNopProgram[] = {
  0303, 0004, 0000,
  0000, 0000, 0000,
  0303, 0000, 0000
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

// -----------------------------------------------------------------------------
// Output memory address and data valules.

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
  // Serial.print(memoryData[i], BIN);
  // Serial.print(":");
  //
  // printOctal(theByte);
  // Serial.print(memoryData[i], OCT);
  // Serial.print(" : ");
  //
  // Serial.println(memoryData[i], DEC);
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
  Serial.println("++ Address: Data value");
  for (int i = 0; i < arraySize; i++) {
    Serial.print("++ ");
    sprintf(charBuffer, "%7d", i);
    Serial.print(charBuffer);
    Serial.print(": ");
    printData(btyeArray[i]);
    Serial.println("");
  }
  Serial.println("+ End of listing.");
}

// -----------------------------------------------------------------------------
// Front Panel LEDs

const int INTE_PIN = 42;    // On, interrupts enabled.
const int PROT_PIN = 42;    // Useful only if RAM has page protection impliemented.
// Status LEDs
const int MEMR_PIN = 42;    // Memory read such as fetching an op code (data instruction)
const int INP_PIN = 42;     // Input
const int MI_PIN = 42;      // On, doing an op code fetch: Machine cycle 1.
const int OUT_PIN = 42;     // Write output.
const int HLTA_PIN = 42;    // Halt acknowledge, halt instruction executed.
const int STACK_PIN = 42;   // On, reading or writing to the stack.
const int WO_PIN = 42;      // Write Output uses inverse logic. On, not writing output.
const int INT_PIN = 42;     // On when executing an interrupt step.
//
const int WAIT_PIN = A2;    // On, program not running. Off, programrunning.
const int HLDA_PIN = 42;    // 8080 processor go into a hold state because of other hardware.

// Video demonstrating status lights:
//    https://www.youtube.com/watch?v=3_73NwB6toY
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
// Front Panel Control Buttons

const int STOP_BUTTON_PIN = 4;   // Nano D4
const int RUN_BUTTON_PIN = 5;
const int STEP_BUTTON_PIN = 6;
const int EXAMINE_BUTTON_PIN = 7;
const int EXAMINENEXT_BUTTON_PIN = 8;

boolean runProgram = false;

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.
boolean stopButtonState = true;
boolean runButtonState = true;
boolean stepButtonState = true;

void checkStopButton() {
  if (digitalRead(STOP_BUTTON_PIN) == HIGH) {
    if (!stopButtonState) {
      Serial.println("+ Stop process.");
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      stopButtonState = false;
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

// -----------------------------------------------------------------------------
// Instruction Set, Opcodes, Registers

// This section is base on section 26: 8080 Instruction Set
//    https://www.altairduino.com/wp-content/uploads/2017/10/Documentation.pdf

/*
  Destination and Source register fields:
    111=A   (Accumulator)
    000=B
    001=C
    010=D
    011=E
    100=H
    101=L
    110=M   (Memory reference through address in H:L)

  Register pair 'RP' fields:
    00=BC   (B:C as 16 bit register)
    01=DE   (D:E as 16 bit register)
    10=HL   (H:L as 16 bit register)
    11=SP   (Stack pointer, refers to PSW (FLAGS:A) for PUSH/POP)

  Instruction parameters:
    db = Data byte (8 bit)
    lb = Low byte of 16 bit value
    hb = High byte of 16 bit value
    pa = Port address (8 bit)
    p  = 8 bit port address
*/

//   Destination and Source register fields.
int regA = 0;   // 111=A   (Accumulator)
int regB = 0;   // 000=B
int regC = 0;   // 001=C
int regD = 0;   // 010=D
int regE = 0;   // 011=E
int regH = 0;   // 100=H
int regL = 0;   // 101=L
int regM = 0;   // 110=M   (Memory reference through address in H:L)

// Octals stored as a bytes.
//
//                               Inst      Encoding          Flags   Description
// Programmed and tested:
const byte HLT = 0166;        // HLT       01110110          -       Halt processor
const byte JMP = 0303;        // JMP a     11000011 lb hb    -       Unconditional jump*
const byte NOP = 0000;        // NOP       00000000          -       No operation
//
// To be programmed:
const byte IN =  B11011011;   // IN p      11011011 pa       -       Read input port into A
const byte STA = B00110010;   // STA a     00110010 lb hb    -       Store A to memory
//
// More program instructions for the Kill the Bit program,
// DAD RP    00RP1001          C       Add register pair to HL (16 bit add)*
// JNC ?
// LDAX RP   00RP1010 *1       -       Load indirect through BC or DE
//                    *1 = Only RP=00(BC) and 01(DE) are allowed for LDAX/STAX
// LXI RP,#  00RP0001 lb hb    -       Load register pair immediate*
// MOV D,S   01DDDSSS          -       Move register to register*
// MVI D,#   00DDD110 db       -       Move immediate to register*
// RRC       00001111          C       Rotate A right
// XRA S     10101SSS          ZSPCA   Exclusive OR register with A

int programCounter = 0;
byte opcode = 0;
int instructionCycle = 0;
int lowOrder = 0;
int highOrder = 0;

void printAddressData() {
  Serial.print("+ Addr: ");
  sprintf(charBuffer, "%4d:", programCounter);
  Serial.print(charBuffer);
  Serial.print(" Data: ");
  printData(memoryData[programCounter]);
}

void processData() {
  printAddressData();
  byte dataByte = memoryData[programCounter];
  if (opcode == JMP) {
    if (instructionCycle == 0) {
      instructionCycle++;
      lowOrder = programCounter++;
      Serial.println("");
      return;
    }
    programCounter = word(memoryData[programCounter], memoryData[lowOrder]);
    Serial.print(" > JMP, jump to:");
    sprintf(charBuffer, "%4d:", programCounter);
    Serial.print(charBuffer);
    printByte((byte)programCounter);
    //
    opcode = 0;
    Serial.println("");
    return;
  }
  switch (dataByte) {
    case HLT:
      Serial.print(" > HLT Instruction, Halt the processor.");
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
      break;
    case JMP:
      opcode = JMP;
      instructionCycle = 0;
      Serial.print(" > JMP opcode, get address low and high order bytes.");
      break;
    case NOP:
      Serial.print(" > NOP Instruction, No operation.");
      // Can add a delay, for example, to wait for an interrupt proccess.
      delay(10);
      break;
    default:
      Serial.print(" - Error, unknow instruction.");
      runProgram = false;
      digitalWrite(WAIT_PIN, HIGH);
  }
  Serial.println("");
  programCounter++;
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  pinMode(WAIT_PIN, OUTPUT);
  digitalWrite(WAIT_PIN, HIGH);
  Serial.println("+ LEDs configured for output.");

  // List a program. Available programs:
  //    jumpLoopProgram
  //    jumpLoopNopProgram
  //    jumpHaltLoopProgram
  int programSize = sizeof(jumpHaltLoopProgram);
  listByteArray(jumpHaltLoopProgram, programSize);
  // Load a program.
  copyByteArrayToMemory(jumpHaltLoopProgram, programSize);

  Serial.println("+++ Start the processor loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing machine code.

static unsigned long timer = millis();
void loop() {

  if (runProgram) {
    // Execute an instruction controlled by the timer.
    // Example, 3000 : once every 3 seconds.
    if (millis() - timer >= 500) {
      processData();
      timer = millis();
    }
    checkStopButton();
  } else {
    checkRunButton();
    checkStepButton();
  }

  delay(60);
}
// -----------------------------------------------------------------------------
