#include "DataSelectorRules.h"
namespace WP
{
	std::vector<std::vector<std::string>> DataSelectorRules::getRule(std::string name)
	{
		if (rules.count(name) >= 1)
			return rules[name];
		return std::vector<std::vector<std::string>>();
	}

	void DataSelectorRules::load(std::string site_url)
	{
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
						rules.insert({ currentName, currentVector });
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
				rules.insert({ currentName, currentVector });
			}
		}
		infile.close();
		if (validate() == false)
		{
			;// ErrorHandler::setErrorMessage("error: site config not correct!");
			throw std::invalid_argument("error: site config not correct!");
		}
	}

	bool DataSelectorRules::validate()
	{
		if (rules.count("Pagination") != 1)
			return false;
		else if (rules.count("productCard") != 1)
			return false;
		else if (rules.count("ref") != 1)
			return false;
		else if (rules.count("price_catalog") != 1)
			return false;
		else if (rules.count("productIsNotAvailable") != 1)
			return false;
		else if (rules.count("name") != 1)
			return false;
		else if (rules.count("price") != 1)
			return false;
		else if (rules.count("productID") != 1)
			return false;
		else if (rules.count("manufacturer") != 1)
			return false;
		else if (rules.count("image") != 1)
			return false;
		else if (rules.count("description") != 1)
			return false;
		return true;
	}

	bool DataSelectorRules::ruleExist(std::string ruleName)
	{
		if (rules.count(ruleName) > 1)
			return true;
		return false;

	}
}

