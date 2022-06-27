#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../libthecore/include/xmd5.h"
#endif
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#ifdef ENABLE_GROWTH_PET_SYSTEM
#include "New_PetSystem.h"
#endif
#include "DragonSoul.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "char.h"
#include "dev_log.h"
#include "item.h"
#ifdef TOURNAMENT_PVP_SYSTEM
	#include "tournament.h"
#endif
#ifdef ENABLE_BIOLOG_SYSTEM
	#include "biolog.h"
#endif
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#ifdef ENABLE_MANAGER_BANK_SYSTEM
	#include "bank.h"
#endif
#include "threeway_war.h"
#include "log.h"
#ifdef ENABLE_TITLE_SYSTEM
#include "title.h"
#endif
#ifdef ENABLE_LOCALIZATION_SYSTEM
	#include "localization.h"
#endif
#ifdef WJ_OFFLINESHOP_SYSTEM
#include "offlineshop_manager.h"
#endif
#include "../../common/VnumHelper.h"
#include "exchange.h"
#include "db.h"
#include "shop.h"
#include "shop_manager.h"
#include <string>



#ifdef ENABLE_REBORN_SYSTEM
	#include "reborn.h"
#endif
#if defined(WJ_COMBAT_ZONE)	
	#include "combat_zone.h"
#endif

#define MAXLIMIT 1999999999
#ifdef WJ_OFFLINESHOP_SYSTEM
#include "offlineshop_manager.h"
#endif
#if defined(WJ_USER_CONTROL)
	#include "user_control.h"
#endif

extern int g_server_id;

extern int g_nPortalLimitTime;

DWORD shopvid;
int shopvnum;
struct NPCBul
	{
		NPCBul() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER) ent;
				if (ch->IsNPC())
				{
					shopvid = ch->GetVID();
				}
			}
		}
	};

LPEVENT bktimer = NULL;

EVENTINFO(TMainEventInfo2)
{
	LPCHARACTER	kim;	
	long skillindexx;	
	
	TMainEventInfo2() 
	: kim( NULL )
	, skillindexx( 0 )
	{
	}

};

EVENTFUNC(bk_event)
{
	TMainEventInfo2 * info = dynamic_cast<TMainEventInfo2 *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "bk_event> <Factor> Null pointer" );
		return 0;
	}
	
	LPCHARACTER	ch = info->kim;
	long skillindex = info->skillindexx;
	
	if (NULL == ch || skillindex == 0)
		return 0;
	
	if (!ch)
		return 0;
	
	if (ch)
	{
		if(ch->CountSpecifyItem(50300) < 1 )
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkyok"));
			return 0;
		}
		
		int skilllevel = ch->GetSkillLevel(skillindex);
		if (skilllevel >= 30)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkskillson"));
			return 0;
		}
		
		int dwVnum = ch->BKBul(skillindex);
		if (dwVnum == 999)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkskillyok"));
			return 0;
		}
			
		LPITEM item = ch->GetInventoryItem(dwVnum);
		
		if (item->GetVnum() != 50300 || item->GetSocket(0) != skillindex)
		{
			return 0;
		}
		
		if (ch->CountSpecifyItem(71001) > 0)
		{
			if (!ch->IsAffectFlag(513))
			{
				ch->AddAffect(513, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
				ch->RemoveSpecifyItem(71001,1);
			}
		}
		
		if (ch->CountSpecifyItem(71001) < 1)
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkkoturuhyok"));
			return 0;
		}

		if (ch->CountSpecifyItem(71094) >= 1)
		{
			if (!ch->IsAffectFlag(512))
			{
				ch->AddAffect(512, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
				ch->RemoveSpecifyItem(71094,1);
			}
		}
		if (item->GetVnum() == 50300)
		{
			if (true == ch->LearnSkillByBook(skillindex))
			{
				if (item->GetVnum() == 50300 && item->GetSocket(0) == skillindex)
				{
					item->SetCount(item->GetCount() - 1);
					int iReadDelay;
					iReadDelay= number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					ch->SetSkillNextReadTime(skillindex, get_global_time() + iReadDelay);
				}
			}
			else
			{
				if (item->GetVnum() == 50300 && item->GetSocket(0) == skillindex)
				{
					item->SetCount(item->GetCount() - 1);
					int iReadDelay;
					iReadDelay= number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					ch->SetSkillNextReadTime(skillindex, get_global_time() + iReadDelay);
				}
			}
		}
		return 1;
	}
	return 0;	
}

#ifdef ENABLE_DS_CHANGE_ATTR
ACMD(do_ds_change_attr)
{
    char arg1[256];
    one_argument(argument, arg1, sizeof(arg1));

    if (!*arg1)
        return;

    if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()) {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¢¥U¢¬¡Í ¡ÆA¡¤¢®A¡ËAI ¢¯¢¬¡Æ¡íoAA¢¯¢®¨ù¢¥A ¡íoA¢®¡ÆA¡¤¢®¢¬| CO¨ùo ¡Æ¢® ¨ú©ª¨öA¢¥I¢¥U."));
        return;
    }

    DWORD dwPos = 0;
    str_to_number(dwPos, arg1);
    //const BYTE needFireCountList[] = { 7, 7, 7, 7, 7};
    BYTE bNeedFire = 7;

    if (dwPos < 0 || dwPos >= DRAGON_SOUL_INVENTORY_MAX_NUM)
        return;

    LPITEM item = ch->GetItem(TItemPos(DRAGON_SOUL_INVENTORY, dwPos));

	if (!item || item->IsExchanging() || item->IsEquipped())
		return;

	if (item->IsDragonSoul()) {
		//bNeedFire = needFireCountList[(item->GetVnum() / 100) % 10];
		if (DSManager::instance().IsActiveDragonSoul(item) == true) 
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_CHANGE_BONUS_OF_EQUIPPED_DRAGON_SOUL_ITEM"));
			return;
		}

		if (ch->CountSpecifyItem(100700) >= bNeedFire) 
		{
			for (BYTE i=0;i < 6;i++) { item->SetForceAttribute(i, 0, 0); }
			bool ret = DSManager::instance().PutAttributes2(item);
			if (ret == true)
			{
				if (ch->GetGold() < 500000)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU_DONT_HAVE_ENOUGH_MONEY"));
					return;
				}
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SUCCESSFULLY_CHANGED_BONUS_OF_DRAGON_SOUL_ITEM"));
				ch->RemoveSpecifyItem(100700, bNeedFire);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "DS_ChangeAttr_Success");
			} 
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("FAILED_CHANGING_BONUS_OF_DRAGON_SOUL_ITEM"));
				ch->ChatPacket(CHAT_TYPE_COMMAND, "DS_ChangeAttr_Failed");
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_CHANGE_BONUS_YOU_HAVE_NOT_ENOUGH_FLAMES"));
			return;
		} 
    }
}
#endif

ACMD(do_bkoku)
{
	int gelen;
	long skillindex;
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	str_to_number(gelen, arg1);
	str_to_number(skillindex, arg2);
	
	if (!*arg1)
		return;

	if (!*arg2)
		return;

	if (!ch)
		return;
 
	if (!ch->IsPC())
		return;
 
    if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHack())
		return;

	if(ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->IsCombOpen() || ch->GetOfflineShopOwner())	
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡Áß(Ã¢°í,±³È¯,»óÁ¡)¿¡´Â °³ÀÎ»óÁ¡À» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	
	int skillgrup = ch->GetSkillGroup();
	if (skillgrup == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkokuyamazsin"));
		return;
	}
	
	if(ch->CountSpecifyItem(50300) < 1 )
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkyok"));
		return;
	}
	
	if (gelen == 1) ///tek
	{
		int skilllevel = ch->GetSkillLevel(skillindex);

		if (skilllevel >= 30)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhskillson"));
			return;
		}
		
		if (ch->GetQuestFlag("bk.yenisure") > get_global_time())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bk1sn"));
			return;
		}
		
		int dwVnum = ch->BKBul(skillindex);
		if (dwVnum == 999)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkskillyok"));
			return;
		}
		
		LPITEM item = ch->GetInventoryItem(dwVnum);
		
		if (item->GetVnum() != 50300 || item->GetSocket(0) != skillindex)
		{
			return;
		}

		if (ch->CountSpecifyItem(71001) > 0)
		{
			if (!ch->IsAffectFlag(513))
			{
				ch->AddAffect(513, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
				ch->RemoveSpecifyItem(71001,1);
			}
		}

		if (ch->CountSpecifyItem(71094) >= 1)
		{
			if (!ch->IsAffectFlag(512))
			{
				ch->AddAffect(512, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
				ch->RemoveSpecifyItem(71094,1);
			}
		}

		if (item->GetVnum() == 50300)
		{
			
			
			if (ch->FindAffect(AFFECT_SKILL_NO_BOOK_DELAY) == false && ch->CountSpecifyItem(71001) < 1)
			{
				ch->SkillLearnWaitMoreTimeMessage(ch->GetSkillNextReadTime(skillindex) - get_global_time());
				return;
			}
			
			if (true == ch->LearnSkillByBook(skillindex))
			{
				if (item->GetVnum() == 50300 && item->GetSocket(0) == skillindex)
				{
					item->SetCount(item->GetCount() - 1);
					int iReadDelay;
					iReadDelay= number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					ch->SetSkillNextReadTime(skillindex, get_global_time() + iReadDelay);
				}	
			}
			else
			{
				if (item->GetVnum() == 50300 && item->GetSocket(0) == skillindex)
				{
					item->SetCount(item->GetCount() - 1);
					int iReadDelay;
					iReadDelay= number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					ch->SetSkillNextReadTime(skillindex, get_global_time() + iReadDelay);
				}
			}
			ch->SetQuestFlag("bk.yenisure",get_global_time()+1);
		}
	}
	else if(gelen == 0) ///hepsi
	{
		if (bktimer)
		{
			event_cancel(&bktimer);
		}
		
		TMainEventInfo2* info = AllocEventInfo<TMainEventInfo2>();
		info->kim = ch;
		info->skillindexx = skillindex;
		bktimer = event_create(bk_event, info, PASSES_PER_SEC(1));
	}

	return;
}

LPEVENT ruhtimer = NULL;

EVENTINFO(TMainEventInfo)
{
	LPCHARACTER	kim;	
	long skillindexx;	
	
	TMainEventInfo() 
	: kim( NULL )
	, skillindexx( 0 )
	{
	}

};

EVENTFUNC(ruh_event)
{
	TMainEventInfo * info = dynamic_cast<TMainEventInfo *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "ruh_event> <Factor> Null pointer" );
		return 0;
	}
	
	LPCHARACTER	ch = info->kim;
	long skillindex = info->skillindexx;
	
	if (NULL == ch || skillindex == 0)
		return 0;
	
	if(ch->CountSpecifyItem(50513) < 1 )
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhtasiyok"));
		return 0;
	}
	
	int skilllevel = ch->GetSkillLevel(skillindex);

	if (skilllevel >= 40)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhskillson"));
		return 0;
	}
	int gerekenderece = (1000+500*(skilllevel-30));
	int derecem = (ch->GetRealAlignment()/10);
	int sonuc = (-19000+gerekenderece);

	if (derecem < 0)
	{
		gerekenderece = gerekenderece*2;
		sonuc = (-19000-gerekenderece);
	}

	if (derecem > sonuc)
	{
		// int gerekliknk = sonuc-derecem;
		int gerekliknk = gerekenderece;
		int kactane = gerekliknk/500;
		if (kactane < 0)
		{
			kactane = 0 - kactane;
		}
		
		if (derecem < 0)
		{
			if (ch->CountSpecifyItem(70102) < kactane)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhzenbitti %d"),kactane);
				return 0;
			}
			
			int delta = MIN(-(ch->GetAlignment()), 500);
			ch->UpdateAlignment(delta*kactane);
			ch->RemoveSpecifyItem(70102,kactane);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhzenbastim"));
		}
	}
		
	if(ch->GetQuestFlag("ruh.sure") > get_global_time())
	{
		if (ch->CountSpecifyItem(71001) < 1 )
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhsuredolmadi"));
			return 0;
		}
		else
		{
			ch->RemoveSpecifyItem(71001,1);
		}

	}
	
	if (ch->CountSpecifyItem(71094) >= 1)
	{
		ch->AddAffect(512, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
		ch->RemoveSpecifyItem(71094,1);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhmunzevikullandim"));
	}
	
	if (gerekenderece < 0)
	{
		ch->UpdateAlignment(gerekenderece*10);
	}
	else
	{
		ch->UpdateAlignment(-gerekenderece*10);
	}
	ch->LearnGrandMasterSkill(skillindex);
	ch->RemoveSpecifyItem(50513,1);
	ch->SetQuestFlag("ruh.sure",get_global_time()+60*60*24);

	return 1;
}

ACMD(do_ruhoku)
{
	int gelen;
	long skillindex;
	char arg1[256], arg2[256];
	
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	str_to_number(gelen, arg1);
	str_to_number(skillindex, arg2);
	
	if (!*arg1)
		return;

	if (!*arg2)
		return;

	if (!ch)
		return;
 
	if (!ch->IsPC())
		return;
 
    if (ch->IsDead() || ch->IsStun())
		return;
 
	if (ch->IsHack())
		return;

	if(ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->IsCombOpen() || ch->GetOfflineShopOwner())	
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡Áß(Ã¢°í,±³È¯,»óÁ¡)¿¡´Â °³ÀÎ»óÁ¡À» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	
	if(ch->CountSpecifyItem(50513) < 1 )
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhtasiyok"));
		return;
	}
	
	LPITEM slot1 = ch->GetWear(WEAR_UNIQUE1);
	LPITEM slot2 = ch->GetWear(WEAR_UNIQUE2);
	
	if (NULL != slot1)
	{
		if (slot1->GetVnum() == 70048)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("pelerin_cikar"));
			return;
		}
	}
	if (NULL != slot2)
	{
		if (slot2->GetVnum() == 70048)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("pelerin_cikar"));
			return;
		}
	}
	
	int skillgrup = ch->GetSkillGroup();
	if (skillgrup == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhokuyamazsin"));
		return;
	}
	
	if (gelen == 1) ///tek
	{
		int skilllevel = ch->GetSkillLevel(skillindex);
		int gerekenderece = (1000+500*(skilllevel-30));
		int derecem = (ch->GetRealAlignment()/10);
		int sonuc = (-19000+gerekenderece);
		
		if (ch->GetQuestFlag("ruh.yenisure") > get_global_time())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruh1sn"));
			return;
		}
		
		if (derecem < 0)
		{
			gerekenderece = gerekenderece*2;
			sonuc = (-19000-gerekenderece);
		}
		if (derecem > sonuc)
		{

			// int gerekliknk = sonuc-derecem;
			int gerekliknk = gerekenderece;
			int kactane = gerekliknk/500;
			if (kactane < 0)
			{
				kactane = 0 - kactane;
			}
			
			if (derecem < 0)
			{
				if (ch->CountSpecifyItem(70102) < kactane)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhzenbitti %d"),kactane);
					return;
				}
				
				int delta = MIN(-(ch->GetAlignment()), 500);
				ch->UpdateAlignment(delta*kactane);
				ch->RemoveSpecifyItem(70102,kactane);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhzenbastim"));
			}
		}

		if(ch->GetQuestFlag("ruh.sure") > get_global_time())
		{
			if (ch->CountSpecifyItem(71001) < 1 )
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhsuredolmadi"));
				return;
			}
			else
			{
				ch->RemoveSpecifyItem(71001,1);
			}
		}
		
		if (ch->CountSpecifyItem(71094) >= 1)
		{
			ch->AddAffect(512, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
			ch->RemoveSpecifyItem(71094,1);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhmunzevikullandim"));
		}
		
		if (gerekenderece < 0)
		{
			ch->UpdateAlignment(gerekenderece*10);
		}
		else
		{
			ch->UpdateAlignment(-gerekenderece*10);
		}
		
		ch->LearnGrandMasterSkill(skillindex);
		ch->RemoveSpecifyItem(50513,1);
		ch->SetQuestFlag("ruh.sure",get_global_time()+60*60*24);
		ch->SetQuestFlag("ruh.yenisure",get_global_time()+1);
	}
	else if(gelen == 0) ///hepsi
	{
		if (ruhtimer)
		{
			event_cancel(&ruhtimer);
		}
		
		TMainEventInfo* info = AllocEventInfo<TMainEventInfo>();
		info->kim = ch;
		info->skillindexx = skillindex;
		ruhtimer = event_create(ruh_event, info, PASSES_PER_SEC(1));
	}
	return;
}

ACMD(do_npcac)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	str_to_number(shopvnum, arg1);

	if (shopvnum == 0 || shopvnum == NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("uzaknpchata"));
		return;
	}

	if (!ch->IsPC())
		return;

	if(ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡Áß(Ã¢°í,±³È¯,»óÁ¡)¿¡´Â °³ÀÎ»óÁ¡À» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	
	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap( ch->GetMapIndex() );

	if (NULL != pSecMap)
	{
		NPCBul f;
		pSecMap->for_each( f );
		
		LPCHARACTER yeninpc = CHARACTER_MANAGER::instance().Find(shopvid);

		if (yeninpc)
		{
			
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("uzaknpc"));
			CShopManager::instance().NPCAC(ch, yeninpc, shopvnum);
		}
		
	}
	return;
}

