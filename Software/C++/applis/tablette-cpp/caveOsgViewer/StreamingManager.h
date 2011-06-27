#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

class StreamingManager
{
	
	public:
		void init(const char* tIp);
		void sendDatagram(const std::string* stream);

	private:
		struct sockaddr_in clientAddr;



};