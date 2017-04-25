#include "LootTable.h"

// returns early and sets isError to true if parsing json fails
LootTable::LootTable(const nlohmann::json& json, bool& isError)
{
	if (!jsonContains(json, "TableName", nlohmann::detail::value_t::string) ||
		!jsonContains(json, "TableEntryCollection", nlohmann::detail::value_t::array))
	{
		// error parsing json
		isError = true;
		return;
	}

	TableName = json["TableName"].get<std::string>();

	auto& entries = json["TableEntryCollection"];
	for (const nlohmann::json& entry : entries)
	{
		Entry::ptr parsedEntry = Entry::ParseEntryType(entry);
		if (parsedEntry == nullptr)
		{
			isError = true;
			return;
		}
		else
		{
			TableEntryCollection.push_back(parsedEntry);
		}
	}
}


LootTable::ptr LootTable::ParseTableType(const nlohmann::json& json)
{
	if (!jsonContains(json, "TableType", nlohmann::detail::value_t::string))
	{
		return nullptr;
	}

	std::string tableType = json["TableType"].get<std::string>();
	bool isError = false;
	
	if (tableType == "Random")
	{
		LootTable::ptr parsedTable = std::make_shared<RandomLootTable>(json, isError);
		if (isError)
		{
			return nullptr;
		}
		else
		{
			return parsedTable;
		}
	}
	else if (tableType == "UniqueRandom")
	{
		LootTable::ptr parsedTable = std::make_shared<UniqueRandomLootTable>(json, isError);
		if (isError)
		{
			return nullptr;
		}
		else
		{
			return parsedTable;
		}
	}
	else
	{
		return nullptr;
	}
}

RandomLootTable::RandomLootTable(const nlohmann::json& json, bool& isError) : LootTable(json, isError) { }
UniqueRandomLootTable::UniqueRandomLootTable(const nlohmann::json& json, bool& isError) : LootTable(json, isError) { }

void RandomLootTable::GenerateLoot(
	std::default_random_engine& random, 
	int count,
	std::unordered_map<std::string, unsigned int> outLoot) const { }

void UniqueRandomLootTable::GenerateLoot(
	std::default_random_engine& random, 
	int count, 
	std::unordered_map<std::string, unsigned int> outLoot) const { }