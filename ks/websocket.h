#pragma once

#include "socklib.h"

class ws : public socklib {
	char version;
	string rcvBuf;
public:
	class exception : public std::exception {
	public:
		exception(const char* msg) : std::exception(msg) {}
	};
	bool connect(short port);
	bool write(const string& s);
	string read();
};