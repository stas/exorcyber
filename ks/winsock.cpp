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
		debug::log("\nSOCK:disconnected");
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

bool socklib::send(const string& s) {
	return SOCKET_ERROR!= ::send(wsd->clnt, s.data(), s.size(), 0);
}

string socklib::recv() {
	static size_t bufSize = 0x200;
	char* data = new char[bufSize];
	int rvl;
	do rvl = ::recv(wsd->clnt, data, bufSize, 0);
	while(SOCKET_ERROR== rvl && WSAEMSGSIZE== WSAGetLastError() && 0<(bufSize=rvl));
	if(SOCKET_ERROR== rvl) {
		debug::log("\nrecv error : %d", WSAGetLastError());
		return "";
	}
	string rv(data, rvl);
	delete data;
	return rv;
}

bool socklib::fill(char* data, size_t size) {
	return size== ::recv(wsd->clnt, data, size, MSG_WAITALL);
}

#endif