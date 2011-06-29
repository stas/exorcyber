#ifdef WIN32

#include <Windows.h>
#include <stdarg.h>
#include "threadlib.h"

#define wtd ((wtData*)lData)

struct wtData {
	LPDWORD id;
	HANDLE hndl;
};

int thread::waitOne(int nt, ...) {
	va_list vl;
	va_start(vl, nt);
	HANDLE* tbl = new HANDLE[nt];
	for(int i=0; i<nt; ++i)
		tbl[i] = ((wtData*)(va_arg(vl,thread*)->lData))->hndl;
	va_end(vl);
	DWORD rv = WaitForMultipleObjects(nt, tbl, false, INFINITE);
	delete tbl;
	if(WAIT_OBJECT_0<= rv && WAIT_OBJECT_0+nt> rv) return rv-WAIT_OBJECT_0;
	if(WAIT_ABANDONED_0<= rv && WAIT_ABANDONED_0+nt> rv) return rv-WAIT_ABANDONED_0;
	return -1;
}

struct launcherParms {
	thread::proc p;
	void* prm;
	bool* running;
};

DWORD WINAPI launcher(__in LPVOID lpParameter) {
	launcherParms* prms = (launcherParms*)lpParameter;
	prms->p(prms->prm);
	*prms->running = false;
	return 0;
}


void thread::start(void* prm = NULL)
{
	lData = new wtData();
	launcherParms prms;
	prms.p = p;
	prms.prm = prm;
	prms.running = &running;
	wtd->hndl = CreateThread(NULL, 0, launcher, &prms, 0, wtd->id);
	running = NULL!= wtd->hndl;
}

void thread::abort() {
	if(running) {
		TerminateThread(wtd->hndl, 0);
		running = false;
	}
	delete lData;
}

void thread::wait() {
	WaitForSingleObject(wtd->hndl, INFINITE);
}

#endif