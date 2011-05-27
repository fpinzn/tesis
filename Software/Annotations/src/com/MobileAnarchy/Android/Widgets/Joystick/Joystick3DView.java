package com.MobileAnarchy.Android.Widgets.Joystick;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.MotionEvent;
import android.view.View;

public class Joystick3DView extends View {
	public static final int INVALID_POINTER_ID = -1;
	
	// =========================================
	// Private Members
	// =========================================
	private final boolean D = false;
	String TAG = "Joystick3DView";
	//boolean indicating if the joystick is righthanded
	private boolean righthanded;
	
	private Paint dbgPaint1;
	private Paint dbgPaint2;
	
	private Paint bgPaint;
	private Paint handlePaint;
	private Paint bg3DPaint;
	private Paint handle3DPaint;

	private int innerPadding;
	private int bgRadius;
	private int bg3DRadius;
	private int handleRadius;

	private int movementRadius;
	private int movement3DRadius;

 
	private Joystick3DMovedListener moveListener;
	private JoystickClickedListener clickListener;

	//# of pixels movement required between reporting to the listener
	private float moveResolution;

	private boolean yAxisInverted;
	private boolean autoReturnToCenter;
	
	//Max range of movement in user coordinate system
	public final static int CONSTRAIN_BOX = 0;
	public final static int CONSTRAIN_CIRCLE = 1;
	private int movementConstraint;
	private float movementRange;

	public final static int COORDINATE_CARTESIAN = 0;		//Regular cartesian coordinates
	public final static int COORDINATE_DIFFERENTIAL = 1;	//Uses polar rotation of 45 degrees to calc differential drive paramaters
	private int userCoordinateSystem;
	
	//Records touch pressure for click handling
	private float touchPressure;
	private boolean clicked;
	private float clickThreshold;
	
	//Last touch point in view coordinates
	private int pointerId = INVALID_POINTER_ID;
	public float touchX, touchY;
	
	//Is the current touching event using the 3D knob?
	private boolean touching3D;
	
	//Last reported position in view coordinates (allows different reporting sensitivities)
	private float reportX, reportY;
	
	//Handle center in view coordinates
	private float handleX, handleY;
	private float handleX3D, handleY3D;
	
	//Center of the view in view coordinates
	private int cX, cY;
	private int cX3D,cY3D;
	//These are used to draw
	private int cX3DHandle,cY3DHandle;
	//These are used to make calculations
	private int cX3DHanldeCalc,cY3DHandleCalc;

	//Size of the view in view coordinates
	private int dimX, dimY;

	//Cartesian coordinates of last touch point - joystick center is (0,0)
	private int cartX, cartY;
	
	//Polar coordinates of the touch point from joystick center
	private double radial;
	private double angle;
	
	//User coordinates of last touch point
	private int userX, userY;

	//Offset co-ordinates (used when touch events are received from parent's coordinate origin)
	private int offsetX;
	private int offsetY;

	private int userZ;


	// =========================================
	// Constructors
	// =========================================

	public Joystick3DView(Context context, boolean righthanded) {
		super(context);
		initJoystickView();
		this.righthanded=righthanded;
	}

	public Joystick3DView(Context context, AttributeSet attrs, boolean righthanded) {
		super(context, attrs);
		initJoystickView();
		this.righthanded=righthanded;
	}

	public Joystick3DView(Context context, AttributeSet attrs, int defStyle, boolean righthanded) {
		super(context, attrs, defStyle);
		initJoystickView();
		this.righthanded=righthanded;
	}

	// =========================================
	// Initialization
	// =========================================

	private void initJoystickView() {
		setFocusable(true);

		dbgPaint1 = new Paint(Paint.ANTI_ALIAS_FLAG);
		dbgPaint1.setColor(Color.RED);
		dbgPaint1.setStrokeWidth(1);
		dbgPaint1.setStyle(Paint.Style.STROKE);
		
		dbgPaint2 = new Paint(Paint.ANTI_ALIAS_FLAG);
		dbgPaint2.setColor(Color.GREEN);
		dbgPaint2.setStrokeWidth(1);
		dbgPaint2.setStyle(Paint.Style.STROKE);
		
		bgPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		bgPaint.setColor(Color.GRAY);
		bgPaint.setStrokeWidth(1);
		bgPaint.setStyle(Paint.Style.FILL_AND_STROKE);
		
		bg3DPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		bg3DPaint.setColor(Color.GRAY);
		bg3DPaint.setStrokeWidth(3);
		bg3DPaint.setStyle(Paint.Style.STROKE);

		handlePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		handlePaint.setColor(Color.DKGRAY);
		handlePaint.setStrokeWidth(1);
		handlePaint.setStyle(Paint.Style.FILL_AND_STROKE);

		handle3DPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		handle3DPaint.setColor(Color.RED);
		handle3DPaint.setStrokeWidth(1);
		handle3DPaint.setStyle(Paint.Style.FILL_AND_STROKE);
		
		innerPadding = 10;
		
		setMovementRange(10);
		setMoveResolution(1.0f);
		setClickThreshold(0.4f);
		setYAxisInverted(true);
		setUserCoordinateSystem(COORDINATE_CARTESIAN);
		setAutoReturnToCenter(true);
	}

