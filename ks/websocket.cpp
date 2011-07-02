#include "websocket.h"
#include "debug.h"
#include "md5.h"
#include <sstream>
#ifdef WIN32
#include <WinSock2.h>
#else
#include <stddef.h>
#include <sys/types.h>
#include <net/hton.h>
#endif

int overload[] = { 8 };	//Length of raw data given after the HTTP request header, in function of the version number

string consume(string rdr, size_t& pos, string lmt = "\r\n") {
	size_t s = lmt.size(), l = rdr.find(lmt, pos) - pos;
	if(string::npos == l) throw ws::exception("Delimiter not found");
	pos += s += l;
	return rdr.substr(pos-s, l);
}

ulong decode0(string ky) {
	string nbr = "";
	int ns = 0;
	for(string::iterator i=ky.begin(); ky.end()!= i; ++i) {
		if(' '== *i) ++ns;
		else if('0'<= *i && '9'>= *i) nbr += *i;
	}
	if(0== ns || ""== nbr) throw ws::exception("Bad client keys");
	ulong rv = stol(nbr);
	if(0!= rv%ns) throw ws::exception("Bad websocket keys");
	return rv/ns;
}

bool ws::connect(short port) {
	if(!socklib::connect(port)) return false;
	size_t pos = 4;
	string sb = recv();
	while(string::npos== sb.find("\r\n\r\n")) sb += recv();
	try {
		//read GET header
		if(' TEG'!= *(const long*)sb.data()) throw ws::exception("Not a GET request");
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

		if( "Upgrade"!= hdrs["Connection"] || "WebSocket"!= hdrs["Upgrade"]) throw ws::exception("Not a WebSocket request");
		map<string, string>::iterator i, e = hdrs.end();
		version = (e!= (i= hdrs.find("Sec-WebSocket-Version"))) ? stoi(i->second) : 0;
		if(0< version) throw ws::exception("Unsupported version");	//TODO: later versions
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
			ulong k[4] = { htonl(decode0(hdrs["Sec-WebSocket-Key1"])), htonl(decode0(hdrs["Sec-WebSocket-Key2"])) };
			const ulong* clntK = (const ulong*)(sb.data()+pos);
			/*This gives "n`9eBk9z$R8pOtVb"
			ulong k[4] = { htonl(decode0("3e6b263  4 17 80")), htonl(decode0("17  9 G`ZD9   2 2b 7X 3 /r90")) };
			const ulong* clntK = (const ulong*)("WjN}|M(6");*/
			k[2] = clntK[0]; k[3] = clntK[1];
			MD5 md5;
			md5.update((const char*)k, 16);
			md5.finalize();
			out << "\r\n" << string((char*)md5.digest, 16);
			break;
		}
		send(out.str());
	} catch(ws::exception x) {
		//TODO: send back 5xx
		clear();
		debug::log("\nBad request : %s", x.what());
		return false;
	}
	return true;
}

bool ws::write(const string& s) {
	debug::log("\nSOCK:sending: %s", s.data());
	//From here : weird inversed-engeneered io.socket protocol
	string ws;

	if(true) {
		ws = string("", 1) + s;
		ws.push_back(-1);
	} else {
		stringstream ss;
		ss << (char)-1;
		size_t l = s.size();
		while(0!= l&(!0x7f))
			ss << (char)(0x80|(l&0x7f));
		ss << (char)(l&0x7f);
		ss << s;
		ws = ss.str();
	}
	return present = send(ws);
}

string ws::read() {
	if(rcvBuf.empty()) rcvBuf = recv();
	char algo = rcvBuf[0];
	string frame;
	size_t pos = 0, beg = 1;
	//TODO: both algos to test
	if(0== (algo&0x80)) {	//00 .. .. .. FF
		while(string::npos== (pos=rcvBuf.find_first_of('\xff', pos))) {
			pos = rcvBuf.size();
			rcvBuf += recv();
		}
	} else {			//7-bit base1 size + content
		//Take care the frame size can be cut in two packets. Unprobable but possible.
		do while(0!= (rcvBuf[beg-1]&0x80) && beg < rcvBuf.size()) pos = (pos<<7) | (rcvBuf[beg++]&0x7f);
		while(0!= (rcvBuf[beg-1]&0x80) && !(rcvBuf+=recv()).empty());	//The empty is not really a test : the '+=' is to be done
		rcvBuf = rcvBuf.substr(beg);
		//Here, 'pos' is the length of the frame.
		pos += beg;
		//Here, 'pos' is the position of the first byte after the frame
		if(rcvBuf.size() < pos) rcvBuf += fill(pos-rcvBuf.size());
	}
	frame = rcvBuf.substr(beg, pos-beg);
	rcvBuf = rcvBuf.substr(pos);
	return frame;
}