#include "jsonws.h"
#include "debug.h"
#include <sstream>

bool jsonws::send(string s) {
	return socklib::send(s.data(), s.size());
}

string consume(string rdr, size_t& pos, string lmt = "\r\n") {
	size_t s = lmt.size(), l = rdr.find(lmt, pos);
	if(string::npos == l) throw jsonws::exception("Delimiter not found");
	pos += s += l;
	return rdr.substr(pos-s, l);
}

long decode0(string ky) {
	string nbr = "";
	int ns = 0;
	for(string::iterator i=ky.begin(); ky.end()!= i; ++i) {
		if(' '== *i) ++ns;
		else if('0'<= *i && '9'>= *i) nbr += *i;
	}
	if(0== ns || ""== nbr) throw jsonws::exception("Bad client keys");
	return stol(nbr)/ns;
}

bool jsonws::connect(short port) {
	if(!socklib::connect(port)) return false;
#define bufSz	0x200
	char buf[bufSz];
	size_t rs = recv(buf, bufSz), pos = 4;
	string sb(buf, rs);
	while(string::npos== sb.find("\r\n\r\n")) {
		rs = recv(buf, bufSz);
		sb += string(buf, rs);
	}
	try {
		//read GET header
		if('GET '!= *(const long*)sb.data()) throw jsonws::exception("Not a GET request");
		string lclRL = consume(sb, pos, " ");
		string httpVer = consume(sb, pos);
		//read header-lines
		map<string, string> hdrs;
		string hn;
		while(""!= (hn = consume(sb, pos, ": "))) hdrs[hn] = consume(sb, pos);

		if( "Upgrade"!= hdrs["Connection"] || "WebSocket"!= hdrs["Upgrade"]) throw jsonws::exception("Not a WebSocket request");
		map<string, string>::iterator i, e = hdrs.end();
		version = (e!= (i= hdrs.find("Sec-WebSocket-Version"))) ? stoi(i->second) : 0;
		if(0< version) throw jsonws::exception("Unsupported version");	//TODO: later versions
		if(e!= (i= hdrs.find("Sec-WebSocket-Version"))) version = stoi(i->second);
		//TODO: 
		int overload[] = { 8 };
		while(sb.size()-pos < overload[version]) {
			rs = recv(buf, bufSz);
			sb += string(buf, rs);
		}
		switch(version) {
		case 0:
			long k[4] = { decode0(hdrs["Sec-WebSocket-Key1"]), decode0(hdrs["Sec-WebSocket-Key2"]) };
			//TODO: to be continued here
			break;
		}

	} catch(jsonws::exception x) {
		//TODO: send back 5xx
		clear();
		return false;
	}
	return true;
}

bool jsonws::write(message& m) {
	string msg = m.json();
	debug::log("\nSOCK:sending: %s", msg);
	//msg.
	return present = send(msg);
}

bool jsonws::read(message& m) {
#define bufSz	0x200
	char buf[bufSz];
	int rv = recv(buf, bufSz);
	m.parse(buf);
	return true;
}