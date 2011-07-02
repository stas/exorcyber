#pragma once

class thread {
public:
	typedef void (*proc)(void* prm);
private:
	void* lData;
	proc p;
	void start(void* prm);
public:
	//create thread and start it
	thread(proc p, void* prm = NULL) : p((proc)p) { start(prm); }
	template<class T> thread(void (*p)(T* prm), T& prm) : p((proc)p) { start((void*)&prm); }
	template<class T> thread(void (*p)(T* prm)) : p((proc)p) { start(NULL); }
		~thread() { if(running) abort(); }
	//args: number-of-threads, thread*, thread*, ...
	//returns: index of thread who woke up
	static int waitOne(int nt, ...);
	bool running;
	void abort();
	void wait();
};