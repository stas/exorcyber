#pragma once
#include <map>
#include <string>

using namespace std;

class message {
	map<string,string> values;
	char* _json;
	void invalidateJSon();
public:
	message();
	void clear();
	template<class T> void add(char* name, T value);
	const char* JSon();
	~message();
};

class messager {
public:
	virtual bool write(message& m) = 0;
	virtual bool read(message& m) = 0;
	bool present;
	messager() { present = true; }
};