#ifdef ENABLE_RONARK_SYSTEM
ACMD(do_ronark_system)
{
	if(ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->IsCombOpen() || ch->GetOfflineShopOwner())
		return;

	if (!ch->GetGuild())
		return;

	quest::CQuestManager& R = quest::CQuestManager::instance();
	CGuild* GetGuildRonark = CGuildManager::instance().FindGuild(ch->GetGuild()->GetID());

	int RonarkKill = 	R.GetEventFlag("RonarkKill");
	if (RonarkKill == 1)
		return;

	int GetOneGuildID = R.GetEventFlag("OneGuildIDRanked");
	int GetTwoGuildID = R.GetEventFlag("TwoGuildIDRanked");
	int GetThreeGuildID = R.GetEventFlag("ThreeGuildIDRanked");
	int GetFourGuildID = R.GetEventFlag("FourGuildIDRanked");
	int GetFiveGuildID = R.GetEventFlag("FiveGuildIDRanked");

	int GetOneGuildPoint = R.GetEventFlag("OneGuildPoint");
	int GetTwoGuildPoint = R.GetEventFlag("TwoGuildPoint");
	int GetThreeGuildPoint = R.GetEventFlag("ThreeGuildPoint");
	int GetFourGuildPoint = R.GetEventFlag("FourGuildPoint");
	int GetFiveGuildPoint = R.GetEventFlag("FiveGuildPoint");

	int GetOneGuildMemberCount = R.GetEventFlag("OneGuildMemberCount");
	int GetTwoGuildMemberCount = R.GetEventFlag("TwoGuildMemberCount");
	int GetThreeGuildMemberCount = R.GetEventFlag("ThreeGuildMemberCount");
	int GetFourGuildMemberCount = R.GetEventFlag("FourGuildMemberCount");
	int GetFiveGuildMemberCount = R.GetEventFlag("FiveGuildMemberCount");

	ch->ChatPacket(CHAT_TYPE_COMMAND,"OneRonarkGuild %d %d %d",GetOneGuildMemberCount ,GetOneGuildID ,GetOneGuildPoint);
	ch->ChatPacket(CHAT_TYPE_COMMAND,"TwoRonarkGuild %d %d %d",GetTwoGuildMemberCount ,GetTwoGuildID ,GetTwoGuildPoint);
	ch->ChatPacket(CHAT_TYPE_COMMAND,"ThreeRonarkGuild %d %d %d",GetThreeGuildMemberCount ,GetThreeGuildID ,GetThreeGuildPoint);
	ch->ChatPacket(CHAT_TYPE_COMMAND,"FourRonarkGuild %d %d %d",GetFourGuildMemberCount ,GetFourGuildID ,GetFourGuildPoint);
	ch->ChatPacket(CHAT_TYPE_COMMAND,"FiveRonarkGuild %d %d %d",GetFiveGuildMemberCount ,GetFiveGuildID ,GetFiveGuildPoint);
}
#endif

ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		// ¸»ÀÌ ¾Æ´Ñ ´Ù¸¥Å»°ÍÀ» Å¸°íÀÖ´Ù.
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì Å»°ÍÀ» ÀÌ¿ëÁßÀÔ´Ï´Ù."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» ¸ÕÀú ¼ÒÈ¯ÇØÁÖ¼¼¿ä."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

#if defined(WJ_USER_CONTROL)
ACMD(do_user_control)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	CUserControl::instance().Get(ch, arg1, arg2);
}
#endif

ACMD(do_ajanikov) //EviLsHow
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hata: Gereksiz islem yapmayin.");
		return;
	}
	if (!ch->GetWarMap())
		return;

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1); 
	if (!tch)
		return;
	if (!tch->IsPC())
		return;	
	if (ch == tch)
		return;
	if (!tch->GetGuild())
		return;	
	if (!ch->GetGuild())
		return;
	if (tch->GetGuild()->GetID() != ch->GetGuild()->GetID())
		return;
	CGuild* loncam = CGuildManager::instance().FindGuild(ch->GetGuild()->GetID());
	CGuild* loncan = CGuildManager::instance().FindGuild(tch->GetGuild()->GetID());
	if ((loncam == NULL) || (loncan == NULL))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Loncan yok");
	}
	else
	{
		if (loncan->GetMasterPID() == ch->GetPlayerID())
		{
			if (loncam->RequestRemoveMember(tch->GetPlayerID()))
			{
				ch->ChatPacket(CHAT_TYPE_GUILD, "%s ajan kovuldu.", tch->GetName());
				tch->GoHome();
				return;
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Hata.");
				return;
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Lonca Lideri Olman Gerekiyor");
			return;
		}
	}
	return;
}

#ifdef BESTPRODUCTION_BOT_KONTROL_SYSTEM
ACMD(do_captcha)
{

	char arg1[256];
	long gelensifre;
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	str_to_number(gelensifre, arg1);

#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot bot control with security key activate"));
		return;
	}
#endif

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken bot ayari yapamazsin.");
		return;
	}
#endif

	if (gelensifre == 0 || gelensifre == NULL)
	{

		ch->GetDesc()->DelayedDisconnect(7);
		ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT("%s_captcha_hata"), ch->GetName());
		return;
	}

	long captcham = ch->GetCaptcha();

	if (captcham == gelensifre)
	{
		ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT("%s_captcha_dogru"), ch->GetName());
		int sans;
		if (ch->GetLevel() >= 75 && ch->GetLevel() < gPlayerMaxLevel)
		{
			sans = number(45, 60);
		}
		else
		{
			sans = number(5, 30);
		}

		ch->SetQuestFlag("captcha.durum", 0);
		ch->SetQuestFlag("captcha.sure", get_global_time() + (300 * sans));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT("%s_captcha_yanlis"), ch->GetName());
		int sayi1 = number(1, 9);
		int sayi2 = number(1, 9);
		int sayi3 = number(1, 9);
		int sayi4 = number(1, 9);
		int sayi5 = number(1, 9);
		long yenicaptcha = (sayi1 * 10000) + (sayi2 * 1000) + (sayi3 * 100) + (sayi4 * 10) + sayi5;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "captcha %d %d %d %d %d", sayi1, sayi2, sayi3, sayi4, sayi5);
		ch->SetCaptcha(yenicaptcha);

	}
	return;
}
#endif

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» µ¹·Áº¸³Â½À´Ï´Ù."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»¿¡¼­ ¸ÕÀú ³»·Á¾ß ÇÕ´Ï´Ù."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» ¸ÕÀú ¼ÒÈ¯ÇØÁÖ¼¼¿ä."));
	}
}

ACMD(do_user_horse_feed)
{
	// °³ÀÎ»óÁ¡À» ¿¬ »óÅÂ¿¡¼­´Â ¸» ¸ÔÀÌ¸¦ ÁÙ ¼ö ¾ø´Ù.
	if (ch->GetMyShop())
		return;

	if (ch->GetHorse() == NULL)
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» ¸ÕÀú ¼ÒÈ¯ÇØÁÖ¼¼¿ä."));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» Åº »óÅÂ¿¡¼­´Â ¸ÔÀÌ¸¦ ÁÙ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»¿¡°Ô %s%s ÁÖ¾ú½À´Ï´Ù."), 
				ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName,
				g_iUseLocale ? "" : under_han(ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName) ? LC_TEXT("À»") : LC_TEXT("¸¦"));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ¾ÆÀÌÅÛÀÌ ÇÊ¿äÇÕ´Ï´Ù"), ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName);
	}
}

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), LC_TEXT("¼Ë´Ù¿îÀÌ %dÃÊ ³²¾Ò½À´Ï´Ù."), *pSec);
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), LC_TEXT("%dÃÊ ÈÄ °ÔÀÓÀÌ ¼Ë´Ù¿î µË´Ï´Ù."), iSec);

	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	if (NULL == ch)
	{
		sys_err("Accept shutdown command from %s.", ch->GetName());
	}
	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}

ACMD(do_tanitim)
{

	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi>  >>Sunucu Bilgileri<< ");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> 1-120 TR Tipi Emek Server.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> 5.Karakter Lycan - (?ft savunma - pen? & býçak).");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Geliþmiþ D?llo Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Geliþmiþ Biyalog Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Ekstra Refine Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Otomatik Event Sistemi C++.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> ?man Destek Sistemi (Afroid).");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Oyuncu S?alama Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Lonca S?alama Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Boss S?alama Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Uzaktan NPC Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Lonca Tan?? Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Lonca Ge?iþi Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Lonca Ajan - ?tastik Sistemi.");	
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Geliþmiþ Banka Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Baþar?Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Hesap Engelleme Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> ?freli Kule Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Otomatik Event Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Oyun Ýçi Oto Bak? Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> GM Oyun Ýçi Y?etim Paneli.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Evcil Binek Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> H?l?Ruh Taþ?ve BK Okuma Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> ?paratorluk Turnuvas?Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Hayomi Turnuvas?Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Arena Turnuvas?Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Ekstra Slot Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Geliþmiþ Oyuncu Paneli Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Ticket Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Geliþmiþ OX Sistemi.");	
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Silah Evrim Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Kost? Efekt Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Official Gaya Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Silah-Z?h ve Kost? Yans?ma Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Krali? Meley Sistemi");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Official B???Orman ve Jotun Thrym.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Bugsuz Sorunsuz Nemere & Razador & Ork Labirenti.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Beceri Kitab?ve Y?seltme Nesneleri i?n Ek Envanter Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Y? baþ?puan?sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Ticaret cam?sistemi (Sorunsuz Bugsuz).");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> ?k Fonksiyonlu ?vrimdýþ?Pazar Sistemi. (Bar, Pul ,Pen? ,Won ile Satýþ ?kan?.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Simya sistemi offical.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Official Kuþak Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Silah g????ve z?h g????fiþi sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Offical güçl?binek sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Offical Won Sistemi.");	
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Eþya Sil-Düþ? Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> 7 ve 8 Skill Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Ok ?ntas?sistemi offical.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> B??bozma taþ?sistemi (z?h , silah).");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Ticarete koyulmayan item efekti sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Moblar'dan ve slotlardan düþen itemleri g?terme sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> 2 Adet Farkl?Efsun Botu (Yeþil Efsun , Normal Efsun Botu) Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Giriþ g?enlik sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Kost? kombinasyon sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> ?em Ruha Ba?ama sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Official Seviyeli Pet Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Kost? efsun de?þtirme ve efsun ekleme sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> H?l?Kanal De?þtirme Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Matkap (Z?h-Silah) 4.Taþ Ekleme Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Youtube Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Offical t? mapler dahildir (B???Orman , Ork Labirenti , Beta Mapler vs).");	
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Sand? Aynas?Sistemi.");
	ch->ChatPacket(CHAT_TYPE_INFO, "<Bilgi> Oyun Tan?? Paneli.");
}

#ifdef ENABLE_TITLE_SYSTEM
ACMD(do_prestige_title)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("your argument is invalid"));
		return;
	}
	
	if (!strcmp(arg1, "prestige_0")){			
		TitleManager::instance().SetTitle(ch, "disable");}	
	
	if (!strcmp(arg1, "prestige_1")){		
		TitleManager::instance().SetTitle(ch, "title1");}
	
	if (!strcmp(arg1, "prestige_2")){		
		TitleManager::instance().SetTitle(ch, "title2");}	
	
	if (!strcmp(arg1, "prestige_3")){		
		TitleManager::instance().SetTitle(ch, "title3");}
	
	if (!strcmp(arg1, "prestige_4")){		
		TitleManager::instance().SetTitle(ch, "title4");}	
	
	if (!strcmp(arg1, "prestige_5")){		
		TitleManager::instance().SetTitle(ch, "title5");}	
	
	if (!strcmp(arg1, "prestige_6")){		
		TitleManager::instance().SetTitle(ch, "title6");}		
	
	if (!strcmp(arg1, "prestige_7")){		
		TitleManager::instance().SetTitle(ch, "title7");}	
	
	if (!strcmp(arg1, "prestige_8")){		
		TitleManager::instance().SetTitle(ch, "title8");}		
	
	if (!strcmp(arg1, "prestige_9")){		
		TitleManager::instance().SetTitle(ch, "title9");}	
	
	if (!strcmp(arg1, "prestige_10")){		
		TitleManager::instance().SetTitle(ch, "title10");}	
	
	if (!strcmp(arg1, "prestige_11")){		
		TitleManager::instance().SetTitle(ch, "title11");}	
	
	if (!strcmp(arg1, "prestige_12")){		
		TitleManager::instance().SetTitle(ch, "title12");}	
	
	if (!strcmp(arg1, "prestige_13")){		
		TitleManager::instance().SetTitle(ch, "title13");}	
	
	if (!strcmp(arg1, "prestige_14")){		
		TitleManager::instance().SetTitle(ch, "title14");}	
	
	if (!strcmp(arg1, "prestige_15")){		
		TitleManager::instance().SetTitle(ch, "title15");}
	
	if (!strcmp(arg1, "prestige_16")){		
		TitleManager::instance().SetTitle(ch, "title16");}	
	
	if (!strcmp(arg1, "prestige_17")){		
		TitleManager::instance().SetTitle(ch, "title17");}	
	
	if (!strcmp(arg1, "prestige_18")){		
		TitleManager::instance().SetTitle(ch, "title18");}	
	
	if (!strcmp(arg1, "prestige_19")){		
		TitleManager::instance().SetTitle(ch, "title19");}	

	if (!strcmp(arg1, "buy_prestige_1")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_1");}	

	if (!strcmp(arg1, "buy_prestige_2")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_2");}		

	if (!strcmp(arg1, "buy_prestige_3")){		
		TitleManager::instance().BuyPotion(ch, "buy_potion_3");}	

	if (!strcmp(arg1, "vegas_transform_title")){		
		TitleManager::instance().TransformTitle(ch);}			
}
#endif

#ifdef ENABLE_REBORN_SYSTEM
ACMD(do_set_reborn)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("your argument is invalid"));
		//ch->ChatPacket(CHAT_TYPE_INFO, "your argument is invalid");
		return;
	}

	if (!strcmp(arg1, "accept"))
	{
		RebornManager::instance().SetLevel(ch);
	}

	if (!strcmp(arg1, "anonymous"))
	{
		int isAnonymous = ch->GetQuestFlag("reborn.valueAnonymous");

		if (isAnonymous != 2 && isAnonymous != 1) 
		{
			ch->SetQuestFlag("reborn.valueAnonymous", 1);
			RebornManager::instance().SetAnonymous(ch, "anonymous");
		}
		else
			RebornManager::instance().SetAnonymous(ch, "anonymous");	
	}

	if (!strcmp(arg1, "remove_affect_administrator"))
	{
		RebornManager::instance().SetAffect(ch, "remove_affect_administrator");
	}
}
#endif

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		if (true == LC_IsEurope() || true == LC_IsYMIR() || true == LC_IsKorea())
		{
			switch (info->subcmd)
			{
				case SCMD_LOGOUT:
				case SCMD_QUIT:
				case SCMD_PHASE_SELECT:
					{
						TPacketNeedLoginLogInfo acc_info;
						acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;

						db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

						LogManager::instance().DetailLoginLog( false, ch );
					}
					break;
			}
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				break;

			case SCMD_PHASE_SELECT:
				{
					ch->Disconnect("timed_event - SCMD_PHASE_SELECT");

					if (d)
					{
						d->SetPhase(PHASE_SELECT);
					}
				}
				break;
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dÃÊ ³²¾Ò½À´Ï´Ù."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	/* RECALL_DELAY
	   if (ch->m_pkRecallEvent != NULL)
	   {
	   ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ãë¼Ò µÇ¾ú½À´Ï´Ù."));
	   event_cancel(&ch->m_pkRecallEvent);
	   return;
	   }
	// END_OF_RECALL_DELAY */

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ãë¼Ò µÇ¾ú½À´Ï´Ù."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("·Î±×ÀÎ È­¸éÀ¸·Î µ¹¾Æ °©´Ï´Ù. Àá½Ã¸¸ ±â´Ù¸®¼¼¿ä."));
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°ÔÀÓÀ» Á¾·á ÇÕ´Ï´Ù. Àá½Ã¸¸ ±â´Ù¸®¼¼¿ä."));
			break;

		case SCMD_PHASE_SELECT:
		{
				if ((ch->GetGMLevel() <= GM_PLAYER) && (ch->GetDungeon()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Burada karakter atamazsiniz.");
					return;
				}
				
				switch (ch->GetMapIndex())
				{
					case 235:
						ch->ChatPacket(CHAT_TYPE_INFO, "Burada karakter atamazsiniz.");
					return;
				}	
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ä³¸¯ÅÍ¸¦ ÀüÈ¯ ÇÕ´Ï´Ù. Àá½Ã¸¸ ±â´Ù¸®¼¼¿ä."));
			break;
		}
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
		false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()) &&
	   	(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_mount)
{
	/*
	   char			arg1[256];
	   struct action_mount_param	param;

	// ÀÌ¹Ì Å¸°í ÀÖÀ¸¸é
	if (ch->GetMountingChr())
	{
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	return;

	param.x		= atoi(arg1);
	param.y		= atoi(arg2);
	param.vid	= ch->GetMountingChr()->GetVID();
	param.is_unmount = true;

	float distance = DISTANCE_SQRT(param.x - (DWORD) ch->GetX(), param.y - (DWORD) ch->GetY());

	if (distance > 600.0f)
	{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á» ´õ °¡±îÀÌ °¡¼­ ³»¸®¼¼¿ä."));
	return;
	}

	action_enqueue(ch, ACTION_TYPE_MOUNT, &param, 0.0f, true);
	return;
	}

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	return;

	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(atoi(arg1));

	if (!tch->IsNPC() || !tch->IsMountable())
	{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°Å±â¿¡´Â Å» ¼ö ¾ø¾î¿ä."));
	return;
	}

	float distance = DISTANCE_SQRT(tch->GetX() - ch->GetX(), tch->GetY() - ch->GetY());

	if (distance > 600.0f)
	{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á» ´õ °¡±îÀÌ °¡¼­ Å¸¼¼¿ä."));
	return;
	}

	param.vid		= tch->GetVID();
	param.is_unmount	= false;

	action_enqueue(ch, ACTION_TYPE_MOUNT, &param, 0.0f, true);
	 */
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

#ifdef ENABLE_LOCALIZATION_SYSTEM
ACMD(do_localization)
{
	if (!ch)
		return;

	if (!ch->IsPC() || NULL == ch)
		return;

	char arg1[256];
	char arg2[256];
	two_arguments (argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (!strcmp(arg1, "open"))
	{
		LocalizationManager::instance().Send_Localization_Open(ch, false);
	}
	
	if (!strcmp(arg1, "refresh"))
	{
		LocalizationManager::instance().Send_Localization_Open(ch, true);
	}

	if (!strcmp(arg1, "teleport")) 
	{
		int idxWarp = 0;
		str_to_number(idxWarp, arg2);
		LocalizationManager::instance().Send_Localization_Teleport(ch, idxWarp);
	}

	if (!strcmp(arg1, "save"))
	{
		int idxSave = 0;
		str_to_number(idxSave, arg2);
		LocalizationManager::instance().Send_Localization_Save(ch, idxSave);
	}
}; 
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
ACMD(do_biolog)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
	{
		BiologManager::instance().SendButton(ch);
		return;
	}
	
	if (!strcmp(arg1, "92_reward_1")) {
		BiologManager::instance().SelectBonusType(ch, "92_reward_1"); return; }		
	if (!strcmp(arg1, "92_reward_2")) {
		BiologManager::instance().SelectBonusType(ch, "92_reward_2"); return; }				
	if (!strcmp(arg1, "92_reward_3")) {
		BiologManager::instance().SelectBonusType(ch, "92_reward_3"); return; }		
	if (!strcmp(arg1, "94_reward_1")) {
		BiologManager::instance().SelectBonusType(ch, "94_reward_1"); return; }			
	if (!strcmp(arg1, "94_reward_2")) {
		BiologManager::instance().SelectBonusType(ch, "94_reward_2"); return; }					
	if (!strcmp(arg1, "94_reward_3")) {
		BiologManager::instance().SelectBonusType(ch, "94_reward_3"); return; }			
}	
#endif

#ifdef __CHANGE_CHANNEL_SYSTEM__
ACMD(do_change_channel)
{
	if (!ch)
		return;

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ãë¼Ò µÇ¾ú½À´Ï´Ù."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Usage: channel <new channel>"));
		return;
	}

	short channel;
	str_to_number(channel, arg1);

	if (channel < 1 || channel > 4)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please enter a valid channel."));
		return;
	}

	if (channel == g_bChannel)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already on channel %d."), g_bChannel);
		return;
	}

	if (g_bChannel == 99)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The map you are at is cross-channel, changing won't have any effect."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change channel while in a dungeon."));
		return;
	}

	TPacketChangeChannel p;
	p.channel = channel;
	p.lMapIndex = ch->GetMapIndex();

	db_clientdesc->DBPacket(HEADER_GD_FIND_CHANNEL, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}
