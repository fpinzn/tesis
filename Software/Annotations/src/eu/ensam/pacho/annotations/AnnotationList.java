package eu.ensam.pacho.annotations;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import eu.ensam.ii.vrpn.clients.R;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

public class AnnotationList extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.annotation_list);
        ListView lv= (ListView)findViewById(R.id.listview);

        // create the grid item mapping
        String[] from = new String[] {"Author", "Content","Priority","Id"};
        int[] to = new int[] {R.id.annotation_author, R.id.annotation_content, R.id.annotation_priority,R.id.annotation_id};

        // prepare the list of all records
        List<HashMap<String, String>> fillMaps = new ArrayList<HashMap<String, String>>();
        for(int i = 0; i < 10; i++){
        	HashMap<String, String> map = new HashMap<String, String>();
        	map.put(from[0], "Frédéric_" + i);
        	map.put(from[1], "Attribution is authorship metadata that is bound to content. No matter how far and wide a piece of content spreads, it never forgets who created it and where it’s from. Despite its importance, web attribution is already in shambles._" + i);
        	map.put(from[2], "" + (i%3));
        	fillMaps.add(map);
        	map.put(from[3], "" + i);
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
            ParentActivity.setTab(0, -1);
    	}
    }
}
