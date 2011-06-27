#pragma once

#include "stdafx.h"

/**
 * A quick and dirty way to send aupdates to a virtual Vrpn tracker through thi Android/JsonCpp tracker
 */
class VrpnClient
{
public:
	VrpnClient(void);
	~VrpnClient(void);

	/**
	 * Initializes the client with Vrpn server parameters.
	 *
	 * @param trackerId the Id of the tracker that will receive the updates.
	 * this must not be the id a an trackert that receives the updates from the Android device
	 * @param serverIpDotttedAddress an address in the form 192.168.0.2, not a host name
	 * @param portNumber : the port numer of the Vrpn server as configured in vrpn.cfg
	 */
	bool init(int trackerId, const char* serverIpDottedAddress, int portNumber);
	bool initTablet(int trackerId, int portNumber);
	/**
	 * Send an update to the Vrpn tracker.
	 *
	 * Send a postion/attitude update to the tracker. Currently there is no way to send 
	 * speed and acceleration updates.
	 * The tracker and server must have bee initilized with a call to init()
	 * 
	 * @param pos : the position part of the update
	 * @param quat : the attitude part of the update
	 */
	bool sendTrackerUpdate(const osg::Vec3& pos, const osg::Quat& quat);
	
	/**
	 * Releases the network resources.
	 */
	void release();
private:
	SOCKET _socket;
	enum {
		INVALID_TRACKER = -1
	};
	int _trackerId;
	osg::Vec3 _lastPos;
	osg::Quat _lastQuat;
};
