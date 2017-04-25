// loot generator by Julia Wlochowski
// prints generated loot to the console based on user input
// reads loot tables from lootDataFilename json located next to the executable
// return code 0 means no errors
// return code 1 means lootDataFilename could not be opened
// return code 2 means the json could not be parsed

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include "json.hpp"
#include <unordered_map>
#include "LootTable.h"

const std::string lootDataFilename = "loot_table_example.json";

// displays helpful text to stdout
void printHelp()
{
	std::cout << R"(
Welcome to LootGenerator!

USAGE
1. Type commands in the format:
		<TableName> <count>
	For example:
		"CurrencyTable 3"
	This will generate 3 drops of loot from the Currency table.
2. Type "tables" to see a list of loot tables you can use.
3. Type "exit" to close the program.
4. Type "help" to see this message again.
)";
}

// prints the names of all the tables that are in the given dictionary
void printTables(const std::unordered_map<std::string, LootTable::ptr>& lootTableLookup)
{
	std::cout << "Available loot tables: \n";
	for (auto kvp : lootTableLookup)
	{
		std::cout << kvp.first << "\n";
	}
}

int main()
{
	std::cout << "Parsing loot table data from " << lootDataFilename << ".\n\n";

	std::ifstream file(lootDataFilename);
	if (!file.is_open())
	{
		return 1;
	}
	nlohmann::json lootTables;
	try
	{
		file >> lootTables;
	}
	catch(...)
	{
		// json malformed
		return 2;
	}

	std::unordered_map<std::string, LootTable::ptr> lootTableLookup;

	// parse json into array of LootTable
	for (const nlohmann::json& lootTable : lootTables)
	{
		auto parsedTable = LootTable::ParseTableType(lootTable);
		if (parsedTable == nullptr)
		{
			return 2;
		}
		else
		{
			lootTableLookup[parsedTable->TableName] = parsedTable;
			std::cout << "Parsed " << parsedTable->TableName << ".\n";
		}
	}
	
	// give the user some instructions!
	printHelp();

	// user input loop
	while (true)
	{
		// get string input
		std::string input;
		std::getline(std::cin, input);


		// end the program when the user types exit
		if (input == "exit")
		{
			return 0;
		}

		// check if the user typed help
		if (input == "help")
		{
			printHelp();
			continue;
		}

		// check if the user wants to list the tables
		if (input == "tables")
		{
			printTables(lootTableLookup);
			continue;
		}

		// try parsing the string as a command
		size_t indexOfSpace = input.find_first_of(' ');
		std::string lootTableStr = input.substr(0,indexOfSpace);
		std::string countStr = input.substr(indexOfSpace + 1);

		// check if the table is in lookup
		if (lootTableLookup.find(lootTableStr)  == lootTableLookup.end())
		{
			// lootTable not found in data
			std::cout << "Loot table " << lootTableStr << " not found in data.\n";
			continue;
		}

		// try parse count
		std::stringstream convertor;
		int count; 

		convertor << countStr;
		convertor >> count;
		
		if (convertor.fail() || !convertor.eof())
		{
			std::cout << "Could not parse count as an integer. Input: " << countStr << "\n";
			continue;
		}

		if (count < 0)
		{
			std::cout << "Cannot generate negative amounts of loot. Input: " << countStr << "\n";
			continue;
		}

		// GENERATE LOOT
		std::unordered_map<std::string, LootTable::ptr> result;
		auto generatedLoot = lootTableLookup[lootTableStr]->GenerateLoot(random, count, result, lootTableLookup);

		// print it
		for (auto kvp : generatedLoot)
		{
			std::cout << "Generated " << kvp.second << " " << kvp.first;
		}
	}
}