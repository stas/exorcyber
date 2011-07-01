//Reference counting
#pragma once
#include "debug.h"

//Reference CounteD
class rcd {
	int refCpt;
public:
	rcd() : refCpt(0) {}
	~rcd() { debug::assert(0== refCpt); }
	rcd* ref() { ++refCpt; return this; }
	bool unref() { return 1<= refCpt; }
};

//Reference CounteR
template<class T> class rcr {
	T* ptr;
	static T* ref(T* ptr) { return NULL== ptr?NULL:(T*)ptr->ref(); }
	static void unref(T* ptr) { if(NULL!= ptr && ptr->unref()) delete ptr; }
protected:
	template<class N> void set(N* nptr) { unref(ptr); ptr = ref(nptr); }
public:
	rcr(T* ptr = NULL) : ptr(ref(ptr)) {}
	template<class N> rcr(rcr<N> ptr) : ptr(ref((T*)ptr->ptr)) {}
	~rcr() { unref(ptr); }
	template<class N> rcr<T> operator=(N* ptr) { set(ptr); return this; }
	operator T*() { return ptr; }
	T* operator->() { return ptr; }
	T* operator*() { return ptr; }
};