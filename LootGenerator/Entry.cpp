#include "Entry.h"
#include "LootTable.h"

bool jsonContains(const nlohmann::json& json, const std::string& key, nlohmann::detail::value_t type)
{
	return json.find(key) != json.end() && json[key].type() == type;
}

bool jsonContainsNumber(const nlohmann::json& json, const std::string& key)
{
	return json.find(key) != json.end() &&
		(json[key].type() == nlohmann::detail::value_t::number_float ||
			json[key].type() == nlohmann::detail::value_t::number_integer ||
			json[key].type() == nlohmann::detail::value_t::number_unsigned);
}

Entry::Entry(const nlohmann::json& json, bool& isError)
{
	if (!jsonContains(json, "EntryName", nlohmann::detail::value_t::string) ||
		!jsonContainsNumber(json, "SelectionWeight") ||
		!jsonContainsNumber(json, "MinDrops") ||
		!jsonContainsNumber(json, "MaxDrops"))
	{
		// error parsing json
		isError = true;
		return;
	}
	EntryName = json["EntryName"].get<std::string>();
	SelectionWeight = json["SelectionWeight"];
	MinDrops = json["MinDrops"];
	MaxDrops = json["MaxDrops"];
}

Entry::ptr Entry::ParseEntryType(const nlohmann::json& json)
{
	if (!jsonContains(json, "EntryType", nlohmann::detail::value_t::string))
	{
		return nullptr;
	}
	std::string entryType = json["EntryType"].get<std::string>();
	bool isError = false;
	if (entryType == "Item")
	{
		ptr parsedEntry = std::make_shared<ItemEntry>(json, isError);
		if (isError)
		{
			return nullptr;
		}
		else
		{
			return parsedEntry;
		}
	}
	else if (entryType == "Table")
	{
		ptr parsedEntry = std::make_shared<TableEntry>(json, isError);
		if (isError)
		{
			return nullptr;
		}
		else
		{
			return parsedEntry;
		}
	}
	else
	{
		return nullptr;
	}
}

ItemEntry::ItemEntry(const nlohmann::json& json, bool& isError) : Entry(json, isError) { }
TableEntry::TableEntry(const nlohmann::json& json, bool& isError) : Entry(json, isError) { }

void ItemEntry::GenerateLoot(
	std::default_random_engine& random,
	LootTableLookupRef LootTableLookup,
	LootDrops outLoot) const
{
	int amountToDrop = GenerateNumDrops(random);
	// update the map with the generated loot
	// if the loot type doesn't exist in the map yet, its value will default to 0
	outLoot[EntryName] += amountToDrop;
}

void TableEntry::GenerateLoot(
	std::default_random_engine& random,
	LootTableLookupRef LootTableLookup,
	LootDrops outLoot) const
{
	unsigned int count = GenerateNumDrops(random);
	// lookup our loot table
	if (LootTableLookup.find(EntryName) == LootTableLookup.end())
	{
		// TODO: scream
		return;
	}
	LootTable::ptr lootTable = LootTableLookup.at(EntryName);
	lootTable->GenerateLoot(random, LootTableLookup, outLoot, count);
}

unsigned int Entry::GenerateNumDrops(std::default_random_engine& random) const
{
	std::uniform_int_distribution<int> uni(MinDrops, MaxDrops);
	return uni(random);
}