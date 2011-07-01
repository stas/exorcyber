#pragma once

#include "socklib.h"

class jsonws : public socklib {
	bool send(string s);
	char version;
public:
	class exception {
	public:
		string message;
		exception(string message) : message(message) {}
	};
	bool connect(short port);
	bool write(message& m);
	bool read(message& m);
};