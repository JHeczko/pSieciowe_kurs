import java.net.*;
import java.nio.charset.StandardCharsets;
import java.io.*;
import java.lang.Thread;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Properties;
import java.util.Scanner;

public class Tester{
    public static final String ANSI_RESET = "\u001B[0m"; 
    public static final String ANSI_RED = "\u001B[31m";
    public static final String ANSI_GREEN = "\u001B[32m";
      
    public static void test3()throws IOException, InterruptedException{
        Socket testSock = new Socket("127.0.0.1", 2020);
        BufferedOutputStream out = new BufferedOutputStream(testSock.getOutputStream());
        BufferedInputStream in = new BufferedInputStream(testSock.getInputStream());

        out.write("tot".getBytes(StandardCharsets.US_ASCII));
        out.flush();
        Thread.sleep(100);
        in.readNBytes(in.available());
        out.write("\r\npies".getBytes(StandardCharsets.US_ASCII));
        out.flush();
        Thread.sleep(100);
        while(in.available() == 0){
        }
        in.readNBytes(in.available());
        out.write("\r\ntot".getBytes(StandardCharsets.US_ASCII));
        out.flush();
        Thread.sleep(100);
        while(in.available() == 0){
        }
        in.readNBytes(in.available());
        out.write("\r\npies".getBytes(StandardCharsets.US_ASCII));
        out.flush();
        Thread.sleep(100);
        while(in.available() == 0){
        }
        in.readNBytes(in.available());
        out.write("\r\ntot".getBytes(StandardCharsets.US_ASCII));
        out.flush();
        Thread.sleep(100);
        while(in.available() == 0){
        }
        in.readNBytes(in.available());
        out.write("\r\npies".getBytes(StandardCharsets.US_ASCII));
        out.flush();
        Thread.sleep(100);
        while(in.available() == 0){
        }
        in.readNBytes(in.available());
        out.write("\r\ntot".getBytes(StandardCharsets.US_ASCII));
        out.flush();
        Thread.sleep(100);
        while(in.available() == 0){
        }
        in.readNBytes(in.available());
        out.write("\r\npies".getBytes(StandardCharsets.US_ASCII));
        out.flush();

        while(in.available() == 0){
        }

        String answearString = new String(in.readNBytes(in.available()));
        String answearCompareString = "1/1\r\n";

        answearString = answearString.replaceAll("\r\n","/r/n");
        answearCompareString = answearCompareString.replaceAll("\r\n","/r/n");
        
        System.out.println("\tTest: \t" + "krotki" + "\n\tExcepted: " + answearCompareString.toString() + "\n\tGot: \t" + answearString.toString());
        if(answearString.equals(answearCompareString)){
            System.out.println(ANSI_GREEN + "\tPASS" + ANSI_RESET);
            System.out.println("-------------------");
        }else{
            System.out.println(ANSI_RED + "\tFAILED" + ANSI_RESET);
            System.out.println("-------------------");
        }

        in.close();
        out.close();
        testSock.close();
    }

    public static void test2()throws IOException, InterruptedException{
        Socket testSock = new Socket("127.0.0.1", 2020);
        BufferedOutputStream out = new BufferedOutputStream(testSock.getOutputStream());
        BufferedInputStream in = new BufferedInputStream(testSock.getInputStream());

        byte[] alfabet = new byte[25];
        byte letter = 'a';
        for (int  i = 0; i < 25 ; i++) {
            alfabet[i] = letter;
            letter++;
        }

        for(int i = 0; i< 25; i++){
            out.write(alfabet[i]);
            out.write(' ');
            out.flush();
            Thread.sleep(10);
        }
        out.write("z\r\n".getBytes(StandardCharsets.US_ASCII));
        out.flush();

        while(in.available() == 0){
        }

        String answearString = new String(in.readNBytes(in.available()));
        String answearCompareString = "26/26\r\n";

        answearString = answearString.replaceAll("\r","/r");
        answearCompareString = answearCompareString.replaceAll("\r","/r");

        answearString = answearString.replaceAll("\n","/n");
        answearCompareString = answearCompareString.replaceAll("\n","/n");
        
        System.out.println("\tTest: \t" + "Dlugi" + "\n\tExcepted: " + answearCompareString.toString() + "\n\tGot: \t" + answearString.toString());
        if(answearString.equals(answearCompareString)){
            System.out.println(ANSI_GREEN + "\tPASS" + ANSI_RESET);
            System.out.println("-------------------");
        }else{
            System.out.println(ANSI_RED + "\tFAILED" + ANSI_RESET);
            System.out.println("-------------------");
        }

        in.close();
        out.close();
        testSock.close();
    }

    public static void test1(Properties properties) throws IOException, InterruptedException{
        for(Object test : properties.keySet()){
            Socket testSock = new Socket("127.0.0.1", 2020);
            BufferedOutputStream out = new BufferedOutputStream(testSock.getOutputStream());
            BufferedInputStream in = new BufferedInputStream(testSock.getInputStream());
            String output = (String)test;
            String answearCompareString = (String)properties.getProperty(output);

            out.write(output.getBytes(StandardCharsets.US_ASCII));
            out.flush();
            
            while(in.available() == 0){

            }
            Thread.sleep(100);
            String answearString = new String(in.readNBytes(in.available()));

            answearString = answearString.replaceAll("\r","/r");
            output = output.replaceAll("\r","/r");
            answearCompareString = answearCompareString.replaceAll("\r","/r");

            answearString = answearString.replaceAll("\n","/n");
            output = output.replaceAll("\n","/n");
            answearCompareString = answearCompareString.replaceAll("\n","/n");

            System.out.println("\tTest: \t" + output.toString() + "\n\tExcepted: " + answearCompareString.toString() + "\n\tGot: \t" + answearString.toString());
            if(answearString.equals(answearCompareString)){
                System.out.println(ANSI_GREEN + "\tPASS" + ANSI_RESET);
                System.out.println("-------------------");
            }else{
                System.out.println(ANSI_RED + "\tFAILED" + ANSI_RESET);
                System.out.println("-------------------");
            }
            in.close();
            out.close();
            testSock.close(); 
        }
    }
    public static void main(String[] args){
        try{
            Properties testy = new Properties();
            testy.setProperty("Koty ali sa dwa wiec dzialaj gupi pieronie bo inaczej lipa bedzie\r\n", "0/12\r\n");
            testy.setProperty("Ala i kot\r\n", "2/3\r\n");
            testy.setProperty("xyz\r\nucho oko\r\n", "0/1\r\n1/2\r\n");
            testy.setProperty("ABBA 1972\r\n", "ERROR\r\n");
            testy.setProperty("b\0c3\\xb3b i fasola\r\n", "ERROR\r\n");
            testy.setProperty("oraz\0zero\r\n", "ERROR\r\n");
            testy.setProperty("Sam koniec\r", "ERROR\r\n");
            testy.setProperty("Literalnie\tLosowelitery\r\n", "ERROR\r\n");

            test1(testy);
            test2();
            test3();
            test2();
            test3();
        }catch(Exception e){
            e.printStackTrace();
        }
    }
}