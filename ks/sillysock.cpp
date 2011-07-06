#include "sillysock.h"
#include "message.h"
#include "debug.h"
#include <sstream>

bool sillysock::connect(short port) {
	if(!ws::connect(port)) return false;
	write("someWTFsessionId");
}

bool sillysock::write(const string& s) {
	stringstream ss;
	ss << "~m~" << s.size() << "~m~" << s;
	return ws::write(ss.str());
}

bool sillysock::write(message& m) {
	stringstream ss;
	ss << "~j~" << m.json();
	return write(ss.str());
}

string sillysock::read() {
	string rv = ws::read();
	//rv is '~m~TheLengthOfMyFrameIAlreadyKnow~m~", so I spot the before-last "~"
	size_t skip = rv.find_first_of("~", 3);
	return rv.substr(skip+3);
}