package org;

import org.json.simple.*;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.rmi.ServerException;
import java.util.ArrayList;
import java.util.HashMap;

enum Links{
    Artist("https://api.discogs.com/artists/"),
    Search("https://api.discogs.com/database/search"),
    Realeases("https://api.discogs.com/releases");

    public String link;
    Links(String link){
        this.link = link;
    }
}

class JsonDataStorage{
    static private class Band implements Comparable<Band>{
        public Long id;
        public String nazwa;
        public Band(Long id, String nazwa){
            this.id = id;
            this.nazwa = nazwa;
        }
        @Override
        public int compareTo(Band o) {
            return this.nazwa.compareTo(o.nazwa);
        }
    }
    private HashMap<Long, ArrayList<String>> artistsInBands; // band ID and ArrayList with that artists who was in the band
    private ArrayList<Band> bands; // band ID and string name associated to that band ID

    public JsonDataStorage(){
        this.artistsInBands = new HashMap<>();
        this.bands = new ArrayList<>();
    }

    private void validateArtist(JSONObject data) throws IOException{
        if(data.containsKey("members")){
            throw new IOException("Given group instead of member");
        }else if(!data.containsKey("groups")){
            throw new IOException("Not an artist");
        }

    }
    public void add(JSONObject obj) throws IOException{
        validateArtist(obj); // if not validated dropping IOException
        StringBuilder builder = new StringBuilder();
        builder.append((String)obj.get("name"));
        builder.append("(");
        builder.append((Long)obj.get("id"));
        builder.append(")");


        JSONArray bandsArr = (JSONArray) obj.get("groups");
        for(int i = 0; i<bandsArr.size(); i++){
            JSONObject zespol = (JSONObject) bandsArr.get(i);
            Long idZespolu = (Long)zespol.get("id");
            if(artistsInBands.containsKey(idZespolu)){
                artistsInBands.get(idZespolu).add(builder.toString());
            }else {
                ArrayList<String> list = new ArrayList<>();
                list.add(builder.toString());
                artistsInBands.put(idZespolu, list);
                bands.add(new Band(idZespolu, (String) zespol.get("name")));
                bands.sort((a,b)->a.compareTo(b));
            }
        }
    }

    public String toString(){
        StringBuilder builder = new StringBuilder();
        builder.append("\n-------------------\n\n");
        for(int i = 0; i<bands.size(); i++){
            if(artistsInBands.get(bands.get(i).id).size() == 1){
                continue;
            }
            builder.append(bands.get(i).nazwa);
            builder.append(" : ");
            builder.append(bands.get(i).id);
            builder.append("\n");
            for(int j = 0; j<artistsInBands.get(bands.get(i).id).size(); j++){ // iteracja po wszystkich czlonkach danego zespolu
                builder.append(j+1);
                builder.append(") ");
                builder.append(artistsInBands.get(bands.get(i).id).get(j));
                builder.append("\n");
            }
            builder.append("\n-------------------\n\n");
        }
        return builder.toString();
    }

}

public class Main {

    public static JSONObject validateAndDropJson(String stringAdress) throws IOException, ParseException {
        URL adress = new URL(stringAdress);
        HttpURLConnection conn = (HttpURLConnection) adress.openConnection();

        //setting the request property up
        conn.setDoInput(true);
        conn.setRequestMethod("GET");
        conn.setRequestProperty("User-Agent", "RestApp");
        conn.connect();

        if(conn.getResponseCode() == 429){
            throw new ServerException("Server overloaded: " + conn.getResponseCode());
        }
        if(conn.getResponseCode() != 200){
            throw new IOException("Bad response code: " + conn.getResponseCode());
        }
        if(!conn.getHeaderField("Content-Type").contains("application/json")){
            throw new IOException("Not application/json");
        }

        String content = "";
        BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
        String buf;
        while((buf = in.readLine()) != null){
            content += buf;
        }

        JSONParser parser = new JSONParser();
        JSONObject jsonObject =(JSONObject) parser.parse(content);
        return jsonObject;
    }

    static public void getAlbums(JSONObject jsonData) throws IOException{
        if(!jsonData.containsKey("releases") || jsonData == null){
            throw new IOException("Bad data");
        }
        JSONArray albumsData =(JSONArray) jsonData.get("releases");
        for(Object obj : albumsData){
            JSONObject jsonObject = (JSONObject) obj;
            System.out.println(jsonObject.get("title").toString());
        }
    }

    static public void sleep(){
        try{
            Thread.sleep(1000*30);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        while (true) {
            try {
//            JSONObject jsonData = validateAndDropJson("https://api.discogs.com/artists/359282/releases");
//            getAlbums(jsonData); // to jest jeszcze zadanie pierwsze
                JsonDataStorage storage = new JsonDataStorage();

                for (int i = 0; i < args.length; i++) {
                    storage.add(validateAndDropJson(Links.Artist.link + args[i]));
                }
                System.out.println(storage.toString());
                break;
            } catch (ServerException e) {
                // mechanizm poradzenia sobie z za duzą iloscia zapytan
                e.printStackTrace();
                sleep();
            } catch (Exception e) {
                e.printStackTrace();
                System.exit(-1);
            }
        }
    }
}
