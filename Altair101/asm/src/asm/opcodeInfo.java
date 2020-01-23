package asm;

import java.util.Arrays;
import java.util.Comparator;

public class opcodeInfo {

    int rollno;
    String name, address;

    // Constructor 
    public opcodeInfo(int rollno, String name, String address) {
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

class Sortbyroll implements Comparator<opcodeInfo> {
    public int compare(opcodeInfo a, opcodeInfo b) {
        return a.rollno - b.rollno;
    }
}

class Sortbyname implements Comparator<opcodeInfo> {
    public int compare(opcodeInfo a, opcodeInfo b) {
        return a.name.compareTo(b.name);
    }
}

class Main {

    public static void main(String[] args) {
        opcodeInfo[] arr = {
            new opcodeInfo(111, "bbbb", "london"),
            new opcodeInfo(131, "aaaa", "nyc"),
            new opcodeInfo(121, "cccc", "jaipur")
        };

        System.out.println("Unsorted");
        for (int i = 0; i < arr.length; i++) {
            System.out.println(arr[i]);
        }
        Arrays.sort(arr, new Sortbyroll());
        System.out.println("\nSorted by rollno");
        for (int i = 0; i < arr.length; i++) {
            System.out.println(arr[i]);
        }
        Arrays.sort(arr, new Sortbyname());
        System.out.println("\nSorted by name");
        for (int i = 0; i < arr.length; i++) {
            System.out.println(arr[i]);
        }
    }
}
