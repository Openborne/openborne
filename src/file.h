#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace Ob {
	class File {
	public:
		template<typename T>
		static T getu(std::ifstream& in);

		static std::string gets(std::ifstream& in, unsigned n);

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
			unsigned char* out;
			int decompress(std::string path);
			~Dcx() { 
				if (uncompressed)
					delete []out;
			}
		private:
			const unsigned hdrsz = 24;
			const unsigned dcasz = 8;
		};

		class Bnd4 {
		public:
			uint32_t filecnt;
			std::string version;
			uint32_t direntry;
			uint32_t offset;
			uint8_t encoding;
			std::map<std::string, std::vector<unsigned char>> out;
			int parse(unsigned char* data);
		};

		static std::string getsep();
	};
}
