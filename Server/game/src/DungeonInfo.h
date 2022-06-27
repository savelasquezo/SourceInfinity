#include "../../common/service.h"
#ifdef ENABLE_DUNGEON_INFO_SYSTEM
#ifndef __INC_METIN_II_GAME_DUNGEON_INFO_H__
#define __INC_METIN_II_GAME_DUNGEON_INFO_H__

class DungeonInfo : public singleton<DungeonInfo>
{
	public:
		DungeonInfo();
		~DungeonInfo();
	
	void	UpdateDungeonRank(LPCHARACTER ch, int dungeonID, int rankingType);
	bool    IsValidTeleportLocation(int x, int y) const;
	void	UpdateRankLog(LPCHARACTER ch, int dungeonID, int fastestTime, int highestDamage);
	int		GetRank(LPCHARACTER ch, int dungeonID, int pointType);
	void	Update(LPCHARACTER ch);
};
#endif
#endif