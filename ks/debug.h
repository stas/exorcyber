#pragma once

class debug {
public:
	class exception {};
	static void log(const char* format, ...);
	inline static void assert(bool a) {
#ifdef _DEBUG
		if(!a) throw exception();
#endif
	}
};