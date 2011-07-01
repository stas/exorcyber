#include "message.h"

class socklib : public messager {
	void* lData;
protected:
	bool send(const char* data, size_t size);
	size_t recv(char* data, size_t size);
	//recv, wait buffer-full to return
	bool fill(char* data, size_t size);
	bool connect(short port);
public:
	class exception {
	public:
		int code;
		char* msg;
		exception(int code, char* msg) { this->code = code; this->msg = msg; }
	};
	socklib();
	~socklib();
	void clear();
};