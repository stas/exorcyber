#include "jsonws.h"
#include "debug.h"
#include "md5.h"
#include <sstream>

int overload[] = { 8 };	//Length of raw data given after the HTTP request header, in function of the version number

string consume(string rdr, size_t& pos, string lmt = "\r\n") {
	size_t s = lmt.size(), l = rdr.find(lmt, pos) - pos;
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
	size_t pos = 4;
	string sb = recv();
	while(string::npos== sb.find("\r\n\r\n")) sb += recv();
	try {
		//read GET header
		if(' TEG'!= *(const long*)sb.data()) throw jsonws::exception("Not a GET request");
		string path = consume(sb, pos, " ");
		string httpVer = consume(sb, pos);
		//read header-lines
		map<string, string> hdrs;
		string hl, hn;
		size_t hv;
		while(""!= (hl = consume(sb, pos))) {
			hv = 0;
			hn = consume(hl, hv, ": ");
			hdrs[hn] = hl.substr(hv);
		}

		if( "Upgrade"!= hdrs["Connection"] || "WebSocket"!= hdrs["Upgrade"]) throw jsonws::exception("Not a WebSocket request");
		map<string, string>::iterator i, e = hdrs.end();
		version = (e!= (i= hdrs.find("Sec-WebSocket-Version"))) ? stoi(i->second) : 0;
		if(0< version) throw jsonws::exception("Unsupported version");	//TODO: later versions
		while(sb.size()-pos < overload[version]) sb += recv();
		stringstream out;
		bool secure = false;	//TODO: read 'secure' flag
		short dftPort = secure?443:80;
		out << "HTTP/1.1 101 ExorCyber\r\n"
			<< "Upgrade: WebSocket\r\n"
			<< "Connection: Upgrade\r\n"
			<< "Sec-WebSocket-Origin: " << hdrs["Origin"] << "\r\n"
			<< "Sec-WebSocket-Location: ws"
				<< (secure?"s":"") 
				<< "://"
				<< hdrs["Host"]/*;
		if(port!=dftPort)
			out << ':' << port;
		out	*/	<< path
				<< "\r\n";
		if(e!= (i= hdrs.find("Sec-WebSocket-Protocol")))
			out << "Sec-WebSocket-Protocol: " << i->second << "\r\n";
		switch(version) {
		case 0:
			long k[4] = { decode0(hdrs["Sec-WebSocket-Key1"]), decode0(hdrs["Sec-WebSocket-Key2"]) };
			const long* clntK = (const long*)(sb.data()+pos);
			k[2] = clntK[0]; k[3] = clntK[1];
			MD5 md5;
			md5.update((const char*)k, 16);
			md5.finalize();
			out << "\r\n" << string((char*)md5.digest, 16);
			break;
		}
		send(out.str());
	} catch(jsonws::exception x) {
		//TODO: send back 5xx
		clear();
		debug::log("\nBad request : %s", x.what());
		return false;
	}
	return true;
}

bool jsonws::write(message& m) {
	string msg = m.json();
	debug::log("\nSOCK:sending: %s", msg.data());
	msg = string("\0", 1) + msg;
	msg.push_back(-1);
	return present = send(msg);
}

bool jsonws::read(message& m) {
	string buf = recv();
	if(""== buf) return false;
	m.parse(buf);
	return true;
}