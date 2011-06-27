package eu.ensam.pacho.Android.Widgets;

public interface Joystick3DMovedListener {
	public void OnReleased();
	public void OnReturnedToCenter();
	void OnMoved(int pan, int tilt, int z);
}
