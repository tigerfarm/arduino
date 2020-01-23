package asm;

import java.util.Arrays;
import java.util.Comparator;

public class opcodeInfoSample {

    int rollno;
    String name, address;

    // Constructor 
    public opcodeInfoSample(int rollno, String name, String address) {
        this.rollno = rollno;
        this.name = name;
        this.address = address;
    }

    // Used to print student details in main() 
    public String toString() {
        return this.rollno + " " + this.name
                + " " + this.address;
    }
}

class Sortbyroll1 implements Comparator<opcodeInfoSample> {

    // Used for sorting in ascending order of 
    // roll number 
    public int compare(opcodeInfoSample a, opcodeInfoSample b) {
        return a.rollno - b.rollno;
    }
}

class Main1 {

    public static void main(String[] args) {
        opcodeInfoSample[] arr = {
            new opcodeInfoSample(111, "bbbb", "london"),
            new opcodeInfoSample(131, "aaaa", "nyc"),
            new opcodeInfoSample(121, "cccc", "jaipur")
        };

        System.out.println("Unsorted");
        for (int i = 0; i < arr.length; i++) {
            System.out.println(arr[i]);
        }

        Arrays.sort(arr, new Sortbyroll1());

        System.out.println("\nSorted by rollno");
        for (int i = 0; i < arr.length; i++) {
            System.out.println(arr[i]);
        }
    }
}