#endif

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Guvenlik acik iken tekrar baslayamazsin.");
		return;
	}
#endif

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken tekrar baslayamazsin.");
		return;
	}
#endif

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / passes_per_sec);

#if defined(WJ_COMBAT_ZONE)	
	if (CCombatZoneManager::Instance().IsCombatZoneMap(ch->GetMapIndex()))
	{
		CCombatZoneManager::Instance().OnRestart(ch, subcmd);
		return;
	}	
#endif

	if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				//¼ºÁö ¸ÊÀÏ°æ¿ì¿¡´Â Ã¼Å© ÇÏÁö ¾Ê´Â´Ù.
				if (false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ Àç½ÃÀÛ ÇÒ ¼ö ¾ø½À´Ï´Ù. (%dÃÊ ³²À½)"), iTimeToDead - (180 - g_nPortalLimitTime));
					return;
				}
			}

			if (iTimeToDead > 170)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ Àç½ÃÀÛ ÇÒ ¼ö ¾ø½À´Ï´Ù. (%dÃÊ ³²À½)"), iTimeToDead - 170);
				return;
			}
		}
	}

	//PREVENT_HACK
	//DESC : Ã¢°í, ±³È¯ Ã¢ ÈÄ Æ÷Å»À» »ç¿ëÇÏ´Â ¹ö±×¿¡ ÀÌ¿ëµÉ¼ö ÀÖ¾î¼­
	//		ÄðÅ¸ÀÓÀ» Ãß°¡
	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->IsHack())
		{
			//±æµå¸Ê, ¼ºÁö¸Ê¿¡¼­´Â Ã¼Å© ÇÏÁö ¾Ê´Â´Ù.
			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
			   	false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ Àç½ÃÀÛ ÇÒ ¼ö ¾ø½À´Ï´Ù. (%dÃÊ ³²À½)"), iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}
		}

		if (iTimeToDead > 173)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ ¸¶À»¿¡¼­ Àç½ÃÀÛ ÇÒ ¼ö ¾ø½À´Ï´Ù. (%d ÃÊ ³²À½)"), iTimeToDead - 173);
			return;
		}
	}
	//END_PREVENT_HACK

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();

	//FORKED_LOAD
	//DESC: »ï°Å¸® ÀüÅõ½Ã ºÎÈ°À» ÇÒ°æ¿ì ¸ÊÀÇ ÀÔ±¸°¡ ¾Æ´Ñ »ï°Å¸® ÀüÅõÀÇ ½ÃÀÛÁöÁ¡À¸·Î ÀÌµ¿ÇÏ°Ô µÈ´Ù.
	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (subcmd == SCMD_RESTART_TOWN || subcmd == SCMD_RESTART_HERE)
		{
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(ch->GetMapIndex()) &&
					false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

				ch->ReviveInvisible(5);
#ifdef __MOUNT_SYSTEM__
				ch->CheckMount();
#endif
#ifdef WJ_EXTENDED_PET_SYSTEM
				ch->CheckPet();
#endif
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				return;
			}

			//¼ºÁö
			if (true == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(ch->GetPlayerID()) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ºÁö¿¡¼­ ºÎÈ° ±âÈ¸¸¦ ¸ðµÎ ÀÒ¾ú½À´Ï´Ù! ¸¶À»·Î ÀÌµ¿ÇÕ´Ï´Ù!"));
					ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
				}
				else
				{
					ch->Show(ch->GetMapIndex(), GetSungziStartX(ch->GetEmpire()), GetSungziStartY(ch->GetEmpire()));
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);
#ifdef __MOUNT_SYSTEM__
				ch->CheckMount();
#endif
#ifdef WJ_EXTENDED_PET_SYSTEM
				ch->CheckPet();
#endif

				return;
			}
		}
	}
	//END_FORKED_LOAD

	if (ch->GetDungeon())
		ch->GetDungeon()->UseRevive(ch);

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						ch->Show(ch->GetMapIndex(), pos.x, pos.y);
					else
						ch->ExitToSavedLocation();

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef __MOUNT_SYSTEM__
					ch->CheckMount();
#endif
#ifdef WJ_EXTENDED_PET_SYSTEM
					ch->CheckPet();
#endif
					break;

				case SCMD_RESTART_HERE:
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef __MOUNT_SYSTEM__
					ch->CheckMount();
#endif
#ifdef WJ_EXTENDED_PET_SYSTEM
					ch->CheckPet();
#endif
					break;
			}

			return;
		}
	}

	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			
			if (ch->GetMapIndex() != 200)
			{
				if (ch->GetMapIndex() == 218)
				{	
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					int durum = quest::CQuestManager::instance().GetEventFlag("imp_fullcan");
					if(durum == 0)
					{
						ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					}
					else
					{
						ch->PointChange(POINT_HP, 50 - ch->GetHP());
					}
					ch->DeathPenalty(0);
					ch->ReviveInvisible(3);
					
				}
				else
				{
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
						ch->WarpSet(pos.x, pos.y);
					else
						ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->DeathPenalty(1);
				}
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arena_engel"));
				sys_log(0, "do_restart: restart here");
				ch->RestartAtSamePos();
				if (ch->GetMapIndex() == 200)
				{
					ch->SetKillerMode(true);
				}
				//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
				if (ch->GetMapIndex() == 200)
				{
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				}
				else
				{
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				}
				ch->DeathPenalty(0);
				ch->ReviveInvisible(5);
			}
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			ch->RestartAtSamePos();
			int durum = quest::CQuestManager::instance().GetEventFlag("imp_fullcan");
			if (ch->GetMapIndex() == 200)
			{
				ch->SetKillerMode(true);
			}
			if(ch->GetMapIndex() == 200)
			{
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			}
			else if(durum == 0 && ch->GetMapIndex() == 218)
			{
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			}
			else
			{
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			}
			ch->DeathPenalty(0);
			ch->ReviveInvisible(5);
#ifdef __MOUNT_SYSTEM__
			ch->CheckMount();
#endif
#ifdef WJ_EXTENDED_PET_SYSTEM
			ch->CheckPet();
#endif
			break;
	}
}
// Statuler ka? kadar çýkacak
#define MAX_STAT 100

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µÐ°© Áß¿¡´Â ´É·ÂÀ» ¿Ã¸± ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µÐ°© Áß¿¡´Â ´É·ÂÀ» ¿Ã¸± ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	// ch->ChatPacket(CHAT_TYPE_INFO, "%s GRP(%d) idx(%u), MAX_STAT(%d), expr(%d)", __FUNCTION__, ch->GetRealPoint(idx), idx, MAX_STAT, ch->GetRealPoint(idx) >= MAX_STAT);
	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

#ifdef ENABLE_PVP_ADVANCED
#include <string>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
const char* m_nDuelTranslate[] = /* You can translate here */
{
	"[You] The money must be in numbers.",
	"[You] The amount introduced is less than 0.", 
	"[You] Entered amount is as high as the maximum possible value in inventory.",
	"[You] The amount introduced is less than that amount that you own in inventory.",
	"[You] The amount introduced + current money is greater than the maximum amount possible in inventory.",
	"[Victim] The amount introduced + current money is greater than the maximum amount possible in inventory.",
	"[Victim] The amount introduced is less than that amount that victim own in inventory.",
	"Now you're in a duel.",
	"The victim was already in a duel, you cannot send him invitation.",
	"Viewing the equipment already is BLOCKED.",
	"Viewing the equipment was BLOCKED, now no one will be able to see when you send your request for a duel.",
	"Viewing the equipment was UNBLOCKED, now all people will be able to see your equipment in duel.",
	"Viewing the equipment already is UNBLOCK."	
}; 

const char* szTableStaticPvP[] = {BLOCK_CHANGEITEM, BLOCK_BUFF, BLOCK_POTION, BLOCK_RIDE, BLOCK_PET, BLOCK_POLY, BLOCK_PARTY, BLOCK_EXCHANGE_, BET_WINNER, CHECK_IS_FIGHT};

void Duel_SendMessage(LPCHARACTER ch, const char* msg)
{
	std::string textLine;
	
	textLine = msg;
	boost::algorithm::replace_all(textLine, " ", "$");
				 
	char buf[512+1];
	snprintf(buf, sizeof(buf), "BINARY_Duel_SendMessage %s", textLine.c_str());
	ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
}

ACMD(do_pvp)
{
#ifdef TOURNAMENT_PVP_SYSTEM
	if (CTournamentPvP::instance().IsTournamentMap(ch, TOURNAMENT_BLOCK_DUEL))
		return;
#endif
#define ENABLE_BLOCK_NEW_DUEL_SYSTEM_FOR_DOJANG
#ifdef ENABLE_BLOCK_NEW_DUEL_SYSTEM_FOR_DOJANG
if ( ch->GetMapIndex() == 115)
    {
        ch->ChatPacket(CHAT_TYPE_INFO,"<Hata> Bu iþlev engellendi !");
        return;
    }
#endif
#if defined(WJ_COMBAT_ZONE)
	if (CCombatZoneManager::Instance().IsCombatZoneMap(ch->GetMapIndex()))
		return;
#endif

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken duello yapamazsin.");
		return;
	}
#endif

	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (int(ch->GetQuestFlag("Kilit.Enable")) == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GUVENLIK_ENGEL"));
		return;
	}

	char arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256], arg9[256], arg10[256];
	one_argument(argument, arg1, sizeof(arg1));

	pvp_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3), arg4, sizeof(arg4), arg5, sizeof(arg5), arg6, sizeof(arg6), arg7, sizeof(arg7), arg8, sizeof(arg8), arg9, sizeof(arg9), arg10, sizeof(arg10));	

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

	if (ch->GetExchange() || pkVictim->GetExchange())
	{
		CPVPManager::instance().Decline(ch, pkVictim);
		return;
	}
	
	if (pkVictim->GetArena() != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÌ ´ë·ÃÁßÀÔ´Ï´Ù."));
		return;
	}

	#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::instance().IsBlocked_Target(ch->GetName(), pkVictim->GetName()))
	{
		//ben bloklad? hac?		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk"), pkVictim->GetName());
		return;
	}
	if (MessengerManager::instance().IsBlocked_Me(ch->GetName(), pkVictim->GetName()))
	{
		//o bloklad?hac?		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk_me"), pkVictim->GetName());
		return;
	}
	#endif

	if (pkVictim->IsBlockMode(BLOCK_DUELLO_MODE))      
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("DUELLO_BLOKLAMA"));
		return;
	}
	
	if (pkVictim->GetArena() != NULL)
	{
		pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÌ ´ë·ÃÁßÀÔ´Ï´Ù."));
		return;
	}

	if (*arg2 && !strcmp(arg2, "accept"))
	{
		int chA_nBetMoney = ch->GetQuestFlag(szTableStaticPvP[8]);
		int chB_nBetMoney = pkVictim->GetQuestFlag(szTableStaticPvP[8]);
		int chTotalGold = (ch->GetGold() + chA_nBetMoney);
		int VictimTotalGold = (pkVictim->GetGold() + chB_nBetMoney);

		if ((ch->GetGold() < chA_nBetMoney) || (pkVictim->GetGold() < chB_nBetMoney))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<PvP> D?lloya baþlayamaz çünk?bahis paras?la ilgili bir sorun var."));
			pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<PvP> D?lloya baþlayamaz çünk?bahis paras?la ilgili bir sorun var."));
			CPVPManager::instance().Decline(ch, pkVictim);
			return;
		}

		if ((chTotalGold > GOLD_MAX) || (VictimTotalGold > GOLD_MAX))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<PvP> D?lloya baþlayamaz çünk?bahis paras?la ilgili bir sorun var."));
			pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<PvP> D?lloya baþlayamaz çünk?bahis paras?la ilgili bir sorun var."));
			CPVPManager::instance().Decline(ch, pkVictim);
			return;
		}

		if (chA_nBetMoney > 0 && chA_nBetMoney > 0)
		{
			ch->SetDuel("IsFight", 1);
			pkVictim->SetDuel("IsFight", 1);

			ch->PointChange(POINT_GOLD, - chA_nBetMoney, true);	
			pkVictim->PointChange(POINT_GOLD, - chB_nBetMoney, true);	
		}

		CPVPManager::instance().Insert(ch, pkVictim);
		return;
	}

	int m_BlockChangeItem = 0, m_BlockBuff = 0, m_BlockPotion = 0, m_BlockRide = 0, m_BlockPet = 0, m_BlockPoly = 0, m_BlockParty = 0, m_BlockExchange = 0, m_BetMoney = 0;

	str_to_number(m_BlockChangeItem, arg2);
	str_to_number(m_BlockBuff, arg3);
	str_to_number(m_BlockPotion, arg4);
	str_to_number(m_BlockRide, arg5);
	str_to_number(m_BlockPet, arg6);
	str_to_number(m_BlockPoly, arg7);
	str_to_number(m_BlockParty, arg8);
	str_to_number(m_BlockExchange, arg9);
	str_to_number(m_BetMoney, arg10);

	if (!isdigit(*arg10))
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[0]);
		return;
	}
	
	if (m_BetMoney < 0)
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[1]);
		return;
	}

	if (m_BetMoney >= GOLD_MAX)
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[2]);
		return;
	}
	
	if (ch->GetGold() < m_BetMoney)
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[3]);
		return;
	}

	if ((ch->GetGold() + m_BetMoney) > GOLD_MAX)
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[4]);
		return;
	}

	if ((pkVictim->GetGold() + m_BetMoney) > GOLD_MAX)
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[5]);		
		return;
	}

	if (pkVictim->GetGold() < m_BetMoney)
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[6]);
		return;
	}

	if (*arg1 && *arg2 && *arg3 && *arg4 && *arg5 && *arg6 && *arg7 && *arg8 && *arg9 && *arg10)
	{
		ch->SetDuel("BlockChangeItem", m_BlockChangeItem);			ch->SetDuel("BlockBuff", m_BlockBuff);
		ch->SetDuel("BlockPotion", m_BlockPotion);					ch->SetDuel("BlockRide", m_BlockRide);
		ch->SetDuel("BlockPet", m_BlockPet);						ch->SetDuel("BlockPoly", m_BlockPoly);	
		ch->SetDuel("BlockParty", m_BlockParty);					ch->SetDuel("BlockExchange", m_BlockExchange);
		ch->SetDuel("BetMoney", m_BetMoney);

		pkVictim->SetDuel("BlockChangeItem", m_BlockChangeItem);	pkVictim->SetDuel("BlockBuff", m_BlockBuff);
		pkVictim->SetDuel("BlockPotion", m_BlockPotion);			pkVictim->SetDuel("BlockRide", m_BlockRide);
		pkVictim->SetDuel("BlockPet", m_BlockPet);					pkVictim->SetDuel("BlockPoly", m_BlockPoly);	
		pkVictim->SetDuel("BlockParty", m_BlockParty);				pkVictim->SetDuel("BlockExchange", m_BlockExchange);
		pkVictim->SetDuel("BetMoney", m_BetMoney);

		CPVPManager::instance().Insert(ch, pkVictim);
	}
}

