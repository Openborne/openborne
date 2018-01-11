#include <ios>
#include <fstream>
#include <iterator>
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

template<typename T>
T File::getu(std::vector<unsigned char>& data, uint32_t& pos)
{
	T res;

	res = 0;
	for (T i = sizeof(T)-1; i > 0; i--)
		res |= data.at(pos+i) << (i * 8);
	res |= data.at(pos);
	pos += sizeof(T);
	return res;
}

std::string File::gets(std::ifstream& in, unsigned n)
{
	char buf[n];
	in.read(buf, n);
	std::string res(buf);
	return res;
}

std::string File::gets(std::vector<unsigned char> data, unsigned n, uint32_t& pos)
{
	std::string res;
	for (unsigned i = 0; i < n; i++)
		res += data.at(pos+i);
	pos += n;
	return res;
}

std::string File::gets(std::vector<unsigned char> data, uint32_t& pos)
{
	unsigned i;
	std::string res;

	for (i = 0; data.at(pos+i) != '\0'; i++)
		res += data.at(pos+i);
	pos += i;
	return res;
}

int File::Dcx::decompress(std::string path)
{
	std::string magictab[] = {
		"DCX\0", "DCS\0", "DCP\0", "DFLT", "DCA\0"
	};
	std::ifstream in (path, std::ios::in | std::ios::binary);
	std::vector<unsigned char> cdata;
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

	out.resize(uncompressed);
	cdata.reserve(compressed);
	cdata.insert(cdata.begin(),
		     std::istreambuf_iterator<char>(in),
		     std::istreambuf_iterator<char>());
	len = uncompressed;
	uncompress(&out[0], &len, &cdata[0], compressed);
	in.close();
	return 0;
}

int File::Bnd4::parse(std::vector<unsigned char> data)
{
	Entry e;
	uint32_t pos = 0;
	const std::string magic = "BND4";

	if (gets(data, magic.length(), pos) != magic)
		return -1; /* bad magic */
	pos += 8;
	filecnt = getu<uint32_t>(data, pos);
	pos += 8;
	version = gets(data, 8, pos);
	direntry = getu<uint32_t>(data, pos);
	pos += 4;
	offset = getu<uint32_t>(data, pos);
	pos += 4;
	encoding = getu<uint8_t>(data, pos);
	pos += 15;

	if (encoding == 1) {
		/* todo: utf-8 reading ... */
	}

	entries.reserve(filecnt);
	for (uint32_t i = 0; i < filecnt; i++) {
		uint32_t orig;
		uint32_t fileentry, fileoffset, filesize;

		pos += 8;
		filesize = getu<uint32_t>(data, pos);
		pos += 4;

		if (direntry == 0x24) {
			pos += 8;
			fileentry = getu<uint32_t>(data, pos);
			pos += 4;
			fileoffset = getu<uint32_t>(data, pos);
		} else {
			fileentry = getu<uint32_t>(data, pos);
			fileoffset = getu<uint32_t>(data, pos);
		}

		orig = pos;
		std::string filename = "";
		if (fileoffset > 0) {
			pos = fileoffset;
			filename = gets(data, pos);
		}

		pos = fileentry;
		e.size = filesize;
		e.name = filename;
		e.data.reserve(filesize);
		e.data.insert(e.data.begin(),
			      data.begin()+pos,
			      data.begin()+pos+filesize);
		entries.push_back(e);
		pos = orig;
	}

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
