import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Arrays;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;

import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.Jsoup;
import org.jsoup.parser.Parser;
import org.jsoup.select.Elements;

public class Main {
    public static void getServerType(String ad){
        try {
            URL adress = new URL(ad);
            HttpURLConnection urlConnection = (HttpURLConnection) adress.openConnection();
            urlConnection.setRequestMethod("GET");
            urlConnection.setConnectTimeout(1000);
            urlConnection.connect();

            if(urlConnection.getResponseCode() == 200){
                System.out.println(urlConnection.getHeaderField("Server"));
            }

            Map mapa = urlConnection.getHeaderFields();

            for(Object key : mapa.keySet()){
                System.out.println(key + ": " + mapa.get(key));
            }

            urlConnection.disconnect();

        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public static void scrapeInteriaWebsite(Document htmlDoc){
        Elements el = htmlDoc.select("div.weather-forecast");
        for(Element e : el){
            String day = e.select("span.weather-forecast-hbh-day-labelRight").text();
            String[] hours = e.select("span.hour").text().split(" ");
            String[] temps = e.select("span.forecast-temp").text().split(" ");
            System.out.println(day);
            System.out.print("[");
            for(String s : hours){
                s+=":00";
                System.out.print(s + ", ");
            }
            System.out.print("]\n");
            System.out.print("[");
            for(String s : temps){
                System.out.print(s + ", ");
            }
            System.out.print("]\n");
        }
    }

    public static Document validateAndDropWebsite(String stringAdress) throws IOException {
        URL adress = new URL(stringAdress);
        HttpURLConnection conn = (HttpURLConnection) adress.openConnection();

        //setting the request property up
        conn.setDoInput(true);
        conn.setRequestMethod("GET");
        conn.connect();

        if(conn.getResponseCode() != 200){
            throw new IOException("Bad response code");
        }
        if(!conn.getHeaderField("Content-Type").contains("text/html")){
            throw new IOException("Not text/html");
        }

        String content = "";
        BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
        String buf;
        while((buf = in.readLine()) != null){
            content += buf;
        }
        if(!content.contains("Kraków")){
            throw new IOException("Not right website");
        }
        Document htmlDoc = Jsoup.parse(content);
        return htmlDoc;
    }



    public static void main(String[] args){
        try {
            //getServerType("https://www.google.com");
            Document htmlDoc = validateAndDropWebsite("https://pogoda.interia.pl/prognoza-szczegolowa-krakow,cId,4970");
            scrapeInteriaWebsite(htmlDoc);
            System.exit(0);
        }catch (Exception e){
            e.printStackTrace();
            System.exit(30);
        }
    }
}