	public void setAutoReturnToCenter(boolean autoReturnToCenter) {
		this.autoReturnToCenter = autoReturnToCenter;
	}
	
	public boolean isAutoReturnToCenter() {
		return autoReturnToCenter;
	}
	
	public void setUserCoordinateSystem(int userCoordinateSystem) {
		if (userCoordinateSystem < COORDINATE_CARTESIAN || movementConstraint > COORDINATE_DIFFERENTIAL)
			Log.e(TAG, "invalid value for userCoordinateSystem");
		else
			this.userCoordinateSystem = userCoordinateSystem;
	}
	
	public int getUserCoordinateSystem() {
		return userCoordinateSystem;
	}
	
	public void setMovementConstraint(int movementConstraint) {
		if (movementConstraint < CONSTRAIN_BOX || movementConstraint > CONSTRAIN_CIRCLE)
			Log.e(TAG, "invalid value for movementConstraint");
		else
			this.movementConstraint = movementConstraint;
	}
	
	public int getMovementConstraint() {
		return movementConstraint;
	}
	
	public boolean isYAxisInverted() {
		return yAxisInverted;
	}
	
	public void setYAxisInverted(boolean yAxisInverted) {
		this.yAxisInverted = yAxisInverted;
	}
	
	/**
	 * Set the pressure sensitivity for registering a click
	 * @param clickThreshold threshold 0...1.0f inclusive. 0 will cause clicks to never be reported, 1.0 is a very hard click
	 */
	public void setClickThreshold(float clickThreshold) {
		if (clickThreshold < 0 || clickThreshold > 1.0f)
			Log.e(TAG, "clickThreshold must range from 0...1.0f inclusive");
		else
			this.clickThreshold = clickThreshold;
	}
	
	public float getClickThreshold() {
		return clickThreshold;
	}
	
	public void setMovementRange(float movementRange) {
		this.movementRange = movementRange;
	}
	
	public float getMovementRange() {
		return movementRange;
	}
	
	public void setMoveResolution(float moveResolution) {
		this.moveResolution = moveResolution;
	}
	
	public float getMoveResolution() {
		return moveResolution;
	}
	
	// =========================================
	// Public Methods 
	// =========================================

	public void setOnJostick3DMovedListener(Joystick3DMovedListener listener) {
		this.moveListener = listener;
	}
	
	public void setOnJostickClickedListener(JoystickClickedListener listener) {
		this.clickListener = listener;
	}
	
