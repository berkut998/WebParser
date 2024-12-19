#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>

namespace WP
{
	class DataSelectorRules final
	{

	public:
		/// <summary>
		/// return list rules by name;
		/// </summary>
		/// <param name="name">name of rule</param>
		/// <returns>list of rule</returns>
		std::vector<std::vector<std::string>> getRule(std::string name);
		/// <summary>
		/// load rules from txt file
		/// </summary>
		/// <param name="site_url">- filename</param>
		void load(std::string site_url);
		/// <summary>
		/// Return true if rule is setted else false.
		/// </summary>
		/// <param name="ruleName"> - name of rule</param>
		/// <returns></returns>
		bool ruleExist(std::string ruleName);

	private:
		std::unordered_map<std::string, std::vector<std::vector<std::string>>> rules;
		bool validate();

	};
}
	

