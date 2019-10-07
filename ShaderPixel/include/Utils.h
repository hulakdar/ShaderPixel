#pragma once

#ifdef _WIN32
#include <sys/stat.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <string>
#include <fstream>
#include <glm/glm.hpp>

namespace Utils
{
	inline std::string StringFromFile(const std::string& filename)
	{
		std::ifstream fs(filename);
		if (fs.bad())
		{
			return "";
		}

		std::string s(
			std::istreambuf_iterator<char>{fs},
			std::istreambuf_iterator<char>{});

		return s;
	}

	inline uint64_t GetFileTimestamp(const char* filename)
	{
		uint64_t timestamp = 0;

	#ifdef _WIN32
		struct __stat64 stFileInfo;
		if (_stat64(filename, &stFileInfo) == 0)
		{
			timestamp = stFileInfo.st_mtime;
		}
	#else
		struct stat fileStat;

		if (stat(filename, &fileStat) == -1)
		{
			perror(filename);
			return 0;
		}

	#ifdef __APPLE__
		timestamp = fileStat.st_mtimespec.tv_sec;
	#else
		timestamp = fileStat.st_mtime;
	#endif
	#endif

		return timestamp;
	}
}

struct Timestamp
{
	uint64_t stamp = 0;

	inline void update(const std::string& path)
	{
		stamp = Utils::GetFileTimestamp(path.c_str());
	}

	inline bool operator == (const Timestamp& Other) { return stamp == Other.stamp; }
	inline bool operator != (const Timestamp& Other) { return !(*this == Other); }
	inline operator bool () { return stamp; }
};


inline std::ostream&
operator << (std::ostream& o, glm::vec3 vec)
{
	o << vec.x<< " " << vec.y<< " " << vec.z;
	return o;
}
