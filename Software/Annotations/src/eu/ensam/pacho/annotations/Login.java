package eu.ensam.pacho.annotations;

import java.io.IOException;

import javax.xml.parsers.ParserConfigurationException;

import org.xml.sax.SAXException;

import eu.ensam.ii.vrpn.clients.R;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class Login extends Activity{
	
	public String name;
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.login);
	}
	
	public void click(View _button){

		TextView text=(TextView)findViewById(R.id.text);
		ProgressBar progress=(ProgressBar)findViewById(R.id.progress);
		EditText nameField=(EditText) findViewById(R.id.name);
		Button button=(Button)findViewById(R.id.button);
		//Confirm there's a name
		name = nameField.getText().toString(); 
		
		if(false){
	
//		if(name.equals("")){
			text.setText("Come on! Tell me your name!");
		}
		else{
			text.setText("Connecting to The Server");
			nameField.setVisibility(View.GONE);
			progress.setVisibility(View.VISIBLE);
			button.setVisibility(View.GONE);

			findViewById(R.id.login_layout).invalidate();
			connect();
			start(_button);
		}
		
	}

	private void connect() {
    	Resources r = getResources();
		String xmlServer=r.getString(R.string.XMLServer);
		int xmlServerPort=r.getInteger(R.integer.XMLServerPort);
		try {
			new XMLManager(xmlServer,xmlServerPort);
			
		} catch (IOException e) {
			showToast("XMLServer: "+e.getMessage());

		} catch (ParserConfigurationException e) {
			showToast("XMLAnnotations: "+e.getMessage());
		} catch (SAXException e) {
			
			showToast("XMLAnnotations: "+e.getMessage());
		}
		
	}
	private void start(View view) {
		Intent myIntent = new Intent(view.getContext(), MainActivity.class);
		Bundle bundle = new Bundle();
		bundle.putString("Author", name);
		myIntent.putExtras(bundle);
		startActivity(myIntent);
	}
	
	private void showToast(String msg){
		Context context = getApplicationContext();
		int duration = Toast.LENGTH_LONG;
		Toast toast = Toast.makeText(context, msg, duration);
		toast.show();
	}
}
