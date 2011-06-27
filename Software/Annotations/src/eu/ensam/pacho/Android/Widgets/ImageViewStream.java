package eu.ensam.pacho.Android.Widgets;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.widget.ImageView;
import eu.ensam.ii.vrpn.VrpnClient;
import eu.ensam.ii.vrpn.clients.R;
import eu.ensam.pacho.annotations.Navigation;
public class ImageViewStream extends ImageView {

	boolean selectingPolygons=false;
	private boolean running=false;
	private Bitmap img;
	private int udpPort;
	public ImageViewStream(Context context) {
		super(context);
    	Resources r = getResources();
    	udpPort=r.getInteger(R.integer.ImageViewStreamPort);    	

	}
	
	public ImageViewStream(Context context, AttributeSet attributes){
		super(context,attributes);
    	Resources r = getResources();
    	udpPort=r.getInteger(R.integer.ImageViewStreamPort);    	

	}
	public boolean onTouchEvent(MotionEvent ev) {
		Log.d("ImageViewStream",ev.getX()+","+ev.getY());
		Resources r = getResources();
		int channelId=r.getInteger(R.id.VrpnTapScreen);
		VrpnClient.getInstance().send2DData(channelId, (int)ev.getX(), (int)ev.getY());

		if(!selectingPolygons){
			
			selectingPolygons=true;
			Navigation parent = Navigation.getInstance();
	        parent.selectPolygons();
	        
		}

		return false;
	}
	
	public void setSelectingPolygons(boolean sp){
		selectingPolygons=sp;
	}
	
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		//This is needed because for extrange reasons the constructor is not called!
		if(!running){
			(new UpdateImageThread()).start();
			running=true;
		}
		if(img!=null){
			
			setImageBitmap(img);
		}
	}
	
	private class UpdateImageThread extends Thread{
		DatagramSocket socket;
		byte buffer[]=new byte[65536];
		public UpdateImageThread() {
			try {
				socket=new DatagramSocket(udpPort);
				
			} catch (SocketException e) {
				Log.d("SocketException", e.getMessage());
				e.printStackTrace();
			}
		}
		
		public void run(){
			 while(true){

			 try {
					//InetAddress address = InetAddress.getByName("192.168.2.4");
					DatagramPacket p = new DatagramPacket(buffer, buffer.length); 
					Log.d("CFI","r1" );
					socket.receive(p);
					Log.d("CFI","r2" );

				  
					byte[] data = p.getData();

					Log.d("CFI","3" );

					Log.d("Datagram","Received datagram with " + data.length + " bytes." );
				  
				  	Log.d("ByteArray",data[0]+","+data[1]+","+data[2]);
					Log.d("CFI","4" );
				    // Make a BufferedImage out of the incoming bytes
				    Bitmap received = BitmapFactory.decodeByteArray(data, 0, data.length);
				   // Put the pixels into the video PImage
				   img=received;

				  }
				  catch (IOException e) {
						Log.e("Image View Stream: IOException",e.getMessage());
					    e.printStackTrace();
				  } 
				  catch (Exception e) {
					Log.d("Image View Stream: "+e.getClass().getName(),e.getMessage());
				    e.printStackTrace();
				  }
				  // Update the PImage pixels
				  try {
					  sleep(30);
				  } catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			 }
		}
		
	}
	
}
