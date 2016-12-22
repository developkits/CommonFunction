#pragma once

#include <ctime>
#include <string>
#include <sstream>
#include <fstream>

namespace std
{
	static std::string get_data()
	{
		time_t t = time(0);
		struct tm now;
		localtime_s(&now, &t);
		std::stringstream ss;
		ss << now.tm_year + 1900 << '_'
			<< now.tm_mon + 1 << "_"
			<< now.tm_mday;
		return ss.str();
	}

	static std::string get_time()
	{
		time_t t = time(0);
		struct tm now;
		localtime_s(&now, &t);
		std::stringstream ss;
		ss << get_data() << ' ';
		ss << now.tm_hour << ':'
			<< now.tm_min << ':'
			<< now.tm_sec;
		return ss.str();
	}

	template <typename T>
	static int to_string(std::stringstream& ss, const T &t)
	{
		ss << t;
		return 1;
	}

	template <typename T, typename...Args>
	static int to_string(std::stringstream& ss, const T &t, const Args&...rest)
	{
		ss << t;
		return to_string(ss, rest...);
	}

	template <typename T, typename...Args>
	static void Log(const T &t, const Args&...rest)
	{
		std::stringstream ss;
		to_string(ss, t, rest...);
		std::string path;
		path += get_data() + ".log";
		std::fstream log_file;
		log_file.open(path, std::ios::out | std::ios::app);
		log_file << "[" << get_time() << "]" << ss.str() << std::endl;
		log_file.close();
	}
};