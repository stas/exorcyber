#include "message.h"

class socklib : public messager {
	void* lData;
public:
	class exception {
	public:
		int code;
		char* msg;
		exception(int code, char* msg) { this->code = code; this->msg = msg; }
	};
	socklib();
	~socklib();
	bool connect(short port);
	void clear();
	
	bool write(message& m);
	bool read(message& m);
};