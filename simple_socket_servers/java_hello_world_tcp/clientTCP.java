import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.*;
import java.nio.charset.StandardCharsets;

public class clientTCP {
    public static void main(String[] args) throws IOException{
        Socket socket = new Socket("127.0.0.1", 20300);
        BufferedInputStream input = new BufferedInputStream(socket.getInputStream());
        byte[] cos = input.readAllBytes();
        String answeaString = new String(cos,StandardCharsets.UTF_8);
        System.out.println(answeaString);

    }
}
