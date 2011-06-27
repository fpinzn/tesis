package eu.ensam.pacho.annotations;

import eu.ensam.ii.vrpn.clients.R;
import android.app.Dialog;
import android.content.Context;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class CreateAnnotationDialog extends Dialog implements android.view.View.OnClickListener{

	Button sendAnnotation;
	Navigation parent;
	EditText text;
	Spinner priority;
	TextView textview;
	Toast toast;
	public CreateAnnotationDialog(Context context, Navigation navigation) {
		super(context);
		setContentView(R.layout.create_annotation_dialog);
		setTitle("Create Annotation");

	    Spinner spinner = (Spinner) findViewById(R.id.spinner);
	    ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(context, R.array.priorities, android.R.layout.simple_spinner_item);
	    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
	    spinner.setAdapter(adapter);
	    
	    sendAnnotation =(Button)findViewById(R.id.btnSendAnnotation);	
	    sendAnnotation.setOnClickListener(this);
	    
	    text=(EditText)findViewById(R.id.annotation_content);
	    priority=(Spinner)findViewById(R.id.spinner);
	    textview=(TextView)findViewById(R.id.text);

	    parent=navigation;
	}
	

	@Override
	public void onClick(View v) {
		Context context = parent;
		CharSequence msg = "Please introduce information to make the Annotation.";
		int duration = Toast.LENGTH_LONG;
		toast = Toast.makeText(context, msg, duration);
		
		if(text.getText().length()==0){
			toast.show();
		}
		else{
			parent.newAnnotation(text.getText().toString(), priority.getSelectedItemPosition()+1);
			text.setText("");
			toast.cancel();
			this.hide();
		}

	}

	public void onBackPressed(){
		super.onBackPressed();
		parent.selectPolygons();
	}
}
