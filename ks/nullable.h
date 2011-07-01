template<class T> class nullable {
public:
	class exception { };
	T value;
	bool hasValue;
	nullable<T>() : hasValue(false) {}
	nullable<T>(const T& value) : hasValue(true), value(value) {}
	nullable<T> operator=(void* null) { hasValue = false; }
	nullable<T> operator=(T v) { hasValue = true; value = v; }
	operator T() {
		if(!hasValue) throw exception();
		return value;
	}
};