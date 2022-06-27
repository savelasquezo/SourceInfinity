#include "../../common/service.h"
#ifdef ENABLE_DUNGEON_INFO_SYSTEM
#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "desc.h"
#include "char.h"
#include "db.h"
#include "config.h"
#include "DungeonInfo.h"
#include "affect.h"
#include "item.h"
#include "questmanager.h"
#include <boost/algorithm/string.hpp>

struct DungeonInfoTable {
	int type; // Dungeon type [ 0 (Unkown), 1 (Private), 2 (Global) ]
	int organization; // Dungeon organization [ 0 (None), 1 (Party), 2 (Guild) ]
	int min_level;  // Dungeon level limit [ min_level ]
	int party_members;  // Dungeon party members [ max_members (0) Unlimited ]
	int map_index;  // Dungeon map index
	char map_name[64]; // Map Name
	char map_entrance[64]; // Map Name Entrance
	int map_coords[2]; // Map Coords (X,Y)
	int cooldown;  // [ 0 (None) ] | Ex: 60 * 60 * 3 = 3 hours
	int duration;  // [ 0 (None) ] | Ex: 60 * 60 * 3 = 3 hours
	int max_level; // Dungeon level min [ max_level ]
	int strength_bonus;  // Strength bonus id against dungeon monsters
	int resistance_bonus; // Resistance bonus id against dungeon monsters
	int item_vnum;  // Required dungeon item
	int boss_vnum; // Boss Vnum
};

DungeonInfoTable dungeonTable[3] = 
{

	{2, 0, 75, 8 , 66, "Demon Tower", "Hwang Temple" , {5906, 1108}, 60, 0 , 95, 63, 0, 0, 1093}, // Index 0
	{2, 0, 80, 8 , 216, "Devil's Catacomb", "Devil's Catacomb" , {5906, 1108}, 1800, 0 , 105, 63, 0, 30319, 2598}, // Index 1
	{1, 0, 95, 8 , 322, "Temple of the Ochao", "Enchanted Forest" , {5906, 1108}, 3600, 0 , 120, 63, 0, 0, 6391} // Index 2
};

DungeonInfo::DungeonInfo()
{
}

DungeonInfo::~DungeonInfo()
{
}

bool DungeonInfo::IsValidTeleportLocation(int x, int y) const
{
	for (const auto& entry : dungeonTable)
	{
		if (entry.map_coords[0] == x && entry.map_coords[1] == y)
			return true;
	}
	return false;
}

void DungeonInfo::UpdateDungeonRank(LPCHARACTER ch, int dungeonID, int rankingType)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CleanDungeonRanking");

	char query[8192];

	if (rankingType == 1)
		snprintf(query, sizeof(query), "SELECT name, level, finished FROM log.dungeon_ranking WHERE dungeon_id = '%d';", dungeonID);
	//else if (rankingType == 2)
	//	snprintf(query, sizeof(query), "SELECT name, level, fastest_time FROM log.dungeon_ranking WHERE dungeon_id = '%d';", dungeonID);
	//else
	//	snprintf(query, sizeof(query), "SELECT name, level, highest_damage FROM log.dungeon_ranking WHERE dungeon_id = '%d';", dungeonID);

	SQLMsg* pkMsg(DBManager::instance().DirectQuery(query));
	SQLResult* Res = pkMsg->Get();

	if (Res->uiNumRows > 0) {
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(Res->pSQLResult)) != NULL)
		{
			int index = 0;
			ch->ChatPacket(CHAT_TYPE_COMMAND, "UpdateDungeonRanking %s %d %d", row[0], atoi(row[1]), atoi(row[2]));

			//sys_err("UpdateDungeonRanking %s %d %d", row[0], atoi(row[1]), atoi(row[2]));
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenDungeonRanking");
}

