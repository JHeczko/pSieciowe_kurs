import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.*;

public class clientTCP {
    public static void main(String[] args) throws IOException{
        Socket socket = new Socket("localhost", 20300);
        BufferedInputStream input = new BufferedInputStream(socket.getInputStream());
        System.out.println(input.read());

    }
}
