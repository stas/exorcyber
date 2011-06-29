#pragma once
#include <map>
#include <string>

using namespace std;

class message {
	map<string,string> values;
	const char* _json;
	void invalidateJSon();
public:
	message();
	void clear();
	template<class T> void add(char* name, T value);
	void set(const char* json);
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