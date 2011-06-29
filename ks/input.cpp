#include "input.h"
#include "debug.h"
#include <stdio.h>

#include <conio.h>	//windows-only for test purpose

input::input() {
}

input::~input() {
}


bool input::read(message& m) {
	debug::log("\nKB:reading:");
	int c = getch();
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