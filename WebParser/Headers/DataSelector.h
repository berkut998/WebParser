#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "DataSelectorRule.h"

namespace WP
{
	class DataSelector final
	{
	public:
		DataSelector(std::unordered_map<std::string, std::vector<std::vector<std::string>>> rules);
		static std::unordered_map<std::string, std::vector<std::vector<std::string>>> loadRules(const std::string pathToFileWithRules);
		static std::string removeHTMLTags(std::string text);
		static std::string removeDoubleSpacesAndLF(std::string text);
		static std::string removeDublicateRow(std::string text);

		std::vector<std::string> DataSelector::selectDataFromHTML(const std::string &ruleName, const std::string_view text);

	private:
		bool ruleExist(const std::string ruleName);
		static bool validateRules(std::unordered_map<std::string, std::vector<std::vector<std::string>>> rules);
		std::string selectInnerValue(const std::string from, const std::string to, const std::string_view text);
		std::string selectTags(std::vector<std::string> tags, std::string_view text);
		std::string getCurrentTag(std::string text, size_t currPosition);
		static std::string getStringFreSpacesInLower(std::string text);

		std::unordered_map<std::string, std::vector<std::vector<std::string>>> rules;

		size_t currPosition = 0;
	};
}