ACMD(do_pvp_advanced)
{
#ifdef TOURNAMENT_PVP_SYSTEM
	if (CTournamentPvP::instance().IsTournamentMap(ch, TOURNAMENT_BLOCK_DUEL))
		return;
#endif
#if defined(WJ_COMBAT_ZONE)	
	if (CCombatZoneManager::Instance().IsCombatZoneMap(ch->GetMapIndex()))
		return;
#endif
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

    if (!pkVictim)
        return;

    if (pkVictim->IsNPC())
        return;

	if (pkVictim->GetArena() != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÌ ´ë·ÃÁßÀÔ´Ï´Ù."));
		return;
	}

	if (ch->GetQuestFlag(szTableStaticPvP[9]) > 0)
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[7]);
		return;
	}

	if (pkVictim->GetQuestFlag(szTableStaticPvP[9]) > 0)
	{
		//Duel_SendMessage(ch, m_nDuelTranslate[8]);
		return;
	}

	int statusEq = pkVictim->GetQuestFlag(BLOCK_EQUIPMENT_);

	CGuild * g = pkVictim->GetGuild();

	const char* m_Name = pkVictim->GetName();
	const char* m_GuildName = "-";

	int m_Vid = pkVictim->GetVID();	
	int m_Level = pkVictim->GetLevel();
	int m_PlayTime = pkVictim->GetRealPoint(POINT_PLAYTIME);
	int m_MaxHP = pkVictim->GetMaxHP();
	int m_MaxSP = pkVictim->GetMaxSP();

	DWORD m_Race = pkVictim->GetRaceNum();

	if (g)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Duel_GetInfo %d %s %s %d %d %d %d %d", m_Vid, m_Name, g->GetName(), m_Level, m_Race, m_PlayTime, m_MaxHP, m_MaxSP);

		if (statusEq < 1)
			pkVictim->SendEquipment(ch);
	}
	else
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_Duel_GetInfo %d %s %s %d %d %d %d %d", m_Vid, m_Name, m_GuildName, m_Level, m_Race, m_PlayTime, m_MaxHP, m_MaxSP);

		if (statusEq < 1)
			pkVictim->SendEquipment(ch);
}

ACMD(do_commands_staff)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	if (!ch)
		return;

	char command = LOWER(*arg1);

	if (!strcmp(arg1, "OpenCBGui"))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCBGui");
	}

	else if (!strcmp(arg1, "OpenCBGui1"))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCBGui1");
	}

	else if (!strcmp(arg1, "OpenCBGui2"))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCBGui2");
	}

	else if (!strcmp(arg1, "OpenCBGui3"))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCBGui3");
	}

	else if (!strcmp(arg1, "OpenCBGui4"))
	{ 
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCBGui4");
	}
}

ACMD(do_decline_pvp)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

	CPVPManager::instance().Decline(ch, pkVictim);
}

ACMD(do_block_equipment)
{
	char arg1[256];
	one_argument (argument, arg1, sizeof(arg1));
	
	if (!ch->IsPC() || NULL == ch)
		return;
	
	int statusEq = ch->GetQuestFlag(BLOCK_EQUIPMENT_);
	
	if (!strcmp(arg1, "BLOCK"))
	{	
		if (statusEq > 0)
		{	
			//Duel_SendMessage(ch, m_nDuelTranslate[9]);
			return;
		}	
		else
			ch->SetQuestFlag(BLOCK_EQUIPMENT_, 1);
			//Duel_SendMessage(ch, m_nDuelTranslate[10]);
	}
	
	if (!strcmp(arg1, "UNBLOCK"))
	{
		if (statusEq == 0)
		{	
			//Duel_SendMessage(ch, m_nDuelTranslate[12]);
			return; // uite
		}	
		else	
			ch->SetQuestFlag(BLOCK_EQUIPMENT_, 0);
			//Duel_SendMessage(ch, m_nDuelTranslate[11]);
	}
}
#endif

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå¿¡ ¼ÓÇØÀÖÁö ¾Ê½À´Ï´Ù."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå ½ºÅ³ ·¹º§À» º¯°æÇÒ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:
#ifdef __7AND8TH_SKILLS__
			case SKILL_ANTI_PALBANG:
			case SKILL_ANTI_AMSEOP:
			case SKILL_ANTI_SWAERYUNG:
			case SKILL_ANTI_YONGBI:
			case SKILL_ANTI_GIGONGCHAM:
			case SKILL_ANTI_HWAJO:
			case SKILL_ANTI_MARYUNG:
			case SKILL_ANTI_BYEURAK:
#ifdef __WOLFMAN_CHARACTER__
			case SKILL_ANTI_SALPOONG:
#endif
			case SKILL_HELP_PALBANG:
			case SKILL_HELP_AMSEOP:
			case SKILL_HELP_SWAERYUNG:
			case SKILL_HELP_YONGBI:
			case SKILL_HELP_GIGONGCHAM:
			case SKILL_HELP_HWAJO:
			case SKILL_HELP_MARYUNG:
			case SKILL_HELP_BYEURAK:
#ifdef __WOLFMAN_CHARACTER__
			case SKILL_HELP_SALPOONG:
#endif
#endif
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
// @version	05/06/20 Bang2ni - Ä¿¸Çµå Ã³¸® Delegate to CHARACTER class
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
// @version	05/06/20 Bang2ni - Ä¿¸Çµå Ã³¸® Delegate to CHARACTER class
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Àß¸øµÈ ¾ÏÈ£¸¦ ÀÔ·ÂÇÏ¼Ì½À´Ï´Ù."));
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Àß¸øµÈ ¾ÏÈ£¸¦ ÀÔ·ÂÇÏ¼Ì½À´Ï´Ù."));
		return;
	}

	if (LC_IsBrazil() == true)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (arg2[i] == '\0')
				break;

			if (isalpha(arg2[i]) == false)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ºñ¹Ð¹øÈ£´Â ¿µ¹®ÀÚ¸¸ °¡´ÉÇÕ´Ï´Ù."));
				return;
			}
		}
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Àß¸øµÈ ¾ÏÈ£¸¦ ÀÔ·ÂÇÏ¼Ì½À´Ï´Ù."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Ã¢°í°¡ ÀÌ¹Ì ¿­·ÁÀÖ½À´Ï´Ù."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10) // 10ÃÊ¿¡ ÇÑ¹ø¸¸ ¿äÃ» °¡´É
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Ã¢°í¸¦ ´ÝÀºÁö 10ÃÊ ¾È¿¡´Â ¿­ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ¼­¹ö ¹®Á¦·Î ÆÄÆ¼ °ü·Ã Ã³¸®¸¦ ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ´øÀü ¾È¿¡¼­´Â ÆÄÆ¼¿¡¼­ ³ª°¥ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ÆÄÆ¼¿¡¼­ ³ª°¡¼Ì½À´Ï´Ù."));
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->IsObserverMode())
		return;
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_savasat)
{
	
	if (!CWarMapManager::instance().IsWarMap(ch->GetMapIndex()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, ("<Guild> Sadece lonca alaninda atabilirsin."));
		return;
	}
	
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	if (!g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, ("<Guild> Sadece lonca savasinda kullanilirsin!"));
		return;
	}
	
	DWORD gm_pid = g->GetMasterPID();

	//¸¶½ºÅÍÀÎÁö Ã¼Å©(±æÀüÀº ±æµåÀå¸¸ÀÌ °¡´É)
	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, ("<Guild> Lonca lideri olmadin icin savastan atamazsin!"));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;
	
	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;
	
	CGuild * g2 = pkVictim->GetGuild();
	
	if (!g2)
		return;
	
	if (!g2->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("sadece_loncada_hac©¥"));
		return;
	}
	
	if (g->GetID() != g2->GetID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("lano_baska_loncada"));
		return;
	}

	if (pkVictim == ch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("mal_o_sensin"));
		return;
	}
	
	//LPDESC d = pkVictim->GetDesc();
	
	//if(d)
	//{
		ch->ChatPacket(CHAT_TYPE_INFO, "<Guild> %s lonca savasindan atildi.", pkVictim->GetName());
		pkVictim->GoHome();
		
	//	d->SetPhase(PHASE_CLOSE);
	//}
}

#if defined(ENABLE_AFFECT_POLYMORPH_REMOVE)
ACMD(do_remove_polymorph)
{
	if (!ch)
		return;
	
	if (!ch->IsPolymorphed())
		return;
	
	ch->SetPolymorph(0);
	ch->RemoveAffect(AFFECT_POLYMORPH);
}
#endif

ACMD(do_war)
{
	//³» ±æµå Á¤º¸¸¦ ¾ò¾î¿À°í
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	//ÀüÀïÁßÀÎÁö Ã¼Å©ÇÑ¹ø!
	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ÀÌ¹Ì ´Ù¸¥ ÀüÀï¿¡ ÂüÀü Áß ÀÔ´Ï´Ù."));
		return;
	}

	//ÆÄ¶ó¸ÞÅÍ¸¦ µÎ¹è·Î ³ª´©°í
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	int type = GUILD_WAR_TYPE_FIELD;
	
	if (!*arg1)
		return;

	if (*arg2)
	{
		str_to_number(type, arg2);

		if (type >= GUILD_WAR_TYPE_MAX_NUM)
			type = GUILD_WAR_TYPE_FIELD;
			
		if(type < 0) //war crash fix
			return;
	}

	//±æµåÀÇ ¸¶½ºÅÍ ¾ÆÀÌµð¸¦ ¾ò¾î¿ÂµÚ
	DWORD gm_pid = g->GetMasterPID();

	//¸¶½ºÅÍÀÎÁö Ã¼Å©(±æÀüÀº ±æµåÀå¸¸ÀÌ °¡´É)
	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀü¿¡ ´ëÇÑ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
		return;
	}

	//»ó´ë ±æµå¸¦ ¾ò¾î¿À°í
	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±×·± ±æµå°¡ ¾ø½À´Ï´Ù."));
		return;
	}

	//»ó´ë±æµå¿ÍÀÇ »óÅÂ Ã¼Å©
	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµå°¡ ÀÌ¹Ì ÀüÀï Áß ÀÔ´Ï´Ù."));
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Àüºñ°¡ ºÎÁ·ÇÏ¿© ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ Àüºñ°¡ ºÎÁ·ÇÏ¿© ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì ¼±ÀüÆ÷°í ÁßÀÎ ±æµåÀÔ´Ï´Ù."));
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµå°¡ ÀÌ¹Ì ÀüÀï Áß ÀÔ´Ï´Ù."));

					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ÀÌ¹Ì ÀüÀïÀÌ ¿¹¾àµÈ ±æµå ÀÔ´Ï´Ù."));
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ÀÌ¹Ì ÀüÀï ÁßÀÎ ±æµåÀÔ´Ï´Ù."));

			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		// ±æµåÀüÀ» ÇÒ ¼ö ÀÖ´Â Á¶°ÇÀ» ¸¸Á·ÇÏÁö¾Ê´Â´Ù.
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ·¹´õ Á¡¼ö°¡ ¸ðÀÚ¶ó¼­ ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀüÀ» ÇÏ±â À§ÇØ¼± ÃÖ¼ÒÇÑ %d¸íÀÌ ÀÖ¾î¾ß ÇÕ´Ï´Ù."), GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	// ÇÊµåÀü Ã¼Å©¸¸ ÇÏ°í ¼¼¼¼ÇÑ Ã¼Å©´Â »ó´ë¹æÀÌ ½Â³«ÇÒ¶§ ÇÑ´Ù.
	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ ·¹´õ Á¡¼ö°¡ ¸ðÀÚ¶ó¼­ ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ ±æµå¿ø ¼ö°¡ ºÎÁ·ÇÏ¿© ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ ±æµåÀåÀÌ Á¢¼ÓÁßÀÌ ¾Æ´Õ´Ï´Ù."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ ±æµåÀåÀÌ Á¢¼ÓÁßÀÌ ¾Æ´Õ´Ï´Ù."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀü¿¡ ´ëÇÑ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±×·± ±æµå°¡ ¾ø½À´Ï´Ù."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesab? kilitliyken pvp moda alamazs??. ");
		return;
	}
#endif

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

//#ifdef TOURNAMENT_PVP_SYSTEM
//	if (CTournamentPvP::instance().IsTournamentMap(ch, TOURNAMENT_BLOCK_CHANGE_PKMODE))
//		return;
//
//	if (mode == PK_MODE_TEAM_A || mode == PK_MODE_TEAM_B)
//		return;
//#endif

#if defined(WJ_COMBAT_ZONE)
	if (CCombatZoneManager::Instance().IsCombatZoneMap(ch->GetMapIndex()))
		return;
#endif

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);

	if (answer != 'y')
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´ÔÀ¸·Î ºÎÅÍ Ä£±¸ µî·ÏÀ» °ÅºÎ ´çÇß½À´Ï´Ù."), ch->GetName());
	}

	MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, answer == 'y' ? false : true); // DENY
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
#ifdef __MOUNT_SYSTEM__
	short sCheck = ch->UnEquipMountUniqueItem();
	if (sCheck == 2)
		ch->Dismount();
	else if (ch->UnEquipSpecialRideUniqueItem())
#else
	if (ch->UnEquipSpecialRideUniqueItem())
#endif
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);
		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
		}
	}
#ifdef __MOUNT_SYSTEM__
	else if (sCheck == 0 && ch->GetPoint(POINT_MOUNT) > 0)
	{
		ch->Dismount();
	}
#endif
}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
#ifdef TOURNAMENT_PVP_SYSTEM
		if (CTournamentPvP::instance().IsTournamentMap(ch, TOURNAMENT_BLOCK_EXIT_OBSERVER_MODE))
			return;
#endif
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else {
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC())
			return;
		/*
		   int iSPCost = ch->GetMaxSP() / 3;

		   if (ch->GetSP() < iSPCost)
		   {
		   ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á¤½Å·ÂÀÌ ºÎÁ·ÇÏ¿© ´Ù¸¥ »ç¶÷ÀÇ Àåºñ¸¦ º¼ ¼ö ¾ø½À´Ï´Ù."));
		   return;
		   }
		   ch->PointChange(POINT_SP, -iSPCost);
		 */
		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
#ifdef TOURNAMENT_PVP_SYSTEM
	if (CTournamentPvP::instance().IsTournamentMap(ch, TOURNAMENT_BLOCK_PARTY))
		return;
#endif
#ifdef ENABLE_BLOCK_NEW_DUEL_SYSTEM_FOR_DOJANG
if ( ch->GetMapIndex() == 115)
    {
        ch->ChatPacket(CHAT_TYPE_INFO,"<Error> Bu islev engellendi !");
        return;
    }
#endif
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì ÆÄÆ¼¿¡ ¼ÓÇØ ÀÖÀ¸¹Ç·Î °¡ÀÔ½ÅÃ»À» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
#ifdef TOURNAMENT_PVP_SYSTEM
	if (CTournamentPvP::instance().IsTournamentMap(ch, TOURNAMENT_BLOCK_PARTY))
		return;
#endif
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

ACMD(do_monarch_warpto)
{
	if (true == LC_IsYMIR() || true == LC_IsKorea())
		return;

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¸¸ÀÌ »ç¿ë °¡´ÉÇÑ ±â´ÉÀÔ´Ï´Ù"));
		return;
	}

	//±ºÁÖ ÄðÅ¸ÀÓ °Ë»ç
	if (!ch->IsMCOK(CHARACTER::MI_WARP))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ÃÊ°£ ÄðÅ¸ÀÓÀÌ Àû¿ëÁßÀÔ´Ï´Ù."), ch->GetMCLTime(CHARACTER::MI_WARP));
		return;
	}

	//±ºÁÖ ¸÷ ¼ÒÈ¯ ºñ¿ë
	const int WarpPrice = 10000;

	//±ºÁÖ ±¹°í °Ë»ç
	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±¹°í¿¡ µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù. ÇöÀç : %u ÇÊ¿ä±Ý¾× : %u"), NationMoney, WarpPrice);
		return;
	}

	int x = 0, y = 0;
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç¿ë¹ý: warpto <character name>"));
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("Å¸Á¦±¹ À¯Àú¿¡°Ô´Â ÀÌµ¿ÇÒ¼ö ¾ø½À´Ï´Ù"));
				return;
			}

			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç À¯Àú´Â %d Ã¤³Î¿¡ ÀÖ½À´Ï´Ù. (ÇöÀç Ã¤³Î %d)"), pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return;
			}

			PIXEL_POSITION pos;

			if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
			else
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, "You warp to (%d, %d)", pos.x, pos.y);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ¿¡°Ô·Î ÀÌµ¿ÇÕ´Ï´Ù"), arg1);
				ch->WarpSet(pos.x, pos.y);

				//±ºÁÖ µ· »è°¨
				CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

				//ÄðÅ¸ÀÓ ÃÊ±âÈ­
				ch->SetMC(CHARACTER::MI_WARP);
			}
		}
		else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
		}

		return;
	}
	else
	{
		if (tch->GetEmpire() != ch->GetEmpire())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Å¸Á¦±¹ À¯Àú¿¡°Ô´Â ÀÌµ¿ÇÒ¼ö ¾ø½À´Ï´Ù"));
			return;
		}
		if (!IsMonarchWarpZone(tch->GetMapIndex()))
		{
			ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return;
		}
		x = tch->GetX();
		y = tch->GetY();
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ¿¡°Ô·Î ÀÌµ¿ÇÕ´Ï´Ù"), arg1);
	ch->WarpSet(x, y);
	ch->Stop();

	//±ºÁÖ µ· »è°¨
	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	//ÄðÅ¸ÀÓ ÃÊ±âÈ­
	ch->SetMC(CHARACTER::MI_WARP);
}

ACMD(do_monarch_transfer)
{
	if (true == LC_IsYMIR() || true == LC_IsKorea())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç¿ë¹ý: transfer <name>"));
		return;
	}

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¸¸ÀÌ »ç¿ë °¡´ÉÇÑ ±â´ÉÀÔ´Ï´Ù"));
		return;
	}

	//±ºÁÖ ÄðÅ¸ÀÓ °Ë»ç
	if (!ch->IsMCOK(CHARACTER::MI_TRANSFER))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ÃÊ°£ ÄðÅ¸ÀÓÀÌ Àû¿ëÁßÀÔ´Ï´Ù."), ch->GetMCLTime(CHARACTER::MI_TRANSFER));
		return;
	}

	//±ºÁÖ ¿öÇÁ ºñ¿ë
	const int WarpPrice = 10000;

	//±ºÁÖ ±¹°í °Ë»ç
	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±¹°í¿¡ µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù. ÇöÀç : %u ÇÊ¿ä±Ý¾× : %u"), NationMoney, WarpPrice);
		return;
	}


	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ Á¦±¹ À¯Àú´Â ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return;
			}
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´ÔÀº %d Ã¤³Î¿¡ Á¢¼Ó Áß ÀÔ´Ï´Ù. (ÇöÀç Ã¤³Î: %d)"), arg1, pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return;
			}
			if (!IsMonarchWarpZone(ch->GetMapIndex()))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´ÔÀ» ¼ÒÈ¯ÇÏ¿´½À´Ï´Ù."), arg1);

			//±ºÁÖ µ· »è°¨
			CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);
			//ÄðÅ¸ÀÓ ÃÊ±âÈ­
			ch->SetMC(CHARACTER::MI_TRANSFER);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÔ·ÂÇÏ½Å ÀÌ¸§À» °¡Áø »ç¿ëÀÚ°¡ ¾ø½À´Ï´Ù."));
		}

		return;
	}


	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÚ½ÅÀ» ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (tch->GetEmpire() != ch->GetEmpire())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ Á¦±¹ À¯Àú´Â ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	if (!IsMonarchWarpZone(tch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	if (!IsMonarchWarpZone(ch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	//tch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());

	//±ºÁÖ µ· »è°¨
	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);
	//ÄðÅ¸ÀÓ ÃÊ±âÈ­
	ch->SetMC(CHARACTER::MI_TRANSFER);
}

ACMD(do_monarch_info)
{
	if (CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("³ªÀÇ ±ºÁÖ Á¤º¸"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			if (n == ch->GetEmpire())
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s±ºÁÖ] : %s  º¸À¯±Ý¾× %lld "), EMPIRE_NAME(n), p->name[n], p->money[n]);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s±ºÁÖ] : %s  "), EMPIRE_NAME(n), p->name[n]);

		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ Á¤º¸"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s±ºÁÖ] : %s  "), EMPIRE_NAME(n), p->name[n]);

		}
	}

}

