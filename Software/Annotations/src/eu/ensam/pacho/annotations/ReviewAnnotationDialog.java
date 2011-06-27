package eu.ensam.pacho.annotations;

import eu.ensam.ii.vrpn.VrpnClient;
import eu.ensam.ii.vrpn.clients.R;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class ReviewAnnotationDialog extends Dialog implements android.view.View.OnClickListener{

	AnnotationList parent;
	TextView content;
	TextView priority;
	TextView date;
	TextView author;
	Button btnGoTo;
	Button btnDelete;
	long id;
	int state=0;
	public ReviewAnnotationDialog(Context context, AnnotationList annotationList, Annotation annotation) {
		super(context);
		Log.d("ReviewAnnotation","Creator");
		setContentView(R.layout.review_annotation_dialog);
		setTitle("Review Annotation");
	    
	    btnGoTo =(Button)findViewById(R.id.btnGotoLocation);	
	    btnGoTo.setOnClickListener(this);
	    
	    
	    btnDelete =(Button)findViewById(R.id.btnDelete);	
	    btnDelete.setOnClickListener(this);
	    
	    content=(TextView)findViewById(R.id.annotation_content);
	    date=(TextView)findViewById(R.id.annotation_date);
	    priority=(TextView)findViewById(R.id.annotation_priority);
	    author=(TextView)findViewById(R.id.annotation_author);
	    id=annotation.getId();
	    
	    content.setText(annotation.getContent());
	    author.setText(annotation.getAuthor());
	    date.setText(annotation.getDate());
	    priority.setText("Priority: "+annotation.getPriority());
	    parent=annotationList;
	}
	

	@Override
	public void onClick(View v) {
		Button btn=(Button)v;
		if(btn.getId()==R.id.btnGotoLocation){
			if(state==0){
			Resources r = parent.getResources();
			int btnId=r.getInteger(R.id.VrpnGoToAnnotation);
			VrpnClient.getInstance().send2DData(btnId, (int)id,0);
			Log.d("GOTO: "+btnId,id+"");
			btnGoTo.setText("Continue");
			state=1;
			}
			else{
				this.hide();
				parent.removeDialog(0);
			}
		}
		else{
			//Delete
			Log.d("Annotation ID",id+"");
   			parent.removeAnnotationFromId(id);
			this.hide();
			parent.removeDialog(0);


		}
	}

	public void onBackPressed(){
		super.onBackPressed();
		//parent.selectPolygons();
	}
}
