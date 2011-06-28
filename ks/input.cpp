#include "input.h"
#include <stdio.h>

input::input() {
}

input::~input() {
}


bool input::read(message& m) {
	int c = getchar();
	char s[5] = {0, 0, 0, 0, 0};
	*(int*)s = c;
	m.add("event", "keypress");
	m.add("key", c);
	m.add("char", s);
	return true;
}

bool input::write(message& m) {
	printf(m.JSon());
	return true;
}