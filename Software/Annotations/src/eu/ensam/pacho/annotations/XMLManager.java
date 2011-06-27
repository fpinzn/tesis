package eu.ensam.pacho.annotations;

import java.io.*;   // for Input/OutputStream
import java.net.*;  // for Socket and ServerSocket
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.*;

import org.xml.sax.SAXException;

import android.util.Log;

public class XMLManager{
	
	  // names of the XML tags
    static final String DATE = "date";
    static final  String PRIORITY = "priority";
    static final  String CONTENTS = "contents";
    static final  String AUTHOR = "author";
    static final  String ANNOTATION = "annotation";
    static final  String ID = "id";
    static final  String ROOT = "annotations";
    
    private static ArrayList<Annotation> annotations;
	public XMLManager(String server,int port) throws IOException, ParserConfigurationException, SAXException{

		InetAddress destAddr = InetAddress.getByName(server);  // Destination address
	    Socket sock = new Socket(destAddr, port);
	    sock.getOutputStream().write("OK".getBytes());
	    /*BufferedReader reader = new BufferedReader(new InputStreamReader(sock.getInputStream(), "UTF-8"));

	    String line;
	    String xml="";
        while ((line = reader.readLine()) != null) {
            xml+=line+"\n";
        }
    	Log.d("XML:",xml.substring(xml.length()-20,xml.length()));
	     */
    	
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        ArrayList<Annotation> annotations = new ArrayList<Annotation>();

        
            DocumentBuilder builder = factory.newDocumentBuilder();
            Document dom = builder.parse(sock.getInputStream(), "UTF-8");

            Element root =dom.getDocumentElement();
            NodeList items = root.getElementsByTagName(ANNOTATION);
            for (int i=0;i<items.getLength();i++){
                Annotation actual = new Annotation();
                Node item = items.item(i);
                NamedNodeMap nnm=item.getAttributes();
                actual.setAuthor(nnm.getNamedItem(AUTHOR).getNodeValue());
                actual.setPriority(Integer.parseInt(nnm.getNamedItem(PRIORITY).getNodeValue()));
                actual.setDate(nnm.getNamedItem(DATE).getNodeValue());
                actual.setId(Integer.parseInt(nnm.getNamedItem(ID).getNodeValue()));
                
                NodeList subelements = item.getChildNodes();
                for (int j=0;j<subelements.getLength();j++){
                    Node property = subelements.item(j);
                    String name = property.getNodeName();
                    if (name.equalsIgnoreCase(CONTENTS)){
                        actual.setContent(property.getFirstChild().getNodeValue());
                    }
                }
                annotations.add(actual);
                Log.d("XML New Annotation", actual.getAuthor()+"("+actual.getPriority()+"/"+actual.getDate()+"):"+actual.getContent());

            }
        
        
        this.annotations=annotations;
        Log.d("XML n. Annotations", annotations.size()+"");
	}
	
	public static ArrayList<Annotation> getAnnotations(){
		//This means the XML wasn't received/didn't exists/is void
		if(annotations==null){
			annotations=new ArrayList<Annotation>();
		}
		return annotations;
	}
	
}

