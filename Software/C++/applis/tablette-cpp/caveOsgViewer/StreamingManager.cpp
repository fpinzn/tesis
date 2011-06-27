#include "stdafx.h"
#include "StreamingManager.h"
#include <string.h>
#include <memory.h>


int PORT=7777;
int serverSock, clientSock; /*server socket, and client sock*/
struct sockaddr_in serverAddr; /*server address*/
struct sockaddr_in clientAddr; /*client address*/

void StreamingManager::init(const char* tIp){
 




   WSADATA wsaData;
   WSAStartup(0x0202,&wsaData); /*windows socket startup */

   memset((char*)&clientAddr,0,sizeof(clientAddr));
   clientAddr.sin_family=AF_INET; /*set client address protocol family*/
   clientAddr.sin_addr.s_addr=INADDR_ANY;
   clientAddr.sin_port=htons(0); /*set client port*/

   serverAddr.sin_family=AF_INET;

  
   serverAddr.sin_addr.s_addr=inet_addr(tIp);/*get the ip address*/
	serverAddr.sin_port=htons((u_short)PORT);/*get the port*/
    

   clientSock=socket(PF_INET,SOCK_DGRAM,0);/*create a socket*/
   if (clientSock<0){
     fprintf(stderr,"socket creating failed\n");
     exit(1);
   }

   if (bind(clientSock,(LPSOCKADDR)&clientAddr,sizeof(struct sockaddr))<0){/*bind a client address and port*/
     fprintf(stderr,"bind failed\n");
     exit(1);
   }
}

void StreamingManager::sendDatagram(const std::string* stream){

	const char* test=stream->c_str();
	sendto(clientSock,test,stream->length(),0,(LPSOCKADDR)&serverAddr,sizeof(struct sockaddr));

	return;

}