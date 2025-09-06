import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.IOException;
import java.net.*;

class SocketRunnable implements Runnable{
    Socket clientSocket;
    SocketRunnable(Socket clientSocket){
        this.clientSocket = clientSocket;
    }

    public void run(){
        try{
            String buf = "Hello im server";
            BufferedOutputStream writeStream = new BufferedOutputStream(clientSocket.getOutputStream());
            writeStream.write(buf.getBytes());
            writeStream.flush();
            writeStream.close();
            clientSocket.close();
        }catch(Exception e){
            e.printStackTrace();
        }
    } 
}

public class serverTCP{
    public static void main(String[] args){
    try{

        ServerSocket serverSocket = new ServerSocket(20300);
        
        while(true){
          Socket clientSocket = serverSocket.accept();
          if(clientSocket.isConnected()){
            new Thread(new SocketRunnable(clientSocket)).start();
          }             
        }}catch (Exception e){
            e.printStackTrace();   

        }
    }
}