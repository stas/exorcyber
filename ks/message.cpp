#include "message.h"
#include <sstream>

const char* message::JSon() {
	if(NULL== _json) {
		stringstream ss;
		ss << '{';
		//TODO: gen json
		map<string,string>::iterator i = values.begin();
		if(values.end()!= i) ss << "'" << i->first << "':" << i->second;
		for(++i; values.end()!= i; ++i)
			ss << ",'" << i->first << "':" << i->second;
		ss << '}';
		_json = ss.str().data();
	}
	return _json;
}

void message::invalidateJSon() {
	_json = NULL;
}

message::message() {
	_json = NULL;
}

message::~message() {
	invalidateJSon();
}

void message::clear() {
	invalidateJSon();
	values.clear();
}

void message::set(const char* json) {
	clear();
	_json = json;
	//TODO: parse & set values
}

template<class T> void message::add(char* name, T value) {
	stringstream ss;
	ss << value;
	values[name] = ss.str();
}

template<> void message::add(char* name, string value) {
	values[name] = "'"+value+"'";
}

template<> void message::add(char* name, char* value) { add(name, (string)value); }
template<> void message::add(char* name, const char* value) { add(name, (string)value); }

template void message::add(char* name, int value);
template void message::add(char* name, long value);
template void message::add(char* name, float value);
template void message::add(char* name, double value);
template void message::add(char* name, bool value);