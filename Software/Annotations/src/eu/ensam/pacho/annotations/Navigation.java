package eu.ensam.pacho.annotations;


import eu.ensam.ii.vrpn.clients.R;
import eu.ensam.pacho.Android.Widgets.AnnotationDialog;
import eu.ensam.pacho.Android.Widgets.DualJoystickView;
import eu.ensam.pacho.Android.Widgets.ImageViewStream;
import eu.ensam.pacho.Android.Widgets.Joystick3DMovedListener;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.Editable;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class Navigation extends Activity{
	private TextView txtX1, txtY1,txtZ1;
	private TextView txtX2, txtY2,txtZ2;
	private DualJoystickView joystick;
	private ImageViewStream render;
	private Button btncontinue,btncancel;
//	private LinearLayout annotation;
//	private EditText annotation_content;
	private static Navigation instance;
	private Toast toast;
	//0=navigating 1=selecting pols 2=introducing annotation
	private int state;
	
	ProgressDialog savingAnnotationDialog;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		super.onCreate(savedInstanceState);
		
		instance=this;
		setContentView(R.layout.navigation);

		txtX1 = (TextView)findViewById(R.id.TextViewX1);
        txtY1 = (TextView)findViewById(R.id.TextViewY1);
        txtZ1 = (TextView)findViewById(R.id.TextViewZ1);
        
		txtX2 = (TextView)findViewById(R.id.TextViewX2);
        txtY2 = (TextView)findViewById(R.id.TextViewY2);
        txtZ2 = (TextView)findViewById(R.id.TextViewZ2);

        joystick = (DualJoystickView)findViewById(R.id.dualjoystickView);
        
        joystick.setOnJostick3DMovedListener(_listenerLeft, _listenerRight);
        
        render=(ImageViewStream)findViewById(R.id.render);
        btncontinue=(Button)findViewById(R.id.btncontinue);
        btncancel=(Button)findViewById(R.id.btncancel);
       // annotation=(LinearLayout)findViewById(R.id.annotation);
       // annotation_content=(EditText)findViewById(R.id.annotation_content);
	}

    private Joystick3DMovedListener _listenerLeft = new Joystick3DMovedListener() {

		@Override
		public void OnMoved(int pan, int tilt,int z) {
			txtX1.setText(Integer.toString(pan));
			txtY1.setText(Integer.toString(tilt));
			txtZ1.setText(Integer.toString(z));
		}

		@Override
		public void OnReleased() {
			txtX1.setText("released");
			txtY1.setText("released");
			txtZ1.setText("released");
		}
		
		public void OnReturnedToCenter() {
			txtX1.setText("stopped");
			txtY1.setText("stopped");
			txtZ1.setText("stopped");
		}

	}; 

    private Joystick3DMovedListener _listenerRight = new Joystick3DMovedListener() {

		@Override
		public void OnMoved(int pan, int tilt,int z) {
			txtX2.setText(Integer.toString(pan));
			txtY2.setText(Integer.toString(tilt));
			txtZ2.setText(Integer.toString(z));
		}

		@Override
		public void OnReleased() {
			txtX2.setText("released");
			txtY2.setText("released");
			txtZ2.setText("released");
		}
		
		public void OnReturnedToCenter() {
			txtX2.setText("stopped");
			txtY2.setText("stopped");
			txtZ2.setText("stopped");
			
		};
	}; 


	public void selectPolygons(){
		state=1;
		joystick.setVisibility(View.GONE);
		btncancel.setVisibility(View.VISIBLE);
		btncontinue.setVisibility(View.VISIBLE);
		Context context = getApplicationContext();
		CharSequence text = "Select the desired polygons and tap Annotate to continue.";
		int duration = Toast.LENGTH_LONG;
		toast = Toast.makeText(context, text, duration);
		toast.show();
	}
	


	public void click(View view){
		int id=view.getId();
		if(state==1){
			if(id==R.id.btncontinue){		
				annotate();
			}
			
			else if(id==R.id.btncancel){
				navigation();
			}
		}
		
		else if(state==2){
			if(id==R.id.btncontinue){		
				navigation();
			}
			
			else if(id==R.id.btncancel){
				selectPolygons();
			}
		}
	}
	
	protected Dialog onCreateDialog(int id){
		if(id==0){
			//Annotation Dialog
			AnnotationDialog dialog = new AnnotationDialog(this,this);
			return dialog;
		}
		return null;
		
	}
	
	private void annotate() {
			
		state=2;
		showDialog(0);
		/*annotation.setVisibility(View.VISIBLE);
		btncontinue.setText("Finish");
		joystick.setVisibility(View.GONE);
		btncancel.setVisibility(View.VISIBLE);
		btncontinue.setVisibility(View.VISIBLE);
		render.setSelectingPolygons(false);*/
		toast.cancel();


	}



	public void navigation(){
		state=0;
		joystick.setVisibility(View.VISIBLE);
		btncancel.setVisibility(View.GONE);
		btncontinue.setVisibility(View.GONE);
		btncontinue.setText("Annotate");
//		annotation.setVisibility(View.GONE);
//		annotation_content.setText("");
		toast.cancel();
		render.setSelectingPolygons(false);

	}
	public final static Navigation getInstance(){
		return instance;
	}



	public void sendAnnotation(Editable text, String priority) {
		Log.d("New Annotation",text+". "+priority);
		savingAnnotationDialog = ProgressDialog.show(Navigation.this, "", 
                "Saving Annotation...", true);
		
		//TODO:Save Annotation
		savingAnnotationDialog.hide();
		navigation();

	}
}
