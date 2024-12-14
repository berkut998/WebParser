#include "SitePatternsLoader.h"

SitePatterns SitePatternsLoader::load(std::string site_url)
{
	SitePatterns patterns;
	std::ifstream infile(site_url + ".txt");
	if (infile.is_open())
	{
		std::string currentLine;
		std::string currentName;
		std::vector<std::vector<std::string>> currentVector;
		std::vector<std::string> currentSubVector;
		while (std::getline(infile, currentLine))
		{
			if (currentLine.size() > 2 && currentLine[0] == '[' && currentLine[1] != '[')
			{
				if (!currentSubVector.empty())
				{
					currentVector.push_back(currentSubVector);
					patterns.parseSetting.insert({ currentName, currentVector });
					currentVector.clear();
					currentSubVector.clear();
				}
				currentName = currentLine.substr(1, currentLine.size() - 2);
			}
			else if (currentLine.size() > 2 && currentLine[0] == '[' && currentLine[1] == '[')
			{
				if (!currentSubVector.empty())
				{
					currentVector.push_back(currentSubVector);
					currentSubVector.clear();
				}
			}
			else
			{
				currentSubVector.push_back(currentLine);
			}

		}
		if (!currentSubVector.empty())
		{
			currentVector.push_back(currentSubVector);
			patterns.parseSetting.insert({ currentName, currentVector });
		}
	}
	infile.close();
	if (validate(patterns) == false)
	{
		;// ErrorHandler::setErrorMessage("error: site config not correct!");
		throw std::invalid_argument("error: site config not correct!");
	}
	return patterns;
}

bool SitePatternsLoader::validate(SitePatterns patterns)
{
	if (patterns.parseSetting.count("Pagination") != 1)
		return false;
	else if(patterns.parseSetting.count("productCard") != 1)
		return false;
	else if (patterns.parseSetting.count("ref") != 1)
		return false;
	else if (patterns.parseSetting.count("price_catalog") != 1)
		return false;
	else if (patterns.parseSetting.count("productIsNotAvailable") != 1)
		return false;
	else if (patterns.parseSetting.count("name") != 1)
		return false;
	else if (patterns.parseSetting.count("price") != 1)
		return false;
	else if (patterns.parseSetting.count("productID") != 1)
		return false;
	else if (patterns.parseSetting.count("manufacturer") != 1)
		return false;
	else if (patterns.parseSetting.count("image") != 1)
		return false;
	else if (patterns.parseSetting.count("description") != 1)
		return false;
	return true;
}

