#include "stdafx.h"
#include "chinese_temple.h"
#include "char.h"
#include "char_manager.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "questmanager.h"

namespace ChineseTemple
{
	void CDungeonMgr::Initialize()
	{
		pkMetin0 = NULL;
		pkMetin1 = NULL;
		Prepare();
	}
	
	void CDungeonMgr::Destroy()
	{
		pkMetin0 = NULL;
		pkMetin1 = NULL;
	}
	
	void CDungeonMgr::Prepare()
	{
		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(CHINESE_TEMPLE_DEFAULT_MAP_INDEX);
		if (!pMap)
			return;
				
		if (pkMetin0 == NULL)
			pkMetin0 = CHARACTER_MANAGER::instance().SpawnMob(CHINESE_TEMPLE_METIN0_VNUM, CHINESE_TEMPLE_DEFAULT_MAP_INDEX, pMap->m_setting.iBaseX + 100 * 100, pMap->m_setting.iBaseY + 100 * 100, 0, true, 1 == 0 ? -1 : (1 - 1) * 45);
		if (pkMetin1 == NULL)
			pkMetin1 = CHARACTER_MANAGER::instance().SpawnMob(CHINESE_TEMPLE_METIN1_VNUM, CHINESE_TEMPLE_DEFAULT_MAP_INDEX, pMap->m_setting.iBaseX + 100 * 100, pMap->m_setting.iBaseY + 100 * 100, 0, true, 1 == 0 ? -1 : (1 - 1) * 45);
	}
	
	void CDungeonMgr::Restart()
	{
		pkMetin0 = NULL;
		pkMetin1 = NULL;
		Prepare();
	}
}