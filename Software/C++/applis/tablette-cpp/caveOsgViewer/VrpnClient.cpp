#include "StdAfx.h"

#include "json/json.h"


#include "VrpnClient.h"

VrpnClient::VrpnClient(void):
_socket(INVALID_SOCKET),
_trackerId(INVALID_TRACKER)
{
}

VrpnClient::~VrpnClient(void)
{
	release();
}

bool VrpnClient::init(int trackerId, const char* serverIpDottedAddress, int serverPortNumber) {
	_trackerId = trackerId;
	std::cout<<"VRPN: "<<trackerId<<"\n "<<serverIpDottedAddress<<":"<<serverPortNumber<<"\n\n";
	int iResult;

    // Initialize Winsock
    WORD versionRequested =  MAKEWORD(2,2);
    WSADATA wsaData;

	iResult = WSAStartup(versionRequested, &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }
    
	struct addrinfo *result = NULL;
	struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;



    // Create a SOCKET for connecting to server
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }
	struct sockaddr_in remoteEndpoint;
	remoteEndpoint.sin_family = AF_INET;
	remoteEndpoint.sin_addr.s_addr = inet_addr(serverIpDottedAddress);
	remoteEndpoint.sin_port = htons(serverPortNumber);
	iResult = connect(_socket, (SOCKADDR*)&remoteEndpoint, sizeof(remoteEndpoint));
    if ( iResult != 0 ) {
        printf("connect failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

	return true;
}



void VrpnClient::release() {
	if (_socket != INVALID_SOCKET) {
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
    WSACleanup();
	_trackerId = INVALID_TRACKER;
}

bool VrpnClient::sendTrackerUpdate(const osg::Vec3& pos, const osg::Quat& quat) {

	if (pos == _lastPos && quat == _lastQuat) {
		// A shortcut if there is no change. To be tested
		//return true;
	}

	// Same as defined in vrpn_Tracker_Android or vrpn_Tracker;JsonCpp
	static const char* const MSG_KEY_TYPE =				"type";
	static const char* const MSG_KEY_SEQUENCE_NUMBER =	"sn";	// not used
	static const char* const MSG_KEY_TIMESTAMP =		"ts";	// not used
		
	static const char* const MSG_KEY_TRACKER_ID =		"id";
	static const char* const MSG_KEY_TRACKER_QUAT =		"quat";
	static const char* const MSG_KEY_TRACKER_POS =		"pos";

	static const char* const MSG_KEY_BUTTON_ID =		"button";
	static const char* const MSG_KEY_BUTTON_STATUS =	"state";
		
	static const char* const MSG_KEY_ANALOG_CHANNEL =	"num";
	static const char* const MSG_KEY_ANALOG_DATA =		"data";
	
	static const int MSG_TYPE_TRACKER = 1;
	static const int MSG_TYPE_BUTTON = 2;
	static const int MSG_TYPE_ANALOG = 3;
	
	Json::Value root;
	root[MSG_KEY_TYPE] = MSG_TYPE_TRACKER;
	
	// oops should be _trackerId
	root[MSG_KEY_TRACKER_ID] = 2;

	Json::Value posArray;
	posArray.append(pos[0]);
	posArray.append(pos[1]);
	posArray.append(pos[2]);
	root[MSG_KEY_TRACKER_POS] = posArray;
	
	Json::Value quatArray;
	quatArray.append(quat[0]);
	quatArray.append(quat[1]);
	quatArray.append(quat[2]);
	quatArray.append(quat[3]);
	root[MSG_KEY_TRACKER_QUAT] = quatArray;

	Json::FastWriter writer;
	std::string jsonString = writer.write(root);
	send(_socket, jsonString.c_str(), jsonString.length(), 0);
	_lastPos = pos;
	_lastQuat = quat;

	return true;

}
