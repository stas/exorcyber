#include "debug.h"
#include <stdio.h>
#include <stdarg.h>

void debug::log(const char* format, ...) {
#ifdef _DEBUG
	va_list args;
	va_start (args, format);
	vprintf (format, args);
	va_end (args);
#endif
}