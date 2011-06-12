package eu.ensam.pacho.annotations;

import java.util.ArrayList;

import android.app.TabActivity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.widget.TabHost;
import android.widget.Toast;
import eu.ensam.ii.vrpn.VrpnClient;
import eu.ensam.ii.vrpn.clients.R;

public class MainActivity extends TabActivity {
	public Navigation navigation;
	public AnnotationList annotationList;
	/*Annotations made in the navigation tab before visiting for the first time
	 * the annotations' screen need to be stored here.
	 * */
	public ArrayList<Annotation> preAnnotations;
	public String author;
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	
    	super.onCreate(savedInstanceState);    
    	Resources r = getResources();

    	getIntent().putExtra(VrpnClient.EXTRA_VRPN_SERVER, r.getString(R.string.VrpnServer));    	
    	getIntent().putExtra(VrpnClient.EXTRA_VRPN_PORT, r.getInteger(R.integer.VrpnServerPort));

    	Bundle bundle = getIntent().getExtras();
		author= bundle.getString("Author");
    	/*
    	 * The folowing line is required
    	 */
    	VrpnClient.getInstance().setupVrpnServer(getApplicationContext(), getIntent());
    	
        TabHost tabHost = getTabHost(); 		// The activity TabHost
        TabHost.TabSpec spec; 					// Reusable TabSpec for each tab
        LayoutInflater inflater = (LayoutInflater)getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.main,  getTabHost().getTabContentView(), true);
        

        /*
         * Using Views inside tabs is easier than using Activities (less context management)
         */
        
        
        /*
         * Add the views to the   tab_group.xml
         */
        // http://www.codeproject.com/KB/android/AndroidTabs.aspx
        // http://stackoverflow.com/questions/2732682/simple-example-of-merge-and-include-usage-in-android-xml-layouts
        spec =  getTabHost().newTabSpec("Navigation");
		spec.setIndicator("Navigation", getResources().getDrawable(R.drawable.ic_menu_manage));
		// use the id of the top level view of the tab view layout
		spec.setContent(new Intent(this,Navigation.class));

		//spec.setContent(R.id.navigation);
        tabHost.addTab(spec); 
        
        spec =  getTabHost().newTabSpec("Annotations");
		spec.setIndicator("Annotations", getResources().getDrawable(R.drawable.ic_lock_silent_mode_vibrate));
		spec.setContent(new Intent(this,AnnotationList.class));
        tabHost.addTab(spec);        
        tabHost.setCurrentTab(0);
        
        
        preAnnotations=new ArrayList<Annotation>();
    }


    @Override
    protected void onDestroy() {
    	super.onDestroy();
    	/*
    	 * By default, the sensor listeners remain active, event after the activity is 
    	 * stopped --> stop the sensor listener
    	 */
    	VrpnClient.getInstance().enableTiltTracker(false);
    }

    public void setTab(int tabCode,int command){
    	getTabHost().setCurrentTab(tabCode);
    	
    	if(command==1 && tabCode==0){
    		//Create new annotation.
    		Context context = getApplicationContext();
    		CharSequence text = "Navigate to the desired position and take a photo tapping the screen.";
    		int duration = Toast.LENGTH_LONG;
    		Toast toast = Toast.makeText(context, text, duration);
    		toast.show();
    	}
    }

}