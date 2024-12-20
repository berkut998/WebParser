#include "DataSelector.h"

namespace WP
{
	DataSelector::DataSelector(std::unordered_map<std::string, std::vector<std::vector<std::string>>> rule)
	{
		rules = rule;
	}
	std::unordered_map<std::string, std::vector<std::vector<std::string>>>
		DataSelector::loadRules(std::string site_url)
	{
		std::ifstream infile(site_url + ".txt");
		std::unordered_map<std::string, std::vector<std::vector<std::string>>> rules;
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
		if (validateRules(rules) == false)
		{
			;// ErrorHandler::setErrorMessage("error: site config not correct!");
			throw std::invalid_argument("error: site config not correct!");
		}
		return rules;
	}

	bool DataSelector::validateRules(std::unordered_map<std::string, std::vector<std::vector<std::string>>> rules)
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

	bool DataSelector::ruleExist(std::string ruleName)
	{
		if (rules.count(ruleName) > 1)
			return true;
		return false;

	}





	std::string DataSelector::selectInnerValue(const std::string from, const std::string to, const std::string_view text)
	{
		size_t innerTextStart = text.find(from, currPosition);
		size_t innerTextEnd = text.find(to, innerTextStart);
		if (innerTextStart == std::string::npos || innerTextEnd == std::string::npos)
		{
			currPosition = std::string::npos;
			return "";
		}
		currPosition = innerTextEnd;
		std::string_view tmpView = text.substr(innerTextStart + from.size(), innerTextEnd - innerTextStart - from.size());
		return std::string(tmpView);
	}

	std::string DataSelector::selectTags(std::vector<std::string> tags, std::string_view text)
	{
		std::string innerValue;
		size_t tmpPosition = currPosition;
		for (size_t i = 0; i < tags.size(); i++)
		{
			if (tags[i] == "||")
				continue;
			if (i + 2 == tags.size() && currPosition != std::string::npos)
			{
				innerValue = selectInnerValue(tags[i], tags[i + 1], text);
				return innerValue;
			}
			currPosition = text.find(tags[i], currPosition);
			if (currPosition != std::string::npos)
				currPosition += tags[i].size();
		}
		return innerValue;
	}

	std::vector<std::string> DataSelector::selectDataFromHTML(const std::string &ruleName, const std::string_view text)
	{
		std::vector<std::string> result;
		size_t tmpPosition = 0;
		currPosition = 0;
		std::vector<std::vector<std::string>> curRule = rules[ruleName];
		for (const auto& rule : curRule)
		{
			if (tmpPosition != 0)
				currPosition = tmpPosition;
			if (rule[0] == "||")
				currPosition = 0;
			while (currPosition != std::string::npos)
			{
				tmpPosition = currPosition;
				std::string value = selectTags(rule, text);
				if (!value.empty())
					result.push_back(value);
			}
		}
		return result;
	}
	/// <summary>
	/// Replace HTML tag with /n <a>ref</a> -> /nref/n
	/// IMPORTANT if tag open "<" and not closed delete all to end of string
	/// </summary>
	/// <param name="text"> - String in which removed all HTML tags</param>
	/// <returns>string without HTML tags</returns>
	std::string DataSelector::removeHTMLTags(std::string text)
	{
		size_t currPosition = 0;
		while (currPosition != std::string::npos)
		{
			currPosition = text.find("<", currPosition);
			if (currPosition != std::string::npos)
			{
				size_t endTag = text.find(">", currPosition);
				text = text.erase(currPosition, endTag + 1 - currPosition);
				text.insert(currPosition, 1, '\n');
			}
		}
		return text;
	}

	/// <summary>
	/// remove LF(/n) and spaces from text. Remove from beginng sequence LF/spaces like
	/// /n/n/n/n(space) last element will be lefted and all first element erased.
	/// </summary>
	/// <param name="text"> - text which will be cleared</param>
	/// <returns>return string without doubled LF(\n) and spaces</returns>
	std::string DataSelector::removeDoubleSpacesAndLF(std::string text)
	{
		size_t currPosition = 0;
		while (currPosition != std::string::npos)
		{
			currPosition = text.find("\n", currPosition);
			if ((text[currPosition + 1] == '\n' || text[currPosition + 1] == ' ') && currPosition != std::string::npos)
				text = text.erase(currPosition, 1);
			else if (currPosition != std::string::npos)
				currPosition++;
		}
		currPosition = 0;
		while (currPosition != std::string::npos)
		{
			currPosition = text.find(" ", currPosition);
			if ((text[currPosition + 1] == ' ' || text[currPosition + 1] == '\n') && currPosition != std::string::npos)
				text = text.erase(currPosition, 1);
			else if (currPosition != std::string::npos)
				currPosition++;
		}
		return text;
	}

	/// <summary>
	/// Remove all dublicate rows (dublicate should be right after previsu row). First apperead row in text will be lefted. separate sybols for rows is /n
	/// </summary>
	/// <param name="text">- text in which remove all dublicates row. separate sybols for rows is /n</param>
	/// <returns>cleared text</returns>
	std::string DataSelector::removeDublicateRow(std::string text)
	{
		size_t currPosition = 0;
		size_t previusPostion = 0;
		std::string firstRow = "";
		std::string secondRow = "";
		while (currPosition != std::string::npos)
		{
			currPosition = text.find("\n", currPosition);
			if (currPosition == std::string::npos)
				break;
			firstRow = text.substr(previusPostion, currPosition - previusPostion);
			currPosition++;
			if (firstRow == secondRow)
			{
				text = text.erase(previusPostion, currPosition - previusPostion);
				currPosition = previusPostion;
			}
			else
				previusPostion = currPosition;
			secondRow = firstRow;

		}
		//compare last row
		firstRow = text.substr(previusPostion, text.length() - previusPostion);
		std::string test1 = getStringFreSpacesInLower(firstRow);
		std::string test2 = getStringFreSpacesInLower(secondRow);
		if (getStringFreSpacesInLower(firstRow) == getStringFreSpacesInLower(secondRow))
			text = text.erase(previusPostion, currPosition - previusPostion);
		return text;

	}
	//used for compare string
	std::string DataSelector::getStringFreSpacesInLower(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(),
			[](unsigned char c) { return std::tolower(c); });
		size_t currPosition = 0;
		while (currPosition != std::string::npos)
		{
			currPosition = text.find(" ", currPosition);
			if (currPosition != std::string::npos)
				text = text.erase(currPosition, 1);
		}
		return text;

	}

}



