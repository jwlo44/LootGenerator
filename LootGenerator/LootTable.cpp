#include "LootTable.h"

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
			Entries.push_back(parsedEntry);
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

unsigned int LootTable::PickRandomIndex(std::default_random_engine& random, const std::vector<Entry::ptr>& entries)
{
	int count = entries.size();
	std::uniform_int_distribution<int> uni(0, count);
	return uni(random);
}


RandomLootTable::RandomLootTable(const nlohmann::json& json, bool& isError) : LootTable(json, isError) { }
UniqueRandomLootTable::UniqueRandomLootTable(const nlohmann::json& json, bool& isError) : LootTable(json, isError) { }

void RandomLootTable::GenerateLoot(
	std::default_random_engine& random,
	LootTableLookupRef LootTableLookup,
	LootDrops outLoot,
	unsigned int count) const 
{
	for (unsigned int i = 0; i < count; i++)
	{
		Entry::ptr entry = Entries[PickRandomIndex(random, Entries)];
		entry->GenerateLoot(random, LootTableLookup, outLoot);
	}
}

void UniqueRandomLootTable::GenerateLoot(
	std::default_random_engine& random, 
	LootTableLookupRef LootTableLookup,
	LootDrops outLoot,
	unsigned int count) const 
{
	// gotta keep track of what we haven't looted yet
	std::vector<Entry::ptr> unusedEntries = Entries;

	// we should stop looting when we run out of unique things to loot
	count = std::max(count, Entries.size());

	for (unsigned int i = 0; i < count; i++)
	{
		unsigned int entryIndex = PickRandomIndex(random, unusedEntries);
		Entry::ptr entry = unusedEntries[entryIndex];
		entry->GenerateLoot(random, LootTableLookup, outLoot);
		unusedEntries.erase(unusedEntries.begin() + entryIndex);
	}
}