ACMD(do_elect)
{
	db_clientdesc->DBPacketHeader(HEADER_GD_COME_TO_VOTE, ch->GetDesc()->GetHandle(), 0);
}

ACMD(do_lonca_gecmis_history_req)
{
	if (!ch || !ch->GetGuild() || ch->GetGuild()->GetMasterPID() != ch->GetPlayerID())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	char szEscapedName[1024];
	DBManager::instance().EscapeString(szEscapedName, sizeof(szEscapedName), arg1, strlen(arg1));


	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery), "SELECT guild, time, leader FROM player.lonca_gecmis WHERE name = '%s' ORDER BY time DESC", szEscapedName);
	std::unique_ptr<SQLMsg> msg_(DBManager::instance().DirectQuery(szQuery));

	if (msg_->Get()->uiNumRows == 0)
	{
		// ch->ChatPacket(CHAT_TYPE_INFO, "The character has never joined to any guild!");
		return;
	}

	while (MYSQL_ROW row1 = mysql_fetch_row(msg_->Get()->pSQLResult))
		ch->ChatPacket(CHAT_TYPE_COMMAND, "AppendLoncaGecmisi %s %s %s", row1[0], row1[1], row1[2]);
	
	ch->ChatPacket(CHAT_TYPE_INFO, "History list completed!");
}


// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

#ifndef __FULL_NOTICE_SYSTEM__
extern void BroadcastNotice(const char * c_pszBuf);
#endif


ACMD(do_monarch_tax)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: monarch_tax <1-50>");
		return;
	}

	// ±ºÁÖ °Ë»ç
	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¸¸ÀÌ »ç¿ëÇÒ¼ö ÀÖ´Â ±â´ÉÀÔ´Ï´Ù"));
		return;
	}

	// ¼¼±Ý¼³Á¤
	int tax = 0;
	str_to_number(tax,  arg1);

	if (tax < 1 || tax > 50)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1-50 »çÀÌÀÇ ¼öÄ¡¸¦ ¼±ÅÃÇØÁÖ¼¼¿ä"));

	quest::CQuestManager::instance().SetEventFlag("trade_tax", tax);

	// ±ºÁÖ¿¡°Ô ¸Þ¼¼Áö ÇÏ³ª
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼¼±ÝÀÌ %d %·Î ¼³Á¤µÇ¾ú½À´Ï´Ù"));

	// °øÁö
	char szMsg[1024];

	snprintf(szMsg, sizeof(szMsg), "±ºÁÖÀÇ ¸íÀ¸·Î ¼¼±ÝÀÌ %d %% ·Î º¯°æµÇ¾ú½À´Ï´Ù", tax);
	BroadcastNotice(szMsg);

	snprintf(szMsg, sizeof(szMsg), "¾ÕÀ¸·Î´Â °Å·¡ ±Ý¾×ÀÇ %d %% °¡ ±¹°í·Î µé¾î°¡°ÔµË´Ï´Ù.", tax);
	BroadcastNotice(szMsg);

	// ÄðÅ¸ÀÓ ÃÊ±âÈ­
	ch->SetMC(CHARACTER::MI_TAX);
}

static const DWORD cs_dwMonarchMobVnums[] =
{
	0,
};

ACMD(do_monarch_mob)
{
	char arg1[256];
	LPCHARACTER	tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¸¸ÀÌ »ç¿ëÇÒ¼ö ÀÖ´Â ±â´ÉÀÔ´Ï´Ù"));
		return;
	}

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mmob <mob name>");
		return;
	}

	BYTE pcEmpire = ch->GetEmpire();
	BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(ch->GetMapIndex());

	if (LC_IsYMIR() == true || LC_IsKorea() == true)
	{
		if (mapEmpire != pcEmpire && mapEmpire != 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÚ±¹ ¿µÅä¿¡¼­¸¸ »ç¿ëÇÒ ¼ö ÀÖ´Â ±â´ÉÀÔ´Ï´Ù"));
			return;
		}
	}

	// ±ºÁÖ ¸÷ ¼ÒÈ¯ ºñ¿ë
	const int SummonPrice = 5000000;

	// ±ºÁÖ ÄðÅ¸ÀÓ °Ë»ç
	if (!ch->IsMCOK(CHARACTER::MI_SUMMON))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ÃÊ°£ ÄðÅ¸ÀÓÀÌ Àû¿ëÁßÀÔ´Ï´Ù."), ch->GetMCLTime(CHARACTER::MI_SUMMON));
		return;
	}

	// ±ºÁÖ ±¹°í °Ë»ç
	if (!CMonarch::instance().IsMoneyOk(SummonPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±¹°í¿¡ µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù. ÇöÀç : %u ÇÊ¿ä±Ý¾× : %u"), NationMoney, SummonPrice);
		return;
	}

	const CMob * pkMob;
	DWORD vnum = 0;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	DWORD count;

	// ¼ÒÈ¯ °¡´É ¸÷ °Ë»ç
	for (count = 0; cs_dwMonarchMobVnums[count] != 0; ++count)
		if (cs_dwMonarchMobVnums[count] == vnum)
			break;

	if (0 == cs_dwMonarchMobVnums[count])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÈ¯ÇÒ¼ö ¾ø´Â ¸ó½ºÅÍ ÀÔ´Ï´Ù. ¼ÒÈ¯°¡´ÉÇÑ ¸ó½ºÅÍ´Â È¨ÆäÀÌÁö¸¦ ÂüÁ¶ÇÏ¼¼¿ä"));
		return;
	}

	tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE,
			true);

	if (tch)
	{
		// ±ºÁÖ µ· »è°¨
		CMonarch::instance().SendtoDBDecMoney(SummonPrice, ch->GetEmpire(), ch);

		// ÄðÅ¸ÀÓ ÃÊ±âÈ­
		ch->SetMC(CHARACTER::MI_SUMMON);
	}
}

static const char* FN_point_string(int apply_number)
{
	switch (apply_number)
	{
		case POINT_MAX_HP:
			return LC_TEXT("ÃÖ´ë »ý¸í·Â +%d");

		case POINT_MAX_SP:
			return LC_TEXT("ÃÖ´ë Á¤½Å·Â +%d");

		case POINT_HT:
			return LC_TEXT("Ã¼·Â +%d");

		case POINT_IQ:
			return LC_TEXT("Áö´É +%d");

		case POINT_ST:
			return LC_TEXT("±Ù·Â +%d");

		case POINT_DX:
			return LC_TEXT("¹ÎÃ¸ +%d");

		case POINT_ATT_SPEED:
			return LC_TEXT("°ø°Ý¼Óµµ +%d");

		case POINT_MOV_SPEED:
			return LC_TEXT("ÀÌµ¿¼Óµµ %d");

		case POINT_CASTING_SPEED:
			return LC_TEXT("ÄðÅ¸ÀÓ -%d");

		case POINT_HP_REGEN:
			return LC_TEXT("»ý¸í·Â È¸º¹ +%d");

		case POINT_SP_REGEN:
			return LC_TEXT("Á¤½Å·Â È¸º¹ +%d");

		case POINT_POISON_PCT:
			return LC_TEXT("µ¶°ø°Ý %d");

		case POINT_BLEEDING_PCT:
			return LC_TEXT("Bleed pct %d%%");

		case POINT_STUN_PCT:
			return LC_TEXT("½ºÅÏ +%d");

		case POINT_SLOW_PCT:
			return LC_TEXT("½½·Î¿ì +%d");

		case POINT_CRITICAL_PCT:
			return LC_TEXT("%d%% È®·ü·Î Ä¡¸íÅ¸ °ø°Ý");

		case POINT_RESIST_CRITICAL:
			return LC_TEXT("»ó´ëÀÇ Ä¡¸íÅ¸ È®·ü %d%% °¨¼Ò");

		case POINT_PENETRATE_PCT:
			return LC_TEXT("%d%% È®·ü·Î °üÅë °ø°Ý");

		case POINT_RESIST_PENETRATE:
			return LC_TEXT("»ó´ëÀÇ °üÅë °ø°Ý È®·ü %d%% °¨¼Ò");

		case POINT_ATTBONUS_HUMAN:
			return LC_TEXT("ÀÎ°£·ù ¸ó½ºÅÍ Å¸°ÝÄ¡ +%d%%");

		case POINT_ATTBONUS_ANIMAL:
			return LC_TEXT("µ¿¹°·ù ¸ó½ºÅÍ Å¸°ÝÄ¡ +%d%%");

		case POINT_ATTBONUS_ORC:
			return LC_TEXT("¿õ±ÍÁ· Å¸°ÝÄ¡ +%d%%");

		case POINT_ATTBONUS_MILGYO:
			return LC_TEXT("¹Ð±³·ù Å¸°ÝÄ¡ +%d%%");

		case POINT_ATTBONUS_UNDEAD:
			return LC_TEXT("½ÃÃ¼·ù Å¸°ÝÄ¡ +%d%%");

		case POINT_ATTBONUS_DEVIL:
			return LC_TEXT("¾Ç¸¶·ù Å¸°ÝÄ¡ +%d%%");

		case POINT_STEAL_HP:
			return LC_TEXT("Å¸°ÝÄ¡ %d%% ¸¦ »ý¸í·ÂÀ¸·Î Èí¼ö");

		case POINT_STEAL_SP:
			return LC_TEXT("Å¸·ÂÄ¡ %d%% ¸¦ Á¤½Å·ÂÀ¸·Î Èí¼ö");

		case POINT_MANA_BURN_PCT:
			return LC_TEXT("%d%% È®·ü·Î Å¸°Ý½Ã »ó´ë Àü½Å·Â ¼Ò¸ð");

		case POINT_DAMAGE_SP_RECOVER:
			return LC_TEXT("%d%% È®·ü·Î ÇÇÇØ½Ã Á¤½Å·Â È¸º¹");

		case POINT_BLOCK:
			return LC_TEXT("¹°¸®Å¸°Ý½Ã ºí·° È®·ü %d%%");

		case POINT_DODGE:
			return LC_TEXT("È° °ø°Ý È¸ÇÇ È®·ü %d%%");

		case POINT_RESIST_SWORD:
			return LC_TEXT("ÇÑ¼Õ°Ë ¹æ¾î %d%%");

		case POINT_RESIST_TWOHAND:
			return LC_TEXT("¾ç¼Õ°Ë ¹æ¾î %d%%");

		case POINT_RESIST_DAGGER:
			return LC_TEXT("µÎ¼Õ°Ë ¹æ¾î %d%%");

		case POINT_RESIST_BELL:
			return LC_TEXT("¹æ¿ï ¹æ¾î %d%%");

		case POINT_RESIST_FAN:
			return LC_TEXT("ºÎÃ¤ ¹æ¾î %d%%");

		case POINT_RESIST_BOW:
			return LC_TEXT("È°°ø°Ý ÀúÇ× %d%%");

#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:
			return LC_TEXT("Defense chance against claws +%d%%");
#endif

		case POINT_RESIST_FIRE:
			return LC_TEXT("È­¿° ÀúÇ× %d%%");

		case POINT_RESIST_ELEC:
			return LC_TEXT("Àü±â ÀúÇ× %d%%");

		case POINT_RESIST_MAGIC:
			return LC_TEXT("¸¶¹ý ÀúÇ× %d%%");

		case POINT_RESIST_WIND:
			return LC_TEXT("¹Ù¶÷ ÀúÇ× %d%%");

		case POINT_RESIST_ICE:
			return LC_TEXT("³Ã±â ÀúÇ× %d%%");

		case POINT_RESIST_EARTH:
			return LC_TEXT("´ëÁö ÀúÇ× %d%%");

		case POINT_RESIST_DARK:
			return LC_TEXT("¾îµÒ ÀúÇ× %d%%");

		case POINT_REFLECT_MELEE:
			return LC_TEXT("Á÷Á¢ Å¸°ÝÄ¡ ¹Ý»ç È®·ü : %d%%");

		case POINT_REFLECT_CURSE:
			return LC_TEXT("ÀúÁÖ µÇµ¹¸®±â È®·ü %d%%");

		case POINT_POISON_REDUCE:
			return LC_TEXT("µ¶ ÀúÇ× %d%%");

#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_REDUCE:
			return LC_TEXT("Bleed reduce %d%%");
#endif

		case POINT_KILL_SP_RECOVER:
			return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã Á¤½Å·Â È¸º¹");

		case POINT_EXP_DOUBLE_BONUS:
			return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã °æÇèÄ¡ Ãß°¡ »ó½Â");

		case POINT_GOLD_DOUBLE_BONUS:
			return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã µ· 2¹è µå·Ó");

		case POINT_ITEM_DROP_BONUS:
			return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã ¾ÆÀÌÅÛ 2¹è µå·Ó");

		case POINT_POTION_BONUS:
			return LC_TEXT("¹°¾à »ç¿ë½Ã %d%% ¼º´É Áõ°¡");

		case POINT_KILL_HP_RECOVERY:
			return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã »ý¸í·Â È¸º¹");

		case POINT_ATT_GRADE_BONUS:
			return LC_TEXT("°ø°Ý·Â +%d");

		case POINT_DEF_GRADE_BONUS:
			return LC_TEXT("¹æ¾î·Â +%d");

		case POINT_MAGIC_ATT_GRADE:
			return LC_TEXT("¸¶¹ý °ø°Ý·Â +%d");

		case POINT_MAGIC_DEF_GRADE:
			return LC_TEXT("¸¶¹ý ¹æ¾î·Â +%d");

		case POINT_MAX_STAMINA:
			return LC_TEXT("ÃÖ´ë Áö±¸·Â +%d");

		case POINT_ATTBONUS_WARRIOR:
			return LC_TEXT("¹«»ç¿¡°Ô °­ÇÔ +%d%%");

		case POINT_ATTBONUS_ASSASSIN:
			return LC_TEXT("ÀÚ°´¿¡°Ô °­ÇÔ +%d%%");

		case POINT_ATTBONUS_SURA:
			return LC_TEXT("¼ö¶ó¿¡°Ô °­ÇÔ +%d%%");

		case POINT_ATTBONUS_SHAMAN:
			return LC_TEXT("¹«´ç¿¡°Ô °­ÇÔ +%d%%");

#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:
			return LC_TEXT("Strong against Lycans +%d%%");
#endif

		case POINT_ATTBONUS_MONSTER:
			return LC_TEXT("¸ó½ºÅÍ¿¡°Ô °­ÇÔ +%d%%");

		case POINT_MALL_ATTBONUS:
			return LC_TEXT("°ø°Ý·Â +%d%%");

		case POINT_MALL_DEFBONUS:
			return LC_TEXT("¹æ¾î·Â +%d%%");

		case POINT_MALL_EXPBONUS:
			return LC_TEXT("°æÇèÄ¡ %d%%");

		case POINT_MALL_ITEMBONUS:
			return LC_TEXT("¾ÆÀÌÅÛ µå·ÓÀ² %.1f¹è");

		case POINT_MALL_GOLDBONUS:
			return LC_TEXT("µ· µå·ÓÀ² %.1f¹è");

		case POINT_MAX_HP_PCT:
			return LC_TEXT("ÃÖ´ë »ý¸í·Â +%d%%");

		case POINT_MAX_SP_PCT:
			return LC_TEXT("ÃÖ´ë Á¤½Å·Â +%d%%");

		case POINT_SKILL_DAMAGE_BONUS:
			return LC_TEXT("½ºÅ³ µ¥¹ÌÁö %d%%");

		case POINT_NORMAL_HIT_DAMAGE_BONUS:
			return LC_TEXT("ÆòÅ¸ µ¥¹ÌÁö %d%%");

		case POINT_SKILL_DEFEND_BONUS:
			return LC_TEXT("½ºÅ³ µ¥¹ÌÁö ÀúÇ× %d%%");

		case POINT_NORMAL_HIT_DEFEND_BONUS:
			return LC_TEXT("ÆòÅ¸ µ¥¹ÌÁö ÀúÇ× %d%%");

		case POINT_RESIST_WARRIOR:
			return LC_TEXT("¹«»ç°ø°Ý¿¡ %d%% ÀúÇ×");

		case POINT_RESIST_ASSASSIN:
			return LC_TEXT("ÀÚ°´°ø°Ý¿¡ %d%% ÀúÇ×");

		case POINT_RESIST_SURA:
			return LC_TEXT("¼ö¶ó°ø°Ý¿¡ %d%% ÀúÇ×");

		case POINT_RESIST_SHAMAN:
			return LC_TEXT("¹«´ç°ø°Ý¿¡ %d%% ÀúÇ×");

#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN:
			return LC_TEXT("Defense chance against Lycan attacks %d%%");
#endif
#ifdef __ANTI_RESIST_MAGIC_BONUS__
		case POINT_ANTI_RESIST_MAGIC:
			return LC_TEXT("Anti Magic Resistance: %d%%");
#endif
#ifdef __ENABLE_BONUS_STRONG_AGAINST_WEAPON__
		case POINT_ATTBONUS_SWORD:	return LC_TEXT("Sword resistance: %d%%");
		case POINT_ATTBONUS_TWOHANDED:	return LC_TEXT("Two Hand resistance: %d%%");
		case POINT_ATTBONUS_DAGGER:	return LC_TEXT("Dagger resistance: %d%%");
		case POINT_ATTBONUS_BELL:	return LC_TEXT("Bell resistance: %d%%");
		case POINT_ATTBONUS_FAN:	return LC_TEXT("Fan resistance: %d%%");
		case POINT_ATTBONUS_BOW:	return LC_TEXT("Bow resistance: %d%%");
#endif
		case POINT_INVINCIBLE:		return LC_TEXT("Invensibilidad: %d%%");
		default:
			return NULL;
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char *buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
		return false;

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int	year, mon, day;
	int	offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
		return false;

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
		return false;

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->bApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int) bufsiz)
		offset = bufsiz - 1;

	localtime_r(&expire, &ltm);

	year	= ltm.tm_year + 1900;
	mon		= ltm.tm_mon + 1;
	day		= ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, LC_TEXT(" (¸¸·áÀÏ : %d³â %d¿ù %dÀÏ)"), year, mon, day);

	return true;
}

