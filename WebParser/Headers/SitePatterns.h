#pragma once
#include <string>
#include <vector>
#include <map>

namespace
{
	class SitePatterns final
	{
	public:
		std::map<std::string, std::vector<std::vector<std::string>>> parseSetting;
	};

}
