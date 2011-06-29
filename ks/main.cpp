#include <stdio.h>
#include <conio.h>
#include "socklib.h"
#include "input.h"

int display(socklib::exception x) {
	if(NULL== x.msg) fprintf(stderr, "\nSocket error.");
	else fprintf(stderr, "\nSocket error (%s).", x.msg);
	return x.code;
}

void transmissionLoop(messager& src, messager& dst) {
	message m;
	// woohoo \o/
	while(src.present && src.read(m) && dst.present && dst.write(m)) m.clear();
}

int main(int argc, char* argv[]) {
	int errc = 0;
	try {
		socklib s;
		input k;
		while(s.connect(8081)) {
			//TODO: 2 threads : one k->s & one s->k
			transmissionLoop(k, s);
			s.clear();
		}
	} catch(socklib::exception x) { errc = display(x); }
	getc(stdin);
	return 0;
}