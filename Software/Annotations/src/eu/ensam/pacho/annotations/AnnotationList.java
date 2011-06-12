package eu.ensam.pacho.annotations;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import eu.ensam.ii.vrpn.VrpnClient;
import eu.ensam.ii.vrpn.clients.R;

import android.app.Activity;
import android.app.Dialog;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnLongClickListener;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

public class AnnotationList extends Activity {
	ArrayList<Annotation> annotations;
	private Annotation selectedAnnotation;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ((MainActivity)this.getParent()).annotationList=this;
        annotations=((MainActivity)this.getParent()).preAnnotations;
        setContentView(R.layout.annotation_list);
        ArrayList<Annotation>  xmlAnnotations=XMLManager.getAnnotations();
        annotations.addAll(0, xmlAnnotations);
        fillTable();
        
    }
    
    private void fillTable() {
        ListView lv= (ListView)findViewById(R.id.listview);
    	// create the grid item mapping
        String[] from = new String[] {"Author", "Content","Priority","Id","Date"};
        int[] to = new int[] {R.id.annotation_author, R.id.annotation_content, R.id.annotation_priority,R.id.annotation_id,R.id.annotation_date};

       
        // prepare the list of all records
        List<HashMap<String, String>> fillMaps = new ArrayList<HashMap<String, String>>();
        for(int i = 0; i < annotations.size(); i++){
        	Annotation actual=annotations.get(i);
        	HashMap<String, String> map = new HashMap<String, String>();
        	map.put(from[0], actual.getAuthor());
        	map.put(from[1], actual.getContent());
        	map.put(from[2], actual.getPriority()+"");
        	map.put(from[3], actual.getId()+"");
        	map.put(from[4], actual.getDate());
        	fillMaps.add(map);
        }

        // fill in the grid_item layout
        SimpleAdapter adapter = new SimpleAdapter(this, fillMaps, R.layout.annotation_list_item, from, to);
        lv.setAdapter(adapter);
        Log.d("List","count : "+adapter.getCount());
		
	}

	public void click(View view){
    	String id=(String)((TextView)view.findViewById(R.id.annotation_id)).getText();
		 MainActivity ParentActivity;
         ParentActivity = (MainActivity) this.getParent();
    	if(id.equals("new_annotation")){
             ParentActivity.setTab(0, 1);
    	}
    	else{
    		//TODO: Sent VRPN Signal to navigate to that annotation
    		selectedAnnotation=getAnnotationFromId(Long.parseLong(id));
			Log.d("selectedAnnotation ("+id+")",selectedAnnotation.getId()+"");

    		showDialog(0);
            //ParentActivity.setTab(0, -1);
    	}
    }
    
	protected Dialog onCreateDialog(int id){
		if(id==0){
			//Annotation Dialog
			ReviewAnnotationDialog dialog = new ReviewAnnotationDialog(this,this,selectedAnnotation);
			Log.d("ReviewAnnn","After create");

			return dialog;
		}
		return null;
		
	}
	
	public Annotation getAnnotationFromId(long id){
		for(int i=0;i<annotations.size();i++){
			Annotation actual=annotations.get(i);
			if(actual.getId()==id){
				return actual;
			}
			
		}
		throw new Error("Annotation with id: "+id+" not found.");
	}

	public void appendAnnotation(Annotation an){
		annotations.add(an);
		fillTable();
	}

	public void removeAnnotationFromId(long id){
		annotations.remove(getAnnotationFromId(id));
		Resources r = getResources();
		int channelId=r.getInteger(R.id.VrpnRemoveAnnotation);
		VrpnClient.getInstance().sendAnalog(channelId,id);
		fillTable();
	}
}