ACMD(do_costume)
{
#ifdef __SASH_SYSTEM__
	char buf[768];
#else
	char buf[512];
#endif
	const size_t bufferSize = sizeof(buf);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CItem* pBody = ch->GetWear(WEAR_COSTUME_BODY);
	CItem* pHair = ch->GetWear(WEAR_COSTUME_HAIR);
#ifdef __SASH_SYSTEM__
	CItem * pSash = ch->GetWear(WEAR_COSTUME_SASH);
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "COSTUME status:");
	if (pBody)
	{
		const char* itemName = pBody->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  BODY : %s", itemName);

		if (pBody->IsEquipped() && arg1[0] == 'b')
			ch->UnequipItem(pBody);
	}

	if (pHair)
	{
		const char* itemName = pHair->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  HAIR : %s", itemName);

		for (int i = 0; i < pHair->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pHair->GetAttribute(i);
			if (0 < attr.bType)
			{
				snprintf(buf, bufferSize, FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pHair->IsEquipped() && arg1[0] == 'h')
			ch->UnequipItem(pHair);
	}

#ifdef __SASH_SYSTEM__
	if (pSash)
	{
		const char * itemName = pSash->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  SASH: %s", itemName);
		for (int i = 0; i < pSash->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pSash->GetAttribute(i);
			if (attr.bType > 0)
			{
				const char * pAttrName = FN_point_string(attr.bType);
				if (pAttrName == NULL)
					continue;

				snprintf(buf, sizeof(buf), FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pSash->IsEquipped() && arg1[0] == 's')
			ch->UnequipItem(pSash);
	}
#endif
}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s",
						index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

#ifdef WJ_ITEM_COMBINATION_SYSTEM
ACMD(do_item_comb)
{
	if (!ch->CanDoComb())
		return;

	dev_log(LOG_DEB0, "item_comb command <%s>: %s", ch->GetName(), argument);

	int combination_index = 0, inven_index = 0;
	const char *line;

	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: item_comb open_costume");
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: item_comb open_skillbook");
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: item_comb close");
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: item_comb add_item <inveltory_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: item_comb del_item <combination_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: item_comb make_cotume");
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: item_comb make_skillbook");
		return;
	}

	const std::string& strArg1 = std::string(arg1);
	if (strArg1 == "open_costume")
	{
		Comb_open_costume(ch);
		return;
	}
	else if (strArg1 == "open_skillbook")
	{
		Comb_open_skillbook(ch);
		return;
	}
	else if (strArg1 == "add_item")
	{
		if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
			return;
		str_to_number(combination_index, arg2);
		str_to_number(inven_index, arg3);
		Comb_add_item(ch, combination_index, inven_index);
	}
	else if (strArg1 == "add_item2")
	{
		if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
			return;
		str_to_number(combination_index, arg2);
		str_to_number(inven_index, arg3);
		Comb_add_item2(ch, combination_index, inven_index);
	}
	else if (strArg1 == "del_item")
	{
		if (0 == arg2[0] || !isdigit(*arg2))
			return;
		str_to_number(combination_index, arg2);
		Comb_del_item(ch, combination_index);
	}
	else if (strArg1 == "make_cotume")
	{
		Comb_make_cotume(ch);
		return;
	}
	else if (strArg1 == "make_skillbook")
	{
		Comb_make_skillbook(ch);
		return;
	}
	else if (strArg1 == "close")
	{
		Comb_close(ch);
		return;
	}
}
#endif

ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

#ifdef ENABLE_GROWTH_PET_SYSTEM
ACMD(do_CubePetAdd) {

	int pos = 0;
	int invpos = 0;

	const char *line;
	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
		return;
	const std::string& strArg1 = std::string(arg1);
	switch (LOWER(arg1[0]))
	{
	case 'a':	// add cue_index inven_index
	{

		if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
			return;


		str_to_number(pos, arg2);
		str_to_number(invpos, arg3);

	}
	break;

	default:
		return;
	}

	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->SetItemCube(pos, invpos);
	else
		return;

}

ACMD(do_PetSkill) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	DWORD skillslot = 0;
	str_to_number(skillslot, arg1);
	if (skillslot > 2 || skillslot < 0)
		return;

	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->DoPetSkill(skillslot);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_IF_USE_SKILL_SUMMON_PET"));
}

ACMD(do_FeedCubePet) {
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	DWORD feedtype = 0;
	str_to_number(feedtype, arg1);
	if (ch->GetNewPetSystem()->IsActivePet())
		ch->GetNewPetSystem()->ItemCubeFeed(feedtype);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_IF_YOU_FEED_PET_SUMMON_PET"));
}

ACMD(do_PetEvo) {

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->IsCombOpen() || ch->GetOfflineShopOwner()) 
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_NOW_NOT_EVO_YOUR_PET"));
		return;
	}
	if (ch->GetNewPetSystem()->IsActivePet()) {

		int it[3][1] = { 
						{ 55005 }, //Ýþte 1 evrim iste?nde bulunmak i?n öðeleri de?þtirin
						{ 55006 }, //Ýþte 2 evrim iste?nde bulunmak i?n öðeleri de?þtirin
						{ 55007 }  //Ýþte 3 evrim iste?nde bulunmak i?n öðeleri de?þtirin
		};
		int ic[3][1] = {{ 10 },
						{ 20 },
						{ 30 }
		};
		int tmpevo = ch->GetNewPetSystem()->GetEvolution();

		if (ch->GetNewPetSystem()->GetLevel() == 40 && tmpevo == 0 ||
			ch->GetNewPetSystem()->GetLevel() == 60 && tmpevo == 1 ||
			ch->GetNewPetSystem()->GetLevel() == 80 && tmpevo == 2) {


			for (int b = 0; b < 1; b++) {
				if (ch->CountSpecifyItem(it[tmpevo][b]) < ic[tmpevo][b]) {
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_ITEMS_REQUIRED"));
					for (int c = 0; c < 1; c++) {
						DWORD vnum = it[tmpevo][c];
						ch->ChatPacket(CHAT_TYPE_INFO, "%s X%d", ITEM_MANAGER::instance().GetTable(vnum)->szLocaleName , ic[tmpevo][c]);
					}
					return;
				}
			}
			for (int c = 0; c < 1; c++) {
				ch->RemoveSpecifyItem(it[tmpevo][c], ic[tmpevo][c]);
			}
			ch->GetNewPetSystem()->IncreasePetEvolution();

		}
		else {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_YOU_CANT_EVO_PET_MORE"));
			return;
		}

	}else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_IF_YOU_EVO_PET_SUMMON"));

}

ACMD(do_PetChangeName)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	
	if (!*arg1 || !*arg2)
		return;
	
	int bCell = 0;
	str_to_number(bCell, arg1);
	LPITEM item = ch->GetInventoryItem(bCell);
	if (!item)
		return;
	if (ch->CountSpecifyItem(55030) < 1)
		return;
	
	if (ch->GetNewPetSystem()->IsActivePet())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ONCE_PETINI_GONDER"));
		return ;
	}
	
	DBManager::instance().DirectQuery("UPDATE new_petsystem SET name = '%s' WHERE id = '%d'", arg2, item->GetID());
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_ISMI_DEGISTIRILDI."));
	ch->RemoveSpecifyItem(55030, 1);
}

#endif

#ifdef ENABLE_OKAY_CARD
ACMD(do_cards)
{
	const char *line;
	char arg1[256], arg2[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	switch (LOWER(arg1[0]))
	{
		case 'o':    // open
			if (isdigit(*arg2))
			{
				DWORD safemode = NULL;
				str_to_number(safemode, arg2);
				ch->Cards_open(safemode);
			}
			break;
		case 'p':    // open
			ch->Cards_pullout();
			break;
		case 'e':    // open
			ch->CardsEnd();
			break;
		case 'd':    // open
			if (isdigit(*arg2))
			{
				DWORD destroy_index = NULL;
				str_to_number(destroy_index, arg2);
				ch->CardsDestroy(destroy_index);
			}
			break;
		case 'a':    // open
			if (isdigit(*arg2))
			{
				DWORD accpet_index = NULL;
				str_to_number(accpet_index, arg2);
				ch->CardsAccept(accpet_index);
			}
			break;
		case 'r':    // open
			if (isdigit(*arg2))
			{
				DWORD restore_index = NULL;
				str_to_number(restore_index, arg2);
				ch->CardsRestore(restore_index);
			}
			break;
		default:
			return;
	}
}
#endif

#ifdef __NEW_ENCHANT_ATTR__
ACMD(do_enchant_new)
{
	if (!ch)
		return;
	
	if (ch->GetEnchantAttr() == 5555)
		return;
	
	LPITEM itemTarget = ch->GetInventoryItem(ch->GetEnchantAttr());
	if (itemTarget == NULL)
		return;
	
	ch->SetEnchantAttr(5555);
	itemTarget->SetForceAttributeNewEnchant(0, ch->GetEnchantType1(), ch->GetEnchantValue1());
	itemTarget->SetForceAttributeNewEnchant(1, ch->GetEnchantType2(), ch->GetEnchantValue2());
	itemTarget->SetForceAttributeNewEnchant(2, ch->GetEnchantType3(), ch->GetEnchantValue3());
	itemTarget->SetForceAttributeNewEnchant(3, ch->GetEnchantType4(), ch->GetEnchantValue4());
	itemTarget->SetForceAttributeNewEnchant(4, ch->GetEnchantType5(), ch->GetEnchantValue5());
	itemTarget->Lock(false);
	
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You choose to take the new bonuses."));
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%u", itemTarget->GetID());
		LogManager::instance().ItemLog(ch, itemTarget, "CHANGE_ATTRIBUTE3 ANSWER_YES", buf);
	}
	
	ch->Save();
}

ACMD(do_enchant_old)
{
	if (!ch)
		return;
	
	if (ch->GetEnchantAttr() == 5555)
		return;
	
	LPITEM itemTarget = ch->GetInventoryItem(ch->GetEnchantAttr());
	if (itemTarget == NULL)
		return;
	
	ch->SetEnchantAttr(5555);
	itemTarget->Lock(false);
	
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You choose to take the old bonuses."));
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%u", itemTarget->GetID());
		LogManager::instance().ItemLog(ch, itemTarget, "CHANGE_ATTRIBUTE3 ANSWER_NO", buf);
	}
}
#endif
#ifdef __NEW_ENCHANT2_ATTR__
ACMD(do_enchant2)
{
	char arg2[256];
	one_argument(argument, arg2, sizeof(arg2));
	if (!*arg2)
		return;
	
	if (!ch)
		return;
	
	if (ch->GetEnchant2Slot() == 5555 && ch->GetEnchant2Attr() == 5555)
		return;
	
	LPITEM item = ch->GetInventoryItem(ch->GetEnchant2Slot());
	if (item == NULL)
		return;
	
	LPITEM itemTarget = ch->GetInventoryItem(ch->GetEnchant2Attr());
	if (itemTarget == NULL)
		return;

	BYTE iBonusNumber = 0;
	str_to_number(iBonusNumber, arg2);
	int bType = itemTarget->GetAttributeType(iBonusNumber);
	int iValue = itemTarget->GetAttributeValue(iBonusNumber);

	item->Lock(false);
	ch->SetEnchant2Slot(5555);
	ch->SetEnchant2Attr(5555);

	if (iBonusNumber == 0 && bType == 71)//UNBUG DOBLE-SKILLBONUS
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Imposible Bloquear este Bonus! Intentalo Nuevamente"));
		return;
	}
	item->SetCount(item->GetCount() - 1);

	itemTarget->ChangeAttributeExcept(bType);
	itemTarget->SetForceAttributeNewEnchant(iBonusNumber, bType, iValue);
	itemTarget->Lock(false);
	
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÏ¿´½À´Ï´Ù."));
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%u", itemTarget->GetID());
		LogManager::instance().ItemLog(ch, itemTarget, "CHANGE_ATTRIBUTE4 ANSWER_YES", buf);
	}
	
	ch->Save();
}

ACMD(do_enchant2_close)
{
	if (!ch)
		return;
	
	if (ch->GetEnchant2Attr() == 5555 && ch->GetEnchant2Slot() == 5555)
		return;
	
	LPITEM item = ch->GetInventoryItem(ch->GetEnchant2Slot());
	if (item == NULL)
		return;
	
	LPITEM itemTarget = ch->GetInventoryItem(ch->GetEnchant2Attr());
	if (itemTarget == NULL)
		return;
	
	item->Lock(false);
	itemTarget->Lock(false);
	ch->SetEnchant2Slot(5555);
	ch->SetEnchant2Attr(5555);
	
	{
		char buf[128];
		snprintf(buf, sizeof(buf), "%u", itemTarget->GetID());
		LogManager::instance().ItemLog(ch, itemTarget, "CHANGE_ATTRIBUTE4 ANSWER_NO", buf);
	}
	
	ch->Save();
}
#endif
ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	dev_log(LOG_DEB0, "CUBE COMMAND <%s>: %s", ch->GetName(), argument);
	int cube_index = 0, inven_index = 0;

	const char *line;
	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));


	if (0 == arg1[0])
	{
		// print usage
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: cube open");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube close");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube add <inveltory_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube delete <cube_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube list");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube cancel");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube make [all]");
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	// r_info (request information)
	// /cube r_info     ==> (Client -> Server) ÇöÀç NPC°¡ ¸¸µé ¼ö ÀÖ´Â ·¹½ÃÇÇ ¿äÃ»
	//					    (Server -> Client) /cube r_list npcVNUM resultCOUNT 123,1/125,1/128,1/130,5
	//
	// /cube r_info 3   ==> (Client -> Server) ÇöÀç NPC°¡ ¸¸µé¼ö ÀÖ´Â ·¹½ÃÇÇ Áß 3¹øÂ° ¾ÆÀÌÅÛÀ» ¸¸µå´Â µ¥ ÇÊ¿äÇÑ Á¤º¸¸¦ ¿äÃ»
	// /cube r_info 3 5 ==> (Client -> Server) ÇöÀç NPC°¡ ¸¸µé¼ö ÀÖ´Â ·¹½ÃÇÇ Áß 3¹øÂ° ¾ÆÀÌÅÛºÎÅÍ ÀÌÈÄ 5°³ÀÇ ¾ÆÀÌÅÛÀ» ¸¸µå´Â µ¥ ÇÊ¿äÇÑ Àç·á Á¤º¸¸¦ ¿äÃ»
	//					   (Server -> Client) /cube m_info startIndex count 125,1|126,2|127,2|123,5&555,5&555,4/120000@125,1|126,2|127,2|123,5&555,5&555,4/120000
	//
	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		case 'c':	// close
			Cube_close(ch);
			break;

		case 'l':	// list
			Cube_show_list(ch);
			break;

		case 'a':	// add cue_index inven_index
			{

				if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
					return;


				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);

				Cube_add_item (ch, cube_index, inven_index);

			}
			break;

		case 'd':	// delete
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':	// make
			if (0 != arg2[0])
			{
				while (true == Cube_make(ch))
					dev_log (LOG_DEB0, "cube make success");
			}
			else
				Cube_make(ch);
			break;

		default:
			return;
	}
}


ACMD(do_in_game_mall)
{
	if (LC_IsEurope() == true)
	{
		char country_code[3];

		switch (LC_GetLocalType())
		{
			case LC_GERMANY:	country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_FRANCE:		country_code[0] = 'f'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_ITALY:		country_code[0] = 'i'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_SPAIN:		country_code[0] = 'e'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_UK:			country_code[0] = 'e'; country_code[1] = 'n'; country_code[2] = '\0'; break;
			case LC_TURKEY:		country_code[0] = 't'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_POLAND:		country_code[0] = 'p'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_PORTUGAL:	country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_GREEK:		country_code[0] = 'g'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_RUSSIA:		country_code[0] = 'r'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_DENMARK:	country_code[0] = 'd'; country_code[1] = 'k'; country_code[2] = '\0'; break;
			case LC_BULGARIA:	country_code[0] = 'b'; country_code[1] = 'g'; country_code[2] = '\0'; break;
			case LC_CROATIA:	country_code[0] = 'h'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_MEXICO:		country_code[0] = 'm'; country_code[1] = 'x'; country_code[2] = '\0'; break;
			case LC_ARABIA:		country_code[0] = 'a'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_CZECH:		country_code[0] = 'c'; country_code[1] = 'z'; country_code[2] = '\0'; break;
			case LC_ROMANIA:	country_code[0] = 'r'; country_code[1] = 'o'; country_code[2] = '\0'; break;
			case LC_HUNGARY:	country_code[0] = 'h'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_NETHERLANDS: country_code[0] = 'n'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_USA:		country_code[0] = 'u'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_CANADA:	country_code[0] = 'c'; country_code[1] = 'a'; country_code[2] = '\0'; break;
			default:
				if (test_server == true)
				{
					country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0';
				}
				break;
		}

		char buf[512+1];
		char sas[33];
		MD5_CTX ctx;
		const char sas_key[] = "BSTPR0YKP06";

		snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

		MD5Init(&ctx);
		MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
#ifdef __FreeBSD__
		MD5End(&ctx, sas);
#else
		static const char hex[] = "0123456789abcdef";
		unsigned char digest[16];
		MD5Final(digest, &ctx);
		int i;
		for (i = 0; i < 16; ++i) {
			sas[i+i] = hex[digest[i] >> 4];
			sas[i+i+1] = hex[digest[i] & 0x0f];
		}
		sas[i+i] = '\0';
#endif

		snprintf(buf, sizeof(buf), "mall http://%s/ishop?pid=%u&c=%s&sid=%d&sas=%s",
				g_strWebMallURL.c_str(), ch->GetPlayerID(), country_code, g_server_id, sas);

		ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
	}
}

