#pragma once
#include "comon.h"

class debug {
public:
	class exception : public std::exception {
	public:
		exception(const char* msg) : std::exception(msg) {}
	};
	static void log(const char* format, ...);
	inline static void assert(bool a) {
#ifdef _DEBUG
		if(!a) throw exception("Assertion failed.");
#endif
	}
};