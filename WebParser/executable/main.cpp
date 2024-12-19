// WebParser.cpp : Defines the entry point for the application.
//
#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include "WebParser.h"

void printHelp()
{
	std::cout << "Usage: WebParser url=example.com file=example" << std::endl;
}

bool validateArguments(std::unordered_map<std::string, std::string> argsMap)
{
//now support only 3 site so no need check file

	if (argsMap.count("file") == 0)
	{
		std::cerr << "file name is not set" << std::endl;
		return false;
	}
	else if (argsMap.count("url") == 0)
	{
		std::cerr << "url is not set" << std::endl;
		return false;
	}
	return true;
		
}

std::unordered_map<std::string, std::string> getArguments(int argc, char* argv[])
{
	std::unordered_map<std::string, std::string> argsMap;
	for (int i = 0; i < argc; i++)
	{
		std::string curArg = argv[i];
		size_t argValPos = curArg.find_first_of('=');
		if (argValPos != std::string::npos)
		{
			std::string argName = curArg.substr(0, argValPos);
			std::string argVal = curArg.substr(argValPos+1, curArg.size() - argValPos);
			argsMap[argName] = argVal;
		}
	}
	return argsMap;
}

int main(int argc, char* argv[])
{
	std::unordered_map<std::string, std::string> argsMap = getArguments(argc, argv);
	if (argsMap.count("help") == 1)
		printHelp();
	else
	{
		if (validateArguments(argsMap))
		{
			WP::WebParser webParser = WP::WebParser(argsMap["file"]);
			std::vector<WP::ProductData> products = webParser.parseAllPages(argsMap["url"]);
			std::cout << "found next products:" << std::endl;
			for (auto& product:products)
			{
				std::cout << product.name << std::endl;
			}
		}
		else
		{
			std::cout << "try use WebParser \"help=\" to get more info" << std::endl;
			return EXIT_FAILURE;
		}
	}		
	return EXIT_SUCCESS;
}
