
//#include <windows.h>
#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdlib.h>

#include "json/json.h"

#include "vrpn_Tracker_Android.h"
#include "quat.h"

// These must match definitions in eu.ensam.ii.vrpn.Vrpn
static const char* const MSG_KEY_TYPE =				"type";
static const char* const MSG_KEY_SEQUENCE_NUMBER =	"sn";
static const char* const MSG_KEY_TIMESTAMP =		"ts";
	
static const char* const MSG_KEY_TRACKER_ID =		"id";
//static const char* const MSG_KEY_TRACKER_TYPE =		"st";
static const char* const MSG_KEY_TRACKER_QUAT =		"quat";
static const char* const MSG_KEY_TRACKER_POS =		"pos";

static const char* const MSG_KEY_BUTTON_ID =		"button";
static const char* const MSG_KEY_BUTTON_STATUS =	"state";
	
static const char* const MSG_KEY_ANALOG_CHANNEL =	"num";
static const char* const MSG_KEY_ANALOG_DATA =		"data";
static const char* const MSG_KEY_ANNOTATION=		"annotation";
// Message types (values for MSG_KEY_TYPE)
static const int MSG_TYPE_TRACKER = 1;
static const int MSG_TYPE_BUTTON = 2;
static const int MSG_TYPE_ANALOG = 3;
static const int MSG_TYPE_DATA = 4;

vrpn_Tracker_GTab::vrpn_Tracker_GTab(const char* name,vrpn_Connection* c,int udp_port) :
	vrpn_Tracker(name, c),
	vrpn_Button(name, c),
	vrpn_Analog(name, c),
	_socket(INVALID_SOCKET),
	_pJsonReader(0)
{
	fprintf(stderr, "vrpn_Tracker_GTab : Device %s listen on port udp port %d\n", name, udp_port);
	if (! _network_init(udp_port)) {
		exit(EXIT_FAILURE);
	}

	// Buttons part

	num_buttons = vrpn_BUTTON_MAX_BUTTONS;
	num_channel = vrpn_CHANNEL_MAX;

	_pJsonReader = new Json::Reader();
}

vrpn_Tracker_GTab::~vrpn_Tracker_GTab(void)
{
	if (_pJsonReader != 0) {
		delete _pJsonReader;
		_pJsonReader = 0;
	}
	_network_release();
}


void vrpn_Tracker_GTab::mainloop() {
	server_mainloop();
	/*
	 * The original Dtrack code uses blocking call to select() in _network_receive with 
	 * a 1 sec timeout. In Dtrack, the data is supposed to be continuously flowing (app. 60 Hz), 
	 * so the timeout is unlikely to happen. However, the data from the Android device flow at a lower
	 * frequency and may not flow at all if the tilt tracker is disabled. 
	 * Thus a 1 sec timeout here causes latency and jerky movements in Dtrack 
	 */
	const int timeout_us = 10 * 1000;

	//int received_length = _network_receive(_network_buffer, _NETWORK_BUFFER_SIZE, 1*1000*1000);
	int received_length = _network_receive(_network_buffer, _NETWORK_BUFFER_SIZE, timeout_us);


	if (received_length < 0) {
		//fprintf(stderr, "vrpn_Tracker_GTab : receive error %d\n", received_length);
		return;
	} 
	_network_buffer[received_length] = '\0';
	//fprintf(stderr, "got data : %.*s\n", received_length, _network_buffer);
	if (!_parse(_network_buffer, received_length)) {
		// whatever error

		return;
	}

	// report trackerchanges
	// TODO really use timestamps
	struct timeval ts ;
	// from vrpn_Tracker_DTrack::dtrack2vrpnbody
	if (d_connection) {
		char msgbuf[1000];
		// Encode pos and d_quat
		int len = vrpn_Tracker::encode_to(msgbuf);
		if (d_connection->pack_message(len, ts, position_m_id, d_sender_id, msgbuf, vrpn_CONNECTION_LOW_LATENCY)) {
			// error
		}
		//fprintf(stderr, "Packed and sent\n");
	}

	vrpn_Button::report_changes();
	vrpn_Analog::report_changes();

}

