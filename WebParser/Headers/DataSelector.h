#pragma once
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

namespace
{
	class DataSelector final
	{
	public:
		std::vector<std::string> selectDataFromHTML(std::vector<std::vector<std::string>> tags, std::string_view text);
		std::string removeHTMLTags(std::string text);
		std::string removeDoubleSpacesAndLF(std::string text);
		std::string removeDublicateRow(std::string text);
	private:
		std::string selectInnerValue(std::string _from, std::string _to, std::string_view text);
		std::string selectTags(std::vector<std::string> tags, std::string_view text);
		std::string getCurrentTag(std::string text, size_t currPosition);
		bool getTagMetaData(std::string& text, size_t& currPosition, std::string tag);
		void getImageMetaData(std::string& text, size_t& currPosition);
		std::string getStringFreSpacesInLower(std::string text);

		size_t currPosition = 0;
	};
}


