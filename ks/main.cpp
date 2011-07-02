#include <stdio.h>
#include <conio.h>
#include "jsonws.h"
#include "input.h"
#include "threadlib.h"

int display(exception x) {
	if(NULL== x.what()) fprintf(stderr, "\nError.");
	else fprintf(stderr, "\nError (%s).", x.what());
	return -1;
}

struct tlParms {
	tlParms(messager& src, messager& dst) : src(src), dst(dst) {}
	messager& src,& dst;
};

void halfDuplex(tlParms* tlp) {
	message m;
	try {
		// woohoo \o/
		while(tlp->src.present && tlp->src.read(m) && tlp->dst.present && tlp->dst.write(m)) m.clear();
	} catch(exception x) { display(x); }
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
	} catch(exception x) { errc = display(x); }
	printf("\npak");
	getc(stdin);
	return 0;
}