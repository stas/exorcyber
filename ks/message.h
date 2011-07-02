#pragma once
#include <map>
#include <string>
#include "rc.h"

//TODO: json not invalidated

typedef long double numeric;

class jsonValue : public rcd {
public:
	class formatException {
	public:
		string type, source, reason;
		size_t position;
		formatException(string type, string source, size_t position, string reason = "")
			: type(type), source(source), position(position), reason(reason) {}
	};
	virtual string json() = 0;
};

template<class T> class atomicValue : public jsonValue {
	friend class msgValue;
	atomicValue(T value) : value(value) {}
public:
	T value;
	string json();
	operator T() { return value; }
};

class msgValue : public rcr<jsonValue> {
public:
	template<class T> msgValue operator=(T* value) { set(value); return *this; }
	msgValue operator=(numeric value) { return *this = new atomicValue<numeric>(value); }
	msgValue operator=(float value) { return *this = new atomicValue<numeric>(value); }
	msgValue operator=(int value) { return *this = new atomicValue<numeric>(value); }
	msgValue operator=(long value) { return *this = new atomicValue<numeric>(value); }
	msgValue operator=(short value) { return *this = new atomicValue<numeric>(value); }
	msgValue operator=(unsigned int value) { return *this = new atomicValue<numeric>(value); }
	msgValue operator=(unsigned long value) { return *this = new atomicValue<numeric>(value); }
	msgValue operator=(unsigned short value) { return *this = new atomicValue<numeric>(value); }
	msgValue operator=(bool value) { return *this = new atomicValue<bool>(value); }
	msgValue operator=(string value) { return *this = new atomicValue<string>(value); }
	msgValue operator=(const char* value) { return *this = new atomicValue<string>(value); }
	msgValue operator=(char* value) { return *this = new atomicValue<string>(value); }
	
};

class message : public jsonValue, public map<string,msgValue> {
public:
	message();
	void parse(const string& s);
	void parse(const string& s, size_t& pos);

	string json();
	~message();
};

class messager {
public:
	virtual bool write(message& m) = 0;
	virtual bool read(message& m) = 0;
	bool present;
	messager() { present = true; }
};