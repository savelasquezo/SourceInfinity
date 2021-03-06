#include "stdafx.h"

#include "config.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "char.h"
#include "char_manager.h"
#include "affect.h"
#include "item.h"
#include "item_manager.h"
#include "guild_manager.h"
#include "war_map.h"
#include "start_position.h"
#include "marriage.h"
#include "mining.h"
#include "p2p.h"
#include "polymorph.h"
#include "desc_client.h"
#include "messenger_manager.h"
#include "log.h"
#include "utils.h"
#include "unique_item.h"
#include "mob_manager.h"
#ifdef TOURNAMENT_PVP_SYSTEM
	#include "tournament.h"
#endif
#if defined(WJ_COMBAT_ZONE)
	#include "combat_zone.h"
#endif
#ifdef ENABLE_REBORN_SYSTEM
	#include "reborn.h"
#endif
#include "shop_manager.h"
#include "shop.h"
#include "input.h"

#ifdef ENABLE_DICE_SYSTEM
#include "party.h"
#endif

#define ENABLE_DRAGONLAIR_SYSTEMS

#include <cctype>
#undef sys_err
#ifndef __WIN32__
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

extern int g_nPortalLimitTime;
extern LPCLIENT_DESC db_clientdesc;
const int ITEM_BROKEN_METIN_VNUM = 28960;
static int ejderde_kac_kisiyiz = 0;
static int kulede_kac_kisiyiz = 0;

static int imp_sari = 0;
static int imp_mavi = 0;
static int imp_kirmizi = 0;
static int imp_online = 0;
static int arena_online = 0;

bool FN_IS_VALID_LOGIN_STRING2(const char *str)
{
	const char*	tmp;

	if (!str || !*str)
		return false;

	for (tmp = str; *tmp; ++tmp)
	{
		if (isnhspace(*tmp))
			return false;

		if (isnhdigit(*tmp))
			return false;

		if (!ishan(*tmp) && isalpha(*tmp))
			return false;
	}

	return true;
}

namespace quest
{
//#ifdef __DUNGEON_INFO_SYSTEM__
//	int pc_get_last_damage(lua_State* L)
//	{
//		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//		if (!ch)
//			return 0;
//		
//		if (lua_isnumber(L, 1))
//		{
//			LogManager::instance().DungeonRegistros(ch->GetPlayerID(), ch->GetName(), ch->GetLevel(), ch->GetQuestDamage(), lua_tonumber(L, 1));
//			return 1;			
//		}
//		return 1;		
//	}
//	
//	int pc_get_last_raking(lua_State* L)
//	{
//		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//		if (!ch)
//			return 0;
//		
//		if (lua_isnumber(L, 1))
//		{
//			LogManager::instance().DungeonRanking(ch->GetPlayerID(), ch->GetName(), ch->GetLevel(), 1, lua_tonumber(L, 1));
//			return 1;			
//		}
//		return 1;		
//	}
//	
//	//int pc_set_time_dung(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfo #%d#%d#", 0, tiempo);
//	//	return 1;
//	//}
//	
//	//int pc_set_time_dung_cata(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfoCata #%d#%d#", 1, tiempo);
//	//	return 1;
//	//}
//	
//	//int pc_set_time_dung_nemere(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfoNemere #%d#%d#", 2, tiempo);
//	//	return 1;
//	//}
//	
//	//int pc_set_time_dung_puertas(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfoPuertas #%d#%d#", 3, tiempo);
//	//	return 1;
//	//}
//	
//	//int pc_set_time_dung_slime(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfoSlime #%d#%d#", 4, tiempo);
//	//	return 1;
//	//}
//	
//	//int pc_set_time_dung_arya(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfoArya #%d#%d#", 5, tiempo);
//	//	return 1;
//	//}
//	
//	//int pc_set_time_dung_bosque(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfoBosque #%d#%d#", 6, tiempo);
//	//	return 1;
//	//}
//	
//	//int pc_set_time_dung_orco(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfoOrco #%d#%d#", 7, tiempo);
//	//	return 1;
//	//}
//	//int pc_set_time_dung_isla(lua_State* L)
//	//{
//	//	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
//	//	if (!ch)
//	//		return 0;
//	//	
//	//	if (!lua_isnumber(L, 1))
//	//	{
//	//		sys_err("Argumentos invalidos, coloca 1 numero.");
//	//		return 1;			
//	//	}
//	//	int tiempo = (int) lua_tonumber(L, 1);
//	//	ch->ChatPacket(CHAT_TYPE_COMMAND, "TiempoDungeonInfoIsla #%d#%d#", 8, tiempo);
//	//	return 1;
//	//}
//#endif
	struct ArenaBaslat
	{
		ArenaBaslat() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					if (!ch->IsPC() || ch->IsGM() || ch->GetMapIndex() != 200)
						return;

					LPPARTY pParty = ch->GetParty();
					if (pParty != NULL)
					{
						if (pParty->GetMemberCount() == 2)
						{
							CPartyManager::instance().DeleteParty(pParty);
						}
						else
						{
							pParty->Quit(ch->GetPlayerID());
						}
					}

					ch->SetKillerMode(true);
				}
			}
		}
	};

	struct ArenaOnline
	{
		ArenaOnline() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					if (!ch->IsPC() || ch->IsGM())
						return;

					arena_online = arena_online + 1;
				}
			}
		}
	};

	int pc_arena_baslat(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L, 1));

		if (NULL != pSecMap)
		{
			ArenaBaslat f;
			pSecMap->for_each(f);
		}

		return 0;
	}

	int pc_arena_hemen_dog(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->RestartAtSamePos();
		ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
		ch->DeathPenalty(0);
		ch->ReviveInvisible(0);

		return 0;
	}

	int pc_arena_killer_mode(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->SetKillerMode(true);
		return 0;
	}

	int pc_arena_online(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L, 1));

		if (NULL != pSecMap)
		{
			ArenaOnline f;
			arena_online = 0;
			pSecMap->for_each(f);
			lua_pushnumber(L, arena_online);
		}

		return 1;
	}

	int pc_change_race(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1)) 
		{
			return 0;
		}
		if (ch->GetWear(WEAR_BODY) || ch->GetWear(WEAR_WEAPON) || ch->GetWear(WEAR_HEAD) || ch->GetWear(WEAR_COSTUME_BODY) || ch->GetWear(WEAR_COSTUME_HAIR) || ch->GetWear(WEAR_COSTUME_WEAPON)) {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Error> Liberate del equipo antes de realizar este cambio!"));
			return 0;
		}

		ch->SetRace(lua_tonumber(L, 1));
		ch->SetSkillGroup(0);
		ch->ClearSkill();
		ch->ClearSubSkill();
		ch->SetPolymorph(101);
		ch->SetPolymorph(0);
		return 0;
	}

	int pc_theprnce_hapis_go(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			lua_pushboolean(L, false);
			return 1;
		}

		int x = (int)lua_tonumber(L, 1);
		int y = (int)lua_tonumber(L, 2);


		PIXEL_POSITION p;

		if (SECTREE_MANAGER::instance().GetMapBasePosition(ch->GetX(), ch->GetY(), p))
		{
			x += p.x / 100;
			y += p.y / 100;
		}

		x *= 100;
		y *= 100;



		ch->Show(201, x, y, 0);
		ch->Stop();


		lua_pushboolean(L, true);

		return 1;
	}

	int pc_theprnce_alisveris_kontrol(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->GetExchange())
		{
			lua_pushnumber(L, 1);
			return 1;
		}
		else if (ch->IsOpenSafebox())
		{
			lua_pushnumber(L, 2);
			return 1;
		}
		else if (ch->GetShopOwner())
		{
			lua_pushnumber(L, 3);
			return 1;
		}
		else if (ch->IsCubeOpen())
		{
			lua_pushnumber(L, 4);
			return 1;
		}
		else
		{
			lua_pushnumber(L, 5);
			return 1;
		}
	}

	int pc_theprnce_chat_ban(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		const char * sure = lua_tostring(L, 1);

		long lBlockDuration = parse_time_str(sure);

		ch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, lBlockDuration, 0, true);
		return 0;
	}

	int pc_is_arena(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetArena() != NULL);
		return 1;
	}

	int pc_is_arena_observer(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetArenaObserverMode());
		return 1;
	}

	EVENTINFO(imp_baslat_event_info)
	{
		DWORD dwWarpMapIndex;

		imp_baslat_event_info()
			: dwWarpMapIndex(0)
		{
		}
	};

	struct FImpBaslat
	{
		FImpBaslat() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					BYTE bEmpire = ch->GetEmpire();
					if (bEmpire == 0)
					{
						sys_err("Unkonwn Empire %s %d ", ch->GetName(), ch->GetPlayerID());
						return;
					}

					if (ch->IsGM())
					{
						return;
					}


					LPPARTY pParty = ch->GetParty();
					if (pParty != NULL)
					{
						if (pParty->GetMemberCount() == 2)
						{
							CPartyManager::instance().DeleteParty(pParty);
						}
						else
						{
							pParty->Quit(ch->GetPlayerID());
						}
					}



					if (bEmpire == 1)
					{
						ch->WarpSet(14759500, 14766900);
					}
					else if (bEmpire == 2)
					{
						ch->WarpSet(14765700, 14759700);
					}
					else if (bEmpire == 3)
					{
						ch->WarpSet(14759800, 14759000);
					}
				}
			}
		}
	};

	EVENTFUNC(impbaslat_event)
	{
		imp_baslat_event_info * info = dynamic_cast<imp_baslat_event_info *>(event->info);

		if (info == NULL)
		{
			sys_err("impbaslat_event> <Factor> Null pointer");
			return 0;
		}

		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(info->dwWarpMapIndex);

		if (NULL != pSecMap)
		{
			FImpBaslat f;
			pSecMap->for_each(f);
		}

		return 0;
	}

	int pc_imp_baslat(lua_State * L)
	{
		int iMapIndex = static_cast<int>(lua_tonumber(L, 1));
		int iSec = static_cast<int>(lua_tonumber(L, 2));

		imp_baslat_event_info* info = AllocEventInfo<imp_baslat_event_info>();

		info->dwWarpMapIndex = iMapIndex;

		event_create(impbaslat_event, info, PASSES_PER_SEC(iSec));


		return 0;
	}

	struct ImpOnline
	{
		ImpOnline() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					BYTE bEmpire = ch->GetEmpire();

					if (!ch->IsPC() || ch->IsGM())
						return;

					imp_online = imp_online + 1;
				}
			}
		}
	};

	int pc_imp_katilimci(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L, 1));

		if (NULL != pSecMap)
		{
			ImpOnline f;
			imp_online = 0;
			pSecMap->for_each(f);
			lua_pushnumber(L, imp_online);
		}

		return 1;
	}

	EVENTINFO(imp_event_kirmizi_event_info)
	{
		DWORD dwWarpMapIndex;

		imp_event_kirmizi_event_info()
			: dwWarpMapIndex(0)
		{
		}
	};

	struct FImpKirmiziEvent
	{
		FImpKirmiziEvent() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					BYTE bEmpire = ch->GetEmpire();
					if (bEmpire == 0)
					{
						sys_err("Unkonwn Empire %s %d ", ch->GetName(), ch->GetPlayerID());
						return;
					}

					if (bEmpire != 1 && !ch->IsGM())
					{
						ch->WarpSet(g_start_position[bEmpire][0], g_start_position[bEmpire][1]);
					}
					else
					{
						ch->WarpSet(14749800, 14774900);
					}
				}
			}
		}
	};

	EVENTFUNC(impkirmizi_event)
	{
		imp_event_kirmizi_event_info * info = dynamic_cast<imp_event_kirmizi_event_info *>(event->info);

		if (info == NULL)
		{
			sys_err("impkirmizi_event> <Factor> Null pointer");
			return 0;
		}

		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(info->dwWarpMapIndex);

		if (NULL != pSecMap)
		{
			FImpKirmiziEvent f;
			pSecMap->for_each(f);
		}

		return 0;
	}

	int pc_imp_event_yolla_kirmizi(lua_State * L)
	{
		int iMapIndex = static_cast<int>(lua_tonumber(L, 1));
		int iSec = static_cast<int>(lua_tonumber(L, 2));

		imp_event_kirmizi_event_info* info = AllocEventInfo<imp_event_kirmizi_event_info>();

		info->dwWarpMapIndex = iMapIndex;

		event_create(impkirmizi_event, info, PASSES_PER_SEC(iSec));


		return 0;
	}

	EVENTINFO(imp_event_sari_event_info)
	{
		DWORD dwWarpMapIndex;

		imp_event_sari_event_info()
			: dwWarpMapIndex(0)
		{
		}
	};

	struct FImpSariEvent
	{
		FImpSariEvent() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					BYTE bEmpire = ch->GetEmpire();
					if (bEmpire == 0)
					{
						sys_err("Unkonwn Empire %s %d ", ch->GetName(), ch->GetPlayerID());
						return;
					}

					if (bEmpire != 2 && !ch->IsGM())
					{
						ch->WarpSet(g_start_position[bEmpire][0], g_start_position[bEmpire][1]);
					}
					else
					{
						ch->WarpSet(14749800, 14774900);
					}
				}
			}
		}
	};

	EVENTFUNC(impsari_event)
	{
		imp_event_sari_event_info * info = dynamic_cast<imp_event_sari_event_info *>(event->info);

		if (info == NULL)
		{
			sys_err("impsari_event> <Factor> Null pointer");
			return 0;
		}

		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(info->dwWarpMapIndex);

		if (NULL != pSecMap)
		{
			FImpSariEvent f;
			pSecMap->for_each(f);
		}

		return 0;
	}

	int pc_imp_event_yolla_sari(lua_State * L)
	{
		int iMapIndex = static_cast<int>(lua_tonumber(L, 1));
		int iSec = static_cast<int>(lua_tonumber(L, 2));

		imp_event_sari_event_info* info = AllocEventInfo<imp_event_sari_event_info>();

		info->dwWarpMapIndex = iMapIndex;

		event_create(impsari_event, info, PASSES_PER_SEC(iSec));


		return 0;
	}

	EVENTINFO(imp_event_mavi_event_info)
	{
		DWORD dwWarpMapIndex;

		imp_event_mavi_event_info()
			: dwWarpMapIndex(0)
		{
		}
	};

	struct FImpmaviEvent
	{
		FImpmaviEvent() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					BYTE bEmpire = ch->GetEmpire();
					if (bEmpire == 0)
					{
						sys_err("Unkonwn Empire %s %d ", ch->GetName(), ch->GetPlayerID());
						return;
					}

					if (bEmpire != 3 && !ch->IsGM())
					{
						ch->WarpSet(g_start_position[bEmpire][0], g_start_position[bEmpire][1]);
					}
					else
					{
						ch->WarpSet(14749800, 14774900);
					}
				}
			}
		}
	};

	EVENTFUNC(impmavi_event)
	{
		imp_event_mavi_event_info * info = dynamic_cast<imp_event_mavi_event_info *>(event->info);

		if (info == NULL)
		{
			sys_err("impmavi_event> <Factor> Null pointer");
			return 0;
		}

		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(info->dwWarpMapIndex);

		if (NULL != pSecMap)
		{
			FImpmaviEvent f;
			pSecMap->for_each(f);
		}

		return 0;
	}

	int pc_imp_event_yolla_mavi(lua_State * L)
	{
		int iMapIndex = static_cast<int>(lua_tonumber(L, 1));
		int iSec = static_cast<int>(lua_tonumber(L, 2));

		imp_event_mavi_event_info* info = AllocEventInfo<imp_event_mavi_event_info>();

		info->dwWarpMapIndex = iMapIndex;

		event_create(impmavi_event, info, PASSES_PER_SEC(iSec));


		return 0;
	}

	struct ImpOnlineKirmizi
	{
		ImpOnlineKirmizi() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{

					BYTE bEmpire = ch->GetEmpire();

					if (!ch->IsPC() || ch->IsGM() || bEmpire != 1)
						return;

					imp_kirmizi = imp_kirmizi + 1;
				}
			}
		}
	};

	struct ImpOnlineSari
	{
		ImpOnlineSari() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{

					BYTE bEmpire = ch->GetEmpire();

					if (!ch->IsPC() || ch->IsGM() || bEmpire != 2)
						return;

					imp_sari = imp_sari + 1;
				}
			}
		}
	};

	struct ImpOnlineMavi
	{
		ImpOnlineMavi() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					BYTE bEmpire = ch->GetEmpire();

					if (!ch->IsPC() || ch->IsGM() || bEmpire != 3)
						return;

					imp_mavi = imp_mavi + 1;
				}
			}
		}
	};

	int pc_imp_kirmizi(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L, 1));

		if (NULL != pSecMap)
		{
			ImpOnlineKirmizi f;
			imp_kirmizi = 0;
			pSecMap->for_each(f);
			lua_pushnumber(L, imp_kirmizi);
		}

		return 1;
	}

	int pc_imp_sari(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L, 1));

		if (NULL != pSecMap)
		{
			ImpOnlineSari f;
			imp_sari = 0;
			pSecMap->for_each(f);
			lua_pushnumber(L, imp_sari);
		}

		return 1;
	}

	int pc_imp_mavi(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L, 1));

		if (NULL != pSecMap)
		{
			ImpOnlineMavi f;
			imp_mavi = 0;
			pSecMap->for_each(f);
			lua_pushnumber(L, imp_mavi);
		}

		return 1;
	}

