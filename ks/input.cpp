#include "input.h"
#include "debug.h"
#include <stdio.h>

#include <conio.h>	//windows-only for test purpose

input::input() {
}

input::~input() {
}

bool input::read(message& m) {
	debug::log("\nCON:reading:");
	int c = _getch();
	char s[5] = {0, 0, 0, 0, 0};
	*(int*)s = c;
	m.clear();
	m["event"] = "keypress";
	m["key"] = c;
	m["char"] = s;
	return true;
}

bool input::write(message& m) {
	printf("\nCON:receive: %s", m.json().data());
	return true;
}