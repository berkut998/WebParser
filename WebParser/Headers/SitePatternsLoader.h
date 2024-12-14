#pragma once
#include <fstream>
#include "SitePatterns.h"
//#include "ErrorHandler.h"
namespace
{
	class SitePatternsLoader final
	{
	public:
		static SitePatterns load(std::string site_url);
	private:
		static bool validate(SitePatterns patterns);
	};
}


