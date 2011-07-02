#pragma once

#include "sillysock.h"

class jsonws : public sillysock {
public:
	bool connect(short port);
	bool write(message& m);	//TODO: dare to make it const message&
	bool read(message& m);
};