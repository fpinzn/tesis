package eu.ensam.pacho.annotations;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.LinearLayout;
import android.widget.ToggleButton;
import eu.ensam.ii.vrpn.VrpnClient;
import eu.ensam.ii.vrpn.clients.R;

/**
 * A layout that in includes a button that controls the Tilt Tracker
 * 
 * @author Philippe
 *
 */
public class VrpnTiltTracker extends LinearLayout {

	
	private ToggleButton enableButton;

	public VrpnTiltTracker(Context context) {
		super(context);
	}

	public VrpnTiltTracker(Context context, AttributeSet attrs) {
		super(context, attrs);
	}
	
	/**
	 * Perform late initialization that requires the view hierarchy to
	 * by completely build 
	 * 
	 */
	@Override
	public void onFinishInflate() {
		super.onFinishInflate();
		
		/*
		 * This toggle button enables the tilt tracker
		 */
		enableButton = (ToggleButton) findViewById(R.id.btnEnableTilt);
		enableButton.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				VrpnClient.getInstance().enableTiltTracker(buttonView.isChecked());
			}
		});
		
	}
}
