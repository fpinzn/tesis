package eu.ensam.pacho.annotations;

import com.MobileAnarchy.Android.Widgets.Joystick.DualJoystickView;
import com.MobileAnarchy.Android.Widgets.Joystick.Joystick3DMovedListener;

import eu.ensam.ii.vrpn.clients.R;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class Navigation extends Activity {
	TextView txtX1, txtY1,txtZ1;
	TextView txtX2, txtY2,txtZ2;
	DualJoystickView joystick;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.navigation);

		txtX1 = (TextView)findViewById(R.id.TextViewX1);
        txtY1 = (TextView)findViewById(R.id.TextViewY1);
        txtZ1 = (TextView)findViewById(R.id.TextViewZ1);
        
		txtX2 = (TextView)findViewById(R.id.TextViewX2);
        txtY2 = (TextView)findViewById(R.id.TextViewY2);
        txtZ2 = (TextView)findViewById(R.id.TextViewZ2);

        joystick = (DualJoystickView)findViewById(R.id.dualjoystickView);
        
        joystick.setOnJostick3DMovedListener(_listenerLeft, _listenerRight);
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

}
