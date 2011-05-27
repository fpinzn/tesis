package eu.ensam.pacho.annotations;

import android.app.TabActivity;
import android.content.Context;
import android.content.res.Resources;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.widget.TabHost;
import eu.ensam.ii.vrpn.VrpnClient;
import eu.ensam.ii.vrpn.clients.R;

public class IiMoove extends TabActivity {

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
		spec.setIndicator("Move", getResources().getDrawable(R.drawable.ic_lock_silent_mode_vibrate));
		spec.setContent(R.id.tab_tilt_tracker);
        tabHost.addTab(spec);        
        
        spec =  getTabHost().newTabSpec("H");
		spec.setIndicator("Settings", getResources().getDrawable(R.drawable.ic_menu_manage));
		// use the id of the top level view of the tab view layout
		spec.setContent(R.id.tab_parameters);
        tabHost.addTab(spec); 
        
        spec =  getTabHost().newTabSpec("H");
		spec.setIndicator("Pad", getResources().getDrawable(R.drawable.ic_tab_one));
		spec.setContent(R.id.dualjoystickView);
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