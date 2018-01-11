#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace Ob {
	class File {
	public:
		template<typename T>
		static T getu(std::ifstream& in);
		template<typename T>
		static T getu(std::vector<unsigned char>& data, uint32_t& n);

		static std::string gets(std::ifstream& in, unsigned n);
		static std::string gets(std::vector<unsigned char> data, unsigned n, uint32_t& pos);
		static std::string gets(std::vector<unsigned char> data, uint32_t& pos);

		class Txt {
		public:
			std::map<std::string, std::string> out;
			int parse(std::string path);
		private:
			enum State {
				Key, Value
			};
		};

		class Dcx {
		public:
			uint32_t uncompressed = 0, compressed = 0;
			uint32_t level;
			std::vector<unsigned char> out;
			int decompress(std::string path);
		private:
			const unsigned hdrsz = 24;
			const unsigned dcasz = 8;
		};

		class Bnd4 {
		public:
			struct Entry {
				uint32_t size;
				std::string name;
				std::vector<unsigned char> data;
			};

			uint32_t filecnt;
			std::string version;
			uint32_t direntry;
			uint32_t offset;
			uint8_t encoding;
			std::vector<Entry> entries;

			int parse(std::vector<unsigned char> data);	
		};

		static std::string getsep();
	};
}
