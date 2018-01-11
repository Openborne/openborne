#include <ios>
#include <fstream>
#include <file.h>
#include <game.h>
#include <zlib.h>

using namespace Ob;

template<typename T>
T File::getu(std::ifstream& in)
{
	T res;
	char tmp[sizeof(T)];
	unsigned char* buf;

	in.read(tmp, sizeof tmp);
	buf = reinterpret_cast<unsigned char*>(tmp);
	for (T i = res = 0; i < sizeof(T); i++)
		res |= buf[sizeof(T)-1-i] << (i * 8);
	return res;
}

std::string File::gets(std::ifstream& in, unsigned n)
{
	char buf[n];
	in.read(buf, n);
	std::string res(buf);
	return res;
}

int File::Dcx::decompress(std::string path)
{
	std::string magictab[] = {
		"DCX\0", "DCS\0", "DCP\0", "DFLT", "DCA\0"
	};
	std::ifstream in (path, std::ios::in | std::ios::binary);
	char* tmp;
	unsigned char* cdata;
	unsigned long len;

	if (!in.is_open())
		return -1; /* could not open */
	if (gets(in, magictab[0].length()+1) != magictab[0])
		return -2; /* bad magic */
	in.ignore(4);
	if (getu<uint32_t>(in) != Dcx::hdrsz)
		return -3; /* bad header size */
	in.ignore(12);
	if (gets(in, magictab[1].length()+1) != magictab[1])
		return -2;
	uncompressed = getu<uint32_t>(in);
	compressed = getu<uint32_t>(in);
	if (gets(in, magictab[2].length()+1) != magictab[2]
	 || gets(in, magictab[3].length()+1) != magictab[3])
		return -2;
	getu<uint32_t>(in);
	level = getu<uint32_t>(in);
	in.ignore(15);

	if (gets(in, magictab[4].length()+1) != magictab[4])
		return -2;
	if (getu<uint32_t>(in) != Dcx::dcasz)
		return -3;

	tmp = new char[compressed];
	cdata = new unsigned char[compressed];
	out = new unsigned char[uncompressed];

	in.read(tmp, compressed);
	cdata = reinterpret_cast<unsigned char*>(tmp);
	len = uncompressed;
	uncompress(out, &len, cdata, compressed);
	delete []cdata;
	in.close();
	return 0;
}

int File::Txt::parse(std::string path)
{
	File::Txt::State state;
	std::string buf;
	std::ifstream in (path);

	if (!in.is_open())
		return -1; /* could not open */
	while (getline(in, buf)) {
		/* all of the action txt files have a comment in the beginning that begins
		   with "BOM" (byte order mark) and some japanese text that roughly
		   translates to: "With UTF8 The first line becomes unanalyzable when
		   saved in DLProperties, Comment out in preparation for a bug in" */
		if (buf[0] == '#' || buf == "")
			continue;
		static std::string kv[2];
		state = File::Txt::State::Key;
		for (unsigned i = 0; i < buf.length(); i++) {
			if (i == buf.length()-1) {
				out[kv[0]] = kv[1];
				break;
			}
			if (buf[i] == ' ' || buf[i] == '"')
				continue;
			else if (buf[i] == '=' && state == File::Txt::State::Key) {
				state = File::Txt::State::Value;
				continue;
			} 

			kv[state] += buf[i];
		}
	}

	in.close();
	return 0;
}

std::string File::getsep()
{
#ifdef _WIN32
	return "\\";
#else
	return "/";
#endif
}