void DungeonInfo::UpdateRankLog(LPCHARACTER ch, int dungeonID, int fastestTime, int highestDamage)
{
	if (ch == NULL)
		return;

	if (!ch->IsPC())
		return;

	int timeValue = fastestTime;
	int highestValue = highestDamage;

	SQLMsg* pkMsg(DBManager::instance().DirectQuery("SELECT * FROM log.dungeon_ranking WHERE dungeon_id = '%d' and name = '%s';", dungeonID, ch->GetName()));
	SQLResult* Res = pkMsg->Get();

	if (Res->uiNumRows > 0) {
		MYSQL_ROW row;
		if ((row = mysql_fetch_row(Res->pSQLResult)) != NULL) {

			if (timeValue < atoi(row[4]))
				timeValue = atoi(row[4]);

			if (highestValue < atoi(row[5]))
				highestValue = atoi(row[5]);

			DBManager::instance().DirectQuery("UPDATE log.dungeon_ranking SET level = '%d', finished = finished + 1, fastest_time = '%d', highest_damage = '%d' WHERE dungeon_id = '%d' and name = '%s';", ch->GetLevel(), timeValue, highestValue, dungeonID, ch->GetName());
		}
	}
	else
		DBManager::instance().DirectQuery("INSERT INTO log.dungeon_ranking VALUES('%d', '%s', '1', '%d', '%d', '%d');", dungeonID, ch->GetName(), ch->GetLevel(), timeValue, highestValue);
}

int DungeonInfo::GetRank(LPCHARACTER ch, int dungeonID, int pointType)
{
	if (ch == NULL)
		return 0;

	if (!ch->IsPC())
		return 0;

	SQLMsg* pkMsg(DBManager::instance().DirectQuery("SELECT finished, fastest_time, highest_damage FROM log.dungeon_ranking WHERE dungeon_id = '%d' and name = '%s';", dungeonID, ch->GetName()));
	SQLResult* Res = pkMsg->Get();

	if (Res->uiNumRows > 0) {
		MYSQL_ROW row;
		if ((row = mysql_fetch_row(Res->pSQLResult)) != NULL) {
			if (pointType == 1)
				return atoi(row[0]);
			else if (pointType == 2)
				return atoi(row[1]);
			else if (pointType == 3)
				return atoi(row[2]);
			else
				return 0;
		}
	}
	else
		return 0;
}

void DungeonInfo::Update(LPCHARACTER ch)
{
	if (ch == NULL)
		return;

	if (!ch->IsPC())
		return;

	if (sizeof(dungeonTable) == 0)
		return;

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CleanDungeonInfo");
	int size = *(&dungeonTable + 1) - dungeonTable;
	for (int i = 0; i <= size - 1; i++)
	{
		int dungeonFinished = GetRank(ch, i, 1);
		int dungeonFastestTime = GetRank(ch, i, 2);
		int dungeonHighestDamage = GetRank(ch, i, 3);

		std::string mapName(dungeonTable[i].map_name);
		boost::algorithm::replace_all(mapName, " ", "_");

		std::string mapEntrance(dungeonTable[i].map_entrance);
		boost::algorithm::replace_all(mapEntrance, " ", "_");

		ch->ChatPacket(CHAT_TYPE_COMMAND, "UpdateDungeonInfo %d %d %d %d %d %s %s %d %d %d %d %d %d %d %d %d %d %d %d",
			dungeonTable[i].type,
			dungeonTable[i].organization,
			dungeonTable[i].min_level,
			dungeonTable[i].party_members,
			dungeonTable[i].map_index,
			mapName.c_str(),
			mapEntrance.c_str(),
			dungeonTable[i].map_coords[0],
			dungeonTable[i].map_coords[1],
			dungeonTable[i].cooldown,
			dungeonTable[i].duration,
			dungeonTable[i].max_level,
			dungeonTable[i].strength_bonus,
			dungeonTable[i].resistance_bonus,
			dungeonTable[i].item_vnum,
			dungeonTable[i].boss_vnum,
			dungeonFinished,
			dungeonFastestTime,
			dungeonHighestDamage
		);
	}
}
#endif