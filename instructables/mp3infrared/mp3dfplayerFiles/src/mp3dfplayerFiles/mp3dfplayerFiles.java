package mp3dfplayerFiles;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class mp3dfplayerFiles {

    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

    public static void main(String[] args) {
        System.out.println("fileio.main");
        mp3dfplayerFiles dataioObj = new mp3dfplayerFiles();
        dataioObj.run();
    }

    public void run() {
        String line = "";
        while (!(line.equals("q"))) {
            System.out.print("fileio.run (q to exit) > ");
            try {
                line = this.br.readLine();
            } catch (Exception e) {
                e.printStackTrace();
            }
            System.out.println("fileio.run echo > "+ line);
        }
        System.out.println("fileio.exit");
    }
}
