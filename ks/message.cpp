#include "message.h"
#include <sstream>

//Atomic values

//Helpers
size_t parseFig(const string& s, size_t pos) {
	char c;
	do c = s[pos++]; while('0'<= c && '9'>= c);
	return --pos;
}

bool strCmp(const string& s, size_t& pos, const char* cmp) {
	return 0== s.compare(pos, strlen(cmp), cmp);
}

map<char, char> strEscaping, strUnescaping;
void initEscapings() {
	if(0== strEscaping.size()) {
		strEscaping['\\'] = '\\';
		strEscaping['\n'] = 'n';
		strEscaping['\r'] = 'r';
		strEscaping['\t'] = 't';
		strEscaping['\''] = '\'';
		strEscaping['"'] = '"';
		for(map<char,char>::iterator i = strEscaping.begin(); strEscaping.end()!= i; ++i)
			strUnescaping[i->second] = i->first;
	}
}

void trim(const string& s, size_t& pos) {
	pos = s.find_first_not_of(" \n\t", pos);
	if(pos== string::npos) pos = s.size();
}

//numeric
template<> string atomicValue<numeric>::json() {
	return to_string(value);
}

numeric parseN(const string& s, size_t& pos) {
	size_t rp = parseFig(s, pos);
	if('.'== s[rp]) rp = parseFig(s, ++rp);
	if(pos==rp) throw jsonValue::formatException("numeric", s, pos);
	return stod(s.substr(pos, rp-pos));
}

//bool
template<> string atomicValue<bool>::json() {
	return value?"true":"false";
}

bool parseB(const string& s, size_t& pos) {
	if(strCmp(s, pos, "true")) return true;
	if(strCmp(s, pos, "false")) return false;
	throw jsonValue::formatException("bool", s, pos);
}

//string
template<> string atomicValue<string>::json() {
	initEscapings();
	char rpl[3] = { '\\', 0, 0 };
	string rv(value);
	for(size_t i=0; i<rv.size(); ++i) {
		map<char,char>::iterator e = strEscaping.find(rv[i]);
		if(strEscaping.end() != e) {
			rpl[1] = e->second;
			rv.replace(i++, 1, rpl);
		}
	}
	rv.push_back('\'');
	return string("'") + rv;
}

string parseS(const string& s, size_t& pos) {
	size_t mp = pos, np;
	stringstream ss;
	char srch[] = "\\.";
	srch[1] = s[mp++];
	if('\''!= srch[1] && '"'!= srch[1]) throw jsonValue::formatException("string", s, pos, "Bad string delimiter");
	do {
		np = s.find_first_of(srch, mp);
		if(np== string::npos) throw jsonValue::formatException("string", s, pos, "Unclosed string");
		ss << s.substr(mp, np-mp);
		mp = np;
		if('\\'== s[mp++]) {
			map<char,char>::iterator e = strUnescaping.find(s[mp]);
			if(e== strUnescaping.end())  throw jsonValue::formatException("string", s, mp, "Bad escape character");
			ss << e->second;
			continue;
		}
	} while(false);
	pos = mp;
	return ss.str();
}

//Messages

string message::json() {
	stringstream ss;
	ss << '{';
	map::iterator i = begin();
	if(end()!= i) {
		ss << "'" << i->first << "':" << i->second->json();
		for(++i; end()!= i; ++i)
			ss << ",'" << i->first << "':" << i->second->json();
	}
	ss << '}';
	return ss.str();
}

message::message() { }

message::~message() { }

void message::parse(const string& s) {
	size_t pos = 0;
	parse(s, pos);
	trim(s, pos);
	if(pos != s.size()) throw jsonValue::formatException("object", s, pos, "Remaining characters after message");
}

void message::parse(const string& s, size_t& pos) {
	clear();
	string name;
	if('{'!= s[pos++]) throw jsonValue::formatException("object", s, pos, "'{' expected");
	if('}'!= s[pos++]) --pos; else return;
	do {
		trim(s, pos);
		char c = s[pos];
		if('"'== c || '\''== c) {
			name = parseS(s, pos);
			trim(s, pos);
			if(':'!= s[pos++]) throw jsonValue::formatException("object", s, pos, "':' expected");
		} else {
			size_t p = s.find_first_of(':', pos);
			if(string::npos == p) throw jsonValue::formatException("object", s, pos, "No further ':' found");
			name = s.substr(pos, p-pos);
			pos = ++p;
		}
		trim(s, pos);
		c = s[pos];
		msgValue v;
		if('"'== c || '\''== c) v = parseS(s, pos);
		else if('0'<= c && '9'>= c) v = parseN(s, pos);
		else if('{'== c) {
			message* m = new message();
			m->parse(s, pos);
			v = m;
		} else try {
			v = parseB(s, pos); 
		} catch(jsonValue::formatException x) {
			throw jsonValue::formatException("object", s, pos, "Expected JSON value");
		}
		(*this)[name] = v;
	} while(','== s[pos]);
	if('}'!= s[pos++]) throw jsonValue::formatException("object", s, pos, "'}' or ',' expected");
}