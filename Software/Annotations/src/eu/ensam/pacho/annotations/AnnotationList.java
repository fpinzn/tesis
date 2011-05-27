package eu.ensam.pacho.annotations;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import eu.ensam.ii.vrpn.clients.R;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ListView;
import android.widget.SimpleAdapter;

public class AnnotationList extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.annotation_list);
        ListView lv= (ListView)findViewById(R.id.listview);

        // create the grid item mapping
        String[] from = new String[] {"Author", "Content","Priority"};
        int[] to = new int[] {R.id.annotation_author, R.id.annotation_content, R.id.annotation_priority};

        // prepare the list of all records
        List<HashMap<String, String>> fillMaps = new ArrayList<HashMap<String, String>>();
        for(int i = 0; i < 10; i++){
        	HashMap<String, String> map = new HashMap<String, String>();
        	map.put(from[2], "" + i);
        	map.put(from[1], "blablabla_" + i);
        	map.put(from[0], "Frédéric_" + i);

        	fillMaps.add(map);
        }

        // fill in the grid_item layout
        SimpleAdapter adapter = new SimpleAdapter(this, fillMaps, R.layout.annotation_list_item, from, to);
        lv.setAdapter(adapter);
        Log.d("List","count : "+adapter.getCount());

    }
}
