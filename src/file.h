#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <codecvt>
#include <locale>

namespace Ob {
	enum Endianness { LittleEndian, BigEndian };

	class Bit {
	public:
		struct Skip {
			size_t amount;
			explicit Skip(size_t n) : amount(n) { }
		};

		static std::string string(std::ifstream& in, size_t n)
		{
			char *buf = new char[n];
			in.read(buf, n);
			std::string res(buf);
			delete []buf;
			return res;
		}

		static std::string string(std::vector<uint8_t> data, size_t n,
					  size_t& pos)
		{
			std::string res;
			for (unsigned i = 0; i < n; i++)
				res += data.at(pos+i);
			pos += n;
			return res;
		}

		static std::string string(std::vector<uint8_t> data, size_t& pos,
					  bool utf8)
		{
			char16_t c;
			std::string res;

			if (utf8) {
				for (;;) {
					unpack(data, pos, LittleEndian, c);
					if (c == '\0')
						break;
					std::wstring_convert<std::codecvt_utf8<char32_t>,
							     char32_t> w32;
					res += w32.to_bytes(c);
				}

				return res;
			}

			for (size_t i = 0; data.at(pos+i) != '\0'; i++)
				res += data.at(pos+i);
			return res;
		}

		static void unpack(std::ifstream& in, Endianness e)
		{
			(void) e;
			(void) in;
		}

		template<typename T, typename... Targs>
		static void unpack(std::ifstream& in, Endianness e, T& u, Targs&&... tail)
		{
			if (e == LittleEndian)
				u_le(in, u);
			else
				u_be(in, u);
			unpack(in, e, std::forward<Targs>(tail)...);
		}

		template<typename... Targs>
		static void unpack(std::ifstream& in, Endianness e, Skip s,
				   Targs&&... tail)
		{
			in.ignore(s.amount);
			unpack(in, e, std::forward<Targs>(tail)...);
		}
		
		static size_t unpack(const uint8_t* data, size_t size, size_t pos,
				     Endianness e)
		{
			(void) e;
			(void) data;
			(void) size;
			return pos;
		}

		template<typename T, typename... Targs>
		static size_t unpack(const uint8_t* data, size_t size, size_t pos,
				     Endianness e, T& u, Targs&&... tail)
		{
			if (sizeof u > size)
				throw std::runtime_error("not enough data");
			if (e == LittleEndian)
				u_le(data, u);
			else
				u_be(data, u);
			return unpack(data + sizeof u,
				      size - sizeof u,
				      pos + sizeof u, e,
				      std::forward<Targs>(tail)...);
		}

		template<typename... Targs>
		static size_t unpack(const uint8_t* data, size_t size, size_t pos,
				     Endianness e, Skip s, Targs&&... tail)
		{
			if (s.amount > size)
				throw std::runtime_error("not enough data");
			return unpack(data + s.amount,
				      size - s.amount,
				      pos + s.amount, e,
				      std::forward<Targs>(tail)...);
		}

		template<typename... Targs>
		static void unpack(std::vector<uint8_t> data, size_t& pos,
				   Endianness e, Skip s, Targs&&... tail)
		{
			pos = unpack(data.data() + pos, data.size(), pos, e, s,
				     std::forward<Targs>(tail)...);
		}

		template<typename T, typename... Targs>
		static void unpack(std::vector<uint8_t> data, size_t& pos,
				   Endianness e, T& u, Targs&&... tail)
		{
			pos = unpack(data.data() + pos, data.size(), pos, e, u,
				     std::forward<Targs>(tail)...);
		}
	private:
		template<typename T>
		static void u_be(const uint8_t* data, T& t)
		{
			t = T();
			for (size_t i = 0; i < sizeof(T); i++)
				t |= data[sizeof(T)-1-i] << (i * 8);
		}
		template<typename T>
		static void u_be(std::ifstream& in, T& t)
		{
			t = T();
			char tmp[sizeof(T)];
			uint8_t* data;

			in.read(tmp, sizeof tmp);
			data = reinterpret_cast<uint8_t*>(tmp);
			for (size_t i = 0; i < sizeof(T); i++)
				t |= data[sizeof(T)-1-i] << (i * 8);
		}
		template<typename T>
		static void u_le(const uint8_t* data, T& t)
		{
			t = T();
			for (size_t i = sizeof(T)-1; i > 0; i--)
				t |= data[i] << (i * 8);
			t |= *data;
		}
		template<typename T>
		static void u_le(std::ifstream& in, T& t)
		{
			t = T();
			char tmp[sizeof(T)];
			uint8_t* data;

			in.read(tmp, sizeof tmp);
			data = reinterpret_cast<uint8_t*>(tmp);
			for (size_t i = sizeof(T)-1; i > 0; i--)
				t |= data[i] << (i * 8);
			t |= *data;
		}
	};

	namespace File {
		class Txt {
		public:
			int parse(std::string path);

			std::map<std::string, std::string> data()
			{
				return out;
			}
		private:
			std::map<std::string, std::string> out;
			enum State {
				Key, Value
			};
		};

		class Dcx {
		public:
			uint32_t uncompressed = 0, compressed = 0;
			uint32_t level;
			int decompress(std::string path);

			std::vector<uint8_t> data()
			{
				return out;
			}
		private:
			std::vector<uint8_t> out;
			const unsigned hdrsz = 24;
			const unsigned dcasz = 8;
		};

		class Bnd4 {
		public:
			struct Entry {
				uint32_t size;
				std::string name;
				std::vector<uint8_t> data;
			};

			uint32_t filecnt;
			std::string version;
			uint32_t direntry;
			uint32_t offset;
			uint8_t encoding;

			int parse(std::vector<uint8_t> data);

			std::vector<Entry> data()
			{
				return entries;
			}
		private:
			std::vector<Entry> entries;
		};

		std::string getsep();
	};
}
