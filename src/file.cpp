#include <ios>
#include <fstream>
#include <file.h>
#include <game.h>
#include <zlib.h>

using namespace Ob;

int File::Dcx::decompress(std::string path)
{
	std::string magictab[] = {
		"DCX\0", "DCS\0", "DCP\0", "DFLT", "DCA\0"
	};
	std::ifstream in (path, std::ios::in | std::ios::binary);
	std::vector<unsigned char> cdata;
	unsigned long len;
	uint32_t header;

	if (!in.is_open())
		return -1; /* could not open */
	if (Bit::string(in, magictab[0].length()+1) != magictab[0])
		return -2; /* bad magic */
	Bit::unpack(in, BigEndian, Bit::Skip(4), header);
	if (header != Dcx::hdrsz)
		return -3; /* bad header size */
	Bit::unpack(in, BigEndian, Bit::Skip(12));
	if (Bit::string(in, magictab[1].length()+1) != magictab[1])
		return -2;
	Bit::unpack(in, BigEndian, uncompressed, compressed);
	if (Bit::string(in, magictab[2].length()+1) != magictab[2]
	 || Bit::string(in, magictab[3].length()+1) != magictab[3])
		return -2;
	Bit::unpack(in, BigEndian, Bit::Skip(4), level, Bit::Skip(15));
	if (Bit::string(in, magictab[4].length()+1) != magictab[4])
		return -2;
	Bit::unpack(in, BigEndian, header);
	if (header != Dcx::dcasz)
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
	size_t pos = 0;
	const std::string magic = "BND4";

	if (Bit::string(data, magic.length(), pos) != magic)
		return -1; /* bad magic */
	Bit::unpack(data, pos, LittleEndian, Bit::Skip(8), filecnt, Bit::Skip(8));
	version = Bit::string(data, 8, pos);
	Bit::unpack(data, pos, LittleEndian, direntry, Bit::Skip(4), offset,
		    Bit::Skip(4), encoding, Bit::Skip(15));
	entries.reserve(filecnt);
	for (size_t i = 0; i < filecnt; i++) {
		size_t orig;
		uint32_t fileentry, fileoffset, filesize;

		Bit::unpack(data, pos, LittleEndian, Bit::Skip(8), filesize, Bit::Skip(4));
		if (direntry == 0x24)
			Bit::unpack(data, pos, LittleEndian, Bit::Skip(8),
				    fileentry, Bit::Skip(4), fileoffset);
		else
			Bit::unpack(data, pos, LittleEndian, fileentry, fileoffset);
		
		orig = pos;
		std::string filename = "";
		if (fileoffset > 0) {
			pos = fileoffset;
			filename = Bit::string(data, pos, encoding);
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
