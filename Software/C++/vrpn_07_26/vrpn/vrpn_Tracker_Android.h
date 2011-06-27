#pragma once

//#include <windows.h>
#include <winsock.h>
//#include <ws2tcpip.h>
//#include "json/json.h"

#include "vrpn_tracker.h"
#include "vrpn_button.h"
#include "vrpn_analog.h"

namespace Json {
	class Reader;
	class Value;
}

class vrpn_Tracker_GTab :
	public vrpn_Tracker, public vrpn_Button, public vrpn_Analog
{
public:
	vrpn_Tracker_GTab(
		const char* name,
		vrpn_Connection* c,
		int udpPort
		);
	~vrpn_Tracker_GTab(void);

	void mainloop();
	enum {
		TILT_TRACKER_ID = 0,
	};

	
private:
	// Network stuff
	bool _network_init(int udp_port);
	int _network_receive(void *buffer, int maxlen, int tout_us);
	void _network_release();
	SOCKET _socket;
	enum {
		_NETWORK_BUFFER_SIZE = 2000,

	};

	char _network_buffer[_NETWORK_BUFFER_SIZE];

	// Json stuff
	bool _parse(const char* buffer, int length);
	bool _parse_tracker_data(const Json::Value& root);
	bool _parse_analog(const Json::Value& root);
	bool _parse_button(const Json::Value& root);
	bool _parse_data(const Json::Value& root);
	Json::Reader* _pJsonReader;
};