#ifdef WJ_CHANGELOOK_SYSTEM
	int pc_open_changelook(lua_State * L)
	{
		CQuestManager & qMgr = CQuestManager::instance();
		LPCHARACTER pkChar = qMgr.GetCurrentCharacterPtr();
		if (pkChar)
			pkChar->ChangeLookWindow(true);
		
		return 0;
	}
#endif

#ifdef __SASH_SYSTEM__
	int pc_open_sash(lua_State * L)
	{
		if (lua_isboolean(L, 1))
		{
			CQuestManager & qMgr = CQuestManager::instance();
			LPCHARACTER pkChar = qMgr.GetCurrentCharacterPtr();
			if (pkChar)
				pkChar->OpenSash(lua_toboolean(L, 1));
		}
		else
			sys_err("Invalid argument: arg1 must be boolean.");
		
		return 0;
	}
#endif

	int pc_has_master_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		bool bHasMasterSkill = false;
		for (int i=0; i< SKILL_MAX_NUM; i++)
			if (ch->GetSkillMasterType(i) >= SKILL_MASTER && ch->GetSkillLevel(i) >= 21)
			{
				bHasMasterSkill = true;
				break;
			}

		lua_pushboolean(L, bHasMasterSkill);
		return 1;
	}

#ifdef __BATTLE_PASS__
	int pc_do_battle_pass(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		
		if (ch->v_counts.empty())
			return 0;
		
		if (!ch->v_counts.empty())
		{
			for (int i=0; i<ch->missions_bp.size(); ++i)
			{
				if (ch->missions_bp[i].type == (int)lua_tonumber(L, 1)){	ch->DoMission(i, (DWORD)lua_tonumber(L, 2));}
			}
		}
		return 1;
	}
#endif

#ifdef ENABLE_OKAY_CARD
	int pc_get_okay_global_rank(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;

		char szBuf[4096+1];
		ch->GetGlobalRank(szBuf, sizeof(szBuf));
		lua_pushstring(L, szBuf);
		return 1;
	}

	int pc_get_okay_rund_rank(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;

		char szBuf[4096+1];
		ch->GetRundRank(szBuf, sizeof(szBuf));
		lua_pushstring(L, szBuf);
		return 1;
	}
#endif
	int pc_remove_skill_book_no_delay(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
		return 0;
	}

	int pc_is_skill_book_no_delay(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushboolean(L, ch->FindAffect(AFFECT_SKILL_NO_BOOK_DELAY) ? true : false);
		return 1;
	}

	int pc_learn_grand_master_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong skill index");
			return 0;
		}

		lua_pushboolean(L, ch->LearnGrandMasterSkill((long)lua_tonumber(L, 1)));
		return 1;
	}

#ifdef ENABLE_MESSENGER_BLOCK
	int pc_is_blocked(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong::pc_is_blocked");
			return 0;
		}
		const char * arg1 = lua_tostring(L, 1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);
		if(!tch || !tch->IsPC())
		{
			sys_err("wrong::pc_is_blocked-2");
			return 0;
		}

		lua_pushboolean(L, MessengerManager::instance().IsBlocked(ch->GetName(), tch->GetName()));
		return 1;
	}
	int pc_is_friend(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong::pc_is_blocked");
			return 0;
		}
		const char * arg1 = lua_tostring(L, 1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);
		if(!tch || !tch->IsPC())
		{
			sys_err("wrong::pc_is_blocked-2");
			return 0;
		}

		lua_pushboolean(L, MessengerManager::instance().IsFriend(ch->GetName(), tch->GetName()));
		return 1;
	}
#endif

	int pc_set_warp_location(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong map index");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("wrong coodinate");
			return 0;
		}

		ch->SetWarpLocation((long)lua_tonumber(L,1), (long)lua_tonumber(L,2), (long)lua_tonumber(L,3));
		return 0;
	}

	int pc_set_warp_location_local(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong map index");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("wrong coodinate");
			return 0;
		}

		long lMapIndex = (long) lua_tonumber(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);

		if (!region)
		{
			sys_err("invalid map index %d", lMapIndex);
			return 0;
		}

		int x = (int) lua_tonumber(L, 2);
		int y = (int) lua_tonumber(L, 3);

		if (x > region->ex - region->sx)
		{
			sys_err("x coordinate overflow max: %d input: %d", region->ex - region->sx, x);
			return 0;
		}

		if (y > region->ey - region->sy)
		{
			sys_err("y coordinate overflow max: %d input: %d", region->ey - region->sy, y);
			return 0;
		}

		ch->SetWarpLocation(lMapIndex, x, y);
		return 0;
	}

	int pc_get_start_location(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushnumber(L, g_start_map[ch->GetEmpire()]);
		lua_pushnumber(L, g_start_position[ch->GetEmpire()][0] / 100);
		lua_pushnumber(L, g_start_position[ch->GetEmpire()][1] / 100);
		return 3;
	}

	int pc_warp(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			lua_pushboolean(L, false);
			return 1;
		}

		long map_index = 0;

		if (lua_isnumber(L, 3))
			map_index = (int) lua_tonumber(L,3);

		//PREVENT_HACK
		if ( ch->IsHack() )
		{
			lua_pushboolean(L, false);
			return 1;
		}
		//END_PREVENT_HACK

		if ( test_server )
			ch->ChatPacket( CHAT_TYPE_INFO, "pc_warp %d %d %d",(int)lua_tonumber(L,1),
					(int)lua_tonumber(L,2),map_index );
		ch->WarpSet((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), map_index);

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_warp_local(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("no map index argument");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("no coodinate argument");
			return 0;
		}

		long lMapIndex = (long) lua_tonumber(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);

		if (!region)
		{
			sys_err("invalid map index %d", lMapIndex);
			return 0;
		}

		int x = (int) lua_tonumber(L, 2);
		int y = (int) lua_tonumber(L, 3);

		if (x > region->ex - region->sx)
		{
			sys_err("x coordinate overflow max: %d input: %d", region->ex - region->sx, x);
			return 0;
		}

		if (y > region->ey - region->sy)
		{
			sys_err("y coordinate overflow max: %d input: %d", region->ey - region->sy, y);
			return 0;
		}

		/*
		int iPulse = thecore_pulse();
		if ( pkChr->GetExchange() || pkChr->GetMyShop() || pkChr->GetShopOwner() || pkChr->IsOpenSafebox() )
		{
			pkChr->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("??????,???? ???? ?? ?????????? ?????????? ???????? ????????" ));

			return;
		}
		//PREVENT_PORTAL_AFTER_EXCHANGE
		//???? ?? ????????
		if ( iPulse - pkChr->GetExchangeTime()  < PASSES_PER_SEC(60) )
		{
			pkChr->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("???? ?? 1?? ???????? ???????????? ???? ?? ?? ????????." ) );
			return;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE
		//PREVENT_ITEM_COPY
		{
			if ( iPulse - pkChr->GetMyShopTime() < PASSES_PER_SEC(60) )
			{
				pkChr->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("???? ?? 1?? ???????? ???????????? ???? ?? ?? ????????." ) );
				return;
			}

		}
		//END_PREVENT_ITEM_COPY
		*/

		CQuestManager::instance().GetCurrentCharacterPtr()->WarpSet(region->sx + x, region->sy + y);
		return 0;
	}

	int pc_warp_exit(lua_State * L)
	{
		CQuestManager::instance().GetCurrentCharacterPtr()->ExitToSavedLocation();
		return 0;
	}

	int pc_in_dungeon(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetDungeon()?1:0);
		return 1;
	}

	int pc_hasguild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetGuild() ? 1 : 0);
		return 1;
	}

	int pc_getguild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetGuild() ? ch->GetGuild()->GetID() : 0);
		return 1;
	}

	int pc_isguildmaster(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		CGuild * g = ch->GetGuild();

		if (g)
			lua_pushboolean(L, (ch->GetPlayerID() == g->GetMasterPID()));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

#ifdef ENABLE_FIX_DESTROY_GUILD_IF_WAR_IS_ACTIVE
	int pc_destroy_guild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		CGuild * g = ch->GetGuild();

		if (g)
		{
			if (g->UnderAnyWar())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Lonca savasi esnasinda loncayi dagitamazsiniz");
				return 0;
			}
			else
				g->RequestDisband(ch->GetPlayerID());
		}

		return 0;
	}

	int pc_remove_from_guild(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		CGuild * g = ch->GetGuild();

		if (g)
		{
			if (g->UnderAnyWar())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Lonca savasi esnasinda loncayi dagitamazsiniz");
				return 0;
			}
			else
				g->RequestRemoveMember(ch->GetPlayerID());
		}

		return 0;
	}
#else
	int pc_destroy_guild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		CGuild * g = ch->GetGuild();

		if (g)
			g->RequestDisband(ch->GetPlayerID());

		return 0;
	}

	int pc_remove_from_guild(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		CGuild * g = ch->GetGuild();

		if (g)
			g->RequestRemoveMember(ch->GetPlayerID());

		return 0;
	}
#endif

	int pc_give_gold(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST : wrong argument");
			return 0;
		}

		int iAmount = (int) lua_tonumber(L, 1);

		if (iAmount <= 0)
		{
			sys_err("QUEST : gold amount less then zero");
			return 0;
		}

		DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), iAmount);
		ch->PointChange(POINT_GOLD, iAmount, true);
		return 0;
	}

	int pc_warp_to_guild_war_observer_position(lua_State* L)
	{
		luaL_checknumber(L, 1);
		luaL_checknumber(L, 2);

		DWORD gid1 = (DWORD)lua_tonumber(L, 1);
		DWORD gid2 = (DWORD)lua_tonumber(L, 2);

		CGuild* g1 = CGuildManager::instance().FindGuild(gid1);
		CGuild* g2 = CGuildManager::instance().FindGuild(gid2);

		if (!g1 || !g2)
		{
			luaL_error(L, "no such guild with id %d %d", gid1, gid2);
		}

		PIXEL_POSITION pos;

		DWORD dwMapIndex = g1->GetGuildWarMapIndex(gid2);

		if (!CWarMapManager::instance().GetStartPosition(dwMapIndex, 2, pos))
		{
			luaL_error(L, "not under warp guild war between guild %d %d", gid1, gid2);
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		//PREVENT_HACK
		if ( ch->IsHack() )
			return 0;
		//END_PREVENT_HACK

		ch->SetQuestFlag("war.is_war_member", 0);
		ch->SaveExitLocation();
		ch->WarpSet(pos.x, pos.y, dwMapIndex);
		return 0;
	}

	int pc_give_item_from_special_item_group(lua_State* L)
	{
		luaL_checknumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		DWORD dwGroupVnum = (DWORD) lua_tonumber(L,1);

		std::vector <DWORD> dwVnums;
		std::vector <DWORD> dwCounts;
		std::vector <LPITEM> item_gets(NULL);
		int count = 0;

		ch->GiveItemFromSpecialItemGroup(dwGroupVnum, dwVnums, dwCounts, item_gets, count);

		for (int i = 0; i < count; i++)
		{
			if (!item_gets[i])
			{
				if (dwVnums[i] == 1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?? %d ???? ????????????."), dwCounts[i]);
				}
				else if (dwVnums[i] == 2)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???????? ???? ?????? ???? ????????."));
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d?? ???????? ????????????."), dwCounts[i]);
				}
			}
		}
		return 0;
	}

	int pc_enough_inventory(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		DWORD item_vnum = (DWORD)lua_tonumber(L, 1);
		TItemTable * pTable = ITEM_MANAGER::instance().GetTable(item_vnum);
		if (!pTable)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		bool bEnoughInventoryForItem = ch->GetEmptyInventory(pTable->bSize) != -1;
		lua_pushboolean(L, bEnoughInventoryForItem);
		return 1;
	}

	int pc_give_item(lua_State* L)
	{
		PC* pPC = CQuestManager::instance().GetCurrentPC();
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, 1) || !(lua_isstring(L, 2)||lua_isnumber(L, 2)))
		{
			sys_err("QUEST : wrong argument");
			return 0;
		}

		DWORD dwVnum;

		if (lua_isnumber(L,2)) // ?????????? ?????? ????.
			dwVnum = (int) lua_tonumber(L, 2);
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 2), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			return 0;
		}

		int icount = 1;

		if (lua_isnumber(L, 3) && lua_tonumber(L, 3) > 0)
		{
			icount = (int)rint(lua_tonumber(L, 3));

			if (icount <= 0)
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L, 2));
				return 0;
			}
		}

		pPC->GiveItem(lua_tostring(L, 1), dwVnum, icount);

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);
		return 0;
	}

	int pc_give_or_drop_item(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, 1) && !lua_isnumber(L, 1))
		{
			sys_err("QUEST Make item call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum;

		if (lua_isnumber(L, 1)) // ?????????? ?????? ????.
		{
			dwVnum = (int) lua_tonumber(L, 1);
		}
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 1), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			lua_pushnumber (L, 0);

			return 1;
		}

		int icount = 1;
		if (lua_isnumber(L,2) && lua_tonumber(L,2)>0)
		{
			icount = (int)rint(lua_tonumber(L,2));
			if (icount<=0)
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L,2));
				lua_pushnumber (L, 0);
				return 1;
			}
		}

		sys_log(0, "QUEST [REWARD] item %s to %s", lua_tostring(L, 1), ch->GetName());

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);

		LPITEM item = ch->AutoGiveItem(dwVnum, icount);

		if ( dwVnum >= 80003 && dwVnum <= 80007 )
		{
			LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), QUEST, "quest: give_item2");
		}

		if (NULL != item)
			lua_pushnumber (L, item->GetID());
		else
			lua_pushnumber (L, 0);
		return 1;
	}

