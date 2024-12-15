#include "DataSelector.h"

/*
как это было
был цикл
была глобальная позиция
конечные значение были динамические
*/
namespace WebPar
{
	std::string DataSelector::selectInnerValue(std::string _from, std::string _to, std::string_view text)
	{
		size_t innerTextStart = text.find(_from, currPosition);
		//if error on agrosem try uncomment this
		/*if (innerTextStart == std::string::npos)
		{
			currPosition = std::string::npos;
			return "";
		}*/
		size_t innerTextEnd = text.find(_to, innerTextStart);
		if (innerTextStart == std::string::npos || innerTextEnd == std::string::npos)
		{
			currPosition = std::string::npos;
			return "";
		}
		currPosition = innerTextEnd;
		std::string_view tmpView = text.substr(innerTextStart + _from.size(), innerTextEnd - innerTextStart - _from.size());
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

	std::vector<std::string> DataSelector::selectDataFromHTML(std::vector<std::vector<std::string>> tags, std::string_view text)
	{
		std::vector<std::string> result;
		size_t tmpPosition = 0;
		currPosition = 0;
		for (size_t i = 0; i < tags.size(); i++)
		{
			if (tmpPosition != 0)
				currPosition = tmpPosition;
			if (tags[i][0] == "||")
				currPosition = 0;
			while (currPosition != std::string::npos)
			{
				tmpPosition = currPosition;
				std::string value = selectTags(tags[i], text);
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
		//getCurrentTag
		// while not white space collect cahr to tag value
		//if tag in switch case then do operation 
		// getImageMetaData
		//	find title 
		//else remove tag

		size_t currPosition = 0;
		while (currPosition != std::string::npos)
		{
			currPosition = text.find("<", currPosition);
			if (currPosition != std::string::npos)
			{
				// not use if not need seedspost!!!
			/*	std::string currTag =  getCurrentTag(text, currPosition);
				bool specialHandling = getTagMetaData(text, currPosition, currTag);
				if (specialHandling == true)
					continue;*/
				size_t endTag = text.find(">", currPosition);
				//text.insert(endTag + 1,1, '\n');
				text = text.erase(currPosition, endTag + 1 - currPosition);
				text.insert(currPosition, 1, '\n');
			}
		}
		return text;
	}

	std::string DataSelector::getCurrentTag(std::string text, size_t currPosition)
	{
		std::string currTag = "";
		//tag start from < so 1 step forward
		currPosition++;
		//utf-8 code for a-Z
		while (((int)text[currPosition] >= 65 && (int)text[currPosition] <= 90) ||
			((int)text[currPosition] >= 97 && (int)text[currPosition] <= 122))
		{
			currTag.push_back(text[currPosition]);
			currPosition++;
		}
		return currTag;
	}

	bool DataSelector::getTagMetaData(std::string& text, size_t& currPosition, std::string tag)
	{
		std::string result = "";
		if (tag == "img")
		{
			getImageMetaData(text, currPosition);
			return true;
		}
		return false;


	}

	void DataSelector::getImageMetaData(std::string& text, size_t& currPosition)
	{
		//not use if not need seedspost!!!
		size_t startMetaData = text.find("title=\"", currPosition);
		text.erase(currPosition, startMetaData + 7 - currPosition);
		size_t endMetaData = text.find("\"", currPosition);
		currPosition = text.find(">", endMetaData);
		text.erase(endMetaData, currPosition - endMetaData + 1);
		currPosition = endMetaData;
		text.insert(currPosition, "\n");
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
		//this code prefer remove LF do not sure which better (text almost without LF)
		//while (currPosition != std::string::npos)
		//{
		//	currPosition = text.find("\n", currPosition);
		//	if ((text[currPosition + 1] == '\n' || text[currPosition + 1] == ' ') && currPosition != std::string::npos)
		//		text = text.erase(currPosition, 1);
		//	else if (currPosition != std::string::npos)
		//		currPosition++;
		//}
		//currPosition = 0;
		//while (currPosition != std::string::npos)
		//{
		//	currPosition = text.find(" ", currPosition);
		//	if ((text[currPosition + 1] == ' ' || text[currPosition + 1] == '\n') && currPosition != std::string::npos)
		//		text = text.erase(currPosition + 1, 1);
		//	else if (currPosition != std::string::npos)
		//		currPosition++;
		//}

		//this code do not make any prefer for deleting (left more LF in text)
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



