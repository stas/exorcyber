#include <stdio.h>
#include <conio.h>
#include "socklib.h"

int main(int argc, char* argv[]) {
	socklib s;
	int errc;
	try {
		s.init();
		while(s.connect(8081)) {

		}
		s.clean();
	} catch(socklib::exception x) {
		if(NULL== x.msg) fprintf(stderr, "Socket error.");
		else fprintf(stderr, "Socket error (%s).", x.msg);
		return errc;
	}
	return 0;
}