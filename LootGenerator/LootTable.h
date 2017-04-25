#pragma once
#include <string>
#include "json.hpp"
#include <random>
#include <vector>
#include "Entry.h"
#include <memory>

// a collection of loot entries for generating loot
class LootTable
{
public:
	using ptr = std::shared_ptr<LootTable>;

	// the key used by TableEntries to look up this LootTable
	std::string TableName;
	// a collection of one or more TableEntries
	std::vector<Entry::ptr> Entries;

	// generates loot from this table a given number of times
	virtual void GenerateLoot(
		std::default_random_engine& random,
		LootTableLookupRef LootTableLookup,
		LootDrops outLoot,
		unsigned int count) const = 0;

	// picks a random entry from the given vector
	static unsigned int PickRandomIndex(std::default_random_engine& random, const std::vector<Entry::ptr>& entries);
	// determines if the json is a UniqueRandom or Random LootTable
	// returns a shared ptr to the parsed LootTable or nullptr if there was a parsing error
	static ptr ParseTableType(const nlohmann::json& json);

	virtual ~LootTable() {}

	// constructs a LootTable from json. sets isError to true if there is a parsing error.
	LootTable(const nlohmann::json& json, bool& isError);
	
};

// a LootTable that selects items randomly and can repeatedly select items
class RandomLootTable : public LootTable
{
public:
	virtual void GenerateLoot(
		std::default_random_engine& random,
		LootTableLookupRef LootTableLookup,
		LootDrops outLoot,
		unsigned int count) const override;
	RandomLootTable(const nlohmann::json& json, bool& isError);
};

// a LootTable that selects items randomly, but it only selects each item once per call to GenerateLoot()
class UniqueRandomLootTable : public LootTable
{
public:
	virtual void  GenerateLoot(
		std::default_random_engine& random,
		LootTableLookupRef LootTableLookup,
		LootDrops outLoot,
		unsigned int count) const override;
	UniqueRandomLootTable(const nlohmann::json& json, bool& isError);
};