// ÁÖ»çÀ§
ACMD(do_dice)
{
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		start = atoi(arg1);
		end = atoi(arg2);
	}
	else if (*arg1 && !*arg2)
	{
		start = 1;
		end = atoi(arg1);
	}

	end = MAX(start, end);
	start = MIN(start, end);

	int n = number(start, end);

#ifdef ENABLE_DICE_SYSTEM
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, LC_TEXT("%s´ÔÀÌ ÁÖ»çÀ§¸¦ ±¼·Á %d°¡ ³ª¿Ô½À´Ï´Ù. (%d-%d)"), ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_DICE_INFO, LC_TEXT("´ç½ÅÀÌ ÁÖ»çÀ§¸¦ ±¼·Á %d°¡ ³ª¿Ô½À´Ï´Ù. (%d-%d)"), n, start, end);
#else
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_TEXT("%s´ÔÀÌ ÁÖ»çÀ§¸¦ ±¼·Á %d°¡ ³ª¿Ô½À´Ï´Ù. (%d-%d)"), ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ç½ÅÀÌ ÁÖ»çÀ§¸¦ ±¼·Á %d°¡ ³ª¿Ô½À´Ï´Ù. (%d-%d)"), n, start, end);
#endif
}

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

#ifdef ENABLE_USER_PANEL
ACMD(do_open_userpanel)
{
	char bufPlayers[CHAT_MAX_LEN + 1];
		snprintf(bufPlayers, sizeof(bufPlayers), "player.count_kills");
	char bufMobs[CHAT_MAX_LEN + 1];
		snprintf(bufMobs, sizeof(bufMobs), "mob.count_kills");
	
	int players, mobs;
	
	players = ch->GetQuestFlag(bufPlayers);
	mobs = ch->GetQuestFlag(bufMobs);
	
	char buf[CHAT_MAX_LEN + 1];
	snprintf(buf, sizeof(buf), "BINARY_UserPanel_Open %d %d", players, mobs);
	ch->ChatPacket(CHAT_TYPE_COMMAND, buf);	
}
#endif

#ifdef ENABLE_MANAGER_BANK_SYSTEM
ACMD(do_bank_create)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	BankManager::instance().Send_Create(ch, arg1, arg2);
}
ACMD(do_bank_open)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	BankManager::instance().Send_Open(ch, arg1, arg2);
}
ACMD(do_bank_add)
{
	const char *r;
	char arg1[256], arg2[256], arg3[256];
	r = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(r, arg3, sizeof(arg3));
	BankManager::instance().Send_AddMoney(ch, arg1, arg2, arg3);
}
ACMD(do_bank_withdraw)
{
	const char *r;
	char arg1[256], arg2[256], arg3[256];
	r = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(r, arg3, sizeof(arg3));
	BankManager::instance().Send_WithdrawMoney(ch, arg1, arg2, arg3);
}
ACMD(do_bank_send)
{
	char arg1[256], arg2[256], arg3[256], arg4[256];
	two_arguments (two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4));
	BankManager::instance().Send_Money(ch, arg1, arg2, arg3, arg4);
}
ACMD(do_bank_changepw)
{
	char arg1[256], arg2[256], arg3[256], arg4[256];
	two_arguments (two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4));
	BankManager::instance().Send_ChangePassword(ch, arg1, arg2, arg3, arg4);
}
#endif

ACMD(do_click_safebox)
{
    if (ch->GetMapIndex() > 10000 || ch->GetMapIndex() == 403 || ch->GetMapIndex() == 200 || ch->GetMapIndex() == 405)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<System> Imposible abrir el Almacen en este Lugar!"));
        return;
    }
    ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}

ACMD(do_click_town)
{
    ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
}

ACMD(do_ride)
{
    dev_log(LOG_DEB0, "[DO_RIDE] start");
    if (ch->IsDead() || ch->IsStun())
	return;

    // ³»¸®±â
    {
	if (ch->IsHorseRiding())
	{
	    dev_log(LOG_DEB0, "[DO_RIDE] stop riding");
	    ch->StopRiding();
	    return;
	}

	if (ch->GetMountVnum())
	{
	    dev_log(LOG_DEB0, "[DO_RIDE] unmount");
	    do_unmount(ch, NULL, 0, 0);
	    return;
	}
    }

    // Å¸±â
    {
	if (ch->GetHorse() != NULL)
	{
	    dev_log(LOG_DEB0, "[DO_RIDE] start riding");
	    ch->StartRiding();
	    return;
	}

	for (UINT i=0; i<INVENTORY_MAX_NUM; ++i)
	{
	    LPITEM item = ch->GetInventoryItem(i);
	    if (NULL == item)
		continue;

	    // À¯´ÏÅ© Å»°Í ¾ÆÀÌÅÛ
		if (item->IsRideItem())
		{
			if (NULL==ch->GetWear(WEAR_UNIQUE1) || NULL==ch->GetWear(WEAR_UNIQUE2))
			{
				dev_log(LOG_DEB0, "[DO_RIDE] USE UNIQUE ITEM");
				//ch->EquipItem(item);
				ch->UseItem(TItemPos (INVENTORY, i));
				return;
			}
		}

	    // ÀÏ¹Ý Å»°Í ¾ÆÀÌÅÛ
	    // TODO : Å»°Í¿ë SubType Ãß°¡
	    switch (item->GetVnum())
	    {
		case 71114:	// Àú½ÅÀÌ¿ë±Ç
		case 71116:	// »ê°ß½ÅÀÌ¿ë±Ç
		case 71118:	// ÅõÁö¹üÀÌ¿ë±Ç kodlarina ekle biner ctrl gtamam client src ac
		case 71120:	// »çÀÚ¿ÕÀÌ¿ë±Ç
		    dev_log(LOG_DEB0, "[DO_RIDE] USE QUEST ITEM");
		    ch->UseItem(TItemPos (INVENTORY, i));
		    return;
	    }

		// GF mantis #113524, 52001~52090 ¹ø Å»°Í
		if( (item->GetVnum() > 52000) && (item->GetVnum() < 52091) )	{
			dev_log(LOG_DEB0, "[DO_RIDE] USE QUEST ITEM");
			ch->UseItem(TItemPos (INVENTORY, i));
		    return;
		}
	}
    }


    // Å¸°Å³ª ³»¸± ¼ö ¾øÀ»¶§
    ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» ¸ÕÀú ¼ÒÈ¯ÇØÁÖ¼¼¿ä."));
}

#ifdef __HIDE_COSTUME_SYSTEM__
ACMD(do_hide_costume)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	bool hidden = true;
	BYTE bPartPos = 0;
	BYTE bHidden = 0;

	str_to_number(bPartPos, arg1);

	if (*arg2)
	{
		str_to_number(bHidden, arg2);

		if (bHidden == 0)
			hidden = false;
	}

	if (bPartPos == 1)
		ch->SetBodyCostumeHidden(hidden);
	else if (bPartPos == 2)
		ch->SetHairCostumeHidden(hidden);
	else if (bPartPos == 3)
		ch->SetSashCostumeHidden(hidden);
	else if (bPartPos == 4)
		ch->SetWeaponCostumeHidden(hidden);
	else
		return;

	ch->UpdatePacket();
}
#endif


#ifdef __DUNGEON_INFO_SYSTEM__
//ACMD(do_raking_finish_ataque)
//{
//	if (!ch)
//		return;
//	
//	const char *line;
//	char arg1[256], arg2[256];
//	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
//	const std::string& strArg1 = std::string(arg1);
//	
//	if (strArg1 == "RANKING_ATAQUE"){
//		std::auto_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery("SELECT * FROM player.dungeon_ataque%s WHERE dungeon_id = %s ORDER BY ataque DESC, date ASC LIMIT 15;", get_table_postfix(),arg2));
//		if (pMsg->Get()->uiNumRows == 0)
//		{
//			ch->ChatPacket(CHAT_TYPE_COMMAND, "CleanDungeonRanking");
//			ch->ChatPacket(CHAT_TYPE_INFO, "<Dungeon Informacion> Actualmente no tenemos registros de ataque de esta dungeon");
//			return;
//		}
//		else
//		{
//			int iLine = 0;
//			MYSQL_ROW mRow;
//			while (NULL != (mRow = mysql_fetch_row(pMsg->Get()->pSQLResult)))
//			{
//				int iCur = 0;
//				DWORD dwlevel = 0, dwataque = 0, dwPlayerID = 0;
//				str_to_number(dwPlayerID, mRow[iCur++]);
//				const char* szPlayerName = mRow[iCur++];
//				str_to_number(dwlevel, mRow[iCur++]);
//				str_to_number(dwataque, mRow[iCur]);
//
//				ch->ChatPacket(CHAT_TYPE_COMMAND, "UpdateDungeonRanking #%s#%d#%d#", szPlayerName, dwlevel, dwataque);
//				iLine++;
//				ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenDungeonRanking");
//			}
//		}
//	}
//}

ACMD(do_raking_finish)
{
	if (!ch)
		return;
	
	const char *line;
	char arg1[256], arg2[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	const std::string& strArg1 = std::string(arg1);
	const std::string& strArg2 = std::string(arg2);
	if (strArg1 == "RANKING_FINALIZADO"){
		std::auto_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery("SELECT * FROM player.dungeon_ranking%s WHERE dungeon_id = %s ORDER BY finished DESC, date ASC LIMIT 15;", get_table_postfix(),arg2));
		if (pMsg->Get()->uiNumRows == 0)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "CleanDungeonRanking");
			ch->ChatPacket(CHAT_TYPE_INFO, "<Dungeon Informacion> Actualmente no tenemos registros de subida de esta dungeon");
			return;
		}
		else
		{
			int iLine = 0;
			MYSQL_ROW mRow;
			while (NULL != (mRow = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				int iCur = 0;
				DWORD dwlevel = 0, dwfinished = 0, dwPlayerID = 0;
				str_to_number(dwPlayerID, mRow[iCur++]);
				const char* szPlayerName = mRow[iCur++];
				str_to_number(dwlevel, mRow[iCur++]);
				str_to_number(dwfinished, mRow[iCur]);

				ch->ChatPacket(CHAT_TYPE_COMMAND, "UpdateDungeonRanking #%s#%d#%d#", szPlayerName, dwlevel, dwfinished);
				iLine++;
				ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenDungeonRanking");
			}
		}
	}
	
	else if (strArg1 == "OPENDUNGEON"){
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenDungeon");
	}
}
#endif

ACMD(do_sananemk)
{
/*********************************************************************
* date        : 2016.10.25
* file        : cmd_general.cpp
* author      : VegaS
* description : Statistics server + configuration for fake
*/
	time_t currentTime;
	struct tm *localTime;
	
	time(&currentTime);
	localTime = localtime(&currentTime);
	int mResultHour = localTime->tm_hour, mResultMin = localTime->tm_min, mResultSec = localTime->tm_sec;	
	int mTotalOnline;
	int * mEmpire;
	int mLocal;

	DESC_MANAGER::instance().GetUserCount(mTotalOnline, &mEmpire, mLocal);
/*
	0 = 00:00	1 = 01:00	2 = 02:00	3 = 03:00	4 = 04:00	5 = 05:00
	6 = 06:00	7 = 07:00	8 = 08:00	9 = 09:00	10 = 10:00	11 = 11:00
	12 = 12:00	13 = 13:00	14 = 14:00	15 = 15:00	16 = 16:00	17 = 17:00	
	18 = 18:00	19 = 19:00	20 = 20:00	21 = 21:00	22 = 22:00	23 = 23:00		
*/
#ifdef ENABLE_COUNTER_FAKE
    static int arrayDesc[23 + 1][2] =
     {	/* 
          First row -> Current hour | Second row -> Value added for players fake 
          Example: From 15:00 until 15:59:59 will be added on statics + 75 players online fake. Because -> { 15, 75 }
		*/
        { 0, 190  },     { 1, 175  },    { 2, 160  },	{ 3, 140  },     { 4, 130  },    { 5, 100  },
        { 6, 80   },     { 7, 75   },    { 8, 60   },	{ 9, 55   },     { 10, 40  },    { 11, 30  },
        { 12, 50  },     { 13, 60  },    { 14, 70  },	{ 15, 75  },     { 16, 80  },    { 17, 90  },
        { 18, 105 },     { 19, 120 },    { 20, 135 },	{ 21, 155 },     { 22, 175 },    { 23, 180 }
     };
	
    for (int i=0; i<_countof(arrayDesc); i++) {
        if (mResultHour == arrayDesc[i][0])
        {
            for (int j=1; j<=3; j++)
                mEmpire[i] += arrayDesc[i][1] / 3;
            mTotalOnline += arrayDesc[i][1];
            break;
        }
    }
#endif
	if (ch->IsGM())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[i] Sunucu zaman dilimi : %d:%d:%d", mResultHour, mResultMin, mResultSec);
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "[i] Mevcut Kanal: [%d]", g_bChannel);	
	ch->ChatPacket(CHAT_TYPE_INFO, "[i]	Sunucuda aktif olan oyuncu durumu (toplam?: [%d]", mTotalOnline);
	ch->ChatPacket(CHAT_TYPE_INFO, "[i] Krall?lar aras?aktif oyuncu durumu: Shinshoo - [%d] | Chunjo - [%d] | Jinno - [%d]", mEmpire[1], mEmpire[2], mEmpire[3]);
//#ifdef ENABLE_OFFLINE_SHOPShinshoo
	///ch->ChatPacket(CHAT_TYPE_INFO, "[i] Bu kanalda su an ?vrimdisi magazalar var: [%d]", mShopsOffline);
//#endif
}

ACMD(do_user_mount_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;
	
	if (ch->GetHorse() != NULL)
		return;
	
	if (ch->IsHorseRiding() != NULL)
		return;

	if (ch->GetMount() != NULL && !ch->IsRiding())
	{
		ch->Mount(ch->GetMount()->GetRaceNum());
	}
	else if (ch->IsRidingMount() && ch->GetMount() == NULL)
	{
		int mount_vnum = ch->GetMountVnum();
		ch->Dismount();
		ch->MountSummon(true, mount_vnum);
	}
}

ACMD(do_add_new_attribute)
{
	std::vector<std::string> vecArgs;
	split_argument(argument,vecArgs);
	
	if (vecArgs.size() < 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_UNKNOWN_ARGUMENT"));
		return;
	}
	
	int sourceItemPos, targetItemPos, attrtype0, attrtype1, attrtype2, attrtype3;
	// int targetItemPos;
	// int attrtype0;
	// int attrtype1;
	// int attrtype2;
	// int attrtype3;
		
	str_to_number(sourceItemPos, vecArgs[1].c_str());
	str_to_number(targetItemPos, vecArgs[2].c_str());
	str_to_number(attrtype0, vecArgs[3].c_str());
	str_to_number(attrtype1, vecArgs[4].c_str());
	str_to_number(attrtype2, vecArgs[5].c_str());
	str_to_number(attrtype3, vecArgs[6].c_str());
	
	LPITEM item = ch->GetInventoryItem(targetItemPos);
	
	if (!item)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_CANNOT_FIND_ITEM"));
		return;
	}
	
	if (item->GetType() != ITEM_WEAPON && item->GetType() != ITEM_ARMOR)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_COSTUME"));
		return;
	}
	
	if (item->IsEquipped())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_EQUIPPED"));
		return;
	}
	
	if (item->IsExchanging())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_EXCHANGING"));
		return;
	}

	if (ch->CountSpecifyItem(71051) < 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_ENOUGH_NEW_ATTRIBUTE"));
		return;
	}
	
	TItemTable const * pProto = item->GetProto();
	
	bool addontype = false;
	if (pProto && pProto->sAddonType)
		addontype = true;
	
	int attrtype0prob = number(1,5), attrtype1prob = number(1,5), attrtype2prob = number(1,5), attrtype3prob = number(1,5);
	// int attrtype1prob = number(1,5);
	// int attrtype2prob = number(1,5);
	// int attrtype3prob = number(1,5);
	
	bool error = false;
	if (addontype == false)
	{
		if (attrtype0 == attrtype1 || attrtype0 == attrtype2 || attrtype0 == attrtype3 || attrtype0 == item->GetAttributeType(4))
			error = true;
		if (attrtype1 == attrtype0 || attrtype1 == attrtype2 || attrtype1 == attrtype3 || attrtype1 == item->GetAttributeType(4))
			error = true;
		if (attrtype2 == attrtype0 || attrtype2 == attrtype0 || attrtype2 == attrtype3 || attrtype2 == item->GetAttributeType(4))
			error = true;
		if (attrtype3 == attrtype0 || attrtype3 == attrtype1 || attrtype3 == attrtype2 || attrtype3 == item->GetAttributeType(4))
			error = true;
		if (item->GetAttributeType(4) == attrtype0 || item->GetAttributeType(4) == attrtype1 || item->GetAttributeType(4) == attrtype2 || item->GetAttributeType(4) == attrtype3)
			error = true;
	}
	else
	{
		if (attrtype0 == attrtype1 || attrtype0 == item->GetAttributeType(4))
			error = true;
		if (attrtype1 == attrtype0 || attrtype1 == item->GetAttributeType(4))
			error = true;
	}

	for (int i=0; i < 4; ++i)
	{
		if (addontype == false)
		{
			if (item->GetAttributeType(i) == attrtype0 && i != 0)
				error = true;
			if (item->GetAttributeType(i) == attrtype1 && i != 1)
				error = true;
			if (item->GetAttributeType(i) == attrtype2 && i != 2)
				error = true;
			if (item->GetAttributeType(i) == attrtype3 && i != 3)
				error = true;
		}
		else
		{
			if (item->GetAttributeType(i+2) == attrtype0 && i != 0)
				error = true;
			if (item->GetAttributeType(i+2) == attrtype1 && i != 1)
				error = true;			
		}
	}
	
	bool error_bind = false;
	if (item->IsSealed())
		error_bind = true;
	
	if (error_bind)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_SOULBIND"));
		return;
	}
		
	if (error)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_ERROR"));
		return;
	}
	
	if (addontype == false)
	{
		item->AddNewAttr(0, attrtype0, attrtype0prob);
		item->AddNewAttr(1, attrtype1, attrtype1prob);
		item->AddNewAttr(2, attrtype2, attrtype2prob);
		item->AddNewAttr(3, attrtype3, attrtype3prob);
	}
	else
	{	
		int oldattr5type = item->GetAttributeType(4), oldattr5value = item->GetAttributeValue(4);
		// int oldattr5value = item->GetAttributeValue(4);

		item->RemoveAttributeAt(0);
		item->RemoveAttributeAt(1);	
		item->ApplyAddon(100);
		item->ChangeAttribute();
		item->AddNewAttr(2, attrtype0, attrtype0prob);
		item->AddNewAttr(3, attrtype1, attrtype1prob);	
		item->AddNewAttr(4, oldattr5type, oldattr5value);

		if (item->GetAttributeType(1) != 71)
		{
			if (item->GetAttributeType(0) != 72)
			{
				item->SetForceAttributeNewEnchant(0, 72, 13);
				item->SetForceAttributeNewEnchant(1, 71, 1);
			}
			else
				item->SetForceAttributeNewEnchant(1, 71, -1);
		}				
	}
	
	ch->RemoveSpecifyItem(71051, 1);
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_ATTRIBUTE_SUCCESS_ATTR"));
}