#ifdef ENABLE_DICE_SYSTEM
	int pc_give_or_drop_item_with_dice(lua_State* L)
	{
		if (!lua_isstring(L, 1) && !lua_isnumber(L, 1))
		{
			sys_err("QUEST Make item call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum;

		if (lua_isnumber(L, 1)) // ?????????? ?????? ????.
		{
			dwVnum = (int) lua_tonumber(L, 1);
		}
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 1), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			lua_pushnumber (L, 0);

			return 1;
		}

		int icount = 1;
		if (lua_isnumber(L,2) && lua_tonumber(L,2)>0)
		{
			icount = (int)rint(lua_tonumber(L,2));
			if (icount<=0)
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L,2));
				lua_pushnumber(L, 0);
				return 1;
			}
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM item = ITEM_MANAGER::instance().CreateItem(dwVnum, icount);
		if (ch->GetParty())
		{
			FPartyDropDiceRoll f(item, ch);
			f.Process(NULL);
			f.GetItemOwner()->AutoGiveItem(item);
		}
		else
			ch->AutoGiveItem(item);


		sys_log(0, "QUEST [REWARD] item %s to %s", lua_tostring(L, 1), ch->GetName());

		LogManager::instance().QuestRewardLog(CQuestManager::instance().GetCurrentPC()->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);

		lua_pushnumber(L, (item) ? item->GetID() : 0);
		return 1;
	}
#endif

	int pc_give_or_drop_item_and_select(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, 1) && !lua_isnumber(L, 1))
		{
			sys_err("QUEST Make item call error : wrong argument");
			return 0;
		}

		DWORD dwVnum;

		if (lua_isnumber(L, 1)) // ?????????? ?????? ????.
		{
			dwVnum = (int) lua_tonumber(L, 1);
		}
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 1), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			return 0;
		}

		int icount = 1;
		if (lua_isnumber(L,2) && lua_tonumber(L,2)>0)
		{
			icount = (int)rint(lua_tonumber(L,2));
			if (icount<=0)
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L,2));
				return 0;
			}
		}

		sys_log(0, "QUEST [REWARD] item %s to %s", lua_tostring(L, 1), ch->GetName());

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);

		LPITEM item = ch->AutoGiveItem(dwVnum, icount);

		if (NULL != item)
			CQuestManager::Instance().SetCurrentItem(item);

		if ( dwVnum >= 80003 && dwVnum <= 80007 )
		{
			LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), QUEST, "quest: give_item2");
		}

		return 0;
	}

	int pc_get_current_map_index(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetMapIndex());
		return 1;
	}

	int pc_get_x(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetX()/100);
		return 1;
	}

	int pc_get_y(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetY()/100);
		return 1;
	}

	int pc_get_local_x(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

		if (pMap)
			lua_pushnumber(L, (ch->GetX() - pMap->m_setting.iBaseX) / 100);
		else
			lua_pushnumber(L, ch->GetX() / 100);

		return 1;
	}

	int pc_get_local_y(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

		if (pMap)
			lua_pushnumber(L, (ch->GetY() - pMap->m_setting.iBaseY) / 100);
		else
			lua_pushnumber(L, ch->GetY() / 100);

		return 1;
	}

	int pc_count_item(lua_State* L)
	{
		if (lua_isnumber(L, -1))
			lua_pushnumber(L,CQuestManager::instance().GetCurrentCharacterPtr()->CountSpecifyItem((DWORD)lua_tonumber(L, -1)));
		else if (lua_isstring(L, -1))
		{
			DWORD item_vnum;

			if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
			{
				sys_err("QUEST count_item call error : wrong item name : %s", lua_tostring(L,1));
				lua_pushnumber(L, 0);
			}
			else
			{
				lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->CountSpecifyItem(item_vnum));
			}
		}
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_remove_item(lua_State* L)
	{
		if (lua_gettop(L) == 1)
		{
			DWORD item_vnum;

			if (lua_isnumber(L,1))
			{
				item_vnum = (DWORD)lua_tonumber(L, 1);
			}
			else if (lua_isstring(L,1))
			{
				if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
				{
					sys_err("QUEST remove_item call error : wrong item name : %s", lua_tostring(L,1));
					return 0;
				}
			}
			else
			{
				sys_err("QUEST remove_item wrong argument");
				return 0;
			}

			sys_log(0,"QUEST remove a item vnum %d of %s[%d]", item_vnum, CQuestManager::instance().GetCurrentCharacterPtr()->GetName(), CQuestManager::instance().GetCurrentCharacterPtr()->GetPlayerID());
			CQuestManager::instance().GetCurrentCharacterPtr()->RemoveSpecifyItem(item_vnum);
		}
		else if (lua_gettop(L) == 2)
		{
			DWORD item_vnum;

			if (lua_isnumber(L, 1))
			{
				item_vnum = (DWORD)lua_tonumber(L, 1);
			}
			else if (lua_isstring(L, 1))
			{
				if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
				{
					sys_err("QUEST remove_item call error : wrong item name : %s", lua_tostring(L,1));
					return 0;
				}
			}
			else
			{
				sys_err("QUEST remove_item wrong argument");
				return 0;
			}

			DWORD item_count = (DWORD) lua_tonumber(L, 2);
			sys_log(0, "QUEST remove items(vnum %d) count %d of %s[%d]",
					item_vnum,
					item_count,
					CQuestManager::instance().GetCurrentCharacterPtr()->GetName(),
					CQuestManager::instance().GetCurrentCharacterPtr()->GetPlayerID());

			CQuestManager::instance().GetCurrentCharacterPtr()->RemoveSpecifyItem(item_vnum, item_count);
		}
		return 0;
	}

	int pc_get_leadership(lua_State * L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetLeadershipSkillLevel());
		return 1;
	}

	int pc_reset_point(lua_State * L)
	{
		CQuestManager::instance().GetCurrentCharacterPtr()->ResetPoint(CQuestManager::instance().GetCurrentCharacterPtr()->GetLevel());
		return 0;
	}

	int pc_get_playtime(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetRealPoint(POINT_PLAYTIME));
		return 1;
	}

	int pc_get_vid(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetVID());
		return 1;
	}
	int pc_get_name(lua_State* L)
	{
		lua_pushstring(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetName());
		return 1;
	}

	int pc_get_next_exp(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetNextExp());
		return 1;
	}

	int pc_get_exp(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetExp());
		return 1;
	}

	int pc_get_race(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetRaceNum());
		return 1;
	}

	int pc_change_sex(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->ChangeSex());
		return 1;
	}

	int pc_get_job(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetJob());
		return 1;
	}

	int pc_get_max_sp(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetMaxSP());
		return 1;
	}

	int pc_get_sp(lua_State * L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetSP());
		return 1;
	}

	int pc_change_sp(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			lua_pushboolean(L, 0);
			return 1;
		}

		long val = (long) lua_tonumber(L, 1);

		if (val == 0)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (val > 0) // ?????????? ???????? ?????? ???? ????
			ch->PointChange(POINT_SP, val);
		else if (val < 0)
		{
			if (ch->GetSP() < -val)
			{
				lua_pushboolean(L, 0);
				return 1;
			}

			ch->PointChange(POINT_SP, val);
		}

		lua_pushboolean(L, 1);
		return 1;
	}

	int pc_get_max_hp(lua_State * L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetMaxHP());
		return 1;
	}

	int pc_get_hp(lua_State * L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetHP());
		return 1;
	}

	int pc_get_level(lua_State * L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetLevel());
		return 1;
	}

	int pc_set_level(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}
		else
		{
			int newLevel = lua_tonumber(L, 1);
			LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();


			sys_log(0,"QUEST [LEVEL] %s jumpint to level %d", ch->GetName(), (int)rint(lua_tonumber(L,1)));

			PC* pPC = CQuestManager::instance().GetCurrentPC();
			LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), newLevel, 0);

			//?????? : ????, ????????, ????
			ch->PointChange(POINT_SKILL, newLevel - ch->GetLevel());
			ch->PointChange(POINT_SUB_SKILL, newLevel < 10 ? 0 : newLevel - MAX(ch->GetLevel(), 9));
			ch->PointChange(POINT_STAT, ((MINMAX(1, newLevel, 90) - ch->GetLevel()) * 3) + ch->GetPoint(POINT_LEVEL_STEP));
			//????
			ch->PointChange(POINT_LEVEL, newLevel - ch->GetLevel());
			//HP, SP
			ch->SetRandomHP((newLevel - 1) * number(JobInitialPoints[ch->GetJob()].hp_per_lv_begin, JobInitialPoints[ch->GetJob()].hp_per_lv_end));
			ch->SetRandomSP((newLevel - 1) * number(JobInitialPoints[ch->GetJob()].sp_per_lv_begin, JobInitialPoints[ch->GetJob()].sp_per_lv_end));


			// ????
			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

			ch->ComputePoints();
			ch->PointsPacket();
			ch->SkillLevelPacket();

			return 0;
		}
	}

	int pc_get_weapon(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_WEAPON);

		if (!item)
			lua_pushnumber(L, 0);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_weapon_id(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_WEAPON);

		if (!item)
			lua_pushnumber(L, 0);
		else
			lua_pushnumber(L, item->GetID());

		return 1;
	}

	int pc_get_arrow_id(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_ARROW);
		lua_pushnumber(L, item ? item->GetID() : 0);
		return 1;
	}

	int pc_get_armor_id(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_BODY);
		lua_pushnumber(L, item ? item->GetID() : 0);
		return 1;
	}

	int pc_get_head_id(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_HEAD);
		lua_pushnumber(L, item ? item->GetID() : 0);
		return 1;
	}

	int pc_get_belt_id(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_BELT);
		lua_pushnumber(L, item ? item->GetID() : 0);
		return 1;
	}

	int pc_get_costume_body_id(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_BODY);
		lua_pushnumber(L, item ? item->GetID() : 0);
		return 1;
	}

	int pc_get_costume_hair_id(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_HAIR);
		lua_pushnumber(L, item ? item->GetID() : 0);
		return 1;
	}

	int pc_get_arrow(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_ARROW);
		lua_pushnumber(L, item ? item->GetVnum() : 0);
		return 1;
	}

	int pc_get_head(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_HEAD);
		lua_pushnumber(L, item ? item->GetVnum() : 0);
		return 1;
	}

	int pc_get_belt(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_BELT);
		lua_pushnumber(L, item ? item->GetVnum() : 0);
		return 1;
	}

	int pc_get_armor(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_BODY);

		if (!item)
			lua_pushnumber(L, 0);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_costume(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_BODY);

		if (!item)
			lua_pushnumber(L, 0);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_costume_body(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_BODY);
		lua_pushnumber(L, item ? item->GetVnum() : 0);
		return 1;
	}

	int pc_get_costume_hair(lua_State * L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_HAIR);
		lua_pushnumber(L, item ? item->GetVnum() : 0);
		return 1;
	}

	int pc_get_wear(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST wrong set flag");
			return 0;
		}

		UINT bCell = lua_tonumber(L, 1);

		LPITEM item = CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(bCell);


		if (!item)
			lua_pushnil(L);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_money(lua_State * L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetGold());
		return 1;
	}

	int pc_get_real_alignment(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetRealAlignment()/10);
		return 1;
	}

	int pc_get_alignment(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetAlignment()/10);
		return 1;
	}

	int pc_change_alignment(lua_State * L)
	{
		int alignment = (int)(lua_tonumber(L, 1)*10);
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->UpdateAlignment(alignment);
		return 0;
	}

	int pc_change_money(lua_State * L)
	{
		int gold = (int)lua_tonumber(L, -1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (gold + ch->GetGold() < 0)
			sys_err("QUEST wrong ChangeGold %d (now %d)", gold, ch->GetGold());
		else
		{
			DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), gold);
			ch->PointChange(POINT_GOLD, gold, true);
		}

		return 0;
	}

	int pc_set_another_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("QUEST wrong set flag");
			return 0;
		}
		else
		{
			const char * sz = lua_tostring(L, 1);
			const char * sz2 = lua_tostring(L, 2);
			CQuestManager & q = CQuestManager::Instance();
			PC * pPC = q.GetCurrentPC();
			pPC->SetFlag(string(sz)+"."+sz2, int(rint(lua_tonumber(L,3))));
			return 0;
		}
	}

	int pc_get_another_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isstring(L,2))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			const char* sz2 = lua_tostring(L,2);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			if (!pPC)
			{
				return 0;
			}
			lua_pushnumber(L,pPC->GetFlag(string(sz)+"."+sz2));
			return 1;
		}
	}

	int pc_get_flag(lua_State* L)
	{
		if (!lua_isstring(L,-1))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,-1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			if (!pPC)
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				sys_err("Nullpointer in CHARACTER::pc_get_flag %lu", ch->GetPlayerID());
				return 0;
			}
			lua_pushnumber(L,pPC->GetFlag(sz));
			return 1;
		}
	}

	int pc_get_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,-1))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,-1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			if (!pPC)
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				sys_err("Nullpointer in CHARACTER::pc_get_quest_flag %lu", ch->GetPlayerID());
				return 0;
			}
			lua_pushnumber(L,pPC->GetFlag(pPC->GetCurrentQuestName() + "."+sz));
			if ( test_server )
				sys_log( 0 ,"GetQF ( %s . %s )", pPC->GetCurrentQuestName().c_str(), sz );
		}
		return 1;
	}

	int pc_set_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			if (!pPC)
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				sys_err("Nullpointer in CHARACTER::pc_set_flag %lu", ch->GetPlayerID());
				return 0;
			}
			pPC->SetFlag(sz, int(rint(lua_tonumber(L,2))));
		}
		return 0;
	}

	int pc_set_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			if (!pPC)
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				sys_err("Nullpointer in CHARACTER::pc_set_quest_flag %lu", ch->GetPlayerID());
				return 0;
			}
			pPC->SetFlag(pPC->GetCurrentQuestName()+"."+sz, int(rint(lua_tonumber(L,2))));
		}
		return 0;
	}

	int pc_del_quest_flag(lua_State *L)
	{
		if (!lua_isstring(L, 1))
		{
			sys_err("argument error");
			return 0;
		}

		const char * sz = lua_tostring(L, 1);
		PC * pPC = CQuestManager::instance().GetCurrentPC();
		if (!pPC)
		{
			LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
			sys_err("Nullpointer in CHARACTER::pc_del_quest_flag %lu", ch->GetPlayerID());
			return 0;
		}
		pPC->DeleteFlag(pPC->GetCurrentQuestName()+"."+sz);
		return 0;
	}

	int pc_give_exp2(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!lua_isnumber(L,1))
			return 0;

		sys_log(0,"QUEST [REWARD] %s give exp2 %d", ch->GetName(), (int)rint(lua_tonumber(L,1)));

		DWORD exp = (DWORD)rint(lua_tonumber(L,1));

		PC* pPC = CQuestManager::instance().GetCurrentPC();
		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);
		ch->PointChange(POINT_EXP, exp);
		return 0;
	}

	int pc_give_exp(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		sys_log(0,"QUEST [REWARD] %s give exp %s %d", ch->GetName(), lua_tostring(L,1), (int)rint(lua_tonumber(L,2)));

		DWORD exp = (DWORD)rint(lua_tonumber(L,2));

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);

		pPC->GiveExp(lua_tostring(L,1), exp);
		return 0;
	}

	int pc_give_exp_perc(lua_State* L)
	{
		CQuestManager & q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (!ch || !lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
			return 0;

		int lev = (int)rint(lua_tonumber(L,2));
		double proc = (lua_tonumber(L,3));

		sys_log(0, "QUEST [REWARD] %s give exp %s lev %d percent %g%%", ch->GetName(), lua_tostring(L, 1), lev, proc);

		DWORD exp = (DWORD)((exp_table[MINMAX(0, lev, PLAYER_EXP_TABLE_MAX)] * proc) / 100);
		PC * pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);

		pPC->GiveExp(lua_tostring(L, 1), exp);
		return 0;
	}

	int pc_get_empire(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetEmpire());
		return 1;
	}

	int pc_get_part(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!lua_isnumber(L,1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}
		int part_idx = (int)lua_tonumber(L, 1);
		lua_pushnumber(L, ch->GetPart(part_idx));
		return 1;
	}

	int pc_set_part(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
		{
			return 0;
		}
		int part_idx = (int)lua_tonumber(L, 1);
		int part_value = (int)lua_tonumber(L, 2);
		ch->SetPart(part_idx, part_value);
		ch->UpdatePacket();
		return 0;
	}

	int pc_get_skillgroup(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetSkillGroup());
		return 1;
	}

	int pc_set_skillgroup(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			sys_err("QUEST wrong skillgroup number");
		else
		{
			CQuestManager & q = CQuestManager::Instance();
			LPCHARACTER ch = q.GetCurrentCharacterPtr();
			ch->RemoveGoodAffect();
			ch->SetSkillGroup((BYTE) rint(lua_tonumber(L, 1)));
		}
		return 0;
	}

	int pc_is_polymorphed(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->IsPolymorphed());
		return 1;
	}

	int pc_remove_polymorph(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveAffect(AFFECT_POLYMORPH);
		ch->SetPolymorph(0);
		return 0;
	}

	int pc_polymorph(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		DWORD dwVnum = (DWORD) lua_tonumber(L, 1);
		int iDuration = (int) lua_tonumber(L, 2);
		ch->AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
		return 0;
	}

	int pc_is_mount(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetMountVnum());
		return 1;
	}

	int pc_mount(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		int length = 60;

		if (lua_isnumber(L, 2))
			length = (int)lua_tonumber(L, 2);

		DWORD mount_vnum = (DWORD)lua_tonumber(L, 1);

		if (length < 0)
			length = 60;

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_PVP_ADVANCED
		if ((ch->GetDuel("BlockRide")))
		{	
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("duel_block_function"));
			lua_pushnumber(L, 0);
			return 1;
		}
