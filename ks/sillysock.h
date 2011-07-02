#pragma once
/*
The library we're using for demo-purpose made some silly, redundant and inefficient framing system.
This is just a hack to work quick&dirty : just make jsonws direct descendant of ws to remove it
*/
#include "websocket.h"

class sillysock : public ws {
public:
	bool connect(short port);
	bool write(const string&);
	bool write(message&);
	string read();
};