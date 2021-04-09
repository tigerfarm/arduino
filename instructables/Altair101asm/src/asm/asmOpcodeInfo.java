/*
    Retrieve and list opcode data from a text file.
    Can sort and select data before printing.

 */
package asm;
// -----------------------------------------------------------------------------
// Opcode data objects.

class asmOpcodeData {

    String valueBinary;
    String code;
    String parameters;
    String parametersNum;

    // Constructor 
    public asmOpcodeData(String valueBinary, String code, String parameters, String parametersNum) {
        // ++ 006 00000110 MVI   mvib  b,d8   1           :B <- byte 2
        this.valueBinary = valueBinary;         // 00000110
        this.code = code;                       // MVI
        this.parameters = parameters;           // b,d8
        this.parametersNum = parametersNum;     // 1
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Opcode processing.
public class asmOpcodeInfo {

    static int opcodeCount = 255;
    static asmOpcodeData[] opcodeArray = new asmOpcodeData[opcodeCount];

    public String getOpcode(String theName) {
        // Given an opcode, return the byte code.
        String returnValue = "";
        for (int i = 0; i < opcodeCount; i++) {
            // System.out.println("++ " + i + ": " + name[i] + " " + value[i]);
            if (opcodeArray[i].valueBinary.equals(theName)) {
                // returnValue = (byte) Integer.parseInt(opcodeArray[i].value, 2);
                returnValue = opcodeArray[i].code;
                break;
            }
        }
        return returnValue;
    }

    public void opcodesList() {
        System.out.println("+ asmOpcodeInfo list.");
        System.out.println("");
        for (int i = 0; i < opcodeCount; i++) {
            System.out.println("++ " + i + ": " + opcodeArray[i].valueBinary + " " + opcodeArray[i].code);
        }
    }

    // -------------------------------------------------------------------------
    public static void main(String[] args) {
        System.out.println("+++ Start.");
        // asmOpcodeData
        asmOpcodeInfo theOpcodeInfo = new asmOpcodeInfo();
        opcodeCount = 0;
        opcodeArray[opcodeCount++] = new asmOpcodeData("00000110", "MVI", "b,d8", "1");
        opcodeArray[opcodeCount++] = new asmOpcodeData("00000001", "MOV", "b", "0");
        opcodeArray[opcodeCount++] = new asmOpcodeData("00000010", "MCTV", "v,d16", "1");
        System.out.println("\n-----------------------------------------------");
        System.out.println("+ Test retrieve methods.\n");
        //
        System.out.println("\n+++ Exit.\n");
    }
}
