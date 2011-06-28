#ifdef WIN32
#include "socklib.h"
#include <winsock2.h>
#define wsd ((wsData*)lData)

#pragma comment(lib,"ws2_32.lib")

//check return-value
inline void crv(int rv, char* msg = NULL) {
	if(0!= rv) throw socklib::exception(rv, msg);
	crv(WSACleanup(), "clean-up");
}

//library data for winsock
struct wsData {
	WSADATA wsa;
	SOCKADDR_IN sinClnt, sinSrvr;
	SOCKET srvr, clnt;
};

socklib::socklib() {
	lData = new wsData();
	crv(WSAStartup(MAKEWORD(2,0), &(wsd->wsa)), "initialisation");
}

socklib::~socklib() {
	delete lData;
}

bool socklib::connect(short port) {
	wsd->sinSrvr.sin_family = AF_INET;
	wsd->sinSrvr.sin_addr.s_addr = INADDR_ANY;
	wsd->sinSrvr.sin_port = htons(port);

	wsd->srvr = socket(AF_INET, SOCK_STREAM, 0);
	crv(bind(wsd->srvr, (SOCKADDR*)&(wsd->sinSrvr), sizeof(SOCKADDR_IN)), "binding");
	crv(listen(wsd->srvr, 0), "listening");
	int saSize = sizeof(SOCKADDR_IN);
	if((wsd->clnt=accept(wsd->srvr,(SOCKADDR*)&(wsd->sinClnt),&saSize))==INVALID_SOCKET) crv(-1, "accetping");
	return true;
}

void socklib::clear() {
	shutdown(wsd->clnt, 2);
}

bool socklib::write(message& m) {
	const char* msg = m.JSon();
	if(SOCKET_ERROR== send(wsd->clnt, msg, strlen(msg), 0))
		return present = false;
	return true;
}

bool socklib::read(message& m) {
	//TODO
	return true;
}

#endif