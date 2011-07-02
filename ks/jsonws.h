#pragma once

#include "socklib.h"

class jsonws : public socklib {
	char version;
public:
	class exception : public std::exception {
	public:
		exception(const char* msg) : std::exception(msg) {}
	};
	bool connect(short port);
	bool write(message& m);
	bool read(message& m);
};