#endif
#ifdef TOURNAMENT_PVP_SYSTEM
		if (CTournamentPvP::instance().IsTournamentMap(ch, TOURNAMENT_BLOCK_MOUNT))
		{
			lua_pushnumber(L, 0);
			return 0;
		}
#endif
#if defined(WJ_COMBAT_ZONE)
		if (CCombatZoneManager::Instance().IsCombatZoneMap(ch->GetMapIndex()))
		{
			if (ch->IsRiding())
				ch->StopRiding();

			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("cz_cannot_use_ride"));	
			return 0;
		}
#endif

		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		// ???? ???????? ?????????? ???????? ?????? ????
		if (ch->GetHorse())
			ch->HorseSummon(false);

		if (mount_vnum)
		{
			ch->AddAffect(AFFECT_MOUNT, POINT_MOUNT, mount_vnum, AFF_NONE, length, 0, true);
			switch (mount_vnum)
			{
			case 20201:
			case 20202:
			case 20203:
			case 20204:
			case 20213:
			case 20216:
			ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 30, AFF_NONE, length, 0, true, true);
			break;

			case 20205:
			case 20206:
			case 20207:
			case 20208:
			case 20214:
			case 20217:
			ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 40, AFF_NONE, length, 0, true, true);
			break;

			case 20209:
			case 20210:
			case 20211:
			case 20212:
			case 20215:
			case 20218:
			ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 50, AFF_NONE, length, 0, true, true);
			break;

			}
		}

		return 0;
	}

	int pc_mount_bonus(lua_State* L)
	{
		BYTE applyOn = static_cast<BYTE>(lua_tonumber(L, 1));
		long value = static_cast<long>(lua_tonumber(L, 2));
		long duration = static_cast<long>(lua_tonumber(L, 3));

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if( NULL != ch )
		{
			if (!ch->GetMountVnum())
				return 0;
			ch->RemoveAffect(AFFECT_MOUNT_BONUS);
			ch->AddAffect(AFFECT_MOUNT_BONUS, aApplyInfo[applyOn].bPointType, value, AFF_NONE, duration, 0, false);
		}

		return 0;
	}

	int pc_unmount(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);
		if (ch->IsHorseRiding())
			ch->StopRiding();
		return 0;
	}

	int pc_get_horse_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetHorseLevel());
		return 1;
	}

	int pc_get_horse_hp(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->GetHorseLevel())
			lua_pushnumber(L, ch->GetHorseHealth());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_get_horse_stamina(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->GetHorseLevel())
			lua_pushnumber(L, ch->GetHorseStamina());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_is_horse_alive(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetHorseLevel() > 0 && ch->GetHorseHealth()>0);
		return 1;
	}

	int pc_revive_horse(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->ReviveHorse();
		return 0;
	}

	int pc_have_map_scroll(lua_State* L)
	{
		if (!lua_isstring(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		const char * szMapName = lua_tostring(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().FindRegionByPartialName(szMapName);

		if (!region)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		bool bFind = false;
		for (int iCell = 0; iCell < INVENTORY_MAX_NUM; iCell++)
		{
			LPITEM item = ch->GetInventoryItem(iCell);
			if (!item)
				continue;

			if (item->GetType() == ITEM_USE &&
					item->GetSubType() == USE_TALISMAN &&
					(item->GetValue(0) == 1 || item->GetValue(0) == 2))
			{
				int x = item->GetSocket(0);
				int y = item->GetSocket(1);
				//if ((x-item_x)*(x-item_x)+(y-item_y)*(y-item_y)<r*r)
				if (region->sx <=x && region->sy <= y && x <= region->ex && y <= region->ey)
				{
					bFind = true;
					break;
				}
			}
		}

		lua_pushboolean(L, bFind);
		return 1;
	}

	int pc_in_guild_map(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		bool savastamiyim = false;
		if (ch->GetWarMap())
		{
			savastamiyim = true;
		}

		lua_pushboolean(L, savastamiyim);
		return 1;
	}

	int pc_get_war_map(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetWarMap() ? ch->GetWarMap()->GetMapIndex() : 0);
		return 1;
	}

	int pc_have_pos_scroll(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L,1) || !lua_isnumber(L,2))
		{
			sys_err("invalid x y position");
			lua_pushboolean(L, 0);
			return 1;
		}

		if (!lua_isnumber(L,2))
		{
			sys_err("invalid radius");
			lua_pushboolean(L, 0);
			return 1;
		}

		int x = (int)lua_tonumber(L, 1);
		int y = (int)lua_tonumber(L, 2);
		float r = (float)lua_tonumber(L, 3);

		bool bFind = false;
		for (int iCell = 0; iCell < INVENTORY_MAX_NUM; iCell++)
		{
			LPITEM item = ch->GetInventoryItem(iCell);
			if (!item)
				continue;

			if (item->GetType() == ITEM_USE &&
					item->GetSubType() == USE_TALISMAN &&
					(item->GetValue(0) == 1 || item->GetValue(0) == 2))
			{
				int item_x = item->GetSocket(0);
				int item_y = item->GetSocket(1);
				if ((x-item_x)*(x-item_x)+(y-item_y)*(y-item_y)<r*r)
				{
					bFind = true;
					break;
				}
			}
		}

		lua_pushboolean(L, bFind);
		return 1;
	}

	int pc_get_equip_refine_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int cell = (int) lua_tonumber(L, 1);
		if (cell < 0 || cell >= WEAR_MAX_NUM)
		{
			sys_err("invalid wear position %d", cell);
			lua_pushnumber(L, 0);
			return 1;
		}

		LPITEM item = ch->GetWear(cell);
		if (!item)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_pushnumber(L, item->GetRefineLevel());
		return 1;
	}

	int pc_refine_equip(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			sys_err("invalid argument");
			lua_pushboolean(L, 0);
			return 1;
		}

		int cell = (int) lua_tonumber(L, 1);
		int level_limit = (int) lua_tonumber(L, 2);
		int pct = lua_isnumber(L, 3) ? (int)lua_tonumber(L, 3) : 100;

		LPITEM item = ch->GetWear(cell);
		if (!item)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (item->GetRefinedVnum() == 0)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (item->GetRefineLevel()>level_limit)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (pct == 100 || number(1, 100) <= pct)
		{
			// ???? ????
			lua_pushboolean(L, 1);

			LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(item->GetRefinedVnum(), 1, 0, false);

			if (pkNewItem)
			{
				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					if (!item->GetSocket(i))
						break;
					else
						pkNewItem->SetSocket(i, 1);

				int set = 0;
				for (int i=0; i<ITEM_SOCKET_MAX_NUM; i++)
				{
					long socket = item->GetSocket(i);
					if (socket > 2 && socket != 28960)
					{
						pkNewItem->SetSocket(set++, socket);
					}
				}

				item->CopyAttributeTo(pkNewItem);

				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

				// some tuits need here -_- pkNewItem->AddToCharacter(this, bCell);
				pkNewItem->EquipTo(ch, cell);

				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

				LogManager::instance().ItemLog(ch, pkNewItem, "REFINE SUCCESS (QUEST)", pkNewItem->GetName());
			}
		}
		else
		{
			// ???? ????
			lua_pushboolean(L, 0);
		}

		return 1;
	}

	int pc_get_skill_level(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			lua_pushnumber(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		DWORD dwVnum = (DWORD) lua_tonumber(L, 1);
		lua_pushnumber(L, ch->GetSkillLevel(dwVnum));

		return 1;
	}

	int pc_give_lotto(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		sys_log(0, "TRY GIVE LOTTO TO pid %u", ch->GetPlayerID());

		DWORD * pdw = M2_NEW DWORD[3];

		pdw[0] = 50001;
		pdw[1] = 1;
		pdw[2] = q.GetEventFlag("lotto_round");

		// ???????? ?????? ????????
		DBManager::instance().ReturnQuery(QID_LOTTO, ch->GetPlayerID(), pdw,
				"INSERT INTO lotto_list VALUES(0, 'server%s', %u, NOW())",
				get_table_postfix(), ch->GetPlayerID());

		return 0;
	}

	int pc_aggregate_monster(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->AggregateMonster();
		return 0;
	}

	int pc_forget_my_attacker(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->ForgetMyAttacker();
		return 0;
	}

	int pc_attract_ranger(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->AttractRanger();
		return 0;
	}

	int pc_select_pid(lua_State* L)
	{
		DWORD pid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER new_ch = CHARACTER_MANAGER::instance().FindByPID(pid);

		if (new_ch)
		{
			CQuestManager::instance().GetPC(new_ch->GetPlayerID());

			lua_pushnumber(L, ch->GetPlayerID());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_select_vid(lua_State* L)
	{
		DWORD vid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER new_ch = CHARACTER_MANAGER::instance().Find(vid);

		if (new_ch)
		{
			CQuestManager::instance().GetPC(new_ch->GetPlayerID());

			lua_pushnumber(L, ch ? (DWORD)ch->GetVID() : 0);
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_sex(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, GET_SEX(ch)); /* 0==MALE, 1==FEMALE */
		return 1;
	}

	int pc_is_engaged(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, marriage::CManager::instance().IsEngaged(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_married(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, marriage::CManager::instance().IsMarried(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_engaged_or_married(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, marriage::CManager::instance().IsEngagedOrMarried(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_gm(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetGMLevel() >= GM_HIGH_WIZARD);
		return 1;
	}

#ifdef ENABLE_RONARK_SYSTEM
	int pc_set_guild_online_member(lua_State * L)
	{
		
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
	
		if (!ch->GetGuild())
			return 0;
		
		quest::CQuestManager& R = quest::CQuestManager::instance();
		int GetGuildCount = R.RonarkGetQuestFlag("RonarkGuildCount", ch->GetGuild()->GetID());

		if (ch->IsPC())
		{
			if (!ch->IsPC() || ch->IsObserverMode() || !ch->GetGuild())
				return 0;

			if(ch->GetGuild() && ch->GetQuestFlag("RonarkCountPlayer") == 0){
				R.RonarkSetQuestFlag("RonarkGuildCount", GetGuildCount+1 , ch->GetGuild()->GetID());
				ch->SetQuestFlag("RonarkCountPlayer",1);
			}
			lua_pushnumber(L, GetGuildCount);
		}
		return 1;
	}
	
	int pc_remove_guild_online_member(lua_State * L)
	{	
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		if (!ch->GetGuild())
			return 0;
		
		quest::CQuestManager& R = quest::CQuestManager::instance();
		int GetGuildCount = R.RonarkGetQuestFlag("RonarkGuildCount", ch->GetGuild()->GetID());
		if (ch->IsPC())
		{
			if (!ch->IsPC() || ch->IsObserverMode() || !ch->GetGuild())
				return 0;

			if(ch->GetGuild() && ch->GetQuestFlag("RonarkCountPlayer") == 1){

				ch->SetQuestFlag("RonarkCountPlayer",0);

				if (GetGuildCount < 1){
					ch->SetQuestFlag("RonarkCountPlayer",0);
					return 0;
				}
				R.RonarkSetQuestFlag("RonarkGuildCount", GetGuildCount-1 , ch->GetGuild()->GetID());
			}
			lua_pushnumber(L, GetGuildCount);
		}

		return 1;
	}

	int pc_not_guild(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, !ch->GetGuild());
		return 1;
	}
#endif

	int pc_get_gm_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetGMLevel());
		return 1;
	}

	int pc_mining(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		ch->mining(npc);
		return 0;
	}

	int pc_diamond_refine(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		int cost = (int) lua_tonumber(L, 1);
		int pct = (int)lua_tonumber(L, 2);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (item)
			lua_pushboolean(L, mining::OreRefine(ch, npc, item, cost, pct, NULL));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

	int pc_ore_refine(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		int cost = (int) lua_tonumber(L, 1);
		int pct = (int)lua_tonumber(L, 2);
		int metinstone_cell = (int)lua_tonumber(L, 3);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		LPITEM metinstone_item = ch->GetInventoryItem(metinstone_cell);

		if (item && metinstone_item)
			lua_pushboolean(L, mining::OreRefine(ch, npc, item, cost, pct, metinstone_item));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

	int pc_clear_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if ( ch == NULL ) return 0;

		ch->ClearSkill();

		return 0;
	}

	int pc_clear_sub_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if ( ch == NULL ) return 0;

		ch->ClearSubSkill();

		return 0;
	}

	int pc_set_skill_point(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int newPoint = (int) lua_tonumber(L, 1);

		ch->SetRealPoint(POINT_SKILL, newPoint);
		ch->SetPoint(POINT_SKILL, ch->GetRealPoint(POINT_SKILL));
		ch->PointChange(POINT_SKILL, 0);
		ch->ComputePoints();
		ch->PointsPacket();

		return 0;
	}

	// RESET_ONE_SKILL
	int pc_clear_one_skill(lua_State* L)
	{
		int vnum = (int)lua_tonumber(L, 1);
		sys_log(0, "%d skill clear", vnum);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if ( ch == NULL )
		{
			sys_log(0, "skill clear fail");
			lua_pushnumber(L, 0);
			return 1;
		}

		sys_log(0, "%d skill clear", vnum);

		ch->ResetOneSkill(vnum);

		return 0;
	}
	// END_RESET_ONE_SKILL

	int pc_is_clear_skill_group(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushboolean(L, ch->GetQuestFlag("skill_group_clear.clear") == 1);

		return 1;
	}

	int pc_save_exit_location(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->SaveExitLocation();

		return 0;
	}

	//????????
	int pc_teleport ( lua_State * L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int x=0,y=0;
		if ( lua_isnumber(L, 1) )
		{
			// ?????? ????
			const int TOWN_NUM = 10;
			struct warp_by_town_name
			{
				const char* name;
				DWORD x;
				DWORD y;
			} ws[TOWN_NUM] =
			{
				{"????????",	4743,	9548},
				{"??????",		3235,	9086},
				{"??????",		3531,	8829},
				{"????????",	638,	1664},
				{"??????",		1745,	1909},
				{"??????",		1455,	2400},
				{"????????",	9599,	2692},
				{"??????",		8036,	2984},
				{"??????",		8639,	2460},
				{"??????",		4350,	2143},
			};
			int idx  = (int)lua_tonumber(L, 1);

			x = ws[idx].x;
			y = ws[idx].y;
			goto teleport_area;
		}

		else
		{
			const char * arg1 = lua_tostring(L, 1);

			LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

			if (!tch)
			{
				const CCI* pkCCI = P2P_MANAGER::instance().Find(arg1);

				if (pkCCI)
				{
					if (pkCCI->bChannel != g_bChannel)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "Target is in %d channel (my channel %d)", pkCCI->bChannel, g_bChannel);
					}
					else
					{

						PIXEL_POSITION pos;

						if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
						}
						else
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "You warp to ( %d, %d )", pos.x, pos.y);
							ch->WarpSet(pos.x, pos.y);
							lua_pushnumber(L, 1 );
						}
					}
				}
				else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
				}

				lua_pushnumber(L, 0 );

				return 1;
			}
			else
			{
				x = tch->GetX() / 100;
				y = tch->GetY() / 100;
			}
		}

teleport_area:

		x *= 100;
		y *= 100;

		ch->ChatPacket(CHAT_TYPE_INFO, "You warp to ( %d, %d )", x, y);
		ch->WarpSet(x,y);
		ch->Stop();
		lua_pushnumber(L, 1 );
		return 1;
	}

	int pc_set_skill_level(lua_State* L)
	{
		DWORD dwVnum = (DWORD)lua_tonumber(L, 1);
		BYTE byLev = (BYTE)lua_tonumber(L, 2);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->SetSkillLevel(dwVnum, byLev);

		ch->SkillLevelPacket();

		return 0;
	}

	int pc_give_polymorph_book(lua_State* L)
	{
		if ( lua_isnumber(L, 1) != true && lua_isnumber(L, 2) != true && lua_isnumber(L, 3) != true && lua_isnumber(L, 4) != true )
		{
			sys_err("Wrong Quest Function Arguments: pc_give_polymorph_book");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CPolymorphUtils::instance().GiveBook(ch, (DWORD)lua_tonumber(L, 1), (DWORD)lua_tonumber(L, 2), (BYTE)lua_tonumber(L, 3), (BYTE)lua_tonumber(L, 4));

		return 0;
	}

	int pc_upgrade_polymorph_book(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM pItem = CQuestManager::instance().GetCurrentItem();

		bool ret = CPolymorphUtils::instance().BookUpgrade(ch, pItem);

		lua_pushboolean(L, ret);

		return 1;
	}

	int pc_get_premium_remain_sec(lua_State* L)
	{
		int	remain_seconds	= 0;
		int	premium_type	= 0;
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong premium index (is not number)");
			return 0;
		}

		premium_type = (int)lua_tonumber(L,1);
		switch (premium_type)
		{
			case PREMIUM_EXP:
			case PREMIUM_ITEM:
			case PREMIUM_SAFEBOX:
			case PREMIUM_AUTOLOOT:
			case PREMIUM_FISH_MIND:
			case PREMIUM_MARRIAGE_FAST:
			case PREMIUM_GOLD:
				break;

			default:
				sys_err("wrong premium index %d", premium_type);
				return 0;
		}

		remain_seconds = ch->GetPremiumRemainSeconds(premium_type);

		lua_pushnumber(L, remain_seconds);
		return 1;
	}

	int pc_send_block_mode(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->SetBlockModeForce((BYTE)lua_tonumber(L, 1));

		return 0;
	}

	int pc_change_empire(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		BYTE bResult = 0;
		str_to_number(bResult, row[0]);

		if (bResult)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("offflineshopunacikken_bunuyapamam_kardes"));
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushnumber(L, ch->ChangeEmpire((unsigned char)lua_tonumber(L, 1)));
		return 1;
	}

	int pc_get_change_empire_count(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushnumber(L, ch->GetChangeEmpireCount());

		return 1;
	}

	int pc_set_change_empire_count(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->SetChangeEmpireCount();

		return 0;
	}

	int pc_change_name(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch->GetNewName().size() != 0 )
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if (ch->GetNewName().size() < 3 && ch->GetNewName().size() > 12)
		{
			sys_err("Hatali isim uzunlugu [isim izi]");
			lua_pushnumber(L, 0);
			return 1;
		}

		if ( lua_isstring(L, 1) != true )
		{
			lua_pushnumber(L, 1);
			return 1;
		}

		const char * szName = lua_tostring(L, 1);

		if ( check_name(szName) == false )
		{
			lua_pushnumber(L, 2);
			return 1;
		}

		if(FN_IS_VALID_LOGIN_STRING2(szName))
		{
			lua_pushnumber(L, 6);
			return 1;
		}

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "SELECT COUNT(*) FROM player%s WHERE name='%s'", get_table_postfix(), szName);
		std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(szQuery));

		if ( pmsg->Get()->uiNumRows > 0 )
		{
			MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

			int	count = 0;
			str_to_number(count, row[0]);

			// ???? ???? ?????? ???? ???????? ????
			if ( count != 0 )
			{
				lua_pushnumber(L, 3);
				return 1;
			}
		}

		DWORD pid = ch->GetPlayerID();
		db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
		db_clientdesc->Packet(&pid, sizeof(DWORD));

		/* delete messenger list */
		MessengerManager::instance().RemoveAllList(ch->GetName());
