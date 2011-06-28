#include "message.h"

class input : public messager {
public:
	input();
	~input();
	
	bool write(message& m);
	bool read(message& m);
};