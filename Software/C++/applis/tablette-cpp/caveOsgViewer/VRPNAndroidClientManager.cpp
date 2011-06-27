#include "stdafx.h"

#include "VRPNAndroidClientManager.h"

#include <vrpn_Tracker.h>
#include <vrpn_Analog.h>
#include <vrpn_Button.h>

#include <iostream>



CVRPNAndroidClientManager::CVRPNAndroidClientManager(void):
m_verbose(false)
{
	m_trackerRemote = NULL;
	m_analogRemote = NULL;
	m_buttonRemote = NULL;
}

CVRPNAndroidClientManager::~CVRPNAndroidClientManager(void)
{
	removeAllTracker();

	if(m_trackerRemote) {
		delete m_trackerRemote;
		m_trackerRemote = NULL;
	}

	if(m_analogRemote) {
		delete m_analogRemote;
		m_analogRemote = NULL;
	}

	if(m_buttonRemote) {
		delete m_buttonRemote;
		m_buttonRemote = NULL;
	}
}

void CVRPNAndroidClientManager::init(const std::string& vrpnName)
{
	m_trackerRemote = new vrpn_Tracker_Remote(vrpnName.c_str());
	m_trackerRemote->register_change_handler(this, handle_tracker);

	m_analogRemote = new vrpn_Analog_Remote(vrpnName.c_str());
	m_analogRemote->register_change_handler(this, handle_analog);

	m_buttonRemote = new vrpn_Button_Remote(vrpnName.c_str());
	m_buttonRemote->register_change_handler(this, handle_button);
}

void CVRPNAndroidClientManager::run()
{
	if(m_trackerRemote) {
		m_trackerRemote->mainloop();
	}

	if(m_analogRemote) {
		m_analogRemote->mainloop();
	}

	if(m_buttonRemote) {
		m_buttonRemote->mainloop();
	}
}

void CVRPNAndroidClientManager::setVerbose(bool verbose) {
	m_verbose = verbose;
}

bool CVRPNAndroidClientManager::isVerbose() const{
	return m_verbose;
}

/***********************************************************************
 *
 *		Analog handling
 *
 **********************************************************************/
void VRPN_CALLBACK CVRPNAndroidClientManager::handle_tracker(void* userData, const vrpn_TRACKERCB t)
{

	CVRPNAndroidClientManager* vrpnCltManager = static_cast<CVRPNAndroidClientManager*>(userData);
	if(vrpnCltManager)
	{
		if (vrpnCltManager->isVerbose()) {
			std::cout << "Tracker '" << t.sensor << "' : " << t.pos[0] << "," <<  t.pos[1] << "," << t.pos[2] << "," << t.quat[0] << "," <<  t.quat[1] << "," << t.quat[2] << "," << t.quat[3] << std::endl;
		}
		osg::Vec3 pos(t.pos[0], t.pos[1], t.pos[2]);
		osg::Quat q(t.quat[0], t.quat[1], t.quat[2], t.quat[3]);
		
		vrpnCltManager->addOrUpdateTracker(t.sensor, pos, q);		
	}
}
void CVRPNClientManager::addOrUpdateTracker(int id, const osg::Vec3& pos, const osg::Quat& q)
{
	if(m_vrpnTrackerCallbackMap.find(id) == m_vrpnTrackerCallbackMap.end())
	{
		// add new tracker
		Tracker* tracker = new Tracker;
		tracker->id = id;
		tracker->firstPos = pos;
		tracker->firstQuat = q;
		tracker->crtPos = pos;
		tracker->crtQuat = q;

		m_vrpnTrackerCallbackMap[id] = tracker;
	}
	else
	{
		// update tracker
		m_vrpnTrackerCallbackMap[id]->crtPos = pos;
		m_vrpnTrackerCallbackMap[id]->crtQuat = q;
	}
}

void CVRPNClientManager::removeAllTracker()
{
	std::map<int, Tracker*, cprInt>::iterator it;
	for(it = m_vrpnTrackerCallbackMap.begin(); it != m_vrpnTrackerCallbackMap.end(); it++)
		delete (it->second);
	m_vrpnTrackerCallbackMap.clear();
}

int CVRPNClientManager::getTrackerCount() const
{
	return m_vrpnTrackerCallbackMap.size();
}

CVRPNClientManager::Tracker* CVRPNClientManager::getTracker(int i) const
{
	if(i < getTrackerCount())
	{
		std::map<int, Tracker*, cprInt>::const_iterator it = m_vrpnTrackerCallbackMap.begin();
		for(int j = i; j--; )
			it++;
		return it->second;
	}
	return NULL;
}

CVRPNClientManager::Tracker* CVRPNClientManager::getTrackerFromId(int id) const
{
	if(m_vrpnTrackerCallbackMap.find(id) == m_vrpnTrackerCallbackMap.end())
		return NULL;
	else
		return m_vrpnTrackerCallbackMap.find(id)->second;
}

/***********************************************************************
 *
 *		Analog handling
 *
 **********************************************************************/

void VRPN_CALLBACK CVRPNAndroidClientManager::handle_analog(void* userData, const vrpn_ANALOGCB t) {
	std::cout << "Analog " << "0: " << t.channel[0] << " 1: " << t.channel[1] << std::endl;
	CVRPNClientManager* vrpnCltManager = static_cast<CVRPNClientManager*>(userData);
	if(vrpnCltManager)
	{
		vrpnCltManager->updateAnalog(t);		
	}
}

void CVRPNClientManager::updateAnalog(const vrpn_ANALOGCB& t)
{
	m_vrpnAnalogCallbackMap = t;
}

void CVRPNClientManager::removeAllAnalog()
{
	
}

int CVRPNClientManager::getAnalogCount() const
{
	return m_vrpnAnalogCallbackMap.num_channel;
}

double CVRPNClientManager::getAnalog(int i) const
{
	if(i < getAnalogCount())
	{
		return m_vrpnAnalogCallbackMap.channel[i];
	}
	return NULL;
}

double CVRPNClientManager::getAnalogFromId(int id) const
{
	if(id < getAnalogCount())
	{
		return m_vrpnAnalogCallbackMap.channel[id];
	} 
	return NULL;
}

/***********************************************************************
 *
 *		Button handling
 *
 **********************************************************************/

void VRPN_CALLBACK CVRPNAndroidClientManager::handle_button(void* userData, const vrpn_BUTTONCB t) {
	std::cout << "Button " << t.button << " : " << t.state <<  std::endl;
	CVRPNClientManager* vrpnCltManager = static_cast<CVRPNClientManager*>(userData);
	if(vrpnCltManager)
	{
		vrpnCltManager->addOrUpdateButton(t.button, t.state != 0);		
	}
}

void CVRPNClientManager::addOrUpdateButton(int id, bool data)
{
	// update Button
	m_vrpnButtonCallbackMap[id] = data;
}

void CVRPNClientManager::removeAllButton()
{

	m_vrpnButtonCallbackMap.clear();
}

int CVRPNClientManager::getButtonCount() const
{
	return m_vrpnButtonCallbackMap.size();
}

bool CVRPNClientManager::getButton(int i) const
{
	if(i < getButtonCount())
	{
		std::map<int, bool>::const_iterator it = m_vrpnButtonCallbackMap.begin();
		for(int j = i; j--; )
			it++;
		return it->second;
	}
	return NULL;
}

bool CVRPNClientManager::getButtonFromId(int id) const
{
	if(m_vrpnButtonCallbackMap.find(id) == m_vrpnButtonCallbackMap.end())
		return NULL;
	else
		return m_vrpnButtonCallbackMap.find(id)->second;

}