	// =========================================
	// Drawing Functionality 
	// =========================================

	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		// Here we make sure that we have a perfect circle
		int measuredWidth = measure(widthMeasureSpec);
		int measuredHeight = measure(heightMeasureSpec);
		setMeasuredDimension(measuredWidth, measuredHeight);
	}

	@Override
	protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
		super.onLayout(changed, left, top, right, bottom);

		//Downsize dimensions to a multiple of 7 to avoid rounding errors.
		int d = Math.min(getMeasuredWidth(), getMeasuredHeight());
		int i;
		for(i=0;i<7&&(d-i)%7!=0;i++);
		
		dimX = d-i;
		dimY = d-i;

		cX=righthanded? 5*d/7:2*d/7;
		cY = 5*d / 7;
		
		cX3D=righthanded? d:0;
		cY3D=d;
		

		
		bgRadius = dimX/3 - innerPadding;
		bg3DRadius=6*d/7;
		handleRadius = (int)(d/6);
		movementRadius = 2*d/7 - handleRadius;
		movement3DRadius = 6*d/7;
		
		//0.8509=sin 45
		cY3DHandle=(int) (d-0.8509*bg3DRadius);
		cY3DHandleCalc=cY3DHandle;
		//0.5253=cos 45
		cX3DHandle=righthanded?(int) (0.5253*bg3DRadius):(int) (d-0.5253*bg3DRadius);
		cX3DHanldeCalc=(int) (0.5253*bg3DRadius);
	
	}

	private int measure(int measureSpec) {
		int result = 0;
		// Decode the measurement specifications.
		int specMode = MeasureSpec.getMode(measureSpec);
		int specSize = MeasureSpec.getSize(measureSpec);
		if (specMode == MeasureSpec.UNSPECIFIED) {
			// Return a default size of 200 if no bounds are specified.
			result = 200;
		} else {
			// As you want to fill the available space
			// always return the full available bounds.
			result = specSize;
		}
		return result;
	}

	@Override
	protected void onDraw(Canvas canvas) {
		canvas.save();
		//Draw 3D background
		canvas.drawCircle(cX3D, cY3D, bg3DRadius, bg3DPaint);
		// Draw the background
		canvas.drawCircle(cX, cY, bgRadius, bgPaint);

		// Draw the handle
		handleX =touching3D?cX:touchX + cX;
		handleY =touching3D?cY: touchY + cY;
		canvas.drawCircle(handleX, handleY, handleRadius, handlePaint);

		// Draw the 3D handle
		handleX3D =touching3D? (righthanded?dimX-touchX:touchX):cX3DHandle;
		handleY3D =touching3D? dimY-touchY:cY3DHandle;
		canvas.drawCircle(handleX3D, handleY3D, handleRadius, handlePaint);

		
		if (D) {
			canvas.drawRect(1, 1, getMeasuredWidth()-1, getMeasuredHeight()-1, dbgPaint1);
			
			canvas.drawCircle(handleX, handleY, 3, dbgPaint1);
			
			if ( movementConstraint == CONSTRAIN_CIRCLE ) {
				canvas.drawCircle(cX, cY, this.movementRadius, dbgPaint1);
			}
			else {
				canvas.drawRect(cX-movementRadius, cY-movementRadius, cX+movementRadius, cY+movementRadius, dbgPaint1);
			}
			
			//Origin to touch point
			canvas.drawLine(cX, cY, handleX, handleY, dbgPaint2);
			
			int baseY = (int) (touchY < 0 ? cY + handleRadius : cY - handleRadius);
			canvas.drawText(String.format("%s (%.0f,%.0f)", TAG, touchX, touchY), handleX-20, baseY-7, dbgPaint2);
			canvas.drawText("("+ String.format("%.0f, %.1f", radial, angle * 57.2957795) + (char) 0x00B0 + ")", handleX-20, baseY+15, dbgPaint2);
		}

//		Log.d(TAG, String.format("touch(%f,%f)", touchX, touchY));
//		Log.d(TAG, String.format("onDraw(%.1f,%.1f)\n\n", handleX, handleY));
		canvas.restore();
	}

	// Constrain touch within a box
	private void constrainBox() {
		if(touching3D){
			double angle=Math.atan2(touchY, touchX);
			
			touchX =(float) (Math.max(movement3DRadius*Math.cos(angle),0));
			touchY =(float) (Math.max(movement3DRadius*Math.sin(angle),0));
			
		}
		else{
			touchX = Math.max(Math.min(touchX, movementRadius), -movementRadius);
			touchY = Math.max(Math.min(touchY, movementRadius), -movementRadius);			
		}
		//Log.d(TAG, "touch box :"+ touchX+","+ touchY+"\n");

	}

	// Constrain touch within a circle
	private void constrainCircle() {
		float diffX = touchX;
		float diffY = touchY;
		double radial = Math.sqrt((diffX*diffX) + (diffY*diffY));
		if ( radial > movementRadius && !touching3D) {
			touchX = (int)((diffX / radial) * movementRadius);
			touchY = (int)((diffY / radial) * movementRadius);
		}
		
		if ( radial > movementRadius && touching3D) {
			touchX = (int)((diffX / radial) * movement3DRadius);
			touchY = (int)((diffY / radial) * movement3DRadius);
			Log.d(TAG, "touch circle:"+ touchX+","+ touchY+"\n");
	

		}
	}
	
	public void setPointerId(int id) {
		this.pointerId = id;
	}
	
	public int getPointerId() {
		return pointerId;
	}

	@Override
	public boolean onTouchEvent(MotionEvent ev) {
	    final int action = ev.getAction();
		switch (action & MotionEvent.ACTION_MASK) {
		    case MotionEvent.ACTION_MOVE: {
	    		return processMoveEvent(ev);
		    }	    
		    case MotionEvent.ACTION_CANCEL: 
		    case MotionEvent.ACTION_UP: {
		    	if ( pointerId != INVALID_POINTER_ID ) {
//			    	Log.d(TAG, "ACTION_UP");
			    	returnHandleToCenter();
		        	setPointerId(INVALID_POINTER_ID);
		    	}
		        break;
		    }
		    case MotionEvent.ACTION_POINTER_UP: {
		    	if ( pointerId != INVALID_POINTER_ID ) {
			        final int pointerIndex = (action & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
			        final int pointerId = ev.getPointerId(pointerIndex);
			        if ( pointerId == this.pointerId ) {
//			        	Log.d(TAG, "ACTION_POINTER_UP: " + pointerId);
			        	returnHandleToCenter();
			        	setPointerId(INVALID_POINTER_ID);
			        	
			    		return true;
			        }
		    	}
		        break;
		    }
		    case MotionEvent.ACTION_DOWN: {
		    	if ( pointerId == INVALID_POINTER_ID ) {
		    		int x = (int) ev.getX();
		    		if ( x >= offsetX && x < offsetX + dimX ) {
			        	setPointerId(ev.getPointerId(0));
//			        	Log.d(TAG, "ACTION_DOWN: " + getPointerId());
				    	int y,origx;
				    	y=dimY-(int)ev.getY();
				    	origx=x;
				    	x=righthanded?offsetX+dimX-x:x-offsetX;
				    	int distance=(int)Math.abs(Math.sqrt(x*x+y*y));
				    	if(distance>bg3DRadius-handleRadius) touching3D=true;
				    	else touching3D=false;
				    	Log.d("touching3D: ",distance+">"+bg3DRadius+" : "+touching3D+" : "+x+"("+origx+")"+","+y);
				    	//Log.d("touching3D: ",offsetX+","+offsetY+" : "+dimX+","+dimY);
			        	Log.d(TAG+(righthanded?"_right":"_left "), "ACTION_POINTER_DOWN: " + pointerId);

			    		return true;
		    		}
		    	}
		        break;
		    }
		    case MotionEvent.ACTION_POINTER_DOWN: {
		    	if ( pointerId == INVALID_POINTER_ID ) {
			        final int pointerIndex = (action & MotionEvent.ACTION_POINTER_INDEX_MASK) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
			        final int pointerId = ev.getPointerId(pointerIndex);
		    		int x = (int) ev.getX(pointerId);
		    		if ( x >= offsetX && x < offsetX + dimX ) {
			        	//Log.d(TAG+(righthanded?"_right":"_left "), "ACTION_POINTER_DOWN: " + pointerId);
		    		   	setPointerId(pointerId);
			        	
			         	int y,origx;
				    	y=dimY-(int)ev.getY();
				    	origx=x;
				    	x=righthanded?offsetX+dimX-x:x-offsetX;
				    	int distance=(int)Math.abs(Math.sqrt(x*x+y*y));
				    	if(distance>bg3DRadius-handleRadius) touching3D=true;
				    	else touching3D=false;
				    	Log.d("touching3D: ",distance+">"+bg3DRadius+" : "+touching3D+" : "+x+"("+origx+")"+","+y);
				    	//Log.d("touching3D: ",offsetX+","+offsetY+" : "+dimX+","+dimY);
			        	Log.d(TAG+(righthanded?"_right":"_left "), "ACTION_POINTER_DOWN: " + pointerId);

			    		return true;
		    		}
		    	}
		        break;
		    }
	    }
		return false;
	}
	
	private boolean processMoveEvent(MotionEvent ev) {
		if ( pointerId != INVALID_POINTER_ID ) {
			final int pointerIndex = ev.findPointerIndex(pointerId);
			
			float x = ev.getX(pointerIndex);
			float y = ev.getY(pointerIndex);

			if(touching3D){
				// Translate touch position to center of the 3D circle coordinates

				touchX =righthanded?-x+dimX+offsetX:x -offsetX;
				touchY =dimY-y -offsetY;

			}
			else{
				// Translate touch position to center of view
				touchX = x - cX - offsetX;
				touchY =y - cY - offsetY;
			}
//        	Log.d(TAG, String.format("ACTION_MOVE: (%03.0f, %03.0f) => (%03.0f, %03.0f)", x, y, touchX, touchY));
        	
			reportOnMoved();
			invalidate();
			
			touchPressure = ev.getPressure(pointerIndex);
			reportOnPressure();
			
			return true;
		}
		return false;
	}

	private void reportOnMoved() {
		if ( movementConstraint == CONSTRAIN_CIRCLE )
			constrainCircle();
		else
			constrainBox();

		calcUserCoordinates();

		if (moveListener != null) {
			
			if(touching3D){
				boolean rx = Math.abs(touchX - reportX) >= moveResolution;
				boolean ry = Math.abs(touchY - reportY) >= moveResolution;
				if (rx || ry) {
					this.reportX = touchX;
					this.reportY = touchY;
					
//					Log.d(TAG, String.format("moveListener.OnMoved(%d,%d)", (int)userX, (int)userY));
					moveListener.OnMoved(userX, userY,userZ);
				}
			}
			else{
				boolean rx = Math.abs(touchX - reportX) >= moveResolution;
				boolean ry = Math.abs(touchY - reportY) >= moveResolution;
				if (rx || ry) {
					this.reportX = touchX;
					this.reportY = touchY;
					
	//				Log.d(TAG, String.format("moveListener.OnMoved(%d,%d)", (int)userX, (int)userY));
					moveListener.OnMoved(userX, userY,0);
				}
			}
		}
	}

	private void calcUserCoordinates() {
		if(touching3D){
			
			//TODO: Revisar
			int x=(int) (touchX);
			int y=(int) (touchY);
			double angle=Math.atan2(y,x);
			double maxangle=Math.atan2(dimY-bg3DRadius,0);
			double minangle=Math.atan2(dimY, bg3DRadius);
			//Log.d("maMA", minangle+","+maxangle);
			//Log.d("user coords", x+","+y+"/"+angle);
			//Log.d("Value",((angle-minangle)*10/maxangle)+"\n");
			//userX=(int) touchX;
			//userY=(int) touchY;
			userZ=(int) ((angle-minangle)*2*movementRange/maxangle);
		}
		else{
		//First convert to cartesian coordinates
		cartX = (int)(touchX / movementRadius * movementRange);
		cartY = (int)(touchY / movementRadius * movementRange);
		
		radial = Math.sqrt((cartX*cartX) + (cartY*cartY));
		angle = Math.atan2(cartY, cartX);
		
		//Invert Y axis if requested
		if ( !yAxisInverted )
			cartY  *= -1;
		
		if ( userCoordinateSystem == COORDINATE_CARTESIAN ) {
			userX = cartX;
			userY = cartY;
		}
		else if ( userCoordinateSystem == COORDINATE_DIFFERENTIAL ) {
			userX = cartY + cartX / 4;
			userY = cartY - cartX / 4;
			
			if ( userX < -movementRange )
				userX = (int)-movementRange;
			if ( userX > movementRange )
				userX = (int)movementRange;

			if ( userY < -movementRange )
				userY = (int)-movementRange;
			if ( userY > movementRange )
				userY = (int)movementRange;
		}
		}
	}
	
	//Simple pressure click
	private void reportOnPressure() {
//		Log.d(TAG, String.format("touchPressure=%.2f", this.touchPressure));
		if ( clickListener != null ) {
			if ( clicked && touchPressure < clickThreshold ) {
				clickListener.OnReleased();
				this.clicked = false;
//				Log.d(TAG, "reset click");
				invalidate();
			}
			else if ( !clicked && touchPressure >= clickThreshold ) {
				clicked = true;
				clickListener.OnClicked();
//				Log.d(TAG, "click");
				invalidate();
				performHapticFeedback(HapticFeedbackConstants.VIRTUAL_KEY);
			}
		}
	}

	private void returnHandleToCenter() {
		if ( autoReturnToCenter ) {
			final int numberOfFrames = 5;

			if(touching3D){
				//80-20
				touchX=500;
				touchY=500;
				reportOnMoved();
				invalidate();
				if(moveListener!=null){
					moveListener.OnReturnedToCenter();
				}
			}
			else{
				
				
				final double intervalsX = (0 - touchX) / numberOfFrames;
				final double intervalsY = (0 - touchY) / numberOfFrames;
	
				for (int i = 0; i < numberOfFrames; i++) {
					final int j = i;
					postDelayed(new Runnable() {
						@Override
						public void run() {
							touchX += intervalsX;
							touchY += intervalsY;
							
							reportOnMoved();
							invalidate();
							
							if (moveListener != null && j == numberOfFrames - 1) {
								moveListener.OnReturnedToCenter();
							}
						}
					}, i * 40);
				}
	
				if (moveListener != null) {
					moveListener.OnReleased();
				}
			}
		}
	}

	public void setTouchOffset(int x, int y) {
		offsetX = x;
		offsetY = y;
	}
}