#ifdef ENABLE_MESSENGER_BLOCK
		MessengerManager::instance().RemoveAllBlockList(ch->GetName());
#endif

		/* change_name_log */
		LogManager::instance().ChangeNameLog(pid, ch->GetName(), szName, ch->GetDesc()->GetHostName());

		snprintf(szQuery, sizeof(szQuery), "UPDATE player%s SET name='%s' WHERE id=%u", get_table_postfix(), szName, pid);
		SQLMsg * msg = DBManager::instance().DirectQuery(szQuery);
		M2_DELETE(msg);

		ch->SetNewName(szName);
		lua_pushnumber(L, 4);
		return 1;
	}

	int pc_is_dead(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			lua_pushboolean(L, ch->IsDead());
			return 1;
		}

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_reset_status( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == true )
		{
			int idx = (int)lua_tonumber(L, 1);

			if ( idx >= 0 && idx < 4 )
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				int point = POINT_NONE;
				char buf[128];

				switch ( idx )
				{
					case 0 : point = POINT_HT; break;
					case 1 : point = POINT_IQ; break;
					case 2 : point = POINT_ST; break;
					case 3 : point = POINT_DX; break;
					default : lua_pushboolean(L, false); return 1;
				}

				int old_val = ch->GetRealPoint(point);
				int old_stat = ch->GetRealPoint(POINT_STAT);

				ch->SetRealPoint(point, 1);
				ch->SetPoint(point, ch->GetRealPoint(point));

				ch->PointChange(POINT_STAT, old_val-1);

				if ( point == POINT_HT )
				{
					BYTE job = ch->GetJob();
					ch->SetRandomHP((ch->GetLevel()-1) * number(JobInitialPoints[job].hp_per_lv_begin, JobInitialPoints[job].hp_per_lv_end));
				}
				else if ( point == POINT_IQ )
				{
					BYTE job = ch->GetJob();
					ch->SetRandomSP((ch->GetLevel()-1) * number(JobInitialPoints[job].sp_per_lv_begin, JobInitialPoints[job].sp_per_lv_end));
				}

				ch->ComputePoints();
				ch->PointsPacket();

				if ( point == POINT_HT )
				{
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				}
				else if ( point == POINT_IQ )
				{
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				}

				switch ( idx )
				{
					case 0 :
						snprintf(buf, sizeof(buf), "reset ht(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 1 :
						snprintf(buf, sizeof(buf), "reset iq(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 2 :
						snprintf(buf, sizeof(buf), "reset st(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 3 :
						snprintf(buf, sizeof(buf), "reset dx(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
				}

				LogManager::instance().CharLog(ch, 0, "RESET_ONE_STATUS", buf);

				lua_pushboolean(L, true);
				return 1;
			}
		}

		lua_pushboolean(L, false);
		return 1;
	}

	int pc_get_ht( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetRealPoint(POINT_HT));
		return 1;
	}

	int pc_set_ht( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int usedPoint = newPoint - ch->GetRealPoint(POINT_HT);
		ch->SetRealPoint(POINT_HT, newPoint);
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_iq( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetRealPoint(POINT_IQ));
		return 1;
	}

	int pc_set_iq( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int usedPoint = newPoint - ch->GetRealPoint(POINT_IQ);
		ch->SetRealPoint(POINT_IQ, newPoint);
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_st( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetRealPoint(POINT_ST));
		return 1;
	}

	int pc_set_st( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int usedPoint = newPoint - ch->GetRealPoint(POINT_ST);
		ch->SetRealPoint(POINT_ST, newPoint);
		ch->PointChange(POINT_ST, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_dx( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch->GetRealPoint(POINT_DX));
		return 1;
	}

	int pc_set_dx( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int usedPoint = newPoint - ch->GetRealPoint(POINT_DX);
		ch->SetRealPoint(POINT_DX, newPoint);
		ch->PointChange(POINT_DX, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_is_near_vid( lua_State* L )
	{
		if ( lua_isnumber(L, 1) != true || lua_isnumber(L, 2) != true )
		{
			lua_pushboolean(L, false);
		}
		else
		{
			LPCHARACTER pMe = CQuestManager::instance().GetCurrentCharacterPtr();
			LPCHARACTER pOther = CHARACTER_MANAGER::instance().Find( (DWORD)lua_tonumber(L, 1) );

			if ( pMe != NULL && pOther != NULL )
			{
				lua_pushboolean(L, (DISTANCE_APPROX(pMe->GetX() - pOther->GetX(), pMe->GetY() - pOther->GetY()) < (int)lua_tonumber(L, 2)*100));
			}
			else
			{
				lua_pushboolean(L, false);
			}
		}

		return 1;
	}

	int pc_get_socket_items( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_newtable( L );

		if ( pChar == NULL ) return 1;

		int idx = 1;

		// ?????? ?????? ?? ???? ???? ??.
		// ?? ?????? ???????? ?????? ?? ????.
		for ( int i=0; i < INVENTORY_MAX_NUM + WEAR_MAX_NUM; i++ )
		{
			LPITEM pItem = pChar->GetInventoryItem(i);

			if ( pItem != NULL )
			{
				if ( pItem->IsEquipped() == false )
				{
					int j = 0;
					for (; j < ITEM_SOCKET_MAX_NUM; j++ )
					{
						long socket = pItem->GetSocket(j);

						if ( socket > 2 && socket != ITEM_BROKEN_METIN_VNUM )
						{
							TItemTable* pItemInfo = ITEM_MANAGER::instance().GetTable( socket );
							if ( pItemInfo != NULL )
							{
								if ( pItemInfo->bType == ITEM_METIN ) break;
							}
						}
					}

					if ( j >= ITEM_SOCKET_MAX_NUM ) continue;

					lua_newtable( L );

					{
						lua_pushstring( L, pItem->GetName() );
						lua_rawseti( L, -2, 1 );

						lua_pushnumber( L, i );
						lua_rawseti( L, -2, 2 );
					}

					lua_rawseti( L, -2, idx++ );
				}
			}
		}

		return 1;
	}

	int pc_get_empty_inventory_count( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber(L, pChar->CountEmptyInventory());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_logoff_interval( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber(L, pChar->GetLogOffInterval());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_player_id( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber( L, pChar->GetPlayerID() );
		}
		else
		{
			lua_pushnumber( L, 0 );
		}

		return 1;
	}

	int pc_get_account_id( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			if ( pChar->GetDesc() != NULL )
			{
				lua_pushnumber( L, pChar->GetDesc()->GetAccountTable().id );
				return 1;
			}
		}

		lua_pushnumber( L, 0 );
		return 1;
	}

	int pc_get_account( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( NULL != pChar )
		{
			if( NULL != pChar->GetDesc() )
			{
				lua_pushstring( L, pChar->GetDesc()->GetAccountTable().login );
				return 1;
			}
		}

		lua_pushstring( L, "" );
		return 1;
	}

	int pc_is_riding(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( NULL != pChar )
		{
			bool is_riding = pChar->IsRiding();

			lua_pushboolean(L, is_riding);

			return 1;
		}

		lua_pushboolean(L, false);
		return 1;
	}

	int pc_get_special_ride_vnum(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			LPITEM Unique1 = pChar->GetWear(WEAR_UNIQUE1);
			LPITEM Unique2 = pChar->GetWear(WEAR_UNIQUE2);

			if (NULL != Unique1)
			{
				if (UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup())
				{
					lua_pushnumber(L, Unique1->GetVnum());
					lua_pushnumber(L, Unique1->GetSocket(2));
					return 2;
				}
			}

			if (NULL != Unique2)
			{
				if (UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup())
				{
					lua_pushnumber(L, Unique2->GetVnum());
					lua_pushnumber(L, Unique2->GetSocket(2));
					return 2;
				}
			}
		}

		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);

		return 2;
	}

	int pc_can_warp(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			lua_pushboolean(L, pChar->CanWarp());
		}
		else
		{
			lua_pushboolean(L, false);
		}

		return 1;
	}

	int pc_dec_skill_point(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			pChar->PointChange(POINT_SKILL, -1);
		}

		return 0;
	}

	int pc_get_skill_point(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			lua_pushnumber(L, pChar->GetPoint(POINT_SKILL));
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_channel_id(lua_State* L)
	{
		lua_pushnumber(L, g_bChannel);

		return 1;
	}

	int pc_give_poly_marble(lua_State* L)
	{
		const int dwVnum = lua_tonumber(L, 1);

		const CMob* MobInfo = CMobManager::instance().Get(dwVnum);

		if (NULL == MobInfo)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		if (0 == MobInfo->m_table.dwPolymorphItemVnum)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		LPITEM item = ITEM_MANAGER::instance().CreateItem( MobInfo->m_table.dwPolymorphItemVnum );

		if (NULL == item)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		item->SetSocket(0, dwVnum);

		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		int iEmptyCell = ch->GetEmptyInventory(item->GetSize());

		if (-1 == iEmptyCell)
		{
			M2_DESTROY_ITEM(item);
			lua_pushboolean(L, false);
			return 1;
		}

		item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyCell));

		const PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), MobInfo->m_table.dwPolymorphItemVnum, dwVnum);

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_get_sig_items (lua_State* L)
	{
		DWORD group_vnum = (DWORD)lua_tonumber (L, 1);
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int count = 0;
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			if (ch->GetInventoryItem(i) != NULL && ch->GetInventoryItem(i)->GetSIGVnum() == group_vnum)
			{
				lua_pushnumber(L, ch->GetInventoryItem(i)->GetID());
				count++;
			}
		}

		return count;
	}

	int pc_charge_cash(lua_State * L)
	{
		TRequestChargeCash packet;

		int amount = lua_isnumber(L, 1) ? (int)lua_tonumber(L, 1) : 0;
		std::string strChargeType = lua_isstring(L, 2) ? lua_tostring(L, 2) : "";

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL == ch || NULL == ch->GetDesc() || 1 > amount || 50000 < amount)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		packet.dwAID = ch->GetDesc()->GetAccountTable().id;
		packet.dwAmount = (DWORD)amount;
		packet.eChargeType = ERequestCharge_Cash;

		if (0 < strChargeType.length())
			std::transform(strChargeType.begin(), strChargeType.end(), strChargeType.begin(), (int(*)(int))std::tolower);

		if ("mileage" == strChargeType)
			packet.eChargeType = ERequestCharge_Mileage;

		db_clientdesc->DBPacketHeader(HEADER_GD_REQUEST_CHARGE_CASH, 0, sizeof(TRequestChargeCash));
		db_clientdesc->Packet(&packet, sizeof(packet));

		lua_pushboolean(L, 1);
		return 1;
	}

	int pc_give_award(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) )
		{
			sys_err("QUEST give award call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum = (int) lua_tonumber(L, 1);

		int icount = (int) lua_tonumber(L, 2);

		sys_log(0, "QUEST [award] item %d to login %s", dwVnum, ch->GetDesc()->GetAccountTable().login);

		DBManager::instance().Query("INSERT INTO item_award (login, vnum, count, given_time, why, mall)select '%s', %d, %d, now(), '%s', 1 from DUAL where not exists (select login, why from item_award where login = '%s' and why  = '%s') ;",
			ch->GetDesc()->GetAccountTable().login,
			dwVnum,
			icount,
			lua_tostring(L,3),
			ch->GetDesc()->GetAccountTable().login,
			lua_tostring(L,3));

		lua_pushnumber (L, 0);
		return 1;
	}
	int pc_give_award_socket(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) || !lua_isstring(L, 4) || !lua_isstring(L, 5) || !lua_isstring(L, 6) )
		{
			sys_err("QUEST give award call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum = (int) lua_tonumber(L, 1);

		int icount = (int) lua_tonumber(L, 2);

		sys_log(0, "QUEST [award] item %d to login %s", dwVnum, ch->GetDesc()->GetAccountTable().login);

		DBManager::instance().Query("INSERT INTO item_award (login, vnum, count, given_time, why, mall, socket0, socket1, socket2, socket3)select '%s', %d, %d, now(), '%s', 1, %s, %s, %s, %s from DUAL where not exists (select login, why from item_award where login = '%s' and why  = '%s') ;", 
			ch->GetDesc()->GetAccountTable().login,
			dwVnum,
			icount,
			lua_tostring(L,3),
			lua_tostring(L,4),
			lua_tostring(L,5),
			lua_tostring(L,6),
			ch->GetDesc()->GetAccountTable().login,
			lua_tostring(L,3));

		lua_pushnumber (L, 0);
		return 1;
	}

	int pc_get_informer_type(lua_State* L)	//???? ???? ????
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( pChar != NULL )
		{
			//sys_err("quest cmd test %s", pChar->GetItemAward_cmd() );
			lua_pushstring(L, pChar->GetItemAward_cmd() );
		}
		else
			lua_pushstring(L, "" );

		return 1;
	}

	int pc_get_informer_item(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( pChar != NULL )
		{
			lua_pushnumber(L, pChar->GetItemAward_vnum() );
		}
		else
			lua_pushnumber(L,0);

		return 1;
	}

	int pc_get_killee_drop_pct(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER pKillee = pChar->GetQuestNPC();

		int iDeltaPercent, iRandRange;
		if (NULL == pKillee || !ITEM_MANAGER::instance().GetDropPct(pKillee, pChar, iDeltaPercent, iRandRange))
		{
			sys_err("killee is null");
			lua_pushnumber(L, -1);
			lua_pushnumber(L, -1);

			return 2;
		}

		lua_pushnumber(L, iDeltaPercent);
		lua_pushnumber(L, iRandRange);

		return 2;
	}

	int pc_get_empire_name(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		const char* tabelle[3] = { "Shinsoo", "Chunjo", "Jinno" };
		int empireave = ch->GetEmpire() - 1;
		lua_pushstring(L, tabelle[empireave]);
		return 1;
	}

	int pc_get_karakter(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		const char* character_list[8] = { "Sava???", "Ninja", "Sura", "?man", "Sava???", "Ninja", "Sura", "?man" };
		int character_job = ch->GetJob();
		lua_pushstring(L, ch ? character_list[character_job] : 0);
		return 1;
	}

	int pc_get_karakter_cinsiyet(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		const char* character_list[8] = { "Erkek Sava???", "K? Ninja", "Erkek Sura", "K? ?man", "K? Sava???", "Erkek Ninja", "K? Sura", "?man Erkek" };
		int character_sex_job = ch->GetJob();
		lua_pushstring(L, ch ? character_list[character_sex_job] : 0);
		return 1;
	}

	int pc_karakter_degisim(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("Hatal?krakter de??im.");
			return 0;
		}

		ch->SetRace(ch ? (int)lua_tonumber(L, 1) : 0);

		DWORD pid = ch->GetPlayerID();
		db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
		db_clientdesc->Packet(&pid, sizeof(DWORD));
		return 1;
	}

#ifdef ENABLE_WOLFMAN_CHARACTER
	int pc_if_bleeding(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->IsAffectFlag(AFF_BLEEDING));
		return 1;
	}
#endif

#ifdef ENABLE_WOLFMAN_CHARACTER
	int pc_sf_bleeding(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if(lua_toboolean(L, 1))
			ch->AddAffect(AFFECT_BLEEDING, 0, 0, AFF_BLEEDING, 30+1, 0, 1, 0);
		else
			ch->RemoveAffect(AFFECT_BLEEDING);
		return 0;
	}
#endif

	int pc_give_and_equip_item(lua_State* L)
    {
        LPCHARACTER pkCharacter = CQuestManager::instance().GetCurrentCharacterPtr();

	    if (!pkCharacter ||
		   !lua_isnumber(L, 1))
	    {
		   lua_pushboolean(L, false);
		   return 1;
	    }

     	LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(lua_tonumber(L, 1));
      	if (pkItem &&
	    	pkCharacter->EquipItem(pkItem))
    	{
	    	lua_pushboolean(L, true);
      	}
       	else
       	{
		    M2_DESTROY_ITEM(pkItem);
		    lua_pushboolean(L, false);
    	}

	return 1;
    }

	int pc_get_mount_vnum(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushnumber(L, ch && ch->IsRiding() ? ch->GetMountVnum() : 0);
		return 1;
	}

	int pc_get_point(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!ch)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if (!lua_isnumber(L, 1))
		{
			sys_err("Argument error.");
			lua_pushnumber(L, 0);
			return 1;
		}

		int bPoint = (int)lua_tonumber(L, 1);
		if (bPoint < 0 || bPoint >= POINT_MAX_NUM)
		{
			sys_err("Invalid point (%d).", bPoint);
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_pushnumber(L, ch->GetPoint(bPoint));
		return 1;
	}

	int pc_get_real_point(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		if (!lua_isnumber(L, 1))
		{
			sys_err("Argument error.");
			lua_pushnumber(L, 0);
			return 1;
		}

		int bPoint = (int)lua_tonumber(L, 1);
		if (bPoint <= POINT_NONE || bPoint >= POINT_MAX_NUM)
		{
			sys_err("Invalid point (%d).", bPoint);
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_pushnumber(L, ch->GetRealPoint(bPoint));
		return 1;
	}

	int pc_specific_effect(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		if (!lua_isstring(L, 1))
		{
			sys_err("Argument error.");
			lua_pushboolean(L, false);
			return 1;
		}

		ch->SpecificEffectPacket(lua_tostring(L, 1));
		lua_pushboolean(L, true);
		return 1;
	}

	int pc_disconnect_with_delay(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		ch->GetDesc()->DelayedDisconnect(lua_isnumber(L, 1) ? (int)lua_tonumber(L, 1) : 10);
		return 0;
	}

	int pc_get_max_level(lua_State* L)
	{
		lua_pushnumber(L, gPlayerMaxLevel);
		return 1;
	}

	int pc_get_ip(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		lua_pushstring(L, ch->GetDesc()->GetHostName());
		return 1;
	}

#ifdef __HIDE_COSTUME_SYSTEM__
	int pc_hide_costume(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("pc_hide_costume::wrong part value (1-4)");
			return 0;
		}

		if (!lua_isboolean(L, 2))
		{
			sys_err("Invalid argument: arg2 must be boolean.");
			return 0;
		}

		/*
		* 1 : WEAR_COSTUME_BODY
		* 2 : WEAR_COSTUME_HAIR
		* 3 : WEAR_COSTUME_SASH
		* 4 : WEAR_COSTUME_WEAPON
		*/
		BYTE bPartPos = (BYTE)lua_tonumber(L, 1);
		bool hidePart = lua_toboolean(L, 2);

		if (bPartPos == 1)
			ch->SetBodyCostumeHidden(hidePart);
		else if (bPartPos == 2)
			ch->SetHairCostumeHidden(hidePart);
		else if (bPartPos == 3)
			ch->SetSashCostumeHidden(hidePart);
		else if (bPartPos == 4)
			ch->SetWeaponCostumeHidden(hidePart);
		else
		{
			sys_err("Invalid part");
			return 0;
		}

		ch->UpdatePacket();
		return 0;
	}
#endif

	int pc_get_client_version(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		lua_pushstring(L, ch->GetDesc()->GetClientVersion());
		return 1;
	}

	int pc_kill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch || ch->IsDead()) return 0;

		ch->EffectPacket(SE_CRITICAL);
		ch->PointChange(POINT_HP, -(ch->GetHP() + number(10, 255)), false);
		ch->Dead();
		return 0;
	}

	int pc_set_title(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		int cheque = (int)lua_tonumber(L, -1);
		ch->UpdateTitle(cheque);
		return 0;
	}
	
	int pc_get_title(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetTitle());
		return 1;
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	int pc_change_cheque(lua_State* L)
	{
		int cheque = (int)lua_tonumber(L, -1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (cheque + ch->GetCheque() < 0)
			sys_err("QUEST wrong ChangeCheque %d (now %d)", cheque, ch->GetCheque());
		else
		{
			//DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), cheque);
			ch->PointChange(POINT_CHEQUE, cheque, true);
		}

		return 0;
	}
	
	int pc_get_cheque(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetCheque());
		return 1;
	}
