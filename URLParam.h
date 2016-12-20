#pragma once

#include <map>
#include <sstream>
#include "Utility.h"

struct URLParam
{
	void AddParam(std::string event, std::string param)
	{
		mapParam[event] = param;
	}
	std::string BuildURL() {
		ostringstream s;
		for (auto it = mapParam.begin(); it != mapParam.end(); it++)
			s << Utility::UrlEncode(it->first) << "=" << Utility::UrlEncode(it->second) << "&";
		return s.str();
	}
	~URLParam()
	{
		mapParam.clear();
	}

	std::map<std::string, std::string> mapParam;
};