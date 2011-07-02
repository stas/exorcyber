#include "jsonws.h"
#include "debug.h"

bool jsonws::connect(short port) {
	if(!sillysock::connect(port)) return false;
}

bool jsonws::write(message& m) {
	return sillysock::write(m.json());
}

bool jsonws::read(message& m) {
	m.parse(sillysock::read());
	return true;
}