bool vrpn_Tracker_GTab::_parse(const char* buffer, int length) {
	Json::Value root;							// will contains the root value after parsing.
	// Beware collectcomment = true crashes
	bool parsingSuccessful = _pJsonReader->parse( buffer, root , false);
	if ( !parsingSuccessful ) {
		// report to the user the failure and their locations in the document.
		fprintf(stderr, "vrpn_Tracker_GTab parse error :%s\n",
						 _pJsonReader->getFormatedErrorMessages());
		fprintf(stderr, "%s\n",buffer);
		return false;
	}

	const Json::Value& constRoot = root;
	// Find MessageType
	const Json::Value& type = constRoot[MSG_KEY_TYPE]; 
	int messageType;
	if (!type.empty() && type.isConvertibleTo(Json::intValue)) {
		messageType = type.asInt();
		// HACK
	} else {
		fprintf(stderr, "vrpn_Tracker_GTab parse error : missing message type\n");
		return false;
	}
	switch (messageType) {
		// TODO cleanup
		case MSG_TYPE_TRACKER:
			return _parse_tracker_data(root);
			break;
		case MSG_TYPE_BUTTON:
			return _parse_button(root);
			break;
		case MSG_TYPE_ANALOG:
			return _parse_analog(root);
			break;
		case MSG_TYPE_DATA:
			return _parse_data(root);
			break;
		default:
			;
	}
	return false;

	
}


bool vrpn_Tracker_GTab::_parse_data(const Json::Value& root) {
	const Json::Value& constRoot = root;
	std::cout<<"Data: "<< root;
	struct timeval timestamp;
	timestamp.tv_sec=10;

	this->send_text_message(root.toStyledString().c_str(),timestamp,vrpn_TEXT_NORMAL,0);
	return true;
}

bool vrpn_Tracker_GTab::_parse_tracker_data(const Json::Value& root) {
	const Json::Value& constRoot = root;
#if 0
	// Timestamp not OK with simulator
	const Json::Value& valueTimestamp = constRoot[MSG_KEY_TIMESTAMP];
	if (!valueTimestamp.empty() && valueTimestamp.isConvertibleTo(Json::intValue)) {
		//	timestamp = valueTimestamp.asInt();
	} else {
		return false;
	}

	// Sequence number
	const Json::Value& valueSequenceNumber = constRoot[MSG_KEY_SEQUENCE_NUMBER];
	if (!valueSequenceNumber.empty() && valueSequenceNumber.isConvertibleTo(Json::intValue)){
		//sequenceNumber = valueSequenceNumber.asInt();
	} else {
		//return false;
	}
#endif

	// Id of the current sensor
	const Json::Value& sensorId = constRoot[MSG_KEY_TRACKER_ID];
	if (!sensorId.empty() && sensorId.isConvertibleTo(Json::intValue)){
		this->d_sensor = sensorId.asInt();
	} else {
		return false;
	}

	/*
	 * mainloop calls vrpn_Tracker::encode_to, that will send d_sensor, d_pos and d_quat.
	 * velocity and acceleration are curretly not handled
	 */
	 
	/*
	 * Look for an attitude and encode the angles into a quat
	 */
	const Json::Value& quatData = constRoot[MSG_KEY_TRACKER_QUAT];
	if (!quatData.empty() && quatData.isArray()) {
		if ( quatData.size() == 3) {
			/*
			 * The rotation rotates Y into the vector provided.
			 * Retrieve euler angles and compute the orientation quaternion
			 * The input from device is in OpenGl coordinates
			 * right = X , rX = pitch
			 * up = Y , rY = yaw
			 * to the eye = Z, rZ = roll
			 */		
			const double rX = quatData[0u].asDouble();
			const double rY = quatData[1].asDouble();
			const double rZ = quatData[2].asDouble();
			//fprintf(stderr, "yaw-rY %.3f pitch-rX %.3f roll-rZ %.3f \n", rY, rX, rZ);
			q_from_euler (
				this->d_quat,
				Q_DEG_TO_RAD(rZ),		// rZ
				Q_DEG_TO_RAD(rY),		// rY
				Q_DEG_TO_RAD(rX)		// rX
				);
		} else if (quatData.size() == 4) {
			this->d_quat[0] = quatData[0u].asDouble();
			this->d_quat[1] = quatData[1].asDouble();
			this->d_quat[2] = quatData[2].asDouble();
			this->d_quat[3] = quatData[3].asDouble();
		}
	} 

	/*
	 * Look for a position 
	 */
	const Json::Value& posData = constRoot[MSG_KEY_TRACKER_POS];
	if (!posData.empty() && posData.isArray() &&  posData.size() == 3) {
		this->pos[0] = posData[0u].asDouble();
		this->pos[1]= posData[1].asDouble();
		this->pos[2]= posData[2].asDouble();
	} 

	return true;
}


