// -----------------------------------------------------------------------------

#define MEMSIZE (256)
byte Mem[MEMSIZE];
#define MWRITE(a,v) { Mem[a]=v; }
#define MREAD(a)    (Mem[a])

// -----------------------------------------------------------------------------
char charBuffer[17];

void printByte(byte b) {
  for (int i = 7; i >= 0; i--) {
    //  Serial_print(bitRead(b, i));
    if (bitRead(b, i)) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
  }
}
void printOctal(byte b) {
  String sValue = String(b, OCT);
  for (int i = 1; i <= 3 - sValue.length(); i++) {
    Serial.print(F("0"));
  }
  Serial.print(sValue);
}
void printHex(byte b) {
  String sValue = String(b, HEX);
  for (int i = 1; i <= 3 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}

// -----------------------------------------------------------------------------

const byte killbits[] = {
  0x21, 0x00, 0x00, 0x16, 0x80, 0x01, 0x0E, 0x00, 0x1A, 0x1A, 0x1A, 0x1A, 0x09, 0xD2, 0x08, 0x00,
  0xDB, 0xFF, 0xAA, 0x0F, 0x57, 0xC3, 0x08, 0x00
};
void loadKtb() {
  int theSize = sizeof(killbits);
  for (int i = 0; i < theSize; i++ ) {
    byte theValue = killbits[i];
    Serial.print(F("+ killbits["));
    if (i < 10) {
      Serial.print(F("0"));
    }
    MWRITE(i, killbits[i]);
    Serial.print(i);
    Serial.print(F("] = D:"));
    if (theValue < 100) {
      Serial.print(F("0"));
    }
    if (theValue < 10) {
      Serial.print(F("0"));
    }
    Serial.print(theValue);
    Serial.print(F(" H:"));
    printHex(theValue);
    Serial.print(F(":B:"));
    printByte(theValue);
    Serial.print(F(":O:"));
    printOctal(theValue);
    Serial.print(F(":"));
    ;
    theValue = MREAD(i);
    if (theValue < 100) {
      Serial.print(F("0"));
    }
    if (theValue < 10) {
      Serial.print(F("0"));
    }
    Serial.print(theValue, DEC);
    Serial.println();
  }
  /*
    //
    Serial.println(F("+ Load program Hex codes."));
    int theSize = sizeof(killbits);
    for (int i = 0; i < theSize; i++ ) {
    MWRITE(i, killbits[i]);
    // Print Binary:Octal:Decimal values.
    Serial.print("D:");
    Serial.print(killbits[i]);
    Serial.print(" H:");
    printHex(MREAD(i));
    Serial.print(":B:");
    printByte(MREAD(i));
    Serial.print(":O:");
    printOctal(MREAD(i));
    Serial.print(F(":"));
    Serial.println(MREAD(i), DEC);
    }
  */
}

// -----------------------------------------------------------------------------
void setup() {
  // Speed for serial read, which matches the sending program.
  Serial.begin(115200);
  delay(1000);
  Serial.println("");
  Serial.println("+++ Setup.");
  loadKtb();
  Serial.println("+++ Go to loop.");
}
// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  delay(30); // Arduino sample code, doesn't use a delay.
}
// -----------------------------------------------------------------------------
