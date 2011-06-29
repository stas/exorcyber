#ifdef WIN32
#include "socklib.h"
#include "debug.h"
#include <winsock2.h>
#define wsd ((wsData*)lData)

#pragma comment(lib,"ws2_32.lib")

//check return-value
inline void crv(int rv, char* msg = NULL) {
	if(0!= rv) throw socklib::exception(rv, msg);
}

//library data for winsock
struct wsData {
	WSADATA wsa;
	SOCKADDR_IN sinClnt, sinSrvr;
	SOCKET srvr, clnt;
};

socklib::socklib() {
	lData = new wsData();
	debug::log("\nSOCK:starting");
	crv(WSAStartup(MAKEWORD(2,0), &(wsd->wsa)), "initialisation");
}

void sclear(SOCKET& s) {
	if(NULL!= s) {
		closesocket(s);
		s = NULL;
	}
}

socklib::~socklib() {
	sclear(wsd->clnt);
	sclear(wsd->srvr);
	delete lData;
	debug::log("\nSOCK:cleaning");
	crv(WSACleanup(), "clean-up");
}

bool socklib::connect(short port) {
	if(NULL!=  wsd->srvr && wsd->sinSrvr.sin_port != htons(port)) {
		closesocket(wsd->srvr);
		wsd->srvr = NULL;
	}
	if(NULL== wsd->srvr) {
		debug::log("\nSOCK:creating");
		wsd->sinSrvr.sin_family = AF_INET;
		wsd->sinSrvr.sin_addr.s_addr = INADDR_ANY;
		wsd->sinSrvr.sin_port = htons(port);
		wsd->srvr = socket(AF_INET, SOCK_STREAM, 0);
		crv(bind(wsd->srvr, (SOCKADDR*)&(wsd->sinSrvr), sizeof(SOCKADDR_IN)), "bind");
		crv(listen(wsd->srvr, 0), "listen");
	}
	int saSize = sizeof(SOCKADDR_IN);
	debug::log("\nSOCK:listening");
	if((wsd->clnt = accept(wsd->srvr,(SOCKADDR*)&(wsd->sinClnt),&saSize))==INVALID_SOCKET) crv(-1, "accept");
	return present = true;
}

void socklib::clear() {
	sclear(wsd->clnt);
}

bool socklib::write(message& m) {
	const char* msg = m.JSon();
	debug::log("\nSOCK:sending: %s", msg);
	return present = SOCKET_ERROR!= send(wsd->clnt, msg, strlen(msg), 0);
}

bool socklib::read(message& m) {
	//TODO
	return true;
}

#endif