#endif

	int pc_is_exchange(lua_State* L)
	{
		lua_pushboolean(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetExchange() ? true : false);
		return 1;
	}

	int pc_get_quest_flag2(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, -1))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L, -1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetPC(ch->GetPlayerID());

			/* QuestFlag exploit Bug Fix // QUESTFLAGEXPLOIT // 27.10.17 - 20:44 */
			if (!pPC)
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				sys_err("Nullpointer in CHARACTER::pc_get_quest_flag2 %lu", ch->GetPlayerID());
				return 0;
			}

			lua_pushnumber(L, pPC->GetFlag(sz));
		}
		return 1;
	}

	int pc_set_quest_flag2(lua_State* L)
	{

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, 1) || !lua_isnumber(L, 2))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			const char* sz = lua_tostring(L, 1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetPC(ch->GetPlayerID());

			/* QuestFlag exploit Bug Fix // QUESTFLAGEXPLOIT // 27.10.17 - 20:44 */
			if (!pPC)
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				sys_err("Nullpointer in CHARACTER::pc_set_quest_flag2 %lu", ch->GetPlayerID());
				return 0;
			}

			pPC->SetFlag(sz, int(rint(lua_tonumber(L, 2))));
		}
		return 0;
	}

	int pc_del_quest_flag2(lua_State *L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isstring(L, 1))
		{
			sys_err("argument error");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L, 1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetPC(ch->GetPlayerID());

			/* QuestFlag exploit Bug Fix // QUESTFLAGEXPLOIT // 27.10.17 - 20:44 */
			if (!pPC)
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
				sys_err("Nullpointer in CHARACTER::pc_del_quest_flag2 %lu", ch->GetPlayerID());
				return 0;
			}

			pPC->DeleteFlag(sz);
		}
		return 0;
	}

	int pc_get_observermode(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch ? ch->IsObserverMode() : 0);
		return 1;
	}

	int pc_set_observermode(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		if (ch->IsObserverMode())
			ch->SetObserverMode(false);
		else
			ch->SetObserverMode(true);

		return 0;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////// NEW END /////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int pc_is_learnable_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int skillvnum = lua_tonumber(L, 1);
		bool ret = ch->IsLearnableSkill(skillvnum);
		lua_pushboolean(L, ret);
		return 1;
	}

	// Ejder Start //
	struct EjderKisi
	{
		EjderKisi() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					if (!ch->IsPC() || ch->IsObserverMode())
						return;

					ejderde_kac_kisiyiz = ejderde_kac_kisiyiz + 1;
				}
			}
		}
	};

	int pc_ejder_kontrol(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L, 1));

		if (NULL != pSecMap)
		{
			EjderKisi f;
			ejderde_kac_kisiyiz = 0;
			pSecMap->for_each(f);
			lua_pushnumber(L, ejderde_kac_kisiyiz);
		}

		return 1;
	}
	// Ejder End //

	struct KuleKisi
	{
		KuleKisi() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
				if (ch->IsPC())
				{
					if (!ch->IsPC() || ch->IsObserverMode())
						return;

					kulede_kac_kisiyiz = kulede_kac_kisiyiz + 1;
				}
			}
		}
	};

	int pc_kule_kontrol(lua_State * L)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lua_tonumber(L, 1));

		if (NULL != pSecMap)
		{
			KuleKisi f;
			kulede_kac_kisiyiz = 0;
			pSecMap->for_each(f);
			lua_pushnumber(L, kulede_kac_kisiyiz);
		}

		return 1;
	}

	int pc_is_observer(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;

		if (ch->IsObserverMode())
			lua_pushboolean(L, true);
		else
			lua_pushboolean(L, false);

		return 1;
	}

	int pc_warp_as_observer(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			lua_pushboolean(L, false);
			return 1;
		}

		long map_index = 0;
		if (lua_isnumber(L, 3))
			map_index = (int)lua_tonumber(L, 3);

		//PREVENT_HACK
		if (ch->IsHack())
		{
			lua_pushboolean(L, false);
			return 1;
		}
		//END_PREVENT_HACK

		ch->SetQuestFlag("warp.is_observer_member", 1);
		//ch->SaveExitLocation();
		ch->WarpSet((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), map_index);

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_izleyici(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch->IsObserverMode())
		{
			sys_err("QUEST: %s bu kisi zaten observer modunda !.", ch->GetName());
			return 0;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "Gozlemci modu aktif.");
		ch->SetObserverMode(true);
		return 0;
	}

	int _pc_set_coins(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		long val = (long)lua_tonumber(L, 1);
		SQLMsg *msg;

		msg = DBManager::instance().DirectQuery("UPDATE account.account SET coins = coins + '%ld' WHERE id = '%d'", val, ch->GetAID());

		if (msg->uiSQLErrno != 0)
		{
			sys_err("_pc_set_coins query failed");
			return 0;
		}
	}

	int _pc_set_crystal(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		long val = (long)lua_tonumber(L, 1);
		SQLMsg *msg;

		msg = DBManager::instance().DirectQuery("UPDATE account.account SET cash = cash + '%ld' WHERE id = '%d'", val, ch->GetAID());

		if (msg->uiSQLErrno != 0)
		{
			sys_err("_pc_set_crystal query failed");
			return 0;
		}
	}

	int pc_get_move_speed(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!ch) return 0;

		lua_pushnumber(L, ch->GetPoint(POINT_MOV_SPEED));
		return 1;

	}

	int pc_online(lua_State* L)
	{

		const char * arg1 = lua_tostring(L, 1);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

		if (!tch)
		{
			const CCI* pkCCI = P2P_MANAGER::instance().Find(arg1);

			if (pkCCI)
			{
				lua_pushnumber(L, 1);
				return 1;
			}
			else
			{
				lua_pushnumber(L, 0);
				return 0;
			}
		}
		else
		{
			lua_pushnumber(L, 1);
			return 1;
		}

	}

	int pc_is_afk(lua_State* L)
	{
		if (CQuestManager::instance().GetCurrentCharacterPtr()->IsAfk())
			lua_pushboolean(L, true);
		else
			lua_pushboolean(L, false);

		return 1;
	}

	int pc_set_afk(lua_State* L)
	{
		CQuestManager::instance().GetCurrentCharacterPtr()->SetAfk();
		return 1;
	}

	int pc_del_afk(lua_State* L)
	{
		CQuestManager::instance().GetCurrentCharacterPtr()->DelAfk();
		return 1;
	}

	int pc_is_open_shop(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetShopOwner() ? 1 : 0);
		return 1;
	}

	int pc_sys_err(lua_State * L)
	{
		if (!lua_isstring(L, 1))
		{
			sys_err("argument error");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L, 1);
			sys_err(sz);
		}
		return 0;
	}

	int pc_gm_quest_packet(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!ch->IsGM())
			return 0;
		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "SELECT gm_account_id FROM common.quest_packet WHERE gm_account_id = '%d'", ch->GetAID());
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows > 0)
			lua_pushboolean(L, true);
		else
			lua_pushboolean(L, false);

		return 1;
	}

	int pc_guvenli_pc_kontrol(lua_State* L)
	{
		if (!lua_isstring(L, 1))
		{
			sys_err("QuestSysError: %s ", lua_tostring(L, 1));
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "SELECT login FROM account.maclistesi WHERE login = '%s'", ch->GetDesc()->GetAccountTable().login);
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows > 0)
			lua_pushboolean(L, true);
		else
			lua_pushboolean(L, false);

		return 1;
	}

	int pc_guvenli_pc_hwid_kontrol(lua_State * L)
	{
		if (!lua_isstring(L, 1))
		{
			sys_err("QuestSysError: %s ", lua_tostring(L, 1));
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		char szQuery[1024];
		const char * szHwid = lua_tostring(L, 1);
		snprintf(szQuery, sizeof(szQuery), "SELECT hwid FROM account.maclistesi WHERE hwid = '%s' and login = '%s'", szHwid, ch->GetDesc()->GetAccountTable().login);
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows > 0)
			lua_pushboolean(L, true);
		else
			lua_pushboolean(L, false);

		return 1;
	}

	int pc_guvenli_pc_islem(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument pc_guvenli_pc_islem");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int dwStatus = (int)lua_tonumber(L, 1);
		SQLMsg *msg;

		msg = DBManager::instance().DirectQuery("UPDATE account.maclistesi SET acik = '%d' where login = '%s'", dwStatus, ch->GetDesc()->GetAccountTable().login);

		if (msg->uiSQLErrno != 0)
		{
			sys_err("pc_guvenli_pc_islem query failed");
			return 0;
		}
	}

	int pc_guvenli_pc_sil(lua_State * L)
	{
		if (!lua_tostring(L, 1))
		{
			sys_err("invalid argument pc_guvenli_pc_sil");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		const char * szHwid = lua_tostring(L, 1);
		SQLMsg *msg;

		msg = DBManager::instance().DirectQuery("DELETE FROM account.maclistesi WHERE hwid = '%s' and login = '%s'", szHwid, ch->GetDesc()->GetAccountTable().login);

		if (msg->uiSQLErrno != 0)
		{
			sys_err("pc_guvenli_pc_sil query failed");
			return 0;
		}
	}

	int pc_guvenli_pc_ekle(lua_State * L)
	{
		if (!lua_tostring(L, 1))
		{
			sys_err("invalid argument pc_guvenli_pc_ekle");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		const char * szHwid = lua_tostring(L, 1);

		DBManager::instance().DirectQuery("INSERT IGNORE INTO account.maclistesi(login, hwid) VALUES('%s', '%s')", ch->GetDesc()->GetAccountTable().login, szHwid);
		return 1;
	}

	int pc_set_item_kilit_sifre(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument pc_set_item_kilit_sifre");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		long dwPass = (long)lua_tonumber(L, 1);
		SQLMsg *msg;

		msg = DBManager::instance().DirectQuery("UPDATE account.account SET itmkltsifre = '%lld' where id = '%s'", dwPass, ch->GetAID());

		if (msg->uiSQLErrno != 0)
		{
			sys_err("pc_set_item_kilit_sifre query failed");
			return 0;
		}
	}

	int pc_set_karakter_kilit_sifre(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument pc_set_karakter_kilit_sifre");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		long dwPass = (long)lua_tonumber(L, 1);
		SQLMsg *msg;

		msg = DBManager::instance().DirectQuery("UPDATE player.player SET sifre = '%lld' where name = '%s'", dwPass, ch->GetName());

		if (msg->uiSQLErrno != 0)
		{
			sys_err("pc_set_karakter_kilit_sifre query failed");
			return 0;
		}
	}

	int pc_get_karakter_kilit_sifre(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		SQLMsg * pkMsg(DBManager::instance().DirectQuery("SELECT sifre FROM player.player WHERE name = '%s'", ch->GetName()));
		SQLResult * pRes = pkMsg->Get();
		if (pRes->uiNumRows > 0)
		{
			int iLine = 0;
			MYSQL_ROW mRow;
			while ((mRow = mysql_fetch_row(pRes->pSQLResult)) != NULL)
			{
				int	szPass = 0;
				str_to_number(szPass, mRow[0]);
				lua_pushnumber(L, szPass);
				iLine++;
			}
		}

		return 1;
	}

	int pc_get_esrarengiz_puan(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		SQLMsg * pkMsg(DBManager::instance().DirectQuery("SELECT esrarengiz_arena FROM player.player WHERE name = '%s'", ch->GetName()));
		SQLResult * pRes = pkMsg->Get();
		if (pRes->uiNumRows > 0)
		{
			int iLine = 0;
			MYSQL_ROW mRow;
			while ((mRow = mysql_fetch_row(pRes->pSQLResult)) != NULL)
			{
				int	szPass = 0;
				str_to_number(szPass, mRow[0]);
				lua_pushnumber(L, szPass);
				iLine++;
			}
		}

		return 1;
	}

	int pc_set_esrarengiz_arena_puan(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		long val = (long)lua_tonumber(L, 1);
		SQLMsg *msg;

		msg = DBManager::instance().DirectQuery("UPDATE player.player SET esrarengiz_arena = esrarengiz_arena + '%ld' WHERE id = '%s'", val, ch->GetName());

		if (msg->uiSQLErrno != 0)
		{
			sys_err("pc_set_esrarengiz_arena_puan query failed");
			return 0;
		}
	}

	int pc_get_pazar_gecmis(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "SELECT alanname, vnum, gold, cheque, time FROM player.pazar_gecmis WHERE name = '%s' ORDER BY time DESC limit 10", ch->GetName());
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows > 0)
		{
			int iLine = 1;
			MYSQL_ROW mRow;
			while (NULL != (mRow = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				int iCur = 0;
				DWORD dwVnum = 0, dwGold = 0, dwCheque = 0;
				const char* szAlanName = mRow[iCur++];
				str_to_number(dwVnum, mRow[iCur++]);
				str_to_number(dwGold, mRow[iCur++]);
				str_to_number(dwCheque, mRow[iCur++]);
				const char* szTime = mRow[iCur++];

				ch->ChatPacket(CHAT_TYPE_COMMAND, "pazar_ekle %d %s %d %d %d %s", iLine, szAlanName, dwVnum, dwGold, dwCheque, szTime);
				iLine++;
			}
		}

		return 1;
	}

	int pc_get_ticaret_gecmis(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "SELECT kisi1, kisi2, time FROM player.tic_gecmis WHERE kisi1 = '%s' or kisi2 = '%s' ORDER BY time DESC limit 10", ch->GetName(), ch->GetName());
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows > 0)
		{
			int iLine = 1;
			MYSQL_ROW mRow;
			while (NULL != (mRow = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				int iCur = 0;
				const char* szKisi = mRow[iCur++];
				const char* szKim = mRow[iCur++];
				const char* szTime = mRow[iCur++];

				ch->ChatPacket(CHAT_TYPE_COMMAND, "tic_ekle %d %s %s %s", iLine, szKisi, szKim, szTime);
				iLine++;
			}
		}

		return 1;
	}

	int pc_get_lonca_gecmis(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		char szQuery[1024];
		const char * szName = lua_tostring(L, 1);
		snprintf(szQuery, sizeof(szQuery), "SELECT guild FROM player.lonca_gecmis WHERE name = '%s'", szName);
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "gecmistemizle");
			return 1;
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "gecmistemizle");
			int iLine = 0;
			MYSQL_ROW mRow;
			while (NULL != (mRow = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				int iCur = 0;
				const char* szGuildName = mRow[iCur++];

				ch->ChatPacket(CHAT_TYPE_COMMAND, "lonca_gecmis_ekle %s", szGuildName);
				iLine++;
			}
		}

		return 1;
	}

	int pc_get_lider_id(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		SQLMsg * pkMsg(DBManager::instance().DirectQuery("SELECT player.guild.id AS guildid FROM player.guild LEFT JOIN player.player ON player.player.id = player.guild.master"));
		SQLResult * pRes = pkMsg->Get();
		if (pRes->uiNumRows == 0)
			return 1;
		else
		{
			int iLine = 0;
			MYSQL_ROW mRow;
			while ((mRow = mysql_fetch_row(pRes->pSQLResult)) != NULL)
			{
				int	szGuildID = 0;
				str_to_number(szGuildID, mRow[0]);
				lua_pushnumber(L, szGuildID);
				iLine++;
			}
		}

		return 1;
	}

#ifdef ENABLE_ACCOUNT_LOCK
	int pc_get_account_lock_size(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		lua_pushnumber(L, q.GetCurrentCharacterPtr()->GetAccountLockSize());
		return 1;
	}

	int pc_set_account_lock_size(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		if (ch->GetAccountLockSize() == 1)
			ch->SetAccountLockSize(0);
		else
			ch->SetAccountLockSize(1);

		return 0;
	}

	int pc_get_account_lock_password(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		lua_pushnumber(L, q.GetCurrentCharacterPtr()->GetAccountLockPassword());
		return 1;
	}

	int pc_set_account_lock_password(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		if (!lua_isnumber(L, 1))
		{
			sys_err("Hatali hesap kilit sifre degisim.");
			return 0;
		}

		ch->SetAccountLockPassword((int)lua_tonumber(L, 1));

		return 0;
	}

	int pc_get_account_lock_id(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		lua_pushnumber(L, q.GetCurrentCharacterPtr()->GetAccountLockId());
		return 1;
	}

	int pc_set_account_lock_id(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch) return 0;

		if (!lua_isnumber(L, 1))
		{
			sys_err("Hatali pc_set_account_lock_id");
			return 0;
		}

		ch->SetAccountLockId((int)lua_tonumber(L, 1));

		return 0;
	}
#endif

	int pc_is_busy(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->IsCombOpen() || ch->GetOfflineShopOwner()));
		return 1;
	}

int _block_exp(lua_State* L)
{
	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
	ch->block_exp = true;
	return 0;
}