bool vrpn_Tracker_GTab::_parse_button(const Json::Value& root) {
	const Json::Value& valueButtonStatus = root[MSG_KEY_BUTTON_STATUS]; 
	bool buttonStatus;
	if (!valueButtonStatus.empty() && valueButtonStatus.isConvertibleTo(Json::booleanValue)) {
		buttonStatus = valueButtonStatus.asBool();
	} else {
		fprintf(stderr, "vrpn_Tracker_GTab::_parse_button parse error : missing status");
		return false;
	}

#if 0
	const int NOT_ATTACHED_TO_A_TRACKER = -1;
	const Json::Value& valueTrackerId = root[MSG_KEY_TRACKER_ID];
	int trackerId = NOT_ATTACHED_TO_A_TRACKER;
	if (!valueTrackerId.empty() && valueTrackerId.isConvertibleTo(Json::intValue)) {
		trackerId = valueTrackerId.asInt();
	} 

	const Json::Value& valueButtonId = root[MSG_KEY_BUTTON_ID]; 
	int messageButtonId;		// buttonId embedded in the message. 
	if (!valueButtonId.empty() && valueButtonId.isConvertibleTo(Json::intValue)) {
		messageButtonId = valueButtonId.asInt();
	} else {
		fprintf(stderr, "vrpn_Tracker_GTab::_parse_button parse error : missing id\n");
		return false;
	}
#else


	const Json::Value& valueButtonId = root[MSG_KEY_BUTTON_ID]; 
	int buttonId;		// buttonId embedded in the message. 
	if (!valueButtonId.empty() && valueButtonId.isConvertibleTo(Json::intValue)) {
		buttonId = valueButtonId.asInt();
	} else {
		fprintf(stderr, "vrpn_Tracker_GTab::_parse_button parse error : missing id\n");
		return false;
	}


#endif
#if 0
	// Timestamp not OK with simulator
	const Json::Value& valueTimestamp = constRoot[MSG_KEY_TIMESTAMP];
	if (!valueTimestamp.empty() && valueTimestamp.isConvertibleTo(Json::intValue)) {
		//	timestamp = valueTimestamp.asInt();
	} else {
		return false;
	}

	// not used
	const Json::Value& valueSequenceNumber = root[MSG_KEY_SEQUENCE_NUMBER];
	if (!valueSequenceNumber.empty() && valueSequenceNumber.isConvertibleTo(Json::intValue)){
		//sequenceNumber = valueSequenceNumber.asInt();
	} else {
		//return false;
	}

#endif



	if (buttonId < 0) {
		fprintf(stderr, "invalid button Id %d (max : %d)\n", buttonId, num_buttons);
	} else {
		buttons[buttonId] = (int)buttonStatus;
	}

	return true;
}



bool vrpn_Tracker_GTab::_parse_analog(const Json::Value& root) {
	const Json::Value& valueData = root[MSG_KEY_ANALOG_DATA]; 
	double data;
	if (!valueData.empty() && valueData.isConvertibleTo(Json::realValue)) {
		data = valueData.asDouble();
	} else {
		fprintf(stderr, "vrpn_Tracker_GTab::_parse_analog parse error : missing status");
		return false;
	}
	

	const Json::Value& channelNumberId = root[MSG_KEY_ANALOG_CHANNEL]; 
	int channelNumber;
	if (!channelNumberId.empty() && channelNumberId.isConvertibleTo(Json::intValue)) {
		channelNumber = channelNumberId.asInt();
	} else {
		fprintf(stderr, "vrpn_Tracker_GTab::_parse_analog parse error : missing id\n");
		return false;
	}


#if 0
	// Timestamp not OK with simulator
	const Json::Value& valueTimestamp = constRoot[MSG_KEY_TIMESTAMP];
	if (!valueTimestamp.empty() && valueTimestamp.isConvertibleTo(Json::intValue)) {
		//	timestamp = valueTimestamp.asInt();
	} else {
		return false;
	}
	
	// not used
	const Json::Value& valueSequenceNumber = root[MSG_KEY_SEQUENCE_NUMBER];
	if (!valueSequenceNumber.empty() && valueSequenceNumber.isConvertibleTo(Json::intValue)){
		//sequenceNumber = valueSequenceNumber.asInt();
	} else {
		//return false;
	}
#endif


	
	if (channelNumber < 0 || channelNumber >= num_channel) {
		fprintf(stderr, "vrpn_Tracker_GTab::_parse_analog id out of bounds %d/%d\n", channelNumber, num_channel);
	} else {
		channel[channelNumber] = data;
	}

	return true;
}


bool vrpn_Tracker_GTab::_network_init(int udp_port) {
	int iResult;

    // Initialize Winsock
    WORD versionRequested =  MAKEWORD(2,2);
    WSADATA wsaData;

	iResult = WSAStartup(versionRequested, &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    // Create a SOCKET for connecting to server
	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        //freeaddrinfo(result);
        WSACleanup();
        return false;
    }
	struct sockaddr_in localSocketAddress;
	localSocketAddress.sin_family = AF_INET;
	localSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	localSocketAddress.sin_port = htons(udp_port);

    // Setup the listening socket
	iResult = bind( _socket, (struct sockaddr*)&localSocketAddress, sizeof(localSocketAddress));
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        //freeaddrinfo(result);
		_network_release();
        return false;
    }

    //freeaddrinfo(result);
	return true;

}

// Right from vrpn_tracker_DTrack::udp_receive
int vrpn_Tracker_GTab::_network_receive(void *buffer, int maxlen, int tout_us)
{
	int nbytes, err;
	fd_set set;
	struct timeval tout;

	// waiting for data:

	FD_ZERO(&set);
	FD_SET(_socket, &set);

	tout.tv_sec = tout_us / 1000000;
	tout.tv_usec = tout_us % 1000000;

	static int received = 0;
	switch((err = select(FD_SETSIZE, &set, NULL, NULL, &tout))){
		case 1:
			//fprintf(stderr, "received %d\n", ++received);
			break;        // data available
		case 0:
			//fprintf(stderr, "net_receive: select timeout (err = 0)\n");
			return -1;    // timeout
			break;
		default:
			//fprintf(stderr, "net_receive: select error %d\n", err);
			return -2;    // error

	}

	// receiving packet:

	while(1){

		// receive one packet:
		nbytes = recv(_socket, (char *)buffer, maxlen, 0);
		if(nbytes < 0){  // receive error
			//fprintf(stderr, "recv_receive: select error %d\n", err);
			return -3;
		}

		// check, if more data available: if so, receive another packet
		FD_ZERO(&set);
		FD_SET(_socket, &set);

		tout.tv_sec = 0;   // timeout with value of zero, thus no waiting
		tout.tv_usec = 0;

		if(select(FD_SETSIZE, &set, NULL, NULL, &tout) != 1){
			// no more data available: check length of received packet and return
			if(nbytes >= maxlen){   // buffer overflow
      			return -4;
		   }
		return nbytes;
		}
	}
}

void vrpn_Tracker_GTab::_network_release() {
	closesocket(_socket);
	WSACleanup();
}