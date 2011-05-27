package eu.ensam.pacho.annotations;

import android.app.TabActivity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.widget.TabHost;
import eu.ensam.ii.vrpn.VrpnClient;
import eu.ensam.ii.vrpn.clients.R;

public class MainActivity extends TabActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
    	
    	super.onCreate(savedInstanceState);    
    	
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
        spec =  getTabHost().newTabSpec("H");
		spec.setIndicator("Navigation", getResources().getDrawable(R.drawable.ic_menu_manage));
		// use the id of the top level view of the tab view layout
		spec.setContent(new Intent(this,Navigation.class));

		//spec.setContent(R.id.navigation);
        tabHost.addTab(spec); 
        
        spec =  getTabHost().newTabSpec("H");
		spec.setIndicator("Annotations", getResources().getDrawable(R.drawable.ic_lock_silent_mode_vibrate));
		spec.setContent(new Intent(this,AnnotationList.class));
        tabHost.addTab(spec);        
        tabHost.setCurrentTab(0);
        
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
}