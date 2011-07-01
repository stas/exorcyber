#include <stdio.h>
#include <conio.h>
#include "jsonws.h"
#include "input.h"
#include "threadlib.h"

int display(socklib::exception x) {
	if(NULL== x.msg) fprintf(stderr, "\nSocket error.");
	else fprintf(stderr, "\nSocket error (%s).", x.msg);
	return x.code;
}

struct tlParms {
	tlParms(messager& src, messager& dst) : src(src), dst(dst) {}
	messager& src,& dst;
};

void halfDuplex(tlParms* tlp) {
	message m;
	// woohoo \o/
	while(tlp->src.present && tlp->src.read(m) && tlp->dst.present && tlp->dst.write(m)) m.clear();
}

void fullDuplex(messager& a, messager& b) {
	thread::waitOne(2, &thread(halfDuplex, tlParms(a,b)), &thread(halfDuplex, tlParms(b,a)));
}

int main(int argc, char* argv[]) {
	int errc = 0;
	try {
		jsonws s;
		input k;
		while(s.connect(8081)) {
			fullDuplex(k, s);
			s.clear();
		}
	} catch(socklib::exception x) { errc = display(x); }
	getc(stdin);
	return 0;
}