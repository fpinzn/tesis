#pragma once

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>
#include <vrpn_Text.h>
#include <osg/vec3>
#include <osg/quat>

#include <map>
#include <string>
#include "Annotation.h"

class CVRPNClientManager
{
public:
	struct Tracker
	{
		int id;
		osg::Vec3 crtPos;
		osg::Quat crtQuat;
		osg::Vec3 firstPos;
		osg::Quat firstQuat;
	};

	CVRPNClientManager(void);
	~CVRPNClientManager(void);

	void init(const std::string& vrpnName);
	void run();
	void setVerbose(bool verbose);
	bool isVerbose() const;

	// Tracker handling
	void addOrUpdateTracker(int id, const osg::Vec3& pos, const osg::Quat& q);
	void removeAllTracker();
	int getTrackerCount() const;
	Tracker* getTracker(int i) const;
	Tracker* getTrackerFromId(int id) const;

	// Button handling
	void addOrUpdateButton(int id, bool value);
	void removeAllButton();
	int getButtonCount() const;
	bool getButton(int i) const;
	bool getButtonFromId(int id, bool& status) const;

	// Analog channels handling
	void updateAnalog(const vrpn_ANALOGCB& t);
	void removeAllAnalog();
	int getAnalogCount() const;
	double getAnalog(int i) const;
	double getAnalogFromId(int id) const;
	
	//Manage annotations
	bool copyLastAnnotation(Annotation* annotation);
	bool copyGoToAnnotation(Annotation* annotation);
	void CVRPNClientManager::updateAnnotation(Annotation* annotation);
	void setNewGoToAnnotationCommand();
	bool theresNewAnnotation();
	bool theresNewGoToAnnotationCommand();
	void printText();
	std::vector<Annotation> annotations;

private:
	struct cprInt
	{
		bool operator()(int s1, int s2) const
		{
			return s1 < s2;
		}
	};

	vrpn_Tracker_Remote* m_trackerRemote;
	vrpn_Analog_Remote* m_analogRemote;
	vrpn_Button_Remote* m_buttonRemote;
	vrpn_Text_Receiver* m_textReceiver;

	std::map<int, Tracker*, cprInt> m_vrpnTrackerCallbackMap;
	std::map<int, bool> m_vrpnButtonCallbackMap;
	vrpn_ANALOGCB m_vrpnAnalogCallbackMap;
	struct Annotation lastAnnotation;
	bool newAnnotation;
	bool goToAnnotation;
	bool m_verbose;
	
};

// callback
void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t);
