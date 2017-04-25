#pragma once
#include <string>
#include <random>
#include "json.hpp"
#include <memory>
#include <unordered_map>

// checks whether a given json object contains the key with the given type
bool jsonContains(const nlohmann::json& json, const std::string& key, nlohmann::detail::value_t type);

// checks whether a given json object contains the key with at least one of the three json number types
bool jsonContainsNumber(const nlohmann::json& json, const std::string& key);

class LootTable;

// an abstract entry in a loot table. generates loot.
class Entry
{
public:
	using ptr = std::shared_ptr<Entry>;

	// the name of this loot type:
	// for ItemEntry the name of the loot dropped
	// for TableEntry the name of the LootTable to drop loot from
	std::string EntryName;

	// the range for the number of times this entry will generate loot when selected from its table
	// inclusive
	unsigned int MinDrops, MaxDrops;

	// the chance of selecting this entry from its loot table
	float SelectionWeight;

	// generates loot from this table
	virtual void GenerateLoot(
		std::default_random_engine& random,
		std::unordered_map<std::string, std::shared_ptr<LootTable>> LootTableLookup,
		std::unordered_map<std::string, unsigned int>& outLoot) const = 0;

	// figures out if the json is an ItemEntry or TableEntry
	// returns shared ptr to parsed entry or nullptr if there was a parsing error
	static ptr ParseEntryType(const nlohmann::json& json);

	// constructs an Entry from json. sets isError to true if there was a parsing error
	Entry(const nlohmann::json& json, bool& isError);
	virtual ~Entry() {}
};

// an entry that generates loot from a LootTable
class TableEntry : public Entry
{
public:
	TableEntry(const nlohmann::json& json, bool& isError);
	virtual void GenerateLoot(
		std::default_random_engine& random,
		std::unordered_map<std::string, std::shared_ptr<LootTable>> LootTableLookup,
		std::unordered_map<std::string, unsigned int>& outLoot) const override;
};

// an entry that generates a single kind of loot
class ItemEntry : public Entry
{
public:
	ItemEntry(const nlohmann::json& json, bool& isError);
	virtual void GenerateLoot(
		std::default_random_engine& random,
		std::unordered_map<std::string, std::shared_ptr<LootTable>> LootTableLookup,
		std::unordered_map<std::string, unsigned int>& outLoot) const override;
};