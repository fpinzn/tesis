#pragma once

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>

#include <osg/vec3>
#include <osg/quat>

#include <map>
#include <string>


class AndroidTiltTracker;

class CVRPNAndroidClientManager
{
public:


	CVRPNAndroidClientManager(void);
	~CVRPNAndroidClientManager(void);

	void init(const std::string& vrpnName);
	void run();
	void setVerbose(bool verbose);
	bool isVerbose() const;


	AndroidTiltTracker& getTiltTracker();
	const AndroidTiltTracker& getTiltTracker() const;


private:
	static void VRPN_CALLBACK handle_analog(void* userData, const vrpn_ANALOGCB t);
	static void VRPN_CALLBACK handle_button(void* userData, const vrpn_BUTTONCB t);
	static void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t);

	vrpn_Tracker_Remote* m_trackerRemote;
	vrpn_Analog_Remote* m_analogRemote;
	vrpn_Button_Remote* m_buttonRemote;

	std::map<int, Tracker*, cprInt> m_vrpnTrackerCallbackMap;
	std::map<int, bool> m_vrpnButtonCallbackMap;
	vrpn_ANALOGCB m_vrpnAnalogCallbackMap;

	bool m_verbose;

	enum {
		TRACKER_ID_TILT = 0,
	};
	
	VRPNAndroidTiltTracker m_tiltTracker;

};

// callback