#ifdef WJ_SECURITY_SYSTEM
ACMD(do_create_security)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;
	
	int iFloodResult = ch->GetQuestFlag("input_security.last_input");
	if (iFloodResult){
		if (get_global_time() < iFloodResult + 1 /* limit */) {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HIZLI_YAPAMAZSIN1"));
			return;
		}
	}	

	
	if (ch->GetSecurityPassword())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already a security password"));
		return;
	}
	
	if (strlen(arg1) < 4 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only 4 - 6 character numbers"));
		return;
	}
	
	if (!is_digits(arg1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only number"));
		return;
	}
	
	ch->SetSecurityPassword(arg1);
	ch->DeactivateSecurity();
	ch->PrepareSecurityGui(false);
	ch->UpdatePacket();
	//ch->UpdateSecurityPacket();
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key successfully created"));
	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseSecurityCreate");
}

ACMD(do_input_security)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;
	
	int iFloodResult = ch->GetQuestFlag("input_security.last_input");
	if (iFloodResult){
		if (get_global_time() < iFloodResult + 1 /* limit */) {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HIZLI_YAPAMAZSIN1"));
			return;
		}
	}	

	if (strlen(arg1) < 4 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only 4 - 6 character numbers"));
		return;
	}
	
	if (!is_digits(arg1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only number"));
		return;
	}
	
	if (!ch->GetSecurityPassword()) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GUVENLIK_SIFRESI_MEVCUT_DEGIL"));
		return;
	}
	
	if (strcmp(ch->GetSecurityPassword(), arg1) == 0)
	{
		ch->DeactivateSecurity();
		ch->PrepareSecurityGui(false);
		ch->UpdatePacket();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security has been closed"));
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseSecurityDialog");
		ch->SetQuestFlag("input_security.last_input", get_global_time() + 2);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key is incorrect"));
		ch->SetQuestFlag("input_security.last_input", get_global_time() + 2);
	}
}

ACMD(do_change_security)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];
	three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));
	
	int iFloodResult = ch->GetQuestFlag("input_security.last_input");
	if (iFloodResult){
		if (get_global_time() < iFloodResult + 1 /* limit */) {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HIZLI_YAPAMAZSIN1"));
			return;
		}
	}	

	if (strlen(arg1) < 4 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only 4 - 6 character numbers"));
		return;
	}

	if (strlen(arg2) < 4 || strlen(arg2) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only 4 - 6 character numbers"));
		return;
	}
	
	if (strlen(arg3) < 4 || strlen(arg3) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only 4 - 6 character numbers"));
		return;
	}
	
	if (!is_digits(arg1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only number"));
		return;
	}
	
	if (!is_digits(arg2))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only number"));
		return;
	}
	
	if (!is_digits(arg3))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has only number"));
		return;
	}
	
	if (ch->GetSecurityPassword() && strcmp(ch->GetSecurityPassword(), arg1) == 0)
	{
		if (strcmp(arg2, arg3) == 0)
		{
			ch->SetSecurityPassword(arg2);
			ch->UpdatePacket();
			//ch->UpdateSecurityPacket();
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Security key has been successfully changed"));
			//ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseSecurityChange");
			ch->SetQuestFlag("input_security.last_input", get_global_time() + 2);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("First New Password And Second Password Not Match"));
			ch->SetQuestFlag("input_security.last_input", get_global_time() + 2);
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Old security key not match"));
		ch->SetQuestFlag("input_security.last_input", get_global_time() + 2);
	}
}

ACMD(do_open_security)
{
	ch->ActivateSecurity();
	ch->PrepareSecurityGui(true);
	if (ch->GetSecurityPassword())
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenSecurityDialog");
	else
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenSecurityCreate");
}
#endif

ACMD(do_ajanisik)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GEREKSIZ_ISLEM_YAPMAYINIZ"));
		return;
	}
	if (!ch->GetWarMap())
		return;

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1); 
	if (!tch)
		return;
	if (!tch->IsPC())
		return;	
	if (ch == tch)
		return;
	if (!tch->GetGuild())
		return;	
	if (!ch->GetGuild())
		return;
	if (tch->GetGuild()->GetID() != ch->GetGuild()->GetID())
		return;
	CGuild* loncam = CGuildManager::instance().FindGuild(ch->GetGuild()->GetID());
	CGuild* loncan = CGuildManager::instance().FindGuild(tch->GetGuild()->GetID());
	if ((loncam == NULL) || (loncan == NULL))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LONCAN_YOK"));
	}
	else
	{
		if (loncan->GetMasterPID() == ch->GetPlayerID())
		{
			if (loncam->RequestRemoveMember(tch->GetPlayerID()))
			{
				ch->ChatPacket(CHAT_TYPE_GUILD, "%s AJAN_OLARAK_SECILDI.", tch->GetName());
				tch->GoHome();
				return;
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HATA_OLUSTUR"));
				return;
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LONCA_LIDERI_OLMALISIN"));
			return;
		}
	}
	return;
}

ACMD (do_envanter_duzenle)
{
	if(ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->IsCombOpen() || ch->GetOfflineShopOwner())
	{
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡Áß(Ã¢°í,±³È¯,»óÁ¡)¿¡´Â °³ÀÎ»óÁ¡À» »ç?ëÇ?¼ö ¾ø½À´Ï´Ù."));
        return;
	}
	
	/*int lastSortInventoryPulse = ch->GetSortInventoryPulse();
	int currentPulse = thecore_pulse();
	
	if (lastSortInventoryPulse > currentPulse) {
		int deltaInSeconds = ((lastSortInventoryPulse / PASSES_PER_SEC(1)) - (currentPulse / PASSES_PER_SEC(1)));
		int minutes = deltaInSeconds / 60;
		int seconds = (deltaInSeconds - (minutes * 60));

		return;
	}*/
		
	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);
		
		if(!item)
			continue;
		
		if(item->isLocked())
			continue;
		
		if(item->GetCount() == 200)
			continue;
		
		if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			for (int j = i; j < INVENTORY_AND_EQUIP_SLOT_MAX; ++j)
			{
				LPITEM item2 = ch->GetInventoryItem(j);
				
				if(!item2)
					continue;
				
				if(item2->isLocked())
					continue;
	
				if (item2->GetVnum() == item->GetVnum())
				{
					bool bStopSockets = false;
					
					for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
					{
						if (item2->GetSocket(k) != item->GetSocket(k))
						{
							bStopSockets = true;
							break;
						}
					}
					
					if(bStopSockets)
						continue;
	
					BYTE bAddCount = MIN(200 - item->GetCount(), item2->GetCount());
	
					item->SetCount(item->GetCount() + bAddCount);
					item2->SetCount(item2->GetCount() - bAddCount);
					
					continue;
				}
			}
		}
	}
}

#ifdef __GAYA__
ACMD(do_gaya_system)
{
	char arg1[255];
	char arg2[255];
	two_arguments (argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (0 == arg1[0])
		return;

	const std::string& strArg1 = std::string(arg1);

	if (strArg1 == "craft"){

		if (0 == arg2[0])
			return;

		int slot = atoi(arg2);
		ch->CraftGayaItems(slot);

	}
	else if(strArg1 == "market"){

		if (0 == arg2[0])
			return;

		int slot = atoi(arg2);
		ch->MarketGayaItems(slot);

	}
	else if(strArg1 == "refresh")
	{
		ch->RefreshGayaItems();
	}
}
#endif

ACMD(do_loncatoplantis)
{
ch->ChatPacket(CHAT_TYPE_INFO, "Bu d?enleme mevcut de?l.");
}

ACMD(do_loncatoplantisi)
{

	if (!ch->GetGuild())
		return;
	
    
	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade != GUILD_LEADER_GRADE)
    return;
    
  g->LoncaToplantisi(ch);
  quest::CQuestManager::instance().RequestSetEventFlagBR(g->GetName(),"loncatoplantix",ch->GetX());
  quest::CQuestManager::instance().RequestSetEventFlagBR(g->GetName(),"loncatoplantiy",ch->GetY());
  quest::CQuestManager::instance().RequestSetEventFlagBR(g->GetName(),"loncatoplantimapindex",ch->GetMapIndex());
  
  ch->ChatPacket(CHAT_TYPE_INFO, "<Lonca> Toplant?iste? lonca ?elerine g?derildi.");

}



ACMD(do_loncatoplantieve)
{
ch->ChatPacket(CHAT_TYPE_INFO, "Bu d?enleme mevcut de?l.");
}

ACMD(do_loncatoplantievet)
{

	if (!ch->GetGuild())
		return;
	
  char buf[256];
  char buf2[256];
  char buf3[256];
			
  snprintf(buf, sizeof(buf), "%sloncatoplantix",ch->GetGuild()->GetName());
  snprintf(buf2, sizeof(buf2), "%sloncatoplantiy",ch->GetGuild()->GetName());
  snprintf(buf3, sizeof(buf3), "%sloncatoplantimapindex",ch->GetGuild()->GetName());
    
 int x = quest::CQuestManager::instance().GetEventFlag(buf);
 int y = quest::CQuestManager::instance().GetEventFlag(buf2);
 int mapindex = quest::CQuestManager::instance().GetEventFlag(buf3);
 ch->WarpSet(x, y, mapindex);

}

ACMD(do_loncaistatisti)
{
	ch->ChatPacket(CHAT_TYPE_INFO, "Bu duzenleme mevcut degil");
}

ACMD(do_loncaistatistik)
{

	if (!ch->GetGuild())
		return;

	if (ch->GetMapIndex() >= 1100000 && ch->GetMapIndex() <= 1120000)
	{
	ch->GetGuild()->LoncaIstatistik(ch);
	ch->ChatPacket(CHAT_TYPE_INFO, "<Lonca> istatistik durumu açýld?");
	}
	else
	ch->ChatPacket(CHAT_TYPE_INFO, "<Lonca> istatistik durumunu a?bilmek i?n savaþta olmal??. !");
}

//BEST OTO AV
ACMD(do_otomatikav_komut1)
{
	ch->AggregateMonster();
}

ACMD(do_otomatikav_komut2)
{
	if (ch->CountSpecifyItem(20171) == 0 && ch->CountSpecifyItem(20172) == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OTO_AV_ICIN_GEREKLI_NESNE_YOK"));
		return;
	}
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OtomatikAvWindow");
}
//BEST OTO AV

ACMD(do_remove_afk)
{
	ch->DelAfk();
}

#ifdef ZODIAC_NISAN_SYSTEM
ACMD(do_zodyakstart)
{
	if (!ch)
		return;
 
	if (!ch->IsPC())
		return;
 
    if (ch->IsDead() || ch->IsStun())
		return;
 
	if (ch->IsHack())
		return;
	
	if(ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()  || ch->IsCombOpen()  || ch->GetOfflineShop())	
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<Nisan> Lutfen aktif pencereleri kapatiniz");
		return;
	}

	if(ch->CountSpecifyItem(33001) == 0 || ch->CountSpecifyItem(33002) == 0 || ch->CountSpecifyItem(33003) == 0 || ch->CountSpecifyItem(33004) == 0 || ch->CountSpecifyItem(33005) == 0 || ch->CountSpecifyItem(33006) == 0 || ch->CountSpecifyItem(33007) == 0 || ch->CountSpecifyItem(33008) == 0 || ch->CountSpecifyItem(33009) == 0 || ch->CountSpecifyItem(33010) == 0 || ch->CountSpecifyItem(33011) == 0 || ch->CountSpecifyItem(33012) == 0 || ch->CountSpecifyItem(33012) == 0 || ch->CountSpecifyItem(33013) == 0 || ch->CountSpecifyItem(33014) == 0 || ch->CountSpecifyItem(33015) == 0 || ch->CountSpecifyItem(33016) == 0 || ch->CountSpecifyItem(33017) == 0 || ch->CountSpecifyItem(33018) == 0 || ch->CountSpecifyItem(33019) == 0 || ch->CountSpecifyItem(33020) == 0 || ch->CountSpecifyItem(33021) == 0 || ch->CountSpecifyItem(33022) == 0)  
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<Nisan> Envanterinizdeki Nisan Tasi yetersiz.");
		return;
	}
	ch->ChatPacket(CHAT_TYPE_INFO, "<Nisan> Tebrikler 1 Adet Altin Zodyak Sandigi Kazandiniz.");
	ch->RemoveSpecifyItem(33001, 1);
	ch->RemoveSpecifyItem(33002, 1);
	ch->RemoveSpecifyItem(33003, 1);
	ch->RemoveSpecifyItem(33004, 1);
	ch->RemoveSpecifyItem(33005, 1);
	ch->RemoveSpecifyItem(33006, 1);
	ch->RemoveSpecifyItem(33007, 1);
	ch->RemoveSpecifyItem(33008, 1);
	ch->RemoveSpecifyItem(33009, 1);
	ch->RemoveSpecifyItem(33010, 1);
	ch->RemoveSpecifyItem(33011, 1);
	ch->RemoveSpecifyItem(33012, 1);
	ch->RemoveSpecifyItem(33013, 1);
	ch->RemoveSpecifyItem(33014, 1);
	ch->RemoveSpecifyItem(33015, 1);
	ch->RemoveSpecifyItem(33016, 1);
	ch->RemoveSpecifyItem(33017, 1);
	ch->RemoveSpecifyItem(33018, 1);
	ch->RemoveSpecifyItem(33019, 1);
	ch->RemoveSpecifyItem(33020, 1);
	ch->RemoveSpecifyItem(33021, 1);
	ch->RemoveSpecifyItem(33022, 1);
	ch->AutoGiveItem(33033, 1);
}
#endif

#ifdef __BATTLE_PASS__
ACMD(open_battlepass)
{
	if (ch->v_counts.empty())
		return;

	if (ch->missions_bp.empty())
		return;

	if (ch->IsOpenSafebox() || ch->IsDead() || ch->GetExchange() || ch->GetMyShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Nu poti face asta!");
		return;
	}

	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);

	int month = vKey.tm_mon;
	
	if (month != ch->iMonthBattlePass)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Time Expire!");
		return;
	}

	for (int i=0; i<ch->missions_bp.size(); ++i)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "missions_bp %d %d %d %d", i, ch->missions_bp[i].type, ch->missions_bp[i].vnum, ch->missions_bp[i].count);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "info_missions_bp %d %d %d %s", i, ch->v_counts[i].count, ch->v_counts[i].status, ch->rewards_bp[i].name);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "rewards_missions_bp %d %d %d %d %d %d %d", i, ch->rewards_bp[i].vnum1, ch->rewards_bp[i].vnum2, ch->rewards_bp[i].vnum3, ch->rewards_bp[i].count1, ch->rewards_bp[i].count2 ,ch->rewards_bp[i].count3);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "size_missions_bp %d ", ch->missions_bp.size());
	ch->ChatPacket(CHAT_TYPE_COMMAND, "final_reward %d %d %d %d %d %d", ch->final_rewards[0].f_vnum1, ch->final_rewards[0].f_vnum2, ch->final_rewards[0].f_vnum3, ch->final_rewards[0].f_count1, ch->final_rewards[0].f_count2 ,ch->final_rewards[0].f_count3);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "show_battlepass");
}

ACMD(final_reward_battlepass)
{
	if (ch->v_counts.empty())
		return;

	if (ch->missions_bp.empty())
		return;

	if (ch->v_counts[0].status == 2)
		return;

	if (ch->IsOpenSafebox() || ch->IsDead() || ch->GetExchange() || ch->GetMyShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Nu poti face asta!");
		return;
	}

	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);

	int month = vKey.tm_mon;
	
	if (month != ch->iMonthBattlePass)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Time Expire!");
		return;
	}

	for (int i=0; i<ch->missions_bp.size(); ++i)
	{
		if (ch->missions_bp[i].count != ch->v_counts[i].count)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Nu ai completat toate misiunile!");
			return;
		}
	}

	ch->FinalRewardBattlePass();
}
#endif