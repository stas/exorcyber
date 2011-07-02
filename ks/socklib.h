#include "message.h"

class socklib : public messager {
	void* lData;
protected:
	bool send(const string& s);
	string recv();
	//recv, wait buffer-full to return
	string fill(size_t size);
	bool connect(short port);
public:
	class exception : public std::exception {
	public:
		int code;
		exception(int code, const char* msg) : std::exception(msg), code(code) {}
	};
	socklib();
	~socklib();
	void clear();
};