int _unblock_exp(lua_State* L)
{
	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
	ch->block_exp = false;
	return 0;
}


	void RegisterPCFunctionTable()
	{
		luaL_reg pc_functions[] =
		{
			{ "get_wear",		pc_get_wear			},
			{ "get_player_id",	pc_get_player_id	},
			{ "get_account_id", pc_get_account_id	},
			{ "get_account",	pc_get_account		},
			{ "get_level",		pc_get_level		},
			{ "set_level",		pc_set_level		},
			{ "get_next_exp",		pc_get_next_exp		},
			{ "get_exp",		pc_get_exp		},
			{ "get_job",		pc_get_job		},
			{ "get_race",		pc_get_race		},
			{ "change_sex",		pc_change_sex	},
			{ "gethp",			pc_get_hp		},
			{ "get_hp",			pc_get_hp		},
			{ "getmaxhp",		pc_get_max_hp		},
			{ "get_max_hp",		pc_get_max_hp		},
			{ "getsp",			pc_get_sp		},
			{ "get_sp",			pc_get_sp		},
			{ "getmaxsp",		pc_get_max_sp		},
			{ "get_max_sp",		pc_get_max_sp		},
			{ "change_sp",		pc_change_sp		},
			{ "getmoney",		pc_get_money		},
			{ "get_money",		pc_get_money		},
			{ "get_real_alignment",	pc_get_real_alignment	},
			{ "get_alignment",		pc_get_alignment	},
			{ "getweapon",		pc_get_weapon		},
			{ "get_weapon",		pc_get_weapon		},
			{ "get_weapon_id",	pc_get_weapon_id	},
			{ "get_arrow_id",				pc_get_arrow_id },
			{ "get_armor_id",				pc_get_armor_id },
			{ "get_head_id",				pc_get_head_id },
			{ "get_belt_id",				pc_get_belt_id },
			{ "get_costume_body_id",		pc_get_costume_body_id },
			{ "get_costume_hair_id",		pc_get_costume_hair_id },
			{ "get_arrow",					pc_get_arrow },
			{ "get_head",					pc_get_head },
			{ "get_belt",					pc_get_belt	},
			{ "get_costume_body",			pc_get_costume_body },
			{ "get_costume_hair",			pc_get_costume_hair },
			{ "getarmor",		pc_get_armor		},
			{ "get_armor",		pc_get_armor		},
			{ "getcostume",		pc_get_costume		},
			{ "get_costume",	pc_get_costume		},
			{ "getgold",		pc_get_money		},
			{ "get_gold",		pc_get_money		},
			{ "changegold",		pc_change_money		},
			{ "changemoney",		pc_change_money		},
			{ "changealignment",	pc_change_alignment	},
			{ "change_gold",		pc_change_money		},
			{ "change_money",		pc_change_money		},
			{ "change_alignment",	pc_change_alignment	},
			{ "getname",		pc_get_name		},
			{ "get_name",		pc_get_name		},
			{ "get_vid",		pc_get_vid		},
			{ "getplaytime",		pc_get_playtime		},
			{ "get_playtime",		pc_get_playtime		},
			{ "getleadership",		pc_get_leadership	},
			{ "get_leadership",		pc_get_leadership	},
			{ "getqf",			pc_get_quest_flag	},
			{ "setqf",			pc_set_quest_flag	},
			{ "delqf",			pc_del_quest_flag	},
			{ "getf",			pc_get_another_quest_flag},
			{ "setf",			pc_set_another_quest_flag},
			{ "get_x",			pc_get_x		},
			{ "get_y",			pc_get_y		},
			{ "getx",			pc_get_x		},
			{ "gety",			pc_get_y		},
			{ "get_local_x",		pc_get_local_x		},
			{ "get_local_y",		pc_get_local_y		},
			{ "getcurrentmapindex",	pc_get_current_map_index},
			{ "get_map_index",		pc_get_current_map_index},
			{ "give_exp",		pc_give_exp		},
			{ "give_exp_perc",		pc_give_exp_perc	},
			{ "give_exp2",		pc_give_exp2		},
			{ "give_item",		pc_give_item		},
			{ "give_item2",		pc_give_or_drop_item	},
#ifdef ENABLE_DICE_SYSTEM
			{ "give_item2_with_dice",	pc_give_or_drop_item_with_dice	},
#endif
			{ "give_item2_select",		pc_give_or_drop_item_and_select	},
			{ "give_gold",		pc_give_gold		},
			{ "count_item",		pc_count_item		},
			{ "remove_item",		pc_remove_item		},
			{ "countitem",		pc_count_item		},
			{ "removeitem",		pc_remove_item		},
			{ "reset_point",		pc_reset_point		},
			{ "has_guild",		pc_hasguild		},
			{ "hasguild",		pc_hasguild		},
			{ "get_guild",		pc_getguild		},
			{ "getguild",		pc_getguild		},
			{ "isguildmaster",		pc_isguildmaster	},
			{ "is_guild_master",	pc_isguildmaster	},
			{ "destroy_guild",		pc_destroy_guild	},
			{ "remove_from_guild",	pc_remove_from_guild	},
			{ "in_dungeon",		pc_in_dungeon		},
			{ "getempire",		pc_get_empire		},
			{ "online",			pc_online		},
			{ "get_empire",		pc_get_empire		},
			{ "get_skill_group",	pc_get_skillgroup	},
			{ "set_skill_group",	pc_set_skillgroup	},
			{ "warp",			pc_warp			},
			{ "warp_local",		pc_warp_local		},
			{ "warp_exit",		pc_warp_exit		},
			{ "set_warp_location",	pc_set_warp_location	},
			{ "set_warp_location_local",pc_set_warp_location_local },
			{ "get_start_location",	pc_get_start_location	},
			{ "has_master_skill",	pc_has_master_skill	},
#ifdef __BATTLE_PASS__
			{ "do_mission",	pc_do_battle_pass	},
#endif
#ifdef ENABLE_OKAY_CARD
			{ "get_okay_global_rank",	pc_get_okay_global_rank	},
			{ "get_okay_rund_rank",	pc_get_okay_rund_rank	},
#endif
			{ "set_part",		pc_set_part		},
			{ "get_part",		pc_get_part		},
			{ "is_polymorphed",		pc_is_polymorphed	},
			{ "remove_polymorph",	pc_remove_polymorph	},
			{ "is_mount",		pc_is_mount		},
			{ "polymorph",		pc_polymorph		},
			{ "mount",			pc_mount		},
			{ "mount_bonus",	pc_mount_bonus	},
			{ "unmount",		pc_unmount		},
			{ "warp_to_guild_war_observer_position", pc_warp_to_guild_war_observer_position	},
			{ "give_item_from_special_item_group", pc_give_item_from_special_item_group	},
			{ "learn_grand_master_skill", pc_learn_grand_master_skill	},
			{ "is_skill_book_no_delay",	pc_is_skill_book_no_delay},
			{ "remove_skill_book_no_delay",	pc_remove_skill_book_no_delay},

			{ "enough_inventory",	pc_enough_inventory	},
			{ "get_horse_level",	pc_get_horse_level	}, // TO BE DELETED XXX
			{ "is_horse_alive",		pc_is_horse_alive	}, // TO BE DELETED XXX
			{ "revive_horse",		pc_revive_horse		}, // TO BE DELETED XXX
			{ "have_pos_scroll",	pc_have_pos_scroll	},
			{ "have_map_scroll",	pc_have_map_scroll	},
			{ "in_guild_map",		pc_in_guild_map		},
			{ "get_war_map",		pc_get_war_map		},
			{ "get_equip_refine_level",	pc_get_equip_refine_level },
			{ "refine_equip",		pc_refine_equip		},
			{ "get_skill_level",	pc_get_skill_level	},
			{ "give_lotto",		pc_give_lotto		},
			{ "aggregate_monster",	pc_aggregate_monster	},
			{ "forget_my_attacker",	pc_forget_my_attacker	},
			{ "pc_attract_ranger",	pc_attract_ranger	},
			{ "select",			pc_select_vid		},
			{ "get_sex",		pc_get_sex		},
			{ "is_married",		pc_is_married		},
			{ "is_engaged",		pc_is_engaged		},
			{ "is_engaged_or_married",	pc_is_engaged_or_married},
			{ "is_gm",			pc_is_gm		},
			{ "get_gm_level",		pc_get_gm_level		},
//#ifdef __DUNGEON_INFO_SYSTEM__
//			{ "get_last_damage", pc_get_last_damage },
//			{ "get_last_raking", pc_get_last_raking },
//			{ "set_time_dung", pc_set_time_dung },
//			{ "set_time_dung_cata", pc_set_time_dung_cata },
//			{ "set_time_dung_nemere", pc_set_time_dung_nemere },
//			{ "set_time_dung_puertas", pc_set_time_dung_puertas },
//			{ "set_time_dung_slime", pc_set_time_dung_slime },
//			{ "set_time_dung_arya", pc_set_time_dung_arya },
//			{ "set_time_dung_bosque", pc_set_time_dung_bosque },
//			{ "set_time_dung_orco", pc_set_time_dung_orco },
//			{ "set_time_dung_isla", pc_set_time_dung_isla },
//#endif
			{ "mining",			pc_mining		},
			{ "ore_refine",		pc_ore_refine		},
			{ "diamond_refine",		pc_diamond_refine	},
#ifdef ENABLE_RONARK_SYSTEM
			{ "not_guild",			pc_not_guild		},
			{ "guild_online_member",			pc_set_guild_online_member		},
			{ "remove_guild_online_member",			pc_remove_guild_online_member		},
#endif
			// RESET_ONE_SKILL
			{ "clear_one_skill",        pc_clear_one_skill      },
			// END_RESET_ONE_SKILL

			{ "clear_skill",                pc_clear_skill          },
			{ "clear_sub_skill",    pc_clear_sub_skill      },
			{ "set_skill_point",    pc_set_skill_point      },

			{ "is_clear_skill_group",	pc_is_clear_skill_group		},

			{ "save_exit_location",		pc_save_exit_location		},
			{ "teleport",				pc_teleport },

			{ "set_skill_level",        pc_set_skill_level      },

            { "give_polymorph_book",    pc_give_polymorph_book  },
            { "upgrade_polymorph_book", pc_upgrade_polymorph_book },
            { "get_premium_remain_sec", pc_get_premium_remain_sec },

			{ "send_block_mode",		pc_send_block_mode	},

			{ "change_empire",			pc_change_empire	},
			{ "get_change_empire_count",	pc_get_change_empire_count	},
			{ "set_change_empire_count",	pc_set_change_empire_count	},

			{ "change_name",			pc_change_name },

			#ifdef ENABLE_MESSENGER_BLOCK
			{ "is_blocked",			pc_is_blocked },
			{ "is_friend",			pc_is_friend },
			#endif

			{ "is_dead",				pc_is_dead	},

			{ "reset_status",		pc_reset_status	},
			{ "get_ht",				pc_get_ht	},
			{ "set_ht",				pc_set_ht	},
			{ "get_iq",				pc_get_iq	},
			{ "set_iq",				pc_set_iq	},
			{ "get_st",				pc_get_st	},
			{ "set_st",				pc_set_st	},
			{ "get_dx",				pc_get_dx	},
			{ "set_dx",				pc_set_dx	},

			{ "is_near_vid",		pc_is_near_vid	},

			{ "get_socket_items",	pc_get_socket_items	},
			{ "get_empty_inventory_count",	pc_get_empty_inventory_count	},

			{ "get_logoff_interval",	pc_get_logoff_interval	},

			{ "is_riding",			pc_is_riding	},
			{ "get_special_ride_vnum",	pc_get_special_ride_vnum	},

			{ "can_warp",			pc_can_warp		},
			{"change_race", pc_change_race},
			{ "dec_skill_point",	pc_dec_skill_point	},
			{ "get_skill_point",	pc_get_skill_point	},

			{ "get_channel_id",		pc_get_channel_id	},

			{ "give_poly_marble",	pc_give_poly_marble	},
			{ "get_sig_items",		pc_get_sig_items	},

			{ "charge_cash",		pc_charge_cash		},
			
			{ "get_informer_type",	pc_get_informer_type	},	//???? ???? ????
			{ "get_informer_item",  pc_get_informer_item	},

			{ "give_award",			pc_give_award			},	//???? ?????? ?????? ???? ????
			{ "give_award_socket",	pc_give_award_socket	},	//?? ?????????? ?????? ????. ???? ?????? ???? ????.

#ifdef __ANTI_EXP_RING__
			{ "block_exp",    _block_exp    },
			{ "unblock_exp",    _unblock_exp    },
#endif

			{ "get_killee_drop_pct",	pc_get_killee_drop_pct	}, 

			{ "get_ip",                pc_get_ip            },
            { "kill",                pc_kill                },
            { "get_empire_name",    pc_get_empire_name    },
			{ "ejder_kontrol",		pc_ejder_kontrol	},
			{ "kule_kontrol", pc_kule_kontrol },
			{ "izleyici",				pc_izleyici			},
			{ "is_learnable_skill", pc_is_learnable_skill },
			{ "get_mount_vnum", pc_get_mount_vnum },
			{ "get_point", pc_get_point },
			{ "get_real_point", pc_get_real_point },
			{ "show_effect", pc_specific_effect },
			{ "disconnect_with_delay", pc_disconnect_with_delay },
			{ "get_max_level", pc_get_max_level },
			{ "get_client_version", pc_get_client_version },
			{ "give_and_equip_item", pc_give_and_equip_item },
			{ "is_observer_member", pc_is_observer },
			{ "warp_as_observer", pc_warp_as_observer },
			{ "set_coins",	_pc_set_coins		},
			{ "set_crystal", _pc_set_crystal },
			{ "get_move_speed", pc_get_move_speed },
			{ "online",			pc_online		},
			{ "is_afk",				pc_is_afk			},
			{ "set_afk",			pc_set_afk			},
			{ "del_afk",			pc_del_afk			},
			{ "is_open_shop",		pc_is_open_shop		},	

			{ "get_karakter",				pc_get_karakter },
			{ "get_karakter_cinsiyet",		pc_get_karakter_cinsiyet },
			{ "karakter_degisim",			pc_karakter_degisim },
			{ "getqf2",						pc_get_quest_flag2	},
			{ "setqf2",						pc_set_quest_flag2	},
			{ "delqf2",						pc_del_quest_flag2	},
			{ "get_observermode",			pc_get_observermode },
			{ "set_observermode",			pc_set_observermode },

			{ "sys_err",			pc_sys_err				},	
			{ "gm_quest_packet",	pc_gm_quest_packet		},	
			{ "guvenli_pc_kontrol", pc_guvenli_pc_kontrol	},
			{ "guvenli_pc_islem",	pc_guvenli_pc_islem		},	
			{ "guvenli_pc_sil",		pc_guvenli_pc_sil		},	
			{ "guvenli_pc_ekle",	pc_guvenli_pc_ekle		},	
			{ "guvenli_pc_hwid_kontrol",	pc_guvenli_pc_hwid_kontrol	},	
			{ "set_item_kilit_sifre",		pc_set_item_kilit_sifre		},
			{ "set_karakter_kilit_sifre",	pc_set_karakter_kilit_sifre		},
			{ "get_karakter_kilit_sifre",	pc_get_karakter_kilit_sifre		},
			{ "set_esrarengiz_puan",		pc_set_esrarengiz_arena_puan	},
			{ "get_esrarengiz_puan",		pc_get_esrarengiz_puan	},
			{ "get_pazar_gecmis",			pc_get_pazar_gecmis			},
			{ "get_ticaret_gecmis",			pc_get_ticaret_gecmis		},

			{ "get_lonca_gecmis",	pc_get_lonca_gecmis		},
			{ "get_lider_id",		pc_get_lider_id			},

#ifdef ENABLE_CHEQUE_SYSTEM
			{ "get_cheque",			pc_get_cheque	},
			{ "change_cheque",			pc_change_cheque	},
#endif

#ifdef ENABLE_ACCOUNT_LOCK
			{ "get_account_lock_size",			pc_get_account_lock_size },
			{ "set_account_lock_size",			pc_set_account_lock_size },

			{ "get_account_lock_password",		pc_get_account_lock_password },
			{ "set_account_lock_password",		pc_set_account_lock_password },

			{ "get_account_lock_id",			pc_get_account_lock_id },
			{ "set_account_lock_id",			pc_set_account_lock_id },
#endif

			{ "is_exchange",		pc_is_exchange			}, 
#ifdef ENABLE_DRAGONLAIR_SYSTEMS
			{ "ejder_kontrol", pc_ejder_kontrol },
#endif
			//Best Ayar
			{ "is_busy",			pc_is_busy	},
			{ "set_title",	pc_set_title },
			{ "get_title",	pc_get_title },

#ifdef WJ_CHANGELOOK_SYSTEM
			{"open_changelook", pc_open_changelook},
#endif


#ifdef __SASH_SYSTEM__
			{"open_sash", pc_open_sash},
#endif

			{ "imp_event_yolla_mavi", pc_imp_event_yolla_mavi },
			{ "imp_event_yolla_sari", pc_imp_event_yolla_sari },
			{ "imp_event_yolla_kirmizi", pc_imp_event_yolla_kirmizi },
			{ "imp_mavi", pc_imp_mavi },
			{ "imp_sari", pc_imp_sari },
			{ "imp_kirmizi", pc_imp_kirmizi },
			{ "imp_baslat", pc_imp_baslat },
			{ "imp_katilimci", pc_imp_katilimci },
			{ "arena_baslat", pc_arena_baslat },
			{ "arena_hemen_dog", pc_arena_hemen_dog },
			{ "arena_online", pc_arena_online },
			{ "arena_killer_mode", pc_arena_killer_mode },
			{ "theprnce_hapis_go", pc_theprnce_hapis_go },
			{ "is_arena", pc_is_arena },
			{ "is_arena_observer", pc_is_arena_observer },
			{ "theprnce_alisveris_kontrol", pc_theprnce_alisveris_kontrol },
			{ "theprnce_chat_ban", pc_theprnce_chat_ban },
#ifdef __HIDE_COSTUME_SYSTEM__
			{ "hide_costume", pc_hide_costume },
#endif
#ifdef ENABLE_WOLFMAN_CHARACTER
			{ "is_flag_bleeding",		pc_if_bleeding		},
#endif
#ifdef ENABLE_WOLFMAN_CHARACTER
			{ "set_flag_bleeding",		pc_sf_bleeding		},
#endif
			{ NULL,			NULL			}
		};

		CQuestManager::instance().AddLuaFunctionTable("pc", pc_functions);
	}
};
