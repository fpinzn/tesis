package eu.ensam.pacho.annotations;

import eu.ensam.ii.vrpn.clients.R;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

public class Login extends Activity{
	
	private String name;
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
			connect();
			start(_button);
		}
		
	}

	private void connect() {
		// TODO Auto-generated method stub
	}

	private void start(View view) {
		Intent myIntent = new Intent(view.getContext(), MainActivity.class);
		startActivity(myIntent);
	}
}
