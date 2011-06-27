import java.awt.image.*; 
import javax.imageio.*;


// Port we are receiving.
int port = 8888; 
DatagramSocket ds; 
// A byte array to read into (max size of 65536, could be smaller)
byte[] buffer = new byte[65536]; 



void setup() {

  try {
    ds = new DatagramSocket(port);
  } catch (SocketException e) {
    e.printStackTrace();
  } 

}

 void draw() {
  // checkForImage() is blocking, stay tuned for threaded example!
  checkForUpdates();

}

void checkForUpdates() {
  DatagramPacket p = new DatagramPacket(buffer, buffer.length); 
  try {
    ds.receive(p);
    String received = new String(p.getData(), 0, p.getLength());
    println(received);
  } catch (IOException e) {
    e.printStackTrace();
  } 
}

