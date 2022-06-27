#include "stdafx.h"
#include "../common/VnumHelper.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "protocol.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "cmd.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "battle.h"
#include "exchange.h"
#include "questmanager.h"
#include "profiler.h"
#include "messenger_manager.h"
#include "party.h"
#include "p2p.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "log.h"
#if defined(WJ_COMBAT_ZONE)
#include "combat_zone.h"
#endif
#ifdef ENABLE_SWITCHBOT
#include "new_switchbot.h"
#endif
#include "banword.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "building.h"
#include "locale_service.h"
#include "gm.h"
#include "spam.h"
#include "ani.h"
#include "motion.h"
#include "OXEvent.h"
#include "locale_service.h"
#include "DragonSoul.h"
#ifdef ENABLE_DROP_ITEM_WORLDARD
#include "drop_item.h"
#endif
#include "../../common/service.h"
#ifdef WJ_OFFLINESHOP_SYSTEM
#include "offline_shop.h"
#include "offlineshop_manager.h"
#endif

extern void SendShout(const char * szText, BYTE bEmpire);
extern int g_nPortalLimitTime;

#include "input.h"

#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	#include "refine.h"
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
#include "New_PetSystem.h"
#endif
static int __deposit_limit()
{
	return (1000*10000); // 1Ãµ¸¸
}


#ifdef WJ_CHANGELOOK_SYSTEM
void CInputMain::ChangeLook(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;
	
	TPacketChangeLook * sPacket = (TPacketChangeLook*) c_pData;
	switch (sPacket->subheader)
	{
		case CL_SUBHEADER_CLOSE:
			{
				pkChar->ChangeLookWindow(false);
			}
			break;
		case CL_SUBHEADER_ADD:
			{
				pkChar->AddClMaterial(sPacket->tPos, sPacket->bPos);
			}
			break;
		case CL_SUBHEADER_REMOVE:
			{
				pkChar->RemoveClMaterial(sPacket->bPos);
			}
			break;
		case CL_SUBHEADER_REFINE:
			{
				pkChar->RefineClMaterials();
			}
			break;
		default:
			break;
	}
}
#endif

#ifdef WJ_SHOP_SEARCH_SYSTEM
#include "entity.h"
#include <boost/unordered_map.hpp>
bool CompareItemVnumAcPriceAC(COfflineShop::OFFLINE_SHOP_ITEM i, COfflineShop::OFFLINE_SHOP_ITEM j)
{
	return (i.vnum < j.vnum) && (i.price < j.price);
}
#include "entity.h"
void CInputMain::ShopSearch(LPCHARACTER ch, const char * data, bool bName)
{
	TPacketCGShopSearch * pinfo = (TPacketCGShopSearch *)data;

	if (!ch)
		return;

	if (!data)
		return;

	if (ch->IsOpenSafebox() || ch->GetShop() || ch->IsCubeOpen() || ch->IsDead() || ch->GetExchange() || ch->GetOfflineShop() || ch->GetMyShop() || ch->IsCombOpen() || ch->IsActivateSecurity() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANT_DO_THIS_BECAUSE_OTHER_WINDOW_OPEN"));
		return;
	}

	if (0 == quest::CQuestManager::instance().GetEventFlag("enable_shop_search"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PRIVATE_SHOP_SEARCH_SYSTEM_DISABLED"));
		return;
	}

#ifndef DISABLE_PRIVATE_SHOP_SEARCH_NEED_ITEM
	if (!ch->FindSpecifyItem(60004) && !ch->FindSpecifyItem(60005))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PRIVATE_SHOP_SEARCH_NEED_ITEM"));
		return;
	}
#endif

	int32_t Race = pinfo->Race;
	int32_t Cat = pinfo->ItemCat;
	int32_t SubCat = pinfo->SubCat;
	int32_t MinLevel = pinfo->MinLevel;
	int32_t MaxLevel = pinfo->MaxLevel;
	int32_t MinRefine = pinfo->MinRefine;
	int32_t MaxRefine = pinfo->MaxRefine;
	uint64_t MinGold = pinfo->MinGold;
	uint64_t MaxGold = pinfo->MaxGold;
	uint64_t MinCheque = pinfo->MinCheque;
	uint64_t MaxCheque = pinfo->MaxCheque;
	std::string itemName = "";

	//Checks
	if (Race < JOB_WARRIOR || Race > JOB_WOLFMAN)
		return;
	if (Cat < ITEM_NONE || Cat > ITEM_GACHA)
		return;
	if (SubCat < USE_POTION || SubCat > USE_RESET_COSTUME_ATTR)
		return;
	if (MinLevel < 0 || MinLevel > PLAYER_MAX_LEVEL_CONST)
		return;
	if (MaxLevel < 0 || MaxLevel > PLAYER_MAX_LEVEL_CONST)
		return;
	if (MinRefine < 0 || MinRefine > 9)
		return;
	if (MaxRefine < 0 || MaxRefine > 9)
		return;
	if (MinGold < 0 || MinGold > GOLD_MAX)
		return;
	if (MaxGold < 0 || MaxGold > GOLD_MAX)
		return;
	if (MinCheque < 0 || MinCheque > 999)
		return;
	if (MaxCheque < 0 || MaxCheque > 999)
		return;
	if (MinLevel > MaxLevel)
		return;
	if (MinRefine > MaxRefine)
		return;
	if (MinGold > MaxGold)
		return;
	if (MinCheque > MaxCheque)
		return;

	if (bName)
	{
		itemName = pinfo->ItemName;
		std::replace(itemName.begin(), itemName.end(), '_', ' ');
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());

	if (!pPC)
		return;

	DWORD dwShopSearchSecCycle = 2; // 1 sec
	DWORD dwNowSec = get_global_time();
	DWORD dwLastShopSearchAttrSec = pPC->GetFlag("ShopSearch.LastShopSearchSecAttr");

	if (dwLastShopSearchAttrSec + dwShopSearchSecCycle > dwNowSec)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PRIVATE_SHOP_SEARCH_NEED_WAIT_%d_LEFT_(%d)"),
			dwShopSearchSecCycle, dwShopSearchSecCycle - (dwNowSec - dwLastShopSearchAttrSec));
		return;
	}

	pPC->SetFlag("ShopSearch.LastShopSearchSecAttr", dwNowSec);

	if (bName)
		sys_log(0, "SHOP_SEARCH: CharName: %s Search: Race: %d Cat: %d SubCat: %d MinLevel: %d MaxLevel: %d MinRefine: %d MaxRefine: %d MinGold: %llu MaxGold: %llu itemName: %s", ch->GetName(), Race, Cat, SubCat, MinLevel, MaxLevel, MinRefine, MaxRefine, MinGold, MaxGold, itemName.c_str());
	else
		sys_log(0, "SHOP_SEARCH: CharName: %s Search: Race: %d Cat: %d SubCat: %d MinLevel: %d MaxLevel: %d MinRefine: %d MaxRefine: %d MinGold: %llu MaxGold: %llu", ch->GetName(), Race, Cat, SubCat, MinLevel, MaxLevel, MinRefine, MaxRefine, MinGold, MaxGold);

	CEntity::ENTITY_MAP viewmap = ch->GetViewMap();
	CEntity::ENTITY_MAP::iterator it = viewmap.begin();
	std::vector<COfflineShop::OFFLINE_SHOP_ITEM> sendItems;

	while (it != viewmap.end())
	{
		if (it->first->GetType() != ENTITY_CHARACTER) {
			++it;
			continue;
		}

		LPCHARACTER tch = static_cast<LPCHARACTER>((it++)->first);

		if (!tch)
			continue;
		
		if (!tch->IsOfflineShopNPC() || tch == ch)
			continue;

		if (!tch->GetOfflineShop())
			continue;
		
		std::vector<COfflineShop::OFFLINE_SHOP_ITEM> shop_items = tch->GetOfflineShop()->GetItemVector();
		std::vector<COfflineShop::OFFLINE_SHOP_ITEM>::iterator item;
		
		for (item = shop_items.begin(); item != shop_items.end(); ++item)
		{
			if (bName)
			{
				if (item->status != 0 || item->vnum == 0)
					continue;
				
				TItemTable * table = ITEM_MANAGER::instance().GetTable(item->vnum);
				
				if (!table)
					continue;
				
				if (itemName.length() > 2)
				{
					if (MinLevel != 0) {
						for (int x = 0; x < ITEM_LIMIT_MAX_NUM; ++x) {
							if (table->aLimits[x].bType == LIMIT_LEVEL) {
								if (table->aLimits[x].lValue < MinLevel) { continue; } } } }
								
					if (MaxLevel != gPlayerMaxLevel)  {
						for (int x = 0; x < ITEM_LIMIT_MAX_NUM; ++x) {
							if (table->aLimits[x].bType == LIMIT_LEVEL) {
								if (table->aLimits[x].lValue > MaxLevel) { continue; } } } }	

					for (int x = 0; x < ITEM_LIMIT_MAX_NUM; ++x) {
						if (table->aLimits[x].bType == LIMIT_REAL_TIME) {
							if (table->aLimits[x].lValue == 0) { continue; } } }
							
					if (!(item->price > MinGold && item->price <= MaxGold))
						continue;
					
					if (item->price_type == 7 && !(item->price2 > MinCheque && item->price2 <= MaxCheque))
						continue;

					bool pushback = false;

					std::string foundName = item->szName;
					if (foundName.find(itemName) != std::string::npos) {
						pushback = true;
					}

					if (pushback)
					{
						sendItems.push_back(*item);
					}
				}

			}
			else
			{
				if (item->status != 0 || item->vnum == 0)
					continue;
				
				TItemTable* table = ITEM_MANAGER::instance().GetTable(item->vnum);
				
				if (!table)
					continue;
				
				if (table->bType == Cat &&	table->bSubType == SubCat)
				{
					if ((table->bType == ITEM_WEAPON || table->bType == ITEM_ARMOR) && !(item->refine_level >= MinRefine && item->refine_level <= MaxRefine))
						continue;
					
					if (MinLevel != 0) {
						for (int x = 0; x < ITEM_LIMIT_MAX_NUM; ++x) {
							if (table->aLimits[x].bType == LIMIT_LEVEL) {
								if (table->aLimits[x].lValue < MinLevel) { continue; } } } }

					if (MaxLevel != gPlayerMaxLevel)  {
						for (int x = 0; x < ITEM_LIMIT_MAX_NUM; ++x) {
							if (table->aLimits[x].bType == LIMIT_LEVEL) {
								if (table->aLimits[x].lValue > MaxLevel) { continue; } } } }

					for (int x = 0; x < ITEM_LIMIT_MAX_NUM; ++x) {
						if (table->aLimits[x].bType == LIMIT_REAL_TIME) {
							if (table->aLimits[x].lValue == 0) { continue; } } }

					if (!(item->price >= MinGold && item->price <= MaxGold))
						continue;
					
					if (item->price_type == 7 && !(item->price2 > MinCheque && item->price2 <= MaxCheque))
						continue;

					bool cont = false;
					switch (Race)
					{
					case JOB_WARRIOR:
						if (IS_SET(table->dwAntiFlags, ITEM_ANTIFLAG_WARRIOR))
							cont = true;
						break;

					case JOB_ASSASSIN:
						if (IS_SET(table->dwAntiFlags, ITEM_ANTIFLAG_ASSASSIN))
							cont = true;
						break;

					case JOB_SURA:
						if (IS_SET(table->dwAntiFlags, ITEM_ANTIFLAG_SURA))
							cont = true;
						break;

					case JOB_SHAMAN:
						if (IS_SET(table->dwAntiFlags, ITEM_ANTIFLAG_SHAMAN))
							cont = true;
						break;
#ifdef ENABLE_WOLFMAN_CHARACTER
					case JOB_WOLFMAN:
						if (IS_SET(table->dwAntiFlags, ITEM_ANTIFLAG_WOLFMAN))
							cont = true;
						break;
#endif
					}

					if (cont)
						continue;

					sendItems.push_back(*item);

				}
			}
		}
	}

	std::stable_sort(sendItems.begin(), sendItems.end(), CompareItemVnumAcPriceAC);
	std::vector<COfflineShop::OFFLINE_SHOP_ITEM>::iterator item;

	for (item = sendItems.begin(); item != sendItems.end(); ++item)
	{
		if (item->status != 0 || item->vnum == 0)
			continue;
		
		TItemTable* table = ITEM_MANAGER::instance().GetTable(item->vnum);
		
		if (table)
		{	
			LPDESC d = ch->GetDesc();

			if (!d)
				return;

			TPacketGCShopSearchItemSet pack;
			pack.header = HEADER_GC_SHOPSEARCH_SET;

			pack.count = static_cast<BYTE>(item->count);
			pack.vnum = item->vnum;
			pack.flags = table->dwFlags;
			pack.anti_flags = table->dwAntiFlags;
			pack.price = item->price;
			pack.price2 = item->price_type == 7 ? item->price2 : 0;
			pack.vid = item->shop_id;
#ifdef WJ_CHANGELOOK_SYSTEM
			pack.transmutation = item->transmutation;
#endif
			pack.Cell = item->pos;

			thecore_memcpy(pack.alSockets, item->alSockets, sizeof(pack.alSockets));
			thecore_memcpy(pack.aAttr, item->aAttr, sizeof(pack.aAttr));

			d->LargePacket(&pack, sizeof(TPacketGCShopSearchItemSet));
		}
	}
}

void CInputMain::ShopSearchBuy(LPCHARACTER ch, const char * data)
{
	TPacketCGShopSearchBuy * pinfo = (TPacketCGShopSearchBuy *)data;

	int32_t shopVid = pinfo->shopVid;
	int32_t shopItemPos = pinfo->shopItemPos;

	if (!ch)
		return;

	if (0 == quest::CQuestManager::instance().GetEventFlag("enable_shop_search"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PRIVATE_SHOP_SEARCH_SYSTEM_DISABLED"));
		return;
	}

#ifndef DISABLE_PRIVATE_SHOP_SEARCH_NEED_ITEM
	if (!ch->FindSpecifyItem(60005))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PRIVATE_SHOP_SEARCH_NEED_ITEM"));
		return;
	}
#endif

	LPCHARACTER pkChrShop = CHARACTER_MANAGER::instance().Find(shopVid);

	if (!pkChrShop)
		return;

	LPOFFLINESHOP pkShop = pkChrShop->GetOfflineShop();

	if (!pkShop)
		return;
	/*if (pkShop->IsClosed())
	{
		sys_err("Player %lu trying to buy from closed shop.", ch->GetPlayerID());
		return;
	}*/

	if (DISTANCE_APPROX(ch->GetX() - pkChrShop->GetX(), ch->GetY() - pkChrShop->GetY()) > VIEW_RANGE)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÁ¡°úÀÇ °Å¸®°¡ ³Ê¹« ¸Ö¾î ¹°°ÇÀ» »ì ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetMapIndex() != pkChrShop->GetMapIndex())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU_CANNOT_BUY_THIS_BECAUSE_NOT_IN_SAME_MAP"));
		return;
	}

	ch->SetOfflineShopOwner(pkChrShop);
	pkShop->SetGuestMap(ch);
	int32_t returnHeader = pkShop->Buy(ch, shopItemPos);

	if (SHOP_SUBHEADER_GC_OK == returnHeader)
	{
		// being lazy starting in 3 2 ..
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShopSearchBuy");
		ch->SetOfflineShop(NULL);
		ch->SetOfflineShopOwner(NULL);
		pkShop->RemoveGuestMap(ch);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShopSearchError %d", returnHeader);
		ch->SetOfflineShop(NULL);
		ch->SetOfflineShopOwner(NULL);
		pkShop->RemoveGuestMap(ch);
	}

	//If, after buying, the shop is closed, destroy it (through its owner char)
	/*if (pkShop->IsClosed() && pkShop->IsPCShop())
	{
		M2_DESTROY_CHARACTER(pkShop->GetOwner());
	}*/

}
#endif

#ifdef __SEND_TARGET_INFO__
void CInputMain::TargetInfoLoad(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGTargetInfoLoad* p = (TPacketCGTargetInfoLoad*)c_pData;
	TPacketGCTargetInfo pInfo;
	pInfo.header = HEADER_GC_TARGET_INFO;
	static std::vector<LPITEM> s_vec_item;
	s_vec_item.clear();
	LPITEM pkInfoItem;
	LPCHARACTER m_pkChrTarget = CHARACTER_MANAGER::instance().Find(p->dwVID);
	if (!ch || !m_pkChrTarget)//targetinfosystem
		return;

	if (ITEM_MANAGER::instance().CreateDropItemVector(m_pkChrTarget, ch, s_vec_item) && (m_pkChrTarget->IsMonster() || m_pkChrTarget->IsStone()))
	{
		if (s_vec_item.size() == 0);
		else if (s_vec_item.size() == 1)
		{
			pkInfoItem = s_vec_item[0];
			pInfo.dwVID	= m_pkChrTarget->GetVID();
			pInfo.race = m_pkChrTarget->GetRaceNum();
			pInfo.dwVnum = pkInfoItem->GetVnum();
			pInfo.count = pkInfoItem->GetCount();
			ch->GetDesc()->Packet(&pInfo, sizeof(TPacketGCTargetInfo));
		}
		else
		{
			int iItemIdx = s_vec_item.size() - 1;
			while (iItemIdx >= 0)
			{
				pkInfoItem = s_vec_item[iItemIdx--];

				if (!pkInfoItem)
				{
					sys_err("pkInfoItem null in vector idx %d", iItemIdx + 1);
					continue;
				}

					pInfo.dwVID	= m_pkChrTarget->GetVID();
					pInfo.race = m_pkChrTarget->GetRaceNum();
					pInfo.dwVnum = pkInfoItem->GetVnum();
					pInfo.count = pkInfoItem->GetCount();
					ch->GetDesc()->Packet(&pInfo, sizeof(TPacketGCTargetInfo));
			}
		}
	}
	// }
}
#endif

void SendBlockChatInfo(LPCHARACTER ch, int sec)
{
	if (sec <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã¤ÆÃ ±ÝÁö »óÅÂÀÔ´Ï´Ù."));
		return;
	}

	long hour = sec / 3600;
	sec -= hour * 3600;

	long min = (sec / 60);
	sec -= min * 60;

	char buf[128+1];

	if (hour > 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d ½Ã°£ %d ºÐ %d ÃÊ µ¿¾È Ã¤ÆÃ±ÝÁö »óÅÂÀÔ´Ï´Ù"), hour, min, sec);
	else if (hour > 0 && min == 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d ½Ã°£ %d ÃÊ µ¿¾È Ã¤ÆÃ±ÝÁö »óÅÂÀÔ´Ï´Ù"), hour, sec);
	else if (hour == 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d ºÐ %d ÃÊ µ¿¾È Ã¤ÆÃ±ÝÁö »óÅÂÀÔ´Ï´Ù"), min, sec);
	else
		snprintf(buf, sizeof(buf), LC_TEXT("%d ÃÊ µ¿¾È Ã¤ÆÃ±ÝÁö »óÅÂÀÔ´Ï´Ù"), sec);

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

EVENTINFO(spam_event_info)
{
	char host[MAX_HOST_LENGTH+1];

	spam_event_info()
	{
		::memset( host, 0, MAX_HOST_LENGTH+1 );
	}
};

typedef boost::unordered_map<std::string, std::pair<unsigned int, LPEVENT> > spam_score_of_ip_t;
spam_score_of_ip_t spam_score_of_ip;

EVENTFUNC(block_chat_by_ip_event)
{
	spam_event_info* info = dynamic_cast<spam_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "block_chat_by_ip_event> <Factor> Null pointer" );
		return 0;
	}

	const char * host = info->host;

	spam_score_of_ip_t::iterator it = spam_score_of_ip.find(host);

	if (it != spam_score_of_ip.end())
	{
		it->second.first = 0;
		it->second.second = NULL;
	}

	return 0;
}

bool SpamBlockCheck(LPCHARACTER ch, const char* const buf, const size_t buflen)
{
	extern int g_iSpamBlockMaxLevel;

	if (ch->GetLevel() < g_iSpamBlockMaxLevel)
	{
		spam_score_of_ip_t::iterator it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());

		if (it == spam_score_of_ip.end())
		{
			spam_score_of_ip.insert(std::make_pair(ch->GetDesc()->GetHostName(), std::make_pair(0, (LPEVENT) NULL)));
			it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());
		}

		if (it->second.second)
		{
			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);
			return true;
		}

		unsigned int score;
		const char * word = SpamManager::instance().GetSpamScore(buf, buflen, score);

		it->second.first += score;

		if (word)
			sys_log(0, "SPAM_SCORE: %s text: %s score: %u total: %u word: %s", ch->GetName(), buf, score, it->second.first, word);

		extern unsigned int g_uiSpamBlockScore;
		extern unsigned int g_uiSpamBlockDuration;

		if (it->second.first >= g_uiSpamBlockScore)
		{
			spam_event_info* info = AllocEventInfo<spam_event_info>();
			strlcpy(info->host, ch->GetDesc()->GetHostName(), sizeof(info->host));

			it->second.second = event_create(block_chat_by_ip_event, info, PASSES_PER_SEC(g_uiSpamBlockDuration));
			sys_log(0, "SPAM_IP: %s for %u seconds", info->host, g_uiSpamBlockDuration);

			LogManager::instance().CharLog(ch, 0, "SPAM", word);

			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);

			return true;
		}
	}

	return false;
}

enum
{
	TEXT_TAG_PLAIN,
	TEXT_TAG_TAG, // ||
	TEXT_TAG_COLOR, // |cffffffff
	TEXT_TAG_HYPERLINK_START, // |H
	TEXT_TAG_HYPERLINK_END, // |h ex) |Hitem:1234:1:1:1|h
	TEXT_TAG_RESTORE_COLOR,
};

int GetTextTag(const char * src, int maxLen, int & tagLen, std::string & extraInfo)
{
	tagLen = 1;

	if (maxLen < 2 || *src != '|')
		return TEXT_TAG_PLAIN;

	const char * cur = ++src;

	if (*cur == '|') // ||¢¥A |¡¤I CY1ACN¢¥U.
	{
		tagLen = 2;
		return TEXT_TAG_TAG;
	}
	else if (*cur == 'c') // color |cffffffffblahblah|r
	{
		tagLen = 2;
		return TEXT_TAG_COLOR;
	}
	else if (*cur == 'H') // hyperlink |Hitem:10000:0:0:0:0|h[ÀÌ¸§]|h
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_START;
	}
	else if (*cur == 'h') // end of hyperlink
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_END;
	}

	return TEXT_TAG_PLAIN;
}

void GetTextTagInfo(const char * src, int src_len, int & hyperlinks, bool & colored)
{
	colored = false;
	hyperlinks = 0;

	int len;
	std::string extraInfo;

	for (int i = 0; i < src_len;)
	{
		int tag = GetTextTag(&src[i], src_len - i, len, extraInfo);

		if (tag == TEXT_TAG_HYPERLINK_START)
			++hyperlinks;

		if (tag == TEXT_TAG_COLOR)
			colored = true;

		i += len;
	}
}

int ProcessTextTag(LPCHARACTER ch, const char * c_pszText, size_t len)
{

	int hyperlinks;
	bool colored;

	GetTextTagInfo(c_pszText, len, hyperlinks, colored);

	if (colored == true && hyperlinks == 0)
		return 4;

	if (ch->GetExchange())
	{
		if (hyperlinks == 0)
			return 0;
		else
			return 3;
	}

	int nPrismCount = ch->CountSpecifyItem(ITEM_PRISM);

	if (nPrismCount < hyperlinks)
		return 0;


	if (!ch->GetMyShop())
	{
		ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
		return 0;
	} else
	{
		int sellingNumber = ch->GetMyShop()->GetNumberByVnum(ITEM_PRISM);
		if(nPrismCount - sellingNumber < hyperlinks)
		{
			return 2;
		} else
		{
			ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
			return 0;
		}
	}

	return 4;
}

int CInputMain::Whisper(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGWhisper* pinfo = reinterpret_cast<const TPacketCGWhisper*>(data);

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGWhisper);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot send whisper message with security key activate"));
		return (iExtraLen);
	}
#endif

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken mesaj gonderemezsin.");
		return (iExtraLen);
	}
#endif

	if (ch->FindAffect(AFFECT_BLOCK_CHAT))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã¤ÆÃ ±ÝÁö »óÅÂÀÔ´Ï´Ù."));
		return (iExtraLen);
	}

#if defined(WJ_COMBAT_ZONE) && defined(WJ_COMBAT_ZONE_HIDE_INFO_USER)	
	if (CCombatZoneManager::Instance().IsCombatZoneMap(ch->GetMapIndex()) && (ch->GetGMLevel() < GM_IMPLEMENTOR))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("cz_cannot_use_whisper"));
		return (iExtraLen);
	}
#endif

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(pinfo->szNameTo);

	if (pkChr == ch)
		return (iExtraLen);

	LPDESC pkDesc = NULL;

	BYTE bOpponentEmpire = 0;

	if (test_server)
	{
		if (!pkChr)
			sys_log(0, "Whisper to %s(%s) from %s", "Null", pinfo->szNameTo, ch->GetName());
		else
			sys_log(0, "Whisper to %s(%s) from %s", pkChr->GetName(), pinfo->szNameTo, ch->GetName());
	}

	if (ch->IsBlockMode(BLOCK_WHISPER))
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		return iExtraLen;
	}

	if (!pkChr)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			pkDesc->SetRelay(pinfo->szNameTo);
			bOpponentEmpire = pkCCI->bEmpire;

			if (test_server)
				sys_log(0, "Whisper to %s from %s (Channel %d Mapindex %d)", "Null", ch->GetName(), pkCCI->bChannel, pkCCI->lMapIndex);
		}
	}
	else
	{
		pkDesc = pkChr->GetDesc();
		bOpponentEmpire = pkChr->GetEmpire();
	}

	if (!pkDesc)
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;

			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_NOT_EXIST;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
			sys_log(0, "WHISPER: no player");
		}
	}
	else
	{
		if (ch->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else if (pkChr && pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_TARGET_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		#ifdef ENABLE_MESSENGER_BLOCK
		else if (pkChr && MessengerManager::instance().IsBlocked_Target(ch->GetName(), pkChr->GetName()))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;

				char msg_2[CHAT_MAX_LEN + 1];
				snprintf(msg_2, sizeof(msg_2), LC_TEXT("%s'yi blokladim"), pkChr->GetName());
				int len = MIN(CHAT_MAX_LEN, strlen(msg_2) + 1);

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + len;
				pack.bType = WHISPER_TYPE_SYSTEM;
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

				TEMP_BUFFER buf;

				buf.write(&pack, sizeof(TPacketGCWhisper));
				buf.write(msg_2, len);
				ch->GetDesc()->Packet(buf.read_peek(), buf.size());
			}
		}
		else if (pkChr && MessengerManager::instance().IsBlocked_Me(ch->GetName(), pkChr->GetName()))
		{
			//beni
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;

				char msg_1[CHAT_MAX_LEN + 1];
				snprintf(msg_1, sizeof(msg_1), LC_TEXT("%s beni blokladi."), pkChr->GetName());
				int len = MIN(CHAT_MAX_LEN, strlen(msg_1) + 1);

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + len;
				pack.bType = WHISPER_TYPE_SYSTEM;
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

				TEMP_BUFFER buf;

				buf.write(&pack, sizeof(TPacketGCWhisper));
				buf.write(msg_1, len);

				ch->GetDesc()->Packet(buf.read_peek(), buf.size());
			}
		}
		else if (pkChr && MessengerManager::instance().IsBlocked_Target(ch->GetName(), pkChr->GetName()) && MessengerManager::instance().IsBlocked_Me(ch->GetName(), pkChr->GetName()))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;

				char msg_3[CHAT_MAX_LEN + 1];
				snprintf(msg_3, sizeof(msg_3), LC_TEXT("%s birbirimiz."), pkChr->GetName());
				int len = MIN(CHAT_MAX_LEN, strlen(msg_3) + 1);

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + len;
				pack.bType = WHISPER_TYPE_SYSTEM;
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

				TEMP_BUFFER buf;

				buf.write(&pack, sizeof(TPacketGCWhisper));
				buf.write(msg_3, len);

				ch->GetDesc()->Packet(buf.read_peek(), buf.size());
			}
		}
		#endif
		else
		{
			BYTE bType = WHISPER_TYPE_NORMAL;

			char buf[CHAT_MAX_LEN + 1];
			strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
			const size_t buflen = strlen(buf);

			if (true == SpamBlockCheck(ch, buf, buflen))
			{
				if (!pkChr)
				{
					CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

					if (pkCCI)
					{
						pkDesc->SetRelay("");
					}
				}
				return iExtraLen;
			}

			if (LC_IsCanada() == false)
			{
				CBanwordManager::instance().ConvertString(buf, buflen);
			}

			if (g_bEmpireWhisper)
				if (!ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
					if (!(pkChr && pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)))
						if (bOpponentEmpire != ch->GetEmpire() && ch->GetEmpire() && bOpponentEmpire // ¼­·Î Á¦±¹ÀÌ ´Ù¸£¸é¼­
								&& ch->GetGMLevel() == GM_PLAYER && gm_get_level(pinfo->szNameTo) == GM_PLAYER) // µÑ´Ù ÀÏ¹Ý ÇÃ·¹ÀÌ¾îÀÌ¸é
							// ÀÌ¸§ ¹Û¿¡ ¸ð¸£´Ï gm_get_level ÇÔ¼ö¸¦ »ç¿ë
						{
							if (!pkChr)
							{
								// ´Ù¸¥ ¼­¹ö¿¡ ÀÖÀ¸´Ï Á¦±¹ Ç¥½Ã¸¸ ÇÑ´Ù. bTypeÀÇ »óÀ§ 4ºñÆ®¸¦ Empire¹øÈ£·Î »ç¿ëÇÑ´Ù.
								bType = ch->GetEmpire() << 4;
							}
							else
							{
								ConvertEmpireText(ch->GetEmpire(), buf, buflen, 10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + ch->GetEmpire() - 1)/*º¯È¯È®·ü*/);
							}
						}

			int processReturn = ProcessTextTag(ch, buf, buflen);
			if (0!=processReturn)
			{
				if (ch->GetDesc())
				{
					TItemTable * pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
						char buf[128];
						int len;
						if (3==processReturn) //±³È¯Áß
							len = snprintf(buf, sizeof(buf), LC_TEXT("´Ù¸¥ °Å·¡Áß(Ã¢°í,±³È¯,»óÁ¡)¿¡´Â °³ÀÎ»óÁ¡À» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."), pTable->szLocaleName);
						else
							len = snprintf(buf, sizeof(buf), LC_TEXT("%sÀÌ ÇÊ¿äÇÕ´Ï´Ù."), pTable->szLocaleName);
						
						if (len < 0 || len >= (int) sizeof(buf))
							len = sizeof(buf) - 1;

						++len;  // \0 ¹®ÀÚ Æ÷ÇÔ

						TPacketGCWhisper pack;

						pack.bHeader = HEADER_GC_WHISPER;
						pack.bType = WHISPER_TYPE_ERROR;
						pack.wSize = sizeof(TPacketGCWhisper) + len;
						strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

						ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
						ch->GetDesc()->Packet(buf, len);

						sys_log(0, "WHISPER: not enough %s: char: %s", pTable->szLocaleName, ch->GetName());
					}
				}

				// ¸±·¡ÀÌ »óÅÂÀÏ ¼ö ÀÖÀ¸¹Ç·Î ¸±·¡ÀÌ¸¦ Ç®¾îÁØ´Ù.
				pkDesc->SetRelay("");
				return (iExtraLen);
			}

			if (ch->IsGM())
				bType = (bType & 0xF0) | WHISPER_TYPE_GM;

			if (buflen > 0)
			{
				TPacketGCWhisper pack;

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + buflen;

				pack.bType = bType;
				strlcpy(pack.szNameFrom, ch->GetName(), sizeof(pack.szNameFrom));

				// desc->BufferedPacketÀ» ÇÏÁö ¾Ê°í ¹öÆÛ¿¡ ½á¾ßÇÏ´Â ÀÌÀ¯´Â 
				// P2P relayµÇ¾î ÆÐÅ¶ÀÌ Ä¸½¶È­ µÉ ¼ö ÀÖ±â ¶§¹®ÀÌ´Ù.
				TEMP_BUFFER tmpbuf;

				tmpbuf.write(&pack, sizeof(pack));
				tmpbuf.write(buf, buflen);

				pkDesc->Packet(tmpbuf.read_peek(), tmpbuf.size());

				// @warme006
				// sys_log(0, "WHISPER: %s -> %s : %s", ch->GetName(), pinfo->szNameTo, buf);
			}
		}
	}
	if(pkDesc)
		pkDesc->SetRelay("");

	return (iExtraLen);
}

struct RawPacketToCharacterFunc
{
	const void * m_buf;
	int	m_buf_len;

	RawPacketToCharacterFunc(const void * buf, int buf_len) : m_buf(buf), m_buf_len(buf_len)
	{
	}

	void operator () (LPCHARACTER c)
	{
		if (!c->GetDesc())
			return;

		c->GetDesc()->Packet(m_buf, m_buf_len);
	}
};

struct FEmpireChatPacket
{
	packet_chat& p;
	const char* orig_msg;
	int orig_len;
	char converted_msg[CHAT_MAX_LEN+1];

	BYTE bEmpire;
	int iMapIndex;
	int namelen;

	FEmpireChatPacket(packet_chat& p, const char* chat_msg, int len, BYTE bEmpire, int iMapIndex, int iNameLen)
		: p(p), orig_msg(chat_msg), orig_len(len), bEmpire(bEmpire), iMapIndex(iMapIndex), namelen(iNameLen)
	{
		memset( converted_msg, 0, sizeof(converted_msg) );
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != iMapIndex)
			return;

		d->BufferedPacket(&p, sizeof(packet_chat));

		if (d->GetEmpire() == bEmpire ||
			bEmpire == 0 ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			d->Packet(orig_msg, orig_len);
		}
		else
		{
			// »ç¶÷¸¶´Ù ½ºÅ³·¹º§ÀÌ ´Ù¸£´Ï ¸Å¹ø ÇØ¾ßÇÕ´Ï´Ù
			size_t len = strlcpy(converted_msg, orig_msg, sizeof(converted_msg));

			if (len >= sizeof(converted_msg))
				len = sizeof(converted_msg) - 1;

			ConvertEmpireText(bEmpire, converted_msg + namelen, len - namelen, 10 + 2 * d->GetCharacter()->GetSkillPower(SKILL_LANGUAGE1 + bEmpire - 1));
			d->Packet(converted_msg, orig_len);
		}
	}
};

struct FYmirChatPacket
{
	packet_chat& packet;
	const char* m_szChat;
	size_t m_lenChat;
	const char* m_szName;

	int m_iMapIndex;
	BYTE m_bEmpire;
	bool m_ring;

	char m_orig_msg[CHAT_MAX_LEN+1];
	int m_len_orig_msg;
	char m_conv_msg[CHAT_MAX_LEN+1];
	int m_len_conv_msg;

	FYmirChatPacket(packet_chat& p, const char* chat, size_t len_chat, const char* name, size_t len_name, int iMapIndex, BYTE empire, bool ring)
		: packet(p),
		m_szChat(chat), m_lenChat(len_chat),
		m_szName(name),
		m_iMapIndex(iMapIndex), m_bEmpire(empire),
		m_ring(ring)
	{
		m_len_orig_msg = snprintf(m_orig_msg, sizeof(m_orig_msg), "%s : %s", m_szName, m_szChat) + 1; // 3I 1¢çAU A¡ÀCO

		if (m_len_orig_msg < 0 || m_len_orig_msg >= (int) sizeof(m_orig_msg))
			m_len_orig_msg = sizeof(m_orig_msg) - 1;

		m_len_conv_msg = snprintf(m_conv_msg, sizeof(m_conv_msg), "??? : %s", m_szChat) + 1; // 3I 1¢çAU 1IA¡ÀCO

		if (m_len_conv_msg < 0 || m_len_conv_msg >= (int) sizeof(m_conv_msg))
			m_len_conv_msg = sizeof(m_conv_msg) - 1;

		ConvertEmpireText(m_bEmpire, m_conv_msg + 6, m_len_conv_msg - 6, 10); // 6Ao "??? : "AC ¡¾aAI
	}

	void operator() (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != m_iMapIndex)
			return;

		if (m_ring ||
			d->GetEmpire() == m_bEmpire ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			packet.size = m_len_orig_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_orig_msg, m_len_orig_msg);
		}
		else
		{
			packet.size = m_len_conv_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_conv_msg, m_len_conv_msg);
		}
	}
};

#ifdef ENABLE_GROWTH_PET_SYSTEM
void CInputMain::BraveRequestPetName(LPCHARACTER ch, const char* c_pData)
{
	if (!ch->GetDesc()) { return; }
	int vid = ch->GetEggVid();
	if (vid == 0) { return; }

	TPacketCGRequestPetName* p = (TPacketCGRequestPetName*)c_pData;

	if (ch->GetGold() < 100000) {
		ch->ChatPacket(CHAT_TYPE_INFO, "[Pet-Kulu?a] 100.000 Yang gerekir");
		return;
	}

	if (ch->CountSpecifyItem(vid) > 0 && check_name(p->petname) != 0) {
		DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), -100000);
		ch->PointChange(POINT_GOLD, -100000, true);
		ch->RemoveSpecifyItem(vid, 1);
		LPITEM item = ch->AutoGiveItem(vid + 300, 1);
		if (!item)
			return;
		int tmpslot = number(3, 3);
		int tmpskill[3] = { 0, 0, 0 };
		for (int i = 0; i < 3; ++i)
		{
			if (i > tmpslot - 1)
				tmpskill[i] = -1;
		}
		int tmpdur = number(1, 14) * 24 * 60;
		int tmpagedur = time(0) - 86400;
		char szQuery1[1024];
		snprintf(szQuery1, sizeof(szQuery1), "INSERT INTO new_petsystem VALUES(%lu,'%s', 1, 0, 0, 0, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)", item->GetID(), p->petname, number(1, 23), number(1, 23), number(1, 23), tmpskill[0], 0, tmpskill[1], 0, tmpskill[2], 0, tmpdur, tmpdur, tmpagedur);
		std::unique_ptr<SQLMsg> pmsg2(DBManager::instance().DirectQuery(szQuery1));
	}
	else {
		ch->ChatPacket(CHAT_TYPE_INFO, "[Pet-Kulu?a] Hatali isim girdiniz");
	}
}
#endif

int CInputMain::Chat(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
		return -1;

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char buf[CHAT_MAX_LEN - (CHARACTER_NAME_MAX_LEN + 3) + 1];
	strlcpy(buf, data + sizeof(TPacketCGChat), MIN(iExtraLen + 1, sizeof(buf)));
	const size_t buflen = strlen(buf);

	if (buflen > 1 && *buf == '/')
	{
		interpret_command(ch, buf + 1, buflen - 1);
		return iExtraLen;
	}

#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot send message with security key activate"));
		return (iExtraLen);
	}
#endif

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken mesaj gonderemezsin.");
		return (iExtraLen);
	}
#endif

	if (ch->IncreaseChatCounter() >= 10)
	{
		if (ch->GetChatCounter() == 10)
		{
			sys_log(0, "CHAT_HACK: %s", ch->GetName());
			ch->GetDesc()->DelayedDisconnect(5);
		}

		return iExtraLen;
	}

	// Ã¤ÆÃ ±ÝÁö Affect Ã³¸®
	const CAffect* pAffect = ch->FindAffect(AFFECT_BLOCK_CHAT);

	if (pAffect != NULL)
	{
		SendBlockChatInfo(ch, pAffect->lDuration);
		return iExtraLen;
	}

	if (true == SpamBlockCheck(ch, buf, buflen))
	{
		return iExtraLen;
	}

#if defined(WJ_COMBAT_ZONE) && defined(WJ_COMBAT_ZONE_HIDE_INFO_USER)	
	if (CCombatZoneManager::Instance().IsCombatZoneMap(ch->GetMapIndex()) && (ch->GetGMLevel() < GM_IMPLEMENTOR))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("cz_cannot_use_chat"));
		return iExtraLen;
	}
#endif

	// @fixme133 begin
	CBanwordManager::instance().ConvertString(buf, buflen);

	int processReturn = ProcessTextTag(ch, buf, buflen);
	if (0!=processReturn)
	{
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

		if (NULL != pTable)
		{
			if (3==processReturn) //±³È¯Áß
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡Áß(Ã¢°í,±³È¯,»óÁ¡)¿¡´Â °³ÀÎ»óÁ¡À» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."), pTable->szLocaleName);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%sÀÌ ÇÊ¿äÇÕ´Ï´Ù."), pTable->szLocaleName);

		}

		return iExtraLen;
	}
	// @fixme133 end
	char chatbuf[CHAT_MAX_LEN + 1];
	//update
	int len;
	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		len = snprintf(chatbuf, sizeof(chatbuf), "|Hmsg:%s|h%s|h|r : %s", ch->GetName(), ch->GetName(), buf);
	}
	else
	{
		len = snprintf(chatbuf, sizeof(chatbuf), "%s : %s", ch->GetName(), buf);
	}

	if (CHAT_TYPE_SHOUT == pinfo->type)
	{
		LogManager::instance().ShoutLog(g_bChannel, ch->GetEmpire(), chatbuf);
	}

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		 const int SHOUT_LIMIT_LEVEL = 15;

		if (ch->GetLevel() < SHOUT_LIMIT_LEVEL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¿ÜÄ¡±â´Â ·¹º§ %d ÀÌ»ó¸¸ »ç¿ë °¡´É ÇÕ´Ï´Ù."), SHOUT_LIMIT_LEVEL);
			return (iExtraLen);
		}

		if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * 15)
			return (iExtraLen);

		ch->SetLastShoutPulse(thecore_heart->pulse);

		bool bDisableShout = quest::CQuestManager::instance().GetEventFlag("shout_disable");
		if (bDisableShout == true)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOUT_IS_DISABLED"));
			return (iExtraLen);
		}

		TPacketGGShout p;

		p.bHeader = HEADER_GG_SHOUT;
		p.bEmpire = ch->GetEmpire();
		strlcpy(p.szText, chatbuf, sizeof(p.szText));

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShout));

		SendShout(chatbuf, ch->GetEmpire());

		return (iExtraLen);
	}
	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = pinfo->type;
	pack_chat.id = ch->GetVID();

	switch (pinfo->type)
	{
		case CHAT_TYPE_TALKING:
			{
				const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

				if (false)
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FYmirChatPacket(pack_chat,
								buf,
								strlen(buf),
								ch->GetName(),
								strlen(ch->GetName()),
								ch->GetMapIndex(),
								ch->GetEmpire(),
								ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)));
				}
				else
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FEmpireChatPacket(pack_chat,
								chatbuf,
								len,
								(ch->GetGMLevel() > GM_PLAYER ||
								 ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)) ? 0 : ch->GetEmpire(),
								ch->GetMapIndex(), strlen(ch->GetName())));
				}
			}
			break;

		case CHAT_TYPE_PARTY:
			{
				if (!ch->GetParty())
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÆÄÆ¼ ÁßÀÌ ¾Æ´Õ´Ï´Ù."));
				else
				{
					TEMP_BUFFER tbuf;

					tbuf.write(&pack_chat, sizeof(pack_chat));
					tbuf.write(chatbuf, len);

					RawPacketToCharacterFunc f(tbuf.read_peek(), tbuf.size());
					ch->GetParty()->ForEachOnlineMember(f);
				}
			}
			break;

		case CHAT_TYPE_GUILD:
			{
				if (!ch->GetGuild())
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±æµå¿¡ °¡ÀÔÇÏÁö ¾Ê¾Ò½À´Ï´Ù."));
				else
					ch->GetGuild()->Chat(chatbuf);
			}
			break;

		default:
			sys_err("Unknown chat type %d", pinfo->type);
			break;
	}

	return (iExtraLen);
}

void CInputMain::ItemUse(LPCHARACTER ch, const char * data)
{
	ch->UseItem(((struct command_item_use *) data)->Cell);
}

void CInputMain::ItemToItem(LPCHARACTER ch, const char * pcData)
{
	TPacketCGItemUseToItem * p = (TPacketCGItemUseToItem *) pcData;
	if (ch)
		ch->UseItem(p->Cell, p->TargetCell);
}

void CInputMain::ItemDrop(LPCHARACTER ch, const char * data)
{
	struct command_item_drop * pinfo = (struct command_item_drop *) data;

	//MONARCH_LIMIT
	//if (ch->IsMonarch())
	//	return;
	//END_MONARCH_LIMIT
	if (!ch)
		return;

	// ¿¤Å©°¡ 0º¸´Ù Å©¸é ¿¤Å©¸¦ ¹ö¸®´Â °Í ÀÌ´Ù.
	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell);
}

void CInputMain::ItemDrop2(LPCHARACTER ch, const char * data)
{
	//MONARCH_LIMIT
	//if (ch->IsMonarch())
	//	return;
	//END_MONARCH_LIMIT

	TPacketCGItemDrop2 * pinfo = (TPacketCGItemDrop2 *) data;

	// ¿¤Å©°¡ 0º¸´Ù Å©¸é ¿¤Å©¸¦ ¹ö¸®´Â °Í ÀÌ´Ù.

	if (!ch)
		return;
	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell, pinfo->count);
}

void CInputMain::ItemDestroy(LPCHARACTER ch, const char * data)
{
   struct command_item_destroy * pinfo = (struct command_item_destroy *) data;
   if (ch)
     ch->DestroyItem(pinfo->Cell);
}

void CInputMain::ItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (ch)
		ch->MoveItem(pinfo->Cell, pinfo->CellTo, pinfo->count);
}

void CInputMain::ItemPickup(LPCHARACTER ch, const char * data)
{
	struct command_item_pickup * pinfo = (struct command_item_pickup*) data;
	if (ch)
		ch->PickupItem(pinfo->vid);
}
#define ENABLE_FIX_QUICK_SLOT
void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_add * pinfo = (struct command_quickslot_add *) data;

#ifdef ENABLE_FIX_QUICK_SLOT
	if(pinfo->slot.type == QUICKSLOT_TYPE_ITEM)
	{
		LPITEM item = NULL;
		
		TItemPos srcCell(INVENTORY, pinfo->slot.pos);
		
		if (!(item = ch->GetItem(srcCell)))
			return;
		
		if (item->GetType() != ITEM_USE && item->GetType() != ITEM_QUEST)
			return;
	}
#endif

	ch->SetQuickslot(pinfo->pos, pinfo->slot);
}

void CInputMain::QuickslotDelete(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_del * pinfo = (struct command_quickslot_del *) data;
	ch->DelQuickslot(pinfo->pos);
}

void CInputMain::QuickslotSwap(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_swap * pinfo = (struct command_quickslot_swap *) data;
	ch->SwapQuickslot(pinfo->pos, pinfo->change_pos);
}

#ifdef ENABLE_NEW_QUICK_SLOT_SYSTEM
void CInputMain::NewQuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_newquickslot_add * pinfo = (struct command_newquickslot_add *) data;
#ifdef ENABLE_FIX_QUICK_SLOT
	if(pinfo->slot.type == QUICKSLOT_TYPE_ITEM)
	{
		LPITEM item = NULL;
		
		TItemPos srcCell(INVENTORY, pinfo->slot.pos);
		
		if (!(item = ch->GetItem(srcCell)))
			return;
		
		if (item->GetType() != ITEM_USE && item->GetType() != ITEM_QUEST)
			return;
	}
#endif
	ch->SetNewQuickslot(pinfo->pos, pinfo->slot);
}

void CInputMain::NewQuickslotDelete(LPCHARACTER ch, const char * data)
{
	struct command_newquickslot_del * pinfo = (struct command_newquickslot_del *) data;
	ch->DelNewQuickslot(pinfo->pos);
}

void CInputMain::NewQuickslotSwap(LPCHARACTER ch, const char * data)
{
	struct command_newquickslot_swap * pinfo = (struct command_newquickslot_swap *) data;
	ch->SwapNewQuickslot(pinfo->pos, pinfo->change_pos);
}
#endif

int CInputMain::Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGMessenger* p = (TPacketCGMessenger*) c_pData;

	if (uiBytes < sizeof(TPacketCGMessenger))
		return -1;

	c_pData += sizeof(TPacketCGMessenger);
	uiBytes -= sizeof(TPacketCGMessenger);

	switch (p->subheader)
	{
		#ifdef ENABLE_MESSENGER_BLOCK
		case MESSENGER_SUBHEADER_CG_ADD_BLOCK_BY_VID:
			{
				if (uiBytes < sizeof(TPacketCGMessengerAddBlockByVID))
					return -1;

				TPacketCGMessengerAddBlockByVID * p2 = (TPacketCGMessengerAddBlockByVID *) c_pData;
				LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

				if (!ch_companion)
					return sizeof(TPacketCGMessengerAddBlockByVID);

				if (ch->IsObserverMode())
					return sizeof(TPacketCGMessengerAddBlockByVID);

				LPDESC d = ch_companion->GetDesc();

				if (!d)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch_companion->GetGuild() == ch->GetGuild() && ch->GetGuild() != NULL && ch_companion->GetGuild() != NULL)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("lonca olmaz"));
					return sizeof(TPacketCGMessengerAddBlockByVID);
				}
				
				if (MessengerManager::instance().IsFriend(ch->GetName(), ch_companion->GetName()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arkadas ekleyemezsin."));
					return sizeof(TPacketCGMessengerAddBlockByVID);
				}
				
				if (MessengerManager::instance().IsBlocked_Target(ch->GetName(), ch_companion->GetName()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("zaten bloklu"));
					return sizeof(TPacketCGMessengerAddBlockByVID);
				}
				
				if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("gm'ye bu yanlis yapilmaz."));
					return sizeof(TPacketCGMessengerAddByVID);
				}

				if (ch->GetDesc() == d) // ??? ??? ? ??.
					return sizeof(TPacketCGMessengerAddBlockByVID);

				MessengerManager::instance().AddToBlockList(ch->GetName(), ch_companion->GetName());
			}
			return sizeof(TPacketCGMessengerAddBlockByVID);

		case MESSENGER_SUBHEADER_CG_ADD_BLOCK_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));
				
				if (gm_get_level(name) != GM_PLAYER)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("gm'ye bu yanlis yapilmaz."));
					return CHARACTER_NAME_MAX_LEN;
				}

				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);
				
				if (!tch)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s yok ki."), name);
				else
				{
					if (tch == ch) // ??? ??? ? ??.
						return CHARACTER_NAME_MAX_LEN;
						
					if (tch->GetGuild() == ch->GetGuild() && ch->GetGuild() != NULL && tch->GetGuild() != NULL)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("lonca olmaz"));
						return CHARACTER_NAME_MAX_LEN;
					}
					
					if (MessengerManager::instance().IsFriend(ch->GetName(), tch->GetName()))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arkadas ekleyemezsin."));
						return CHARACTER_NAME_MAX_LEN;
					}
					
					if (MessengerManager::instance().IsBlocked_Target(ch->GetName(), tch->GetName()))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("zaten bloklu"));
						return CHARACTER_NAME_MAX_LEN;
					}
						
					MessengerManager::instance().AddToBlockList(ch->GetName(), tch->GetName());
				}
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_REMOVE_BLOCK:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));
				
				if (!MessengerManager::instance().IsBlocked_Target(ch->GetName(), char_name))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bloklu degil"));
					return CHARACTER_NAME_MAX_LEN;
				}
				MessengerManager::instance().RemoveFromBlockList(ch->GetName(), char_name);
			}
			return CHARACTER_NAME_MAX_LEN;
		#endif
		
		case MESSENGER_SUBHEADER_CG_ADD_BY_VID:
			{
				if(ch->GetMapIndex() == 200)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arena_engel"));
				}
				else
				{
					if (uiBytes < sizeof(TPacketCGMessengerAddByVID))
						return -1;

					TPacketCGMessengerAddByVID * p2 = (TPacketCGMessengerAddByVID *) c_pData;
					LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

					if (!ch_companion)
						return sizeof(TPacketCGMessengerAddByVID);

					if (ch->IsObserverMode())
						return sizeof(TPacketCGMessengerAddByVID);

					if (ch_companion->IsBlockMode(BLOCK_MESSENGER_INVITE))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÌ ¸Þ½ÅÁ® Ãß°¡ °ÅºÎ »óÅÂÀÔ´Ï´Ù."));
						return sizeof(TPacketCGMessengerAddByVID);
					}

					LPDESC d = ch_companion->GetDesc();

					if (!d)
						return sizeof(TPacketCGMessengerAddByVID);

					if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<¸Þ½ÅÁ®> ¿î¿µÀÚ´Â ¸Þ½ÅÁ®¿¡ Ãß°¡ÇÒ ¼ö ¾ø½À´Ï´Ù."));
						return sizeof(TPacketCGMessengerAddByVID);
					}

					if (ch->GetDesc() == d) // ÀÚ½ÅÀº Ãß°¡ÇÒ ¼ö ¾ø´Ù.
						return sizeof(TPacketCGMessengerAddByVID);

					if (MessengerManager::instance().IsFriend(ch->GetName(), ch_companion->GetName()))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("zaten arkadas."));
						return sizeof(TPacketCGMessengerAddByVID);
					}
					
					if (MessengerManager::instance().IsBlocked(ch->GetName(), ch_companion->GetName()))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("blok ekleyemezsin"));
						return sizeof(TPacketCGMessengerAddByVID);
					}
	
					MessengerManager::instance().RequestToAdd(ch, ch_companion);
					//MessengerManager::instance().AddToList(ch->GetName(), ch_companion->GetName());
				}
			}
			return sizeof(TPacketCGMessengerAddByVID);

		case MESSENGER_SUBHEADER_CG_ADD_BY_NAME:
			{
				if(ch->GetMapIndex() == 200)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arena_engel"));
				}
				else
				{
					if (uiBytes < CHARACTER_NAME_MAX_LEN)
						return -1;

					char name[CHARACTER_NAME_MAX_LEN + 1];
					strlcpy(name, c_pData, sizeof(name));

					if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(name) != GM_PLAYER)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<¸Þ½ÅÁ®> ¿î¿µÀÚ´Â ¸Þ½ÅÁ®¿¡ Ãß°¡ÇÒ ¼ö ¾ø½À´Ï´Ù."));
						return CHARACTER_NAME_MAX_LEN;
					}

					LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

					if (!tch)
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´ÔÀº Á¢¼ÓµÇ ÀÖÁö ¾Ê½À´Ï´Ù."), name);
					else
					{
						if (tch == ch) // ÀÚ½ÅÀº Ãß°¡ÇÒ ¼ö ¾ø´Ù.
							return CHARACTER_NAME_MAX_LEN;

						if (tch->IsBlockMode(BLOCK_MESSENGER_INVITE) == true)
						{
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÌ ¸Þ½ÅÁ® Ãß°¡ °ÅºÎ »óÅÂÀÔ´Ï´Ù."));
						}
						else
						{
							if (MessengerManager::instance().IsFriend(ch->GetName(), tch->GetName()))
							{
								ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("zaten arkadas."));
								return CHARACTER_NAME_MAX_LEN;
	
	
	
	
							}
							
							if (MessengerManager::instance().IsBlocked(ch->GetName(), tch->GetName()))
							{
								ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("blok ekleyemezsin"));
								return CHARACTER_NAME_MAX_LEN;
	
							}
							// ???? ?????? ??? ??
							MessengerManager::instance().RequestToAdd(ch, tch);
							//MessengerManager::instance().AddToList(ch->GetName(), tch->GetName());
						}
					}
				}
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_REMOVE:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));
				if (!MessengerManager::instance().IsFriend(ch->GetName(), char_name))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arkadas degil"));
					return CHARACTER_NAME_MAX_LEN;
				}
				MessengerManager::instance().RemoveFromList(ch->GetName(), char_name);
			}
			return CHARACTER_NAME_MAX_LEN;

		default:
			sys_err("CInputMain::Messenger : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

int CInputMain::Shop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *) data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (test_server)
		sys_log(0, "CInputMain::Shop() ==> SubHeader %d", p->subheader);

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
			CShopManager::instance().StopShopping(ch);
			return 0;

		case SHOP_SUBHEADER_CG_BUY:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE bPos = *(c_pData + 1);
				sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), bPos);
				CShopManager::instance().Buy(ch, bPos);
				return (sizeof(BYTE) + sizeof(BYTE));
			}

		case SHOP_SUBHEADER_CG_SELL:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE pos = *c_pData;

				sys_log(0, "INPUT: %s SHOP: SELL", ch->GetName());
				CShopManager::instance().Sell(ch, pos);
				return sizeof(BYTE);
			}

		case SHOP_SUBHEADER_CG_SELL2:
			{
				if (uiBytes < sizeof(UINT) + sizeof(BYTE))
					return -1;

				UINT pos = *reinterpret_cast<const UINT*>(c_pData++);
				BYTE count = *(c_pData);

				sys_log(0, "INPUT: %s SHOP: SELL2", ch->GetName());
				CShopManager::instance().Sell(ch, pos, count);
				return sizeof(UINT) + sizeof(BYTE);
			}

		default:
			sys_err("CInputMain::Shop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

#ifdef WJ_OFFLINESHOP_SYSTEM
int CInputMain::OfflineShop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *)data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (test_server)
		sys_log(0, "CInputMain::OfflineShop ==> SubHeader %d", p->subheader);

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);
	
#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot offlineshop with security key activate"));
		return 0;
	}
#endif

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s OFFLINE_SHOP: END", ch->GetName());
			COfflineShopManager::instance().StopShopping(ch);
			return 0;
		case SHOP_SUBHEADER_CG_BUY:
		{
			if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
				return -1;

			BYTE bPos = *(c_pData + 1);
			sys_log(1, "INPUT: %s OFFLINE_SHOP: BUY %d", ch->GetName(), bPos);
			COfflineShopManager::instance().Buy(ch, bPos);
			return (sizeof(BYTE) + sizeof(BYTE));
		}
		case SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP:
			sys_log(1, "INPUT: %s OFFLINE_SHOP_DESTROY", ch->GetName());
			COfflineShopManager::instance().DestroyOfflineShop(ch, ch->GetOfflineShopVID(), true);
			return 0;
		case SHOP_SUBHEADER_CG_ADD_ITEM:
		{
			if (uiBytes < sizeof(TOfflineShopItemTable2))
				return -1;

			TOfflineShopItemTable2 * pTable = (TOfflineShopItemTable2 *)(c_pData);
			COfflineShopManager::instance().AddItem(ch, pTable->bDisplayPos, pTable->bPos, pTable->lPrice, pTable->lPrice2, pTable->sPriceType);
			return (sizeof(TOfflineShopItemTable2));
		}
		case SHOP_SUBHEADER_CG_REMOVE_ITEM:
		{
			if (uiBytes < sizeof(BYTE))
				return -1;

			BYTE bPos = *c_pData;
			sys_log(0, "INPUT: %s REMOVE_ITEM : %d", ch->GetName(), bPos);
			COfflineShopManager::instance().RemoveItem(ch, bPos);
			return (sizeof(BYTE));
		}
		case SHOP_SUBHEADER_CG_REFRESH:
			sys_log(0, "INPUT: %s OFFLINE_SHOP_REFRESH_ITEM", ch->GetName());
			COfflineShopManager::instance().Refresh(ch);
			return 0;
		case SHOP_SUBHEADER_CG_REFRESH_MONEY:
		{
			sys_log(0, "INPUT: %s OFFLINE_SHOP_REFRESH_MONEY", ch->GetName());
			COfflineShopManager::instance().RefreshMoney(ch);
			return 0;
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_MONEY:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			const DWORD gold = *reinterpret_cast<const DWORD*>(c_pData);		
			sys_log(0, "INPUT: %s(%d) OFFLINE_SHOP_WITHDRAW_MONEY", ch->GetName(), gold);	
			COfflineShopManager::instance().WithdrawMoney(ch, gold);
			return (sizeof(DWORD));
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_BAR_1:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			const DWORD bar = *reinterpret_cast<const DWORD*>(c_pData);		
			sys_log(0, "INPUT: %s(%d) SHOP_SUBHEADER_CG_WITHDRAW_BAR_1", ch->GetName(), bar);	
			COfflineShopManager::instance().WithdrawBar1(ch, bar);
			return (sizeof(DWORD));	
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_BAR_2:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			const DWORD bar = *reinterpret_cast<const DWORD*>(c_pData);		
			sys_log(0, "INPUT: %s(%d) SHOP_SUBHEADER_CG_WITHDRAW_BAR_2", ch->GetName(), bar);	
			COfflineShopManager::instance().WithdrawBar2(ch, bar);
			return (sizeof(DWORD));	
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_BAR_3:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			const DWORD bar = *reinterpret_cast<const DWORD*>(c_pData);		
			sys_log(0, "INPUT: %s(%d) SHOP_SUBHEADER_CG_WITHDRAW_BAR_3", ch->GetName(), bar);	
			COfflineShopManager::instance().WithdrawBar3(ch, bar);
			return (sizeof(DWORD));	
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_SOUL_STONE:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			const DWORD soulstone = *reinterpret_cast<const DWORD*>(c_pData);		
			sys_log(0, "INPUT: %s(%d) SHOP_SUBHEADER_CG_WITHDRAW_SOUL_STONE", ch->GetName(), soulstone);	
			COfflineShopManager::instance().WithdrawSoulStone(ch, soulstone);
			return (sizeof(DWORD));	
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_DRAGON_SCALE:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			const DWORD dragonscale = *reinterpret_cast<const DWORD*>(c_pData);		
			sys_log(0, "INPUT: %s(%d) SHOP_SUBHEADER_CG_WITHDRAW_DRAGON_SCALE", ch->GetName(), dragonscale);	
			COfflineShopManager::instance().WithdrawDragonScale(ch, dragonscale);
			return (sizeof(DWORD));	
		}
		case SHOP_SUBHEADER_CG_WITHDRAW_DRAGON_CLAW:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			const DWORD dragonclaw = *reinterpret_cast<const DWORD*>(c_pData);		
			sys_log(0, "INPUT: %s(%d) SHOP_SUBHEADER_CG_WITHDRAW_DRAGON_CLAW", ch->GetName(), dragonclaw);	
			COfflineShopManager::instance().WithdrawDragonClaw(ch, dragonclaw);
			return (sizeof(DWORD));	
		}
#ifdef ENABLE_CHEQUE_SYSTEM
		case SHOP_SUBHEADER_CG_WITHDRAW_CHEQUE:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			const DWORD cheque = *reinterpret_cast<const DWORD*>(c_pData);		
			sys_log(0, "INPUT: %s(%d) SHOP_SUBHEADER_CG_WITHDRAW_CHEQUE", ch->GetName(), cheque);	
			COfflineShopManager::instance().WithdrawCheque(ch, cheque);
			return (sizeof(DWORD));	
		}
#endif
		case SHOP_SUBHEADER_CG_REFRESH_UNSOLD_ITEMS:
		{
			sys_log(0, "INPUT: %s OFFLINE_SHOP_REFRESH_UNSOLD_ITEMS", ch->GetName());
			COfflineShopManager::instance().RefreshUnsoldItems(ch);
			return 0;
		}
		case SHOP_SUBHEADER_CG_TAKE_ITEM:
		{
			if (uiBytes < sizeof(BYTE))
				return -1;

			BYTE bPos = *c_pData;
			sys_log(0, "INPUT: %s OFFLINE_SHOP_TAKE_ITEM", ch->GetName());
			COfflineShopManager::instance().TakeItem(ch, bPos);
			return (sizeof(BYTE));
		}
		case SHOP_SUBHEADER_CG_CHECK:
			COfflineShopManager::instance().HasOfflineShop(ch);
			return 0;
		default:
			sys_err("CInputMain::OfflineShop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}
#endif

#include "target.h"
class CFuncShopView
{
private:
	int dwViewRange;
public:
	LPENTITY m_me;
	const char* szItemName;
	int iJob;
	int iType;
	int iSubType;
	int iMinLevel;
	int iMaxLevel;
	int iMinRefine;
	int iMaxRefine;
	int iMinPrice;
	int iMaxPrice;
	int iMinCheque;
	int iMaxCheque;
	bool bClear;
	CFuncShopView(LPENTITY ent, const char* item_name, int player_job,int item_type, int item_subtype, int item_minlevel, int item_maxlevel, int item_minrefine, int item_maxrefine, int item_minprice, int item_maxprice, int item_mincheque, int item_maxcheque, bool clear) :
		dwViewRange(VIEW_RANGE + VIEW_BONUS_RANGE),
		m_me(ent), szItemName(item_name),iJob(player_job), iType(item_type), iSubType(item_subtype), iMinLevel(item_minlevel), iMaxLevel(item_maxlevel), iMinRefine(item_minrefine), iMaxRefine(item_maxrefine), iMinPrice(item_minprice), iMaxPrice(item_maxprice), iMinCheque(item_mincheque), iMaxCheque(item_maxcheque), bClear(clear)
	{
	}

	void operator () (LPENTITY ent)
	{
		/*if (!ent->IsType(ENTITY_OBJECT))
		if (DISTANCE_APPROX(ent->GetX() - m_me->GetX(), ent->GetY() - m_me->GetY()) > dwViewRange)
		return;*/
		if (!m_me->GetDesc())
			return;
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER chMe = (LPCHARACTER)m_me;
			LPCHARACTER chEnt = (LPCHARACTER)ent;
			if (chEnt->GetRaceNum() == 30000 && chEnt->GetMyShop())
			{
				if (bClear == true)
				{
					//TargetInfo * pInfo_check = CTargetManager::instance().GetTargetInfo(chMe->GetPlayerID(), 3169, chEnt->GetVID());
					//if (pInfo_check)
						//CTargetManager::Instance().DeleteTarget(chMe->GetPlayerID(), chEnt->IsPrivShop() ? chEnt->GetPrivShop() : chEnt->GetPlayerID(), "SHOP_SEARCH_TARGET");
					return;
				}
				LPSHOP shop = chEnt->GetMyShop();
				if (!shop)
					return;
				if (!shop->SearchItem(chMe, szItemName, iJob, iType, iSubType, iMinLevel, iMaxLevel, iMinRefine, iMaxRefine, iMinPrice, iMaxPrice, iMinCheque, iMaxCheque))
					return;
				chMe->SetQuestFlag("shop.search", chMe->GetQuestFlag("shop.search") + 1);
			}
		}
	}
};
void ClearShopSearch(LPCHARACTER ch)
{
	CFuncShopView f(ch, NULL, 0,0,0,0,0,0,0,0,0,0,0, true);
	ch->GetSectree()->ForEachAround(f);
}

void CInputMain::Shop2(LPCHARACTER ch, const char * data)
{
	struct command_shop2 * p = (struct command_shop2 *) data;
	int shop2_subheader = (int)p->subheader;

	if (!ch){
		sys_err("TPacketCGShop2 packet coming from unknown target");
		return;
	}

	int iFloodResult = ch->GetQuestFlag("search.searchlast");
	if (iFloodResult){
		if (get_global_time() < iFloodResult + 1 /* limit */) {
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("HIZLI_YAPAMAZSIN1"));
			return;
		}
	}

	if (shop2_subheader == SHOP2_SUBHEADER_CG_SEARCH)
	{
		if (!p->iType && !p->iSubtype){
			ClearShopSearch(ch);
		}

		sys_log(0, "TPacketCGShop2: SHOP2_SUBHEADER_CG_SEARCH  USER: %s", ch->GetName());


		if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You can not use this in now");
			return;
		}
		
		ClearShopSearch(ch);
		CFuncShopView f(ch, p->cItemNameForSearch, p->iJob, p->iType, p->iSubtype, p->iMinLevel, p->iMaxLevel, p->iMinRefine, p->iMaxRefine, p->iMinPrice, p->iMaxPrice, p->iMinCheque, p->iMaxCheque, false);
		ch->SetQuestFlag("shop.search", 0);
		ch->GetSectree()->ForEachAround(f);
		int c = ch->GetQuestFlag("shop.search");
		if (c>0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Found in %d shops"), c);
			ch->SetQuestFlag("shop.search", 0);
		}
		ch->SetQuestFlag("shop.searchlast", get_global_time() + 2);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CreateSearchedItemList");
	}
	else if (shop2_subheader == SHOP2_SUBHEADER_CG_BUY)
	{
		{
			sys_log(0, "TPacketCGShop2: SHOP2_SUBHEADER_CG_BUY  USER: %s POS: %d SELLER: %d", ch->GetName(), p->bItemPos, p->iVid);

			if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "You can not use this in now");
				return;
			}
			
			LPCHARACTER pkVic = CHARACTER_MANAGER::instance().Find(p->iVid);
			
			if (!pkVic)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Oyuncu bulunamadi.");
				return;
			}
			//if (!CTargetManager::instance().GetTargetInfo(ch->GetPlayerID(), 3169, p->iVid))
				//CTargetManager::Instance().CreateTarget(ch->GetPlayerID(), pkVic->IsPrivShop() ? pkVic->GetPrivShop() : pkVic->GetPlayerID(), "SHOP_SEARCH_TARGET", 3169, pkVic->GetVID(), 0, pkVic->GetMapIndex(), "shop");
			
			ch->SetQuestFlag("item_search.flood_check", get_global_time());
		}
	}
	else
	{
		sys_err("CInputMain::Shop2 : Unknown subheader '%d' - From: %s", shop2_subheader, ch->GetName());
		return;
	}
}

void CInputMain::OnClick(LPCHARACTER ch, const char * data)
{
	struct command_on_click *	pinfo = (struct command_on_click *) data;
	LPCHARACTER			victim;

#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot onclick with security key activate"));
		return;
	}
#endif

	if ((victim = CHARACTER_MANAGER::instance().Find(pinfo->vid)))
		victim->OnClick(ch);
	else if (test_server)
	{
		sys_err("CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]", ch->GetName(), pinfo->vid);
	}
}

void CInputMain::Exchange(LPCHARACTER ch, const char * data)
{
	struct command_exchange * pinfo = (struct command_exchange *) data;
	LPCHARACTER	to_ch = NULL;

	if (!ch->CanHandleItem())
		return;

	int iPulse = thecore_pulse();

	if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
	{
		if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°Å·¡ ÈÄ %dÃÊ ÀÌ³»¿¡ Ã¢°í¸¦ ¿­¼ö ¾ø½À´Ï´Ù."), g_nPortalLimitTime);
			return;
		}

		if( true == to_ch->IsDead() )
		{
			return;
		}
	}

	sys_log(0, "CInputMain()::Exchange()  SubHeader %d ", pinfo->sub_header);

	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°Å·¡ ÈÄ %dÃÊ ÀÌ³»¿¡ Ã¢°í¸¦ ¿­¼ö ¾ø½À´Ï´Ù."), g_nPortalLimitTime);
		return;
	}


	switch (pinfo->sub_header)
	{
		case EXCHANGE_SUBHEADER_CG_START:	// arg1 == vid of target character
			if (!ch->GetExchange())
			{
				if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
				{
					//MONARCH_LIMIT
					/*
					if (to_ch->IsMonarch() || ch->IsMonarch())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¿Í´Â °Å·¡¸¦ ÇÒ¼ö°¡ ¾ø½À´Ï´Ù"), g_nPortalLimitTime);
						return;
					}
					//END_MONARCH_LIMIT
					*/
					if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã¢°í¸¦ ¿¬ÈÄ %dÃÊ ÀÌ³»¿¡´Â °Å·¡¸¦ ÇÒ¼ö ¾ø½À´Ï´Ù."), g_nPortalLimitTime);

						if (test_server)
							ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã¢°í¸¦ ¿¬ÈÄ %dÃÊ ÀÌ³»¿¡´Â °Å·¡¸¦ ÇÒ¼ö ¾ø½À´Ï´Ù."), g_nPortalLimitTime);


						if (test_server)
							to_ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, to_ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (ch->GetGold() >= GOLD_MAX)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾×¼ö°¡ 20¾ï ³ÉÀ» ÃÊ°úÇÏ¿© °Å·¡¸¦ ÇÒ¼ö°¡ ¾ø½À´Ï´Ù.."));

						sys_err("[OVERFLOG_GOLD] START (%u) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
						return;
					}

					if (to_ch->IsPC())
					{
						if (quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(), to_ch))
						{
							sys_log(0, "Exchange canceled by quest %s %s", ch->GetName(), to_ch->GetName());
							return;
						}
					}

					if (ch->GetMyShop() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->IsCombOpen() || ch->GetOfflineShopOwner())

					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡ÁßÀÏ°æ¿ì °³ÀÎ»óÁ¡À» ¿­¼ö°¡ ¾ø½À´Ï´Ù."));
						return;
					}

#ifdef WJ_ITEM_COMBINATION_SYSTEM
					if (ch->IsCombOpen())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡ÁßÀÏ°æ¿ì °³ÀÎ»óÁ¡À» ¿­¼ö°¡ ¾ø½À´Ï´Ù."));
						return;
					}
#endif

					ch->ExchangeStart(to_ch);
				}
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_ADD:	// arg1 == position of item, arg2 == position in exchange window
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_DEL:	// arg1 == position of item
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->RemoveItem(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ELK_ADD:	// arg1 == amount of gold
			if (ch->GetExchange())
			{
				const int64_t nTotalGold = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<int64_t>(pinfo->arg1);

				if (GOLD_MAX <= nTotalGold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÇ ÃÑ±Ý¾×ÀÌ 20¾ï ³ÉÀ» ÃÊ°úÇÏ¿© °Å·¡¸¦ ÇÒ¼ö°¡ ¾ø½À´Ï´Ù.."));

					sys_err("[OVERFLOW_GOLD] ELK_ADD (%u) id %u name %s ",
							ch->GetExchange()->GetCompany()->GetOwner()->GetGold(),
							ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
						   	ch->GetExchange()->GetCompany()->GetOwner()->GetName());

					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddGold(pinfo->arg1);
			}
			break;
#ifdef ENABLE_CHEQUE_SYSTEM
		case EXCHANGE_SUBHEADER_CG_CHEQUE_ADD:	// arg1 == amount of cheque
			if (ch->GetExchange())
			{
				const int16_t nTotalCheque = static_cast<int16_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetCheque()) + static_cast<int16_t>(pinfo->arg1);

				if (CHEQUE_MAX <= nTotalCheque)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other player has cheque over the limite."));

					sys_err("[OVERFLOW_CHEQUE] CHEQUE_ADD (%u) id %u name %s ",
							ch->GetExchange()->GetCompany()->GetOwner()->GetCheque(),
							ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
						   	ch->GetExchange()->GetCompany()->GetOwner()->GetName());

					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddCheque(pinfo->arg1);
			}
			break;
#endif

		case EXCHANGE_SUBHEADER_CG_ACCEPT:	// arg1 == not used
			if (ch->GetExchange())
			{
				sys_log(0, "CInputMain()::Exchange() ==> ACCEPT ");
				ch->GetExchange()->Accept(true);
			}

			break;

		case EXCHANGE_SUBHEADER_CG_CANCEL:	// arg1 == not used
			if (ch->GetExchange())
				ch->GetExchange()->Cancel();
			break;
	}
}

void CInputMain::Position(LPCHARACTER ch, const char * data)
{
	struct command_position * pinfo = (struct command_position *) data;

	switch (pinfo->position)
	{
		case POSITION_GENERAL:
			ch->Standup();
			break;

		case POSITION_SITTING_CHAIR:
			ch->Sitdown(0);
			break;

		case POSITION_SITTING_GROUND:
			ch->Sitdown(1);
			break;
	}
}

static const int ComboSequenceBySkillLevel[3][8] =
{
	// 0   1   2   3   4   5   6   7
	{ 14, 15, 16, 17,  0,  0,  0,  0 },
	{ 14, 15, 16, 18, 20,  0,  0,  0 },
	{ 14, 15, 16, 18, 19, 17,  0,  0 },
};

#define COMBO_HACK_ALLOWABLE_MS	100

// [2013 09 11 CYH]
DWORD ClacValidComboInterval( LPCHARACTER ch, BYTE bArg )
{
	int nInterval = 300;
	float fAdjustNum = 1.5f; // ÀÏ¹Ý À¯Àú°¡ speed hack ¿¡ °É¸®´Â °ÍÀ» ¸·±â À§ÇØ. 2013.09.10 CYH

	if( !ch )
	{
		sys_err( "ClacValidComboInterval() ch is NULL");
		return nInterval;
	}	

	if( bArg == 13 )
	{
		float normalAttackDuration = CMotionManager::instance().GetNormalAttackDuration(ch->GetRaceNum());
		nInterval = (int) (normalAttackDuration / (((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f) + fAdjustNum );
	}
	else if( bArg == 14 )
	{		
		nInterval = (int)(ani_combo_speed(ch, 1 ) / ((ch->GetPoint(POINT_ATT_SPEED) / 100.f) + fAdjustNum) );
	}
	else if( bArg > 14 && bArg << 22 )
	{
		nInterval = (int)(ani_combo_speed(ch, bArg - 13 ) / ((ch->GetPoint(POINT_ATT_SPEED) / 100.f) + fAdjustNum) );
	}
	else
	{
		sys_err( "ClacValidComboInterval() Invalid bArg(%d) ch(%s)", bArg, ch->GetName() );		
	}	

	return nInterval;
}

bool CheckComboHack(LPCHARACTER ch, BYTE bArg, DWORD dwTime, bool CheckSpeedHack)
{
	if (ch->IsStun() || ch->IsDead())
		return false;
#ifdef COSTUME_WEAPON
	CItem * pCostumeWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
	if (pCostumeWeapon)
		return false;
#endif
	int ComboInterval = dwTime - ch->GetLastComboTime();
	int HackScalar = 0; // ±âº» ½ºÄ®¶ó ´ÜÀ§ 1

	// [2013 09 11 CYH] debugging log
		/*sys_log(0, "COMBO_TEST_LOG: %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
						ch->GetName(),
						bArg,
						ComboInterval,
						ch->GetValidComboInterval(),
						ch->GetPoint(POINT_ATT_SPEED),
						ch->IsRiding() ? "yes" : "no");*/

#if 0	
	sys_log(0, "COMBO: %s arg:%u seq:%u delta:%d checkspeedhack:%d",
			ch->GetName(), bArg, ch->GetComboSequence(), ComboInterval - ch->GetValidComboInterval(), CheckSpeedHack);
#endif

	if (bArg == 14)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 300;
			//sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s", ch->GetName(), bArg, ComboInterval, ch->GetValidComboInterval(), ch->GetPoint(POINT_ATT_SPEED), ch->IsRiding() ? "yes" : "no");
		}

		ch->SetComboSequence(1);
		ch->SetValidComboInterval( ClacValidComboInterval(ch, bArg) );
		ch->SetLastComboTime(dwTime);
	}
	else if (bArg > 14 && bArg < 22)
	{
		int idx = MIN(2, ch->GetComboIndex());

		if (ch->GetComboSequence() > 5)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);
			sys_log(0, "COMBO_HACK: 5 %s combo_seq:%d", ch->GetName(), ch->GetComboSequence());
		}
		// AU¡Æ¢¥ ¨öO¨ùo A¨­¨¬¢¬ ¢¯©ö¢¯UA©ø¢¬¢ç
		else if (bArg == 21 &&
				 idx == 2 &&
				 ch->GetComboSequence() == 5 &&
				 ch->GetJob() == JOB_ASSASSIN &&
				 ch->GetWear(WEAR_WEAPON) &&
				 ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_DAGGER)
			ch->SetValidComboInterval(300);
#ifdef ENABLE_WOLFMAN_CHARACTER
		else if (bArg == 21 && idx == 2 && ch->GetComboSequence() == 5 && ch->GetJob() == JOB_WOLFMAN && ch->GetWear(WEAR_WEAPON) && ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_CLAW)
			ch->SetValidComboInterval(300);
#endif
		else if (ComboSequenceBySkillLevel[idx][ch->GetComboSequence()] != bArg)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);

			sys_log(0, "COMBO_HACK: 3 %s arg:%u valid:%u combo_idx:%d combo_seq:%d",
					ch->GetName(),
					bArg,
					ComboSequenceBySkillLevel[idx][ch->GetComboSequence()],
					idx,
					ch->GetComboSequence());
		}
		else
		{
			if (CheckSpeedHack && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
			{
				HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

				sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
						ch->GetName(),
						bArg,
						ComboInterval,
						ch->GetValidComboInterval(),
						ch->GetPoint(POINT_ATT_SPEED),
						ch->IsRiding() ? "yes" : "no");
			}

			if (ch->IsRiding())
				ch->SetComboSequence(ch->GetComboSequence() == 1 ? 2 : 1);
			else
				ch->SetComboSequence(ch->GetComboSequence() + 1);

			ch->SetValidComboInterval( ClacValidComboInterval(ch, bArg) );
			ch->SetLastComboTime(dwTime);
		}
	}
	else if (bArg == 13)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;
			//sys_log(0, "COMBO_HACK: 6 %s arg:%u interval:%d valid:%u atkspd:%u", ch->GetName(), bArg, ComboInterval, ch->GetValidComboInterval(), ch->GetPoint(POINT_ATT_SPEED));
		}

		if (ch->GetRaceNum() >= MAIN_RACE_MAX_NUM)
		{
			// POLYMORPH_BUG_FIX
			
			// DELETEME
			/*
			const CMotion * pkMotion = CMotionManager::instance().GetMotion(ch->GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, MOTION_NORMAL_ATTACK));

			if (!pkMotion)
				sys_err("cannot find motion by race %u", ch->GetRaceNum());
			else
			{
				// Á¤»óÀû °è»êÀÌ¶ó¸é 1000.f¸¦ °öÇØ¾ß ÇÏÁö¸¸ Å¬¶óÀÌ¾ðÆ®°¡ ¾Ö´Ï¸ÞÀÌ¼Ç ¼ÓµµÀÇ 90%¿¡¼­
				// ´ÙÀ½ ¾Ö´Ï¸ÞÀÌ¼Ç ºí·»µùÀ» Çã¿ëÇÏ¹Ç·Î 900.f¸¦ °öÇÑ´Ù.
				int k = (int) (pkMotion->GetDuration() / ((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f);
				ch->SetValidComboInterval(k);
				ch->SetLastComboTime(dwTime);
			}
			*/

			// 2013 09 11 CYH edited
			//float normalAttackDuration = CMotionManager::instance().GetNormalAttackDuration(ch->GetRaceNum());
			//int k = (int) (normalAttackDuration / ((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f);			
			//ch->SetValidComboInterval(k);
			ch->SetValidComboInterval( ClacValidComboInterval(ch, bArg) );
			ch->SetLastComboTime(dwTime);
			// END_OF_POLYMORPH_BUG_FIX
		}
		else
		{
			//if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
			//{
			//	LogManager::instance().HackLog("Hacker", ch);
			//	sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
			//}
		}
	}
	else
	{
		if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
		{
			LogManager::instance().HackLog("Hacker", ch);
			sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
		}

		HackScalar = 10;
		ch->SetValidComboInterval(300);
	}

	if (HackScalar)
	{
		if (get_dword_time() - ch->GetLastMountTime() > 1500)
			ch->IncreaseComboHackCount(1 + HackScalar);

		ch->SkipComboAttackByTime(ch->GetValidComboInterval());
	}

	return HackScalar;
}

void CInputMain::Move(LPCHARACTER ch, const char * data)
{
	if (!ch->CanMove())
		return;

	#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move item with security key activate"));
		return;
	}
	#endif

	struct command_move * pinfo = (struct command_move *) data;

	if (pinfo->bFunc >= FUNC_MAX_NUM && !(pinfo->bFunc & 0x80))
	{
		sys_err("invalid move type: %s", ch->GetName());
		return;
	}

#ifdef SURA_RUHU_ENGEL
	int SuraDist = DISTANCE_APPROX(ch->GetX() - 180200, ch->GetY() - 1220700);
	if (SuraDist <= quest::CQuestManager::instance().GetEventFlag("mesafe"))
	{

		ch->ChatPacket(CHAT_TYPE_INFO, "Sura Ruhuna bu kadar yaklaþamazs?!");
		ch->SetQuestFlag("ryuuku.ruhengel", ch->GetQuestFlag("ryuuku.ruhengel") + 1);
		if (ch->GetQuestFlag("ryuuku.ruhengel") < 5)
			ch->ChatPacket(CHAT_TYPE_INFO, "Hata: %d 5. uyar?a karakterin ?d??ecektir !", ch->GetQuestFlag("ryuuku.ruhengel"));

		ch->Show(73, 180354, 1220479);
		ch->Stop();

		if (ch->GetQuestFlag("ryuuku.ruhengel") >= 5)
		{
			ch->Dead();
			ch->SetQuestFlag("ryuuku.ruhengel", 0);
		}
	}
#endif

	//enum EMoveFuncType
	//{
	//	FUNC_WAIT,
	//	FUNC_MOVE,
	//	FUNC_ATTACK,
	//	FUNC_COMBO,
	//	FUNC_MOB_SKILL,
	//	_FUNC_SKILL,
	//	FUNC_MAX_NUM,
	//	FUNC_SKILL = 0x80,
	//};

	// ÅÚ·¹Æ÷Æ® ÇÙ Ã¼Å©

//	if (!test_server)
	{
		const float fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);

		if (((false == ch->IsRiding() && fDist > 40) || fDist > 60) && OXEVENT_MAP_INDEX != ch->GetMapIndex())
		{
			if( false == LC_IsEurope() )
			{
				const PIXEL_POSITION & warpPos = ch->GetWarpPosition();

				if (warpPos.x == 0 && warpPos.y == 0)
					LogManager::instance().HackLog("Teleport", ch); // ºÎÁ¤È®ÇÒ ¼ö ÀÖÀ½
			}

			sys_log(0, "MOVE: %s trying to move too far (dist: %.1fm) Riding(%d)", ch->GetName(), fDist, ch->IsRiding());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}

#ifdef ENABLE_CHECK_GHOSTMODE
		if (ch->IsPC() && ch->IsDead())
		{
			sys_log(0, "MOVE: %s trying to move as dead", ch->GetName());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
#endif

		//
		// ½ºÇÇµåÇÙ(SPEEDHACK) Check
		//
		DWORD dwCurTime = get_dword_time();
		// ½Ã°£À» SyncÇÏ°í 7ÃÊ ÈÄ ºÎÅÍ °Ë»çÇÑ´Ù. (20090702 ÀÌÀü¿£ 5ÃÊ¿´À½)
		bool CheckSpeedHack = (false == ch->GetDesc()->IsHandshaking() && dwCurTime - ch->GetDesc()->GetClientTime() > 7000);

		if (CheckSpeedHack)
		{
			int iDelta = (int) (pinfo->dwTime - ch->GetDesc()->GetClientTime());
			int iServerDelta = (int) (dwCurTime - ch->GetDesc()->GetClientTime());

			iDelta = (int) (dwCurTime - pinfo->dwTime);

			// ½Ã°£ÀÌ ´Ê°Ô°£´Ù. ÀÏ´Ü ·Î±×¸¸ ÇØµÐ´Ù. ÁøÂ¥ ÀÌ·± »ç¶÷µéÀÌ ¸¹ÀºÁö Ã¼Å©ÇØ¾ßÇÔ. TODO
			if (iDelta >= 30000)
			{
				sys_log(0, "SPEEDHACK: slow timer name %s delta %d", ch->GetName(), iDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}
			// 1ÃÊ¿¡ 20msec »¡¸® °¡´Â°Å ±îÁö´Â ÀÌÇØÇÑ´Ù.
			else if (iDelta < -(iServerDelta / 50))
			{
				sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", ch->GetName(), iDelta, iServerDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}
		}

		//
		// ÄÞº¸ÇÙ ¹× ½ºÇÇµåÇÙ Ã¼Å©
		//
		if (pinfo->bFunc == FUNC_COMBO && g_bCheckMultiHack)
		{
			CheckComboHack(ch, pinfo->bArg, pinfo->dwTime, CheckSpeedHack); // ÄÞº¸ Ã¼Å©
		}
	}

	if (pinfo->bFunc == FUNC_MOVE)
	{
		if (ch->GetLimitPoint(POINT_MOV_SPEED) == 0)
			return;

		ch->SetRotation(pinfo->bRot * 5);	// Áßº¹ ÄÚµå
		ch->ResetStopTime();				// ""

		ch->Goto(pinfo->lX, pinfo->lY);
	}
	else
	{
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
			ch->OnMove(true);
		else if (pinfo->bFunc & FUNC_SKILL)
		{
			const int MASK_SKILL_MOTION = 0x7F;
			unsigned int motion = pinfo->bFunc & MASK_SKILL_MOTION;

			if (!ch->IsUsableSkillMotion(motion))
			{
				const char* name = ch->GetName();
				unsigned int job = ch->GetJob();
				unsigned int group = ch->GetSkillGroup();

				char szBuf[256];
				snprintf(szBuf, sizeof(szBuf), "SKILL_HACK: name=%s, job=%d, group=%d, motion=%d", name, job, group, motion);
				LogManager::instance().HackLog(szBuf, ch->GetDesc()->GetAccountTable().login, ch->GetName(), ch->GetDesc()->GetHostName());
				//sys_log(0, "%s", szBuf);

				if (test_server)
				{
					ch->GetDesc()->DelayedDisconnect(number(2, 8));
					ch->ChatPacket(CHAT_TYPE_INFO, szBuf);
				}
				else
				{
					ch->GetDesc()->DelayedDisconnect(number(150, 500));
				}
			}

			ch->OnMove();
		}

		ch->SetRotation(pinfo->bRot * 5);	// Áßº¹ ÄÚµå
		ch->ResetStopTime();				// ""

		ch->Move(pinfo->lX, pinfo->lY);
		ch->Stop();
		ch->StopStaminaConsume();
	}

	TPacketGCMove pack;

	pack.bHeader      = HEADER_GC_MOVE;
	pack.bFunc        = pinfo->bFunc;
	pack.bArg         = pinfo->bArg;
	pack.bRot         = pinfo->bRot;
	pack.dwVID        = ch->GetVID();
	pack.lX           = pinfo->lX;
	pack.lY           = pinfo->lY;
	pack.dwTime       = pinfo->dwTime;
	pack.dwDuration   = (pinfo->bFunc == FUNC_MOVE) ? ch->GetCurrentMoveDuration() : 0;

	ch->PacketAround(&pack, sizeof(TPacketGCMove), ch);
/*
	if (pinfo->dwTime == 10653691) // µð¹ö°Å ¹ß°ß
	{
		if (ch->GetDesc()->DelayedDisconnect(number(15, 30)))
			LogManager::instance().HackLog("Debugger", ch);

	}
	else if (pinfo->dwTime == 10653971) // Softice ¹ß°ß
	{
		if (ch->GetDesc()->DelayedDisconnect(number(15, 30)))
			LogManager::instance().HackLog("Softice", ch);
	}
*/
	/*
	sys_log(0,
			"MOVE: %s Func:%u Arg:%u Pos:%dx%d Time:%u Dist:%.1f",
			ch->GetName(),
			pinfo->bFunc,
			pinfo->bArg,
			pinfo->lX / 100,
			pinfo->lY / 100,
			pinfo->dwTime,
			fDist);
	*/
}

#ifdef __SKILL_COLOR_SYSTEM__
void CInputMain::SetSkillColor(LPCHARACTER ch, const char* pcData)
{
	TPacketCGSkillColor * p = (TPacketCGSkillColor*)pcData;

	if (p->skill >= ESkillColorLength::MAX_SKILL_COUNT)
		return;

	DWORD data[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memcpy(data, ch->GetSkillColor(), sizeof(data));

	data[p->skill][0] = p->col1;
	data[p->skill][1] = p->col2;
	data[p->skill][2] = p->col3;
	data[p->skill][3] = p->col4;
	data[p->skill][4] = p->col5;

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have changed the color of your skill."));

	ch->SetSkillColor(data[0]);

	TSkillColor db_pack;
	memcpy(db_pack.dwSkillColor, data, sizeof(data));
	db_pack.player_id = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_SKILL_COLOR_SAVE, 0, sizeof(TSkillColor));
	db_clientdesc->Packet(&db_pack, sizeof(TSkillColor));
}
#endif

void CInputMain::Attack(LPCHARACTER ch, const BYTE header, const char* data)
{
	if (NULL == ch)
		return;

	struct type_identifier
	{
		BYTE header;
		BYTE type;
	};

	const struct type_identifier* const type = reinterpret_cast<const struct type_identifier*>(data);

	if (type->type > 0)
	{
		if (false == ch->CanUseSkill(type->type))
		{
			return;
		}

		switch (type->type)
		{
			case SKILL_GEOMPUNG:
			case SKILL_SANGONG:
			case SKILL_YEONSA:
			case SKILL_KWANKYEOK:
			case SKILL_HWAJO:
			case SKILL_GIGUNG:
			case SKILL_PABEOB:
			case SKILL_MARYUNG:
			case SKILL_TUSOK:
			case SKILL_MAHWAN:
			case SKILL_BIPABU:
			case SKILL_NOEJEON:
			case SKILL_CHAIN:
			case SKILL_ILGWANGPYO:
			case SKILL_MABEOBAGGWI:
			case SKILL_METEO:
			case SKILL_HORSE_WILDATTACK_RANGE:
				if (HEADER_CG_SHOOT != type->header)
				{
					if (test_server)
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Attack :name[%s] Vnum[%d] can't use skill by attack(warning)"), type->type);
					return;
				}
				break;
		}
	}

	switch (header)
	{
		case HEADER_CG_ATTACK:
			{
				if (NULL == ch->GetDesc())
					return;

				const TPacketCGAttack* const packMelee = reinterpret_cast<const TPacketCGAttack*>(data);

				ch->GetDesc()->AssembleCRCMagicCube(packMelee->bCRCMagicCubeProcPiece, packMelee->bCRCMagicCubeFilePiece);

				LPCHARACTER	victim = CHARACTER_MANAGER::instance().Find(packMelee->dwVID);

				if (NULL == victim || ch == victim)
					return;

				switch (victim->GetCharType())
				{
					case CHAR_TYPE_NPC:
					case CHAR_TYPE_WARP:
					case CHAR_TYPE_GOTO:
						return;
				}

				if (packMelee->bType > 0)
				{
					if (false == ch->CheckSkillHitCount(packMelee->bType, victim->GetVID()))
					{
						return;
					}
				}

				ch->Attack(victim, packMelee->bType);
			}
			break;

		case HEADER_CG_SHOOT:
			{
				const TPacketCGShoot* const packShoot = reinterpret_cast<const TPacketCGShoot*>(data);

				ch->Shoot(packShoot->bType);
			}
			break;
	}
}

int CInputMain::SyncPosition(LPCHARACTER ch, const char * c_pcData, size_t uiBytes)
{
	const TPacketCGSyncPosition* pinfo = reinterpret_cast<const TPacketCGSyncPosition*>( c_pcData );

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGSyncPosition);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		//ch->GetDesc()->SetPhase(PHASE_CLOSE);
		//return -1;
	}

	if (0 != (iExtraLen % sizeof(TPacketCGSyncPositionElement)))
	{
		sys_err("invalid packet length %d (name: %s)", pinfo->wSize, ch->GetName());
		return iExtraLen;
	}

	int iCount = iExtraLen / sizeof(TPacketCGSyncPositionElement);

	if (iCount <= 0)
		return iExtraLen;

	static const int nCountLimit = 32;//FIX SyncPosition 6/11/2020

	if( iCount > nCountLimit )
	{
		iCount = nCountLimit;
	}

	TEMP_BUFFER tbuf;
	LPBUFFER lpBuf = tbuf.getptr();

	TPacketGCSyncPosition * pHeader = (TPacketGCSyncPosition *) buffer_write_peek(lpBuf);
	buffer_write_proceed(lpBuf, sizeof(TPacketGCSyncPosition));

	const TPacketCGSyncPositionElement* e =
		reinterpret_cast<const TPacketCGSyncPositionElement*>(c_pcData + sizeof(TPacketCGSyncPosition));

	timeval tvCurTime;
	gettimeofday(&tvCurTime, NULL);

	for (int i = 0; i < iCount; ++i, ++e)
	{
		LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(e->dwVID);

		if (!victim)
			continue;

		switch (victim->GetCharType())
		{
			case CHAR_TYPE_NPC:
			case CHAR_TYPE_WARP:
			case CHAR_TYPE_GOTO:
				continue;
		}

		// ¼ÒÀ¯±Ç °Ë»ç
		if (!victim->SetSyncOwner(ch))
			continue;

		const float fDistWithSyncOwner = DISTANCE_SQRT( (victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100 );
		static const float fLimitDistWithSyncOwner = 2500.f + 1000.f;

		const float fDist = DISTANCE_SQRT( (victim->GetX() - e->lX) / 100, (victim->GetY() - e->lY) / 100 );
		static const long g_lValidSyncInterval = 100 * 1000; // 100ms
		const timeval &tvLastSyncTime = victim->GetLastSyncTime();
		timeval *tvDiff = timediff(&tvCurTime, &tvLastSyncTime);

		// SyncPositionÀ» ¾Ç¿ëÇÏ¿© Å¸À¯Àú¸¦ ÀÌ»óÇÑ °÷À¸·Î º¸³»´Â ÇÙ ¹æ¾îÇÏ±â À§ÇÏ¿©,
		// °°Àº À¯Àú¸¦ g_lValidSyncInterval ms ÀÌ³»¿¡ ´Ù½Ã SyncPositionÇÏ·Á°í ÇÏ¸é ÇÙÀ¸·Î °£ÁÖ.
		if (tvDiff->tv_sec == 0 && tvDiff->tv_usec < g_lValidSyncInterval)
		{
			// g_iSyncHackLimitCount¹ø ±îÁö´Â ºÁÁÜ.
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
		}
		else
		{
			victim->SetLastSyncTime(tvCurTime);
			victim->Sync(e->lX, e->lY);
			buffer_write(lpBuf, e, sizeof(TPacketCGSyncPositionElement));
		}
	}

	if (buffer_size(lpBuf) != sizeof(TPacketGCSyncPosition))
	{
		pHeader->bHeader = HEADER_GC_SYNC_POSITION;
		pHeader->wSize = buffer_size(lpBuf);

		ch->PacketAround(buffer_read_peek(lpBuf), buffer_size(lpBuf), ch);
	}

	return iExtraLen;
}

void CInputMain::FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader)
{
	TPacketCGFlyTargeting * p = (TPacketCGFlyTargeting *) pcData;
	ch->FlyTarget(p->dwTargetVID, p->x, p->y, bHeader);
}

void CInputMain::UseSkill(LPCHARACTER ch, const char * pcData)
{
#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot use skill with security key activate"));
		return;
	}
#endif
	TPacketCGUseSkill * p = (TPacketCGUseSkill *) pcData;
	ch->UseSkill(p->dwVnum, CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::ScriptButton(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptButton * p = (TPacketCGScriptButton *) c_pData;
	sys_log(0, "QUEST ScriptButton pid %d idx %u", ch->GetPlayerID(), p->idx);

	quest::PC* pc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pc && pc->IsConfirmWait())
	{
		quest::CQuestManager::instance().Confirm(ch->GetPlayerID(), quest::CONFIRM_TIMEOUT);
	}
	else if (p->idx & 0x80000000)
	{
		quest::CQuestManager::Instance().QuestInfo(ch->GetPlayerID(), p->idx & 0x7fffffff);
	}
	else
	{
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx);
	}
}

void CInputMain::ScriptAnswer(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptAnswer * p = (TPacketCGScriptAnswer *) c_pData;
	sys_log(0, "QUEST ScriptAnswer pid %d answer %d", ch->GetPlayerID(), p->answer);

	if (p->answer > 250) // ´ÙÀ½ ¹öÆ°¿¡ ´ëÇÑ ÀÀ´äÀ¸·Î ¿Â ÆÐÅ¶ÀÎ °æ¿ì
	{
		quest::CQuestManager::Instance().Resume(ch->GetPlayerID());
	}
	else // ¼±ÅÃ ¹öÆ°À» °ñ¶ó¼­ ¿Â ÆÐÅ¶ÀÎ °æ¿ì
	{
		quest::CQuestManager::Instance().Select(ch->GetPlayerID(),  p->answer);
	}
}


// SCRIPT_SELECT_ITEM
void CInputMain::ScriptSelectItem(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*) c_pData;
	sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", ch->GetPlayerID(), p->selection);
	quest::CQuestManager::Instance().SelectItem(ch->GetPlayerID(), p->selection);
}
// END_OF_SCRIPT_SELECT_ITEM

void CInputMain::QuestInputString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputString * p = (TPacketCGQuestInputString*) c_pData;

	char msg[65];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}

void CInputMain::QuestConfirm(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*) c_pData;
	LPCHARACTER ch_wait = CHARACTER_MANAGER::instance().FindByPID(p->requestPID);
	if (p->answer)
		p->answer = quest::CONFIRM_YES;
	sys_log(0, "QuestConfirm from %s pid %u name %s answer %d", ch->GetName(), p->requestPID, (ch_wait)?ch_wait->GetName():"", p->answer);
	if (ch_wait)
	{
		quest::CQuestManager::Instance().Confirm(ch_wait->GetPlayerID(), (quest::EQuestConfirmType) p->answer, ch->GetPlayerID());
	}
}

void CInputMain::Target(LPCHARACTER ch, const char * pcData)
{
	TPacketCGTarget * p = (TPacketCGTarget *) pcData;

	building::LPOBJECT pkObj = building::CManager::instance().FindObjectByVID(p->dwVID);

	if (pkObj)
	{
		TPacketGCTarget pckTarget;
		pckTarget.header = HEADER_GC_TARGET;
		pckTarget.dwVID = p->dwVID;
		ch->GetDesc()->Packet(&pckTarget, sizeof(TPacketGCTarget));
	}
	else
		ch->SetTarget(CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::Warp(LPCHARACTER ch, const char * pcData)
{
	ch->WarpEnd();
}

#ifdef ENABLE_ADMIN_BAN_MANAGER
enum ETypeActionBan
{
	ACTION_BAN_PERMANENTLY = 0,
	ACTION_BAN_IP = 1	,	
	ACTION_BAN_TIME = 2,
};

bool CheckIsStaffAdmin(LPCHARACTER ch)
{
	const char* arListMembers[9] = /* Here add people member from staff what you want to have acces on administration area ban, unlimited slots !*/
	{
		"EDW4RD",
		"[GM]EDW4RD",
		"[GM]TheBesT",
		"[E2]CRYSTL",
		"[SGM]EDW4RD",
		"[TL]EDW4RD",
		"[GA]TheBesT",
		"[SGA]EDW4RD",
		"[TEC]EDW4RD"
	};

	for(unsigned int key = 0; key < _countof(arListMembers); key++) {
		return ((!strcmp(arListMembers[key], ch->GetName())) && ch->GetGMLevel() > GM_PLAYER) ? true : false;
	}
}

bool CheckIsBlocked(LPCHARACTER ch, const char* c_szName)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT account_id FROM player.player WHERE name = '%s'", c_szName));
	MYSQL_ROW row_id = mysql_fetch_row(pMsg->Get()->pSQLResult);
			
	if (pMsg->uiSQLErrno != 0 || !pMsg->Get()->uiNumRows)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("admin_manager_error_cannot_find_user"), c_szName);
		return false;
	}
			
	int account_id = 0;
	str_to_number(account_id, row_id[0]);

	std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery("SELECT status FROM account.account WHERE id = '%d'", account_id));
	MYSQL_ROW row_inf = mysql_fetch_row(pMsg2->Get()->pSQLResult);

	const char* c_szStatus = row_inf[0];
	
	if (!strcmp(c_szStatus, "BLOCK"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("admin_manager_error_is_blocked"), c_szName);	
		return true;
	}
	
	return false;
}

void InsertLogsBan(const char* c_pszWho, const char* c_pszAction, const char* c_pszVictim, const char* c_pszReason)
{
	DBManager::Instance().DirectQuery("INSERT INTO log.tool_ban (who, action, victim, reason, date) VALUES('%s', '%s', '%s', '%s', NOW())", c_pszWho, c_pszAction, c_pszVictim, c_pszReason);		
}

void CInputMain::SendAdminBanManager(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGAdminBanManger* p = reinterpret_cast<const TPacketCGAdminBanManger*>(c_pData);
	
	if (!ch)
		return;
	
	if (!CheckIsStaffAdmin(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "<Server> Error 0x01.");
		return;
	}
	
	char szEscapedUserName[CHAT_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szEscapedUserName, sizeof(szEscapedUserName), p->user_name, strlen(p->user_name));
				
	char szEscapedReason[CHAT_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szEscapedReason, sizeof(szEscapedReason), p->reason, strlen(p->reason));
	
	if (!strcmp(szEscapedUserName, ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("admin_manager_error_yourself"));	
		return;
	}
	
	if (CheckIsBlocked(ch, szEscapedUserName))
		return;

	switch (p->action)
	{
		case ACTION_BAN_PERMANENTLY:
		case ACTION_BAN_TIME:
		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT name, account_id FROM player.player WHERE name = '%s'", szEscapedUserName));
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			
			if (pMsg->uiSQLErrno != 0 || !pMsg->Get()->uiNumRows)
				return;
			
			int account_id = 0;
			str_to_number(account_id, row[1]);
			
			if (p->action == ACTION_BAN_PERMANENTLY)
			{
				DBManager::Instance().DirectQuery("UPDATE account.account SET status = 'BLOCK', ban_reason = '%s' WHERE id = '%d'", p->reason, account_id);
				InsertLogsBan(ch->GetName(), "ACTION_BAN_PERMANENTLY", szEscapedUserName, p->reason);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("admin_manager_succes_ban_permanently"), szEscapedUserName);
			}
			else
			{
				if (p->duration < 300)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("admin_manager_error_timer"));
					return;	
				}
				
				DBManager::Instance().DirectQuery("UPDATE account.account SET availDt = FROM_UNIXTIME(UNIX_TIMESTAMP(CURRENT_TIMESTAMP()) + %i), ban_reason = '%s' WHERE id = %d", p->duration, p->reason, account_id);
				InsertLogsBan(ch->GetName(), "ACTION_BAN_TIME", szEscapedUserName, p->reason);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("admin_manager_succes_ban_time"), szEscapedUserName);
			}
		}
		break;
		
		case ACTION_BAN_IP:
		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT name, ip FROM player.player WHERE name = '%s'", szEscapedUserName));
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			
			if (pMsg->uiSQLErrno != 0 || !pMsg->Get()->uiNumRows)
				return;

			std::unique_ptr<SQLMsg> pMsg2(DBManager::Instance().DirectQuery("UPDATE account.account INNER JOIN player.player ON player.account_id = account.id SET status = 'BLOCK', ban_reason = '%s' WHERE player.ip = '%s'", p->reason, row[1]));
			InsertLogsBan(ch->GetName(), "ACTION_BAN_IP", szEscapedUserName, p->reason);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("admin_manager_succes_ban_ip"), szEscapedUserName, pMsg2->Get()->uiAffectedRows);
		}
		break;
		default:
			return;
	}
}
#endif

void CInputMain::SafeboxCheckin(LPCHARACTER ch, const char * c_pData)
{
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;

	TPacketCGSafeboxCheckin * p = (TPacketCGSafeboxCheckin *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox = ch->GetSafebox();
	LPITEM pkItem = ch->GetItem(p->ItemPos);

	if (!pkSafebox || !pkItem)
		return;

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken envanterden depoya item taþ?amazs?. ");
		return;
	}
#endif

	if (pkItem->GetType() == ITEM_BELT && pkItem->IsEquipped())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("KEMER_ENVANTERI_DOLU"));
		return;
	}

	if (pkItem->GetType() == ITEM_WEAPON || pkItem->GetType() == ITEM_ARMOR || pkItem->GetType() == ITEM_BELT)
	{
		char szEventFlag[30];
		snprintf(szEventFlag, sizeof(szEventFlag), "%d.Engel", pkItem->GetID());
		if (*szEventFlag)
		{
			if (quest::CQuestManager::instance().GetEventFlag(szEventFlag))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("item_engel"));
				return;
			}
		}
	}
	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
	    ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Ã¢°í·Î ¿Å±æ ¼ö ¾ø´Â ¾ÆÀÌÅÛ ÀÔ´Ï´Ù."));
	    return;
	}
	
	if (true == pkItem->IsEquipped())  // giyili itemi depolayamazsin..
    {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Sistema> Imposible Almacenar %s! Necesitas Desequiparlo!"), ch->GetName());
        return;
    }

	if (true == pkItem->IsSealed()) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Sistema>Imposible Almacenar articulos Unidos al Alma!"));
		return;
	}

	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ¿Å±æ ¼ö ¾ø´Â À§Ä¡ÀÔ´Ï´Ù."));
		return;
	}

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ÀÌ ¾ÆÀÌÅÛÀº ³ÖÀ» ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if( IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX) )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ÀÌ ¾ÆÀÌÅÛÀº ³ÖÀ» ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (true == pkItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ÀÌ ¾ÆÀÌÅÛÀº ³ÖÀ» ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	pkItem->RemoveFromCharacter();
	if (!pkItem->IsDragonSoul())
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 1000);
#ifdef ENABLE_NEW_QUICK_SLOT_SYSTEM
	if (!pkItem->IsDragonSoul())
	{
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 1000);
		ch->SyncNewQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 1000);
	}
#else
	if (!pkItem->IsDragonSoul())
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 1000);
#endif
	pkSafebox->Add(p->bSafePos, pkItem);

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX PUT", szHint);
}

void CInputMain::SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall)
{
	TPacketCGSafeboxCheckout * p = (TPacketCGSafeboxCheckout *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox;

	if (bMall)
		pkSafebox = ch->GetMall();
	else
		pkSafebox = ch->GetSafebox();

	if (!pkSafebox)
		return;

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken depodan envantere item taþ?amazs?. ");
		return;
	}
#endif

	LPITEM pkItem = pkSafebox->Get(p->bSafePos);

	if (!pkItem)
		return;

	if (!ch->IsEmptyItemGrid(p->ItemPos, pkItem->GetSize()))
		return;

	for (WORD belt_index = BELT_INVENTORY_SLOT_START; belt_index < BELT_INVENTORY_SLOT_END; ++belt_index)
	{
		if (pkItem->GetType() != 3 && p->ItemPos.cell == belt_index)
		{
			if(pkItem->GetSubType() != 0 || pkItem->GetSubType() != 11 || pkItem->GetSubType() != 7)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("|cFFff0000|H|h<Check> Depodan Kemer Envanterine item yerlestiremezsin !"));
				return;
			}
		}   
	}

	// ¾ÆÀÌÅÛ ¸ô¿¡¼­ ÀÎº¥À¸·Î ¿Å±â´Â ºÎºÐ¿¡¼­ ¿ëÈ¥¼® Æ¯¼ö Ã³¸®
	// (¸ô¿¡¼­ ¸¸µå´Â ¾ÆÀÌÅÛÀº item_proto¿¡ Á¤ÀÇµÈ´ë·Î ¼Ó¼ºÀÌ ºÙ±â ¶§¹®¿¡,
	//  ¿ëÈ¥¼®ÀÇ °æ¿ì, ÀÌ Ã³¸®¸¦ ÇÏÁö ¾ÊÀ¸¸é ¼Ó¼ºÀÌ ÇÏ³ªµµ ºÙÁö ¾Ê°Ô µÈ´Ù.)
	if (pkItem->IsDragonSoul())
	{
		if (bMall)
		{
			DSManager::instance().DragonSoulItemInitialize(pkItem);
		}

		if (DRAGON_SOUL_INVENTORY != p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ¿Å±æ ¼ö ¾ø´Â À§Ä¡ÀÔ´Ï´Ù."));
			return;
		}

		TItemPos DestPos = p->ItemPos;
		if (!DSManager::instance().IsValidCellForThisItem(pkItem, DestPos))
		{
			int iCell = ch->GetEmptyDragonSoulInventory(pkItem);
			if (iCell < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ¿Å±æ ¼ö ¾ø´Â À§Ä¡ÀÔ´Ï´Ù."));
				return ;
			}
			DestPos = TItemPos (DRAGON_SOUL_INVENTORY, iCell);
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, DestPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	else if (pkItem->IsSkillBook())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ¿Å±æ ¼ö ¾ø´Â À§Ä¡ÀÔ´Ï´Ù."));
			return;
		}
		
		if (p->ItemPos.IsSkillBookInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_MOVE_ITEM_THIS_WINDOW"));
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else if (pkItem->IsStone())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ¿Å±æ ¼ö ¾ø´Â À§Ä¡ÀÔ´Ï´Ù."));
			return;
		}
		
		if (p->ItemPos.IsStoneInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_MOVE_ITEM_THIS_WINDOW"));
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);		
	}
	else if (pkItem->IsGiftBox())
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ¿Å±æ ¼ö ¾ø´Â À§Ä¡ÀÔ´Ï´Ù."));
			return;
		}
		
		if (p->ItemPos.IsGiftBoxInventoryPosition() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_MOVE_ITEM_THIS_WINDOW"));
			return;
		}
		
		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);		
	}
#endif
	else
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> ¿Å±æ ¼ö ¾ø´Â À§Ä¡ÀÔ´Ï´Ù."));
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		if (bMall)
		{
			if (NULL == pkItem->GetProto())
			{
				sys_err ("pkItem->GetProto() == NULL (id : %d)",pkItem->GetID());
				return ;
			}
			// 100% È®·ü·Î ¼Ó¼ºÀÌ ºÙ¾î¾ß ÇÏ´Âµ¥ ¾È ºÙ¾îÀÖ´Ù¸é »õ·Î ºÙÈù´Ù. ...............
			if (100 == pkItem->GetProto()->bAlterToMagicItemPct && 0 == pkItem->GetAttributeCount())
			{
				pkItem->AlterToMagicItem();
			}
		}
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}

	DWORD dwID = pkItem->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	if (bMall)
		LogManager::instance().ItemLog(ch, pkItem, "MALL GET", szHint);
	else
		LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX GET", szHint);
}

void CInputMain::SafeboxItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (!ch->CanHandleItem())
		return;

	if (!ch->GetSafebox())
		return;

	ch->GetSafebox()->MoveItem(pinfo->Cell.cell, pinfo->CellTo.cell, pinfo->count);
}

// PARTY_JOIN_BUG_FIX
void CInputMain::PartyInvite(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	
	if (int(ch->GetQuestFlag("Kilit.Enable")) == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GUVENLIK_ENGEL"));
		return;
	}

	TPacketCGPartyInvite * p = (TPacketCGPartyInvite*) c_pData;

	LPCHARACTER pInvitee = CHARACTER_MANAGER::instance().Find(p->vid);

	if (!pInvitee || !ch->GetDesc() || !pInvitee->GetDesc())
	{
		sys_err("PARTY Cannot find invited character");
		return;
	}

	#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::instance().IsBlocked_Target(ch->GetName(), pInvitee->GetName()))
	{
		//ben bloklad? hac?		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk"), pInvitee->GetName());
		return;
	}
	if (MessengerManager::instance().IsBlocked_Me(ch->GetName(), pInvitee->GetName()))
	{
		//o bloklad?hac?		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk_me"), pInvitee->GetName());
		return;
	}
	#endif

	ch->PartyInvite(pInvitee);
}

void CInputMain::PartyInviteAnswer(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesab? kilitliyken gruba giremezsin. ");
		return;
	}
#endif

	TPacketCGPartyInviteAnswer * p = (TPacketCGPartyInviteAnswer*) c_pData;

	LPCHARACTER pInviter = CHARACTER_MANAGER::instance().Find(p->leader_vid);

	// pInviter °¡ ch ¿¡°Ô ÆÄÆ¼ ¿äÃ»À» Çß¾ú´Ù.

	if (!pInviter)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ÆÄÆ¼¿äÃ»À» ÇÑ Ä³¸¯ÅÍ¸¦ Ã£À»¼ö ¾ø½À´Ï´Ù."));
	else if (!p->accept)
		pInviter->PartyInviteDeny(ch->GetPlayerID());
	else
		pInviter->PartyInviteAccept(ch);
}
// END_OF_PARTY_JOIN_BUG_FIX

void CInputMain::PartySetState(LPCHARACTER ch, const char* c_pData)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ¼­¹ö ¹®Á¦·Î ÆÄÆ¼ °ü·Ã Ã³¸®¸¦ ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	TPacketCGPartySetState* p = (TPacketCGPartySetState*) c_pData;

	if (!ch->GetParty())
		return;

	if (ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ¸®´õ¸¸ º¯°æÇÒ ¼ö ÀÖ½À´Ï´Ù."));
		return;
	}

	if (!ch->GetParty()->IsMember(p->pid))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> »óÅÂ¸¦ º¯°æÇÏ·Á´Â »ç¶÷ÀÌ ÆÄÆ¼¿øÀÌ ¾Æ´Õ´Ï´Ù."));
		return;
	}

	DWORD pid = p->pid;
	sys_log(0, "PARTY SetRole pid %d to role %d state %s", pid, p->byRole, p->flag ? "on" : "off");

	switch (p->byRole)
	{
		case PARTY_ROLE_NORMAL:
			break;

		case PARTY_ROLE_ATTACKER:
		case PARTY_ROLE_TANKER:
		case PARTY_ROLE_BUFFER:
		case PARTY_ROLE_SKILL_MASTER:
		case PARTY_ROLE_HASTE:
		case PARTY_ROLE_DEFENDER:
			if (ch->GetParty()->SetRole(pid, p->byRole, p->flag))
			{
				TPacketPartyStateChange pack;
				pack.dwLeaderPID = ch->GetPlayerID();
				pack.dwPID = p->pid;
				pack.bRole = p->byRole;
				pack.bFlag = p->flag;
				db_clientdesc->DBPacket(HEADER_GD_PARTY_STATE_CHANGE, 0, &pack, sizeof(pack));
			}
			/* else
			   ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ¾îÅÂÄ¿ ¼³Á¤¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù.")); */
			break;

		default:
			sys_err("wrong byRole in PartySetState Packet name %s state %d", ch->GetName(), p->byRole);
			break;
	}
}

void CInputMain::PartyRemove(LPCHARACTER ch, const char* c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ¼­¹ö ¹®Á¦·Î ÆÄÆ¼ °ü·Ã Ã³¸®¸¦ ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ´øÀü ¾È¿¡¼­´Â ÆÄÆ¼¿¡¼­ Ãß¹æÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	TPacketCGPartyRemove* p = (TPacketCGPartyRemove*) c_pData;

	if (!ch->GetParty())
		return;

	LPPARTY pParty = ch->GetParty();
	if (pParty->GetLeaderPID() == ch->GetPlayerID())
	{
		if (ch->GetDungeon())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ´øÁ¯³»¿¡¼­´Â ÆÄÆ¼¿øÀ» Ãß¹æÇÒ ¼ö ¾ø½À´Ï´Ù."));
		}
		else
		{
			// leader can remove any member
			if (p->pid == ch->GetPlayerID() || pParty->GetMemberCount() == 2)
			{
				// party disband
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				LPCHARACTER B = CHARACTER_MANAGER::instance().FindByPID(p->pid);
				if (B)
				{
					//pParty->SendPartyRemoveOneToAll(B);
					B->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ÆÄÆ¼¿¡¼­ Ãß¹æ´çÇÏ¼Ì½À´Ï´Ù."));
					//pParty->Unlink(B);
					//CPartyManager::instance().SetPartyMember(B->GetPlayerID(), NULL);
				}
				pParty->Quit(p->pid);
			}
		}
	}
	else
	{
		// otherwise, only remove itself
		if (p->pid == ch->GetPlayerID())
		{
			if (ch->GetDungeon())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ´øÁ¯³»¿¡¼­´Â ÆÄÆ¼¸¦ ³ª°¥ ¼ö ¾ø½À´Ï´Ù."));
			}
			else
			{
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
					//CPartyManager::instance().SetPartyMember(ch->GetPlayerID(), NULL);
				}
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ´Ù¸¥ ÆÄÆ¼¿øÀ» Å»Åð½ÃÅ³ ¼ö ¾ø½À´Ï´Ù."));
		}
	}
}

void CInputMain::AnswerMakeGuild(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGAnswerMakeGuild* p = (TPacketCGAnswerMakeGuild*) c_pData;

	if (ch->GetGold() < 200000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "|cFFff0000|H|h<Hata> Yetersiz yang!");
		return;
	}

	if (ch->GetLevel() < 40)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "|cFFff0000|H|h<Hata> Yetersiz seviye!");
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_disband_time") <
			CGuildManager::instance().GetDisbandDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ÇØ»êÇÑ ÈÄ %dÀÏ ÀÌ³»¿¡´Â ±æµå¸¦ ¸¸µé ¼ö ¾ø½À´Ï´Ù."), 
				quest::CQuestManager::instance().GetEventFlag("guild_disband_delay"));
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_withdraw_time") <
			CGuildManager::instance().GetWithdrawDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Å»ÅðÇÑ ÈÄ %dÀÏ ÀÌ³»¿¡´Â ±æµå¸¦ ¸¸µé ¼ö ¾ø½À´Ï´Ù."), 
				quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay"));
		return;
	}

	if (ch->GetGuild())
		return;

	CGuildManager& gm = CGuildManager::instance();

	TGuildCreateParameter cp;
	memset(&cp, 0, sizeof(cp));

	cp.master = ch;
	strlcpy(cp.name, p->guild_name, sizeof(cp.name));

	if (cp.name[0] == 0 || !check_name(cp.name))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀûÇÕÇÏÁö ¾ÊÀº ±æµå ÀÌ¸§ ÀÔ´Ï´Ù."));
		return;
	}

	DWORD dwGuildID = gm.CreateGuild(cp);

	if (dwGuildID)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> [%s] ±æµå°¡ »ý¼ºµÇ¾ú½À´Ï´Ù."), cp.name);

		int GuildCreateFee;

		if (LC_IsBrazil())
		{
			GuildCreateFee = 500000;
		}
		else
		{
			GuildCreateFee = 200000;
		}

		ch->PointChange(POINT_GOLD, -GuildCreateFee);
		DBManager::instance().SendMoneyLog(MONEY_LOG_GUILD, ch->GetPlayerID(), -GuildCreateFee);

		char Log[128];
		snprintf(Log, sizeof(Log), "GUILD_NAME %s MASTER %s", cp.name, ch->GetName());
		LogManager::instance().CharLog(ch, 0, "MAKE_GUILD", Log);

		DBManager::instance().Query("INSERT INTO lonca_gecmis%s (name, guild) values('%s', '%s')", get_table_postfix(), ch->GetName(), cp.name);

		if (g_iUseLocale)
			ch->RemoveSpecifyItem(GUILD_CREATE_ITEM_VNUM, 1);
		//ch->SendGuildName(dwGuildID);
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå »ý¼º¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù."));
}

void CInputMain::PartyUseSkill(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGPartyUseSkill* p = (TPacketCGPartyUseSkill*) c_pData;
	if (!ch->GetParty())
		return;

	if (ch->GetPlayerID() != ch->GetParty()->GetLeaderPID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ÆÄÆ¼ ±â¼úÀº ÆÄÆ¼Àå¸¸ »ç¿ëÇÒ ¼ö ÀÖ½À´Ï´Ù."));
		return;
	}

	switch (p->bySkillIndex)
	{
		case PARTY_SKILL_HEAL:
			ch->GetParty()->HealParty();
			break;
		case PARTY_SKILL_WARP:
			{
				LPCHARACTER pch = CHARACTER_MANAGER::instance().Find(p->vid);
				if (pch)
				{
					if (pch->GetMapIndex() != ch->GetMapIndex())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("AYNI_HARITADA_OLMAK_ZORUNDASIN"));
						return;
					}

					ch->GetParty()->SummonToLeader(pch->GetPlayerID());
				}
				else
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ¼ÒÈ¯ÇÏ·Á´Â ´ë»óÀ» Ã£À» ¼ö ¾ø½À´Ï´Ù."));
			}
			break;
	}
}

void CInputMain::PartyParameter(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGPartyParameter * p = (TPacketCGPartyParameter *) c_pData;

	if (ch->GetParty())
		ch->GetParty()->SetParameter(p->bDistributeMode);
}

size_t GetSubPacketSize(const GUILD_SUBHEADER_CG& header)
{
	switch (header)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_ADD_MEMBER:					return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:			return 10;
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:		return sizeof(BYTE) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_OFFER:						return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHARGE_GSP:					return sizeof(int);
		case GUILD_SUBHEADER_CG_POST_COMMENT:				return 1;
		case GUILD_SUBHEADER_CG_DELETE_COMMENT:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:			return 0;
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_USE_SKILL:					return sizeof(TPacketCGGuildUseSkill);
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:		return sizeof(DWORD) + sizeof(BYTE);
	}

	return 0;
}

int CInputMain::Guild(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuild))
		return -1;

	const TPacketCGGuild* p = reinterpret_cast<const TPacketCGGuild*>(data);
	const char* c_pData = data + sizeof(TPacketCGGuild);

	uiBytes -= sizeof(TPacketCGGuild);

	const GUILD_SUBHEADER_CG SubHeader = static_cast<GUILD_SUBHEADER_CG>(p->subheader);
	const size_t SubPacketLen = GetSubPacketSize(SubHeader);

	if (uiBytes < SubPacketLen)
	{
		return -1;
	}

	CGuild* pGuild = ch->GetGuild();

	if (NULL == pGuild)
	{
		if (SubHeader != GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå¿¡ ¼ÓÇØÀÖÁö ¾Ê½À´Ï´Ù."));
			return SubPacketLen;
		}
	}

	switch (SubHeader)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
			{
				// by mhh : ±æµåÀÚ±ÝÀº ´çºÐ°£ ³ÖÀ» ¼ö ¾ø´Ù.
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), __deposit_limit());

				if (gold < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Àß¸øµÈ ±Ý¾×ÀÔ´Ï´Ù."));
					return SubPacketLen;
				}

				if (ch->GetGold() < gold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> °¡Áö°í ÀÖ´Â µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù."));
					return SubPacketLen;
				}

				pGuild->RequestDepositMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
			{
				// by mhh : ±æµåÀÚ±ÝÀº ´çºÐ°£ »¬ ¼ö ¾ø´Ù.
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), 500000);

				if (gold < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Àß¸øµÈ ±Ý¾×ÀÔ´Ï´Ù."));
					return SubPacketLen;
				}

				pGuild->RequestWithdrawMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_ADD_MEMBER:
			{
				if(ch->GetMapIndex() == 200)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arena_engel"));
				}
				else
				{
					const DWORD vid = *reinterpret_cast<const DWORD*>(c_pData);
					LPCHARACTER newmember = CHARACTER_MANAGER::instance().Find(vid);

					if (!newmember)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±×·¯ÇÑ »ç¶÷À» Ã£À» ¼ö ¾ø½À´Ï´Ù."));
						return SubPacketLen;
					}

					if (!newmember->IsPC())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ISLEM_ENGELLENDI"));
						return SubPacketLen;
					}
					if (!newmember->IsPC())
						return SubPacketLen;

					#ifdef ENABLE_MESSENGER_BLOCK
					if (MessengerManager::instance().IsBlocked_Target(ch->GetName(), newmember->GetName()))
					{
						//ben bloklad? hac?
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk"), newmember->GetName());
						return SubPacketLen;
					}
					if (MessengerManager::instance().IsBlocked_Me(ch->GetName(), newmember->GetName()))
					{
						//o bloklad?hac?
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk_me"), newmember->GetName());
						return SubPacketLen;
					}
					#endif

					if (LC_IsCanada() == true)
					{
						if (newmember->GetQuestFlag("change_guild_master.be_other_member") > get_global_time())
						{
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ¾ÆÁ÷ °¡ÀÔÇÒ ¼ö ¾ø´Â Ä³¸¯ÅÍÀÔ´Ï´Ù"));
							return SubPacketLen;
						}
					}
	
					pGuild->Invite(ch, newmember);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
			{
				if (pGuild->UnderAnyWar() != 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀü Áß¿¡´Â ±æµå¿øÀ» Å»Åð½ÃÅ³ ¼ö ¾ø½À´Ï´Ù."));
					return SubPacketLen;
				}

				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				LPCHARACTER member = CHARACTER_MANAGER::instance().FindByPID(pid);

				if (member)
				{
					if (member->GetGuild() != pGuild)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æÀÌ °°Àº ±æµå°¡ ¾Æ´Õ´Ï´Ù."));
						return SubPacketLen;
					}

					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå¿øÀ» °­Á¦ Å»Åð ½ÃÅ³ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
						return SubPacketLen;
					}

					member->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
					pGuild->RequestRemoveMember(member->GetPlayerID());

					if (LC_IsBrazil() == true)
					{
						DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", pGuild->GetID(), get_global_time());
					}
				}
				else
				{
					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå¿øÀ» °­Á¦ Å»Åð ½ÃÅ³ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
						return SubPacketLen;
					}

					if (pGuild->RequestRemoveMember(pid))
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå¿øÀ» °­Á¦ Å»Åð ½ÃÄ×½À´Ï´Ù."));
					else
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±×·¯ÇÑ »ç¶÷À» Ã£À» ¼ö ¾ø½À´Ï´Ù."));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
			{
				char gradename[GUILD_GRADE_NAME_MAX_LEN + 1];
				strlcpy(gradename, c_pData + 1, sizeof(gradename));

				const TGuildMember * m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Á÷À§ ÀÌ¸§À» º¯°æÇÒ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀåÀÇ Á÷À§ ÀÌ¸§Àº º¯°æÇÒ ¼ö ¾ø½À´Ï´Ù."));
				}
				else if (!check_name(gradename))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ÀûÇÕÇÏÁö ¾ÊÀº Á÷À§ ÀÌ¸§ ÀÔ´Ï´Ù."));
				}
				else
				{
					pGuild->ChangeGradeName(*c_pData, gradename);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
			{
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Á÷À§ ±ÇÇÑÀ» º¯°æÇÒ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀåÀÇ ±ÇÇÑÀº º¯°æÇÒ ¼ö ¾ø½À´Ï´Ù."));
				}
				else
				{
					pGuild->ChangeGradeAuth(*c_pData, *(c_pData + 1));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_OFFER:
			{
				if (ch->IsBlockMode(BLOCK_POINT_EXP))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("exp_bloklu"));
					return SubPacketLen;
				}
				else;
				DWORD offer = *reinterpret_cast<const DWORD*>(c_pData);

				if (pGuild->GetLevel() >= GUILD_MAX_LEVEL && LC_IsHongKong() == false)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå°¡ ÀÌ¹Ì ÃÖ°í ·¹º§ÀÔ´Ï´Ù."));
				}
				else
				{
					offer /= 100;
					offer *= 100;

					if (pGuild->OfferExp(ch, offer))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> %uÀÇ °æÇèÄ¡¸¦ ÅõÀÚÇÏ¿´½À´Ï´Ù."), offer);
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> °æÇèÄ¡ ÅõÀÚ¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù."));
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHARGE_GSP:
			{
				const int offer = *reinterpret_cast<const int*>(c_pData);
				const int gold = offer * 100;

				if (offer < 0 || gold < offer || gold < 0 || ch->GetGold() < gold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù."));
					return SubPacketLen;
				}

				if (!pGuild->ChargeSP(ch, offer))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ¿ë½Å·Â È¸º¹¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù."));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_POST_COMMENT:
			{
				const size_t length = *c_pData;

				if (length > GUILD_COMMENT_MAX_LEN)
				{
					// Àß¸øµÈ ±æÀÌ.. ²÷¾îÁÖÀÚ.
					sys_err("POST_COMMENT: %s comment too long (length: %u)", ch->GetName(), length);
					ch->GetDesc()->SetPhase(PHASE_CLOSE);
					return -1;
				}

				if (uiBytes < 1 + length)
					return -1;

				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (length && !pGuild->HasGradeAuth(m->grade, GUILD_AUTH_NOTICE) && *(c_pData + 1) == '!')
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> °øÁö±ÛÀ» ÀÛ¼ºÇÒ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
				}
				else
				{
					std::string str(c_pData + 1, length);
					pGuild->AddComment(ch, str);
				}

				return (1 + length);
			}

		case GUILD_SUBHEADER_CG_DELETE_COMMENT:
			{
				const DWORD comment_id = *reinterpret_cast<const DWORD*>(c_pData);

				pGuild->DeleteComment(ch, comment_id);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
			pGuild->RefreshComment(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE grade = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Á÷À§¸¦ º¯°æÇÒ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
				else if (ch->GetPlayerID() == pid)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀåÀÇ Á÷À§´Â º¯°æÇÒ ¼ö ¾ø½À´Ï´Ù."));
				else if (grade == 1)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀåÀ¸·Î Á÷À§¸¦ º¯°æÇÒ ¼ö ¾ø½À´Ï´Ù."));
				else
					pGuild->ChangeMemberGrade(pid, grade);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_USE_SKILL:
			{
				const TPacketCGGuildUseSkill* p = reinterpret_cast<const TPacketCGGuildUseSkill*>(c_pData);

				pGuild->UseSkill(p->dwVnum, ch, p->dwPID);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE is_general = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Àå±ºÀ» ÁöÁ¤ÇÒ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
				}
				else
				{
					if (!pGuild->ChangeMemberGeneral(pid, is_general))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ´õÀÌ»ó Àå¼ö¸¦ ÁöÁ¤ÇÒ ¼ö ¾ø½À´Ï´Ù."));
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
			{
				const DWORD guild_id = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE accept = *(c_pData + sizeof(DWORD));

				CGuild * g = CGuildManager::instance().FindGuild(guild_id);

				if (g)
				{
					if (accept)
						g->InviteAccept(ch);
					else
						g->InviteDeny(ch->GetPlayerID());
				}
			}
			return SubPacketLen;

	}

	return 0;
}

void CInputMain::Fishing(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFishing* p = (TPacketCGFishing*)c_pData;
	ch->SetRotation(p->dir * 5);
	ch->fishing();
	return;
}

void CInputMain::ItemGive(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGGiveItem* p = (TPacketCGGiveItem*) c_pData;
	LPCHARACTER to_ch = CHARACTER_MANAGER::instance().Find(p->dwTargetVID);

	if (to_ch)
		ch->GiveItem(to_ch, p->ItemPos);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛÀ» °Ç³×ÁÙ ¼ö ¾ø½À´Ï´Ù."));
}

void CInputMain::Hack(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGHack * p = (TPacketCGHack *) c_pData;

	char buf[sizeof(p->szBuf)];
	strlcpy(buf, p->szBuf, sizeof(buf));

	sys_err("HACK_DETECT: %s %s", ch->GetName(), buf);

	// CoAc A¡þ¢ÒoAI3?A¢ç?!1¡© AI A?A¢ÒA¡í o¢¬3¡í¢¥A ¡Æa?i¡Æ! 3oA¢¬1C¡¤I 1¡ìA¢Ò¡ÆC 2¡À¥ì¥ì¡¤I CN¢¥U
	ch->GetDesc()->SetPhase(PHASE_CLOSE);
}

long long CInputMain::MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyShop * p = (TPacketCGMyShop *) c_pData;
	long long iExtraLen = p->bCount * sizeof(TShopItemTable2);
#ifdef SHOP_BLOCK_GAME99
	if (g_bChannel >= 99 & ch->GetGMLevel() <= 0) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "SHOP_OX_ERROR"));
		return (iExtraLen);
	}
#endif
#ifdef SHOP_GM_PRIVILEGES
	if (GM_PLAYER < ch->GetGMLevel() && SHOP_GM_PRIVILEGES >ch->GetGMLevel()) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_PRIV_ERROR"));
		return (iExtraLen);
	}
#endif
	if (uiBytes < sizeof(TPacketCGMyShop) + iExtraLen)
		return -1;
	#ifndef FULL_YANG
 	if (ch->GetGold() >= GOLD_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÀ¯ µ·ÀÌ 20¾ï³ÉÀ» ³Ñ¾î °Å·¡¸¦ ÇÛ¼ö°¡ ¾ø½À´Ï´Ù."));
		sys_log(0, "MyShop ==> OverFlow Gold id %u name %s ", ch->GetPlayerID(), ch->GetName());
		return (iExtraLen);
	}
	#endif
	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->GetOfflineShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡ÁßÀÏ°æ¿ì °³ÀÎ»óÁ¡À» ¿­¼ö°¡ ¾ø½À´Ï´Ù."));
		return (iExtraLen);
	}

#ifdef WJ_ITEM_COMBINATION_SYSTEM
	if (ch->IsCombOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡ÁßÀÏ°æ¿ì °³ÀÎ»óÁ¡À» ¿­¼ö°¡ ¾ø½À´Ï´Ù."));
		return (iExtraLen);
	}
#endif

	sys_log(0, "MyShop count %d", p->bCount);
	//ch->OpenMyShop(p->szSign, (TShopItemTable2 *)(c_pData + sizeof(TPacketCGMyShop)), p->bCount);
	return (iExtraLen);
}

#ifdef ENABLE_SHOW_CHEST_DROP
void CInputMain::ChestDropInfo(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGChestDropInfo* p = (TPacketCGChestDropInfo*) c_pData;

	if(p->wInventoryCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		return;
	
	LPITEM pkItem = ch->GetInventoryItem(p->wInventoryCell);

	if (!pkItem)
		return;
	
	std::vector<TChestDropInfoTable> vec_ItemList;
	ITEM_MANAGER::instance().GetChestItemList(pkItem->GetVnum(), vec_ItemList);

	TPacketGCChestDropInfo packet;
	packet.bHeader = HEADER_GC_CHEST_DROP_INFO;
	packet.wSize = sizeof(packet) + sizeof(TChestDropInfoTable) * vec_ItemList.size();
	packet.dwChestVnum = pkItem->GetVnum();

	ch->GetDesc()->BufferedPacket(&packet, sizeof(packet));
	ch->GetDesc()->Packet(&vec_ItemList[0], sizeof(TChestDropInfoTable) * vec_ItemList.size());
}
#endif

#ifdef __ENABLE_FISH_EVENT_SYSTEM__
int CInputMain::FishEvent(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGFishEvent * p = (TPacketCGFishEvent *) data;

	if (uiBytes < sizeof(TPacketCGFishEvent))
		return -1;

	const char * c_pData = data + sizeof(TPacketCGFishEvent);
	uiBytes -= sizeof(TPacketCGFishEvent);

	switch (p->bSubheader)
	{
		case FISH_EVENT_SUBHEADER_BOX_USE:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(WORD))
					return -1;

				BYTE bWindow = *(c_pData++);
				WORD wCell = *(c_pData);

				sys_log(0, "INPUT: %s FISH_EVENT: USE_BOX", ch->GetName());
				
				ch->FishEventUseBox(TItemPos(bWindow, wCell));
				return (sizeof(BYTE) + sizeof(WORD));
			}
			
		case FISH_EVENT_SUBHEADER_SHAPE_ADD:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE shapePos = *c_pData;

				sys_log(0, "INPUT: %s FISH_EVENT: ADD_SHAPE", ch->GetName());
				
				ch->FishEventAddShape(shapePos);
				return sizeof(BYTE);
			}

		default:
			sys_err("CInputMain::FishEvent : Unknown subheader %d : %s", p->bSubheader, ch->GetName());
			break;
	}

	return 0;
}
#endif

#ifdef WJ_OFFLINESHOP_SYSTEM
int CInputMain::MyOfflineShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyOfflineShop * p = (TPacketCGMyOfflineShop *)c_pData;
	int iExtraLen = p->bCount * sizeof(TOfflineShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyOfflineShop) + iExtraLen)
		return -1;

	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->GetOfflineShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ °Å·¡ÁßÀÏ°æ¿ì °³ÀÎ»óÁ¡À» ¿­¼ö°¡ ¾ø½À´Ï´Ù."));
		return (iExtraLen);
	}
	
	sys_log(0, "MyOfflineShop count %d", p->bCount);
	ch->OpenMyOfflineShop(p->szSign, (TOfflineShopItemTable *)(c_pData + sizeof(TPacketCGMyOfflineShop)), p->bCount, p->bNpcType);
	return (iExtraLen);
}
#endif

void CInputMain::Refine(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGRefine* p = reinterpret_cast<const TPacketCGRefine*>(c_pData);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen() || ch->IsCombOpen() || ch->GetOfflineShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã¢°í,°Å·¡Ã¢µîÀÌ ¿­¸° »óÅÂ¿¡¼­´Â °³·®À» ÇÒ¼ö°¡ ¾ø½À´Ï´Ù"));
		ch->ClearRefineMode();
		return;
	}

#ifdef WJ_ITEM_COMBINATION_SYSTEM
	if (ch->IsCombOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO,  LC_TEXT("Ã¢°í,°Å·¡Ã¢µîÀÌ ¿­¸° »óÅÂ¿¡¼­´Â °³·®À» ÇÒ¼ö°¡ ¾ø½À´Ï´Ù"));
		ch->ClearRefineMode();
		return;
	}
#endif

	if (p->type == 255)
	{
		// DoRefine Cancel
		ch->ClearRefineMode();
		return;
	}

	if (p->pos >= INVENTORY_MAX_NUM)
	{
		ch->ClearRefineMode();
		return;
	}

#ifdef WJ_SECURITY_SYSTEM
	if (ch->IsActivateSecurity() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot refine with security key activate"));
		ch->ClearRefineMode();
		return;
	}
#endif

#ifdef ENABLE_ACCOUNT_LOCK
	if (ch->GetAccountLockSize() == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken yukseltme yapamazsin.");
		ch->ClearRefineMode();
		return;
	}
#endif

	LPITEM item = ch->GetInventoryItem(p->pos);

	if (!item)
	{
		ch->ClearRefineMode();
		return;
	}

	if (item->IsSealed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Kilitli bir esyayi yukseltemezsin.");
		ch->ClearRefineMode();
		return;
	}


#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	if (!CRefineManager::instance().GetPercentage(ch, p->lLow, p->lMedium, p->lExtra, p->lTotal, item))
	{
		ch->ClearRefineMode();
		return;
	}

	CRefineManager::instance().Increase(ch, p->lLow, p->lMedium, p->lExtra);
#endif

	ch->SetRefineTime();

	if (p->type == REFINE_TYPE_NORMAL)
	{
		sys_log (0, "refine_type_normal");
		ch->DoRefine(item);
	}
	else if (p->type == REFINE_TYPE_SCROLL || p->type == REFINE_TYPE_HYUNIRON || p->type == REFINE_TYPE_MUSIN || p->type == REFINE_TYPE_BDRAGON)
	{
		sys_log (0, "refine_type_scroll, ...");
		ch->DoRefineWithScroll(item);
	}
	else if (p->type == REFINE_TYPE_MONEY_ONLY)
	{
		const LPITEM item = ch->GetInventoryItem(p->pos);

		if (NULL != item)
		{
			if (ch->GetQuestFlag("DevilTowerRenewal.CanRefine"))
			{
					if (ch->DoRefine(item, true))
					{
						ch->SetQuestFlag("DevilTowerRenewal.CanRefine", 0);
					}
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "»ç±Í Å¸¿ö ¿Ï·á º¸»óÀº ÇÑ¹ø±îÁö »ç¿ë°¡´ÉÇÕ´Ï´Ù.");
			}
		}
	}

	ch->ClearRefineMode();
}

void CInputMain::BulkWhisperManager(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGBulkWhisper * f = (TPacketCGBulkWhisper *)c_pData;

	if (!ch)
		return;

	if (ch->GetGMLevel() != GM_IMPLEMENTOR)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[SYSTEM] GM degilsin.");
		return;
	}

	TPacketGGBulkWhisper p;
	p.bHeader = HEADER_GG_BULK_WHISPER;
	p.lSize = strlen(f->szText) + 1;

	TEMP_BUFFER tmpbuf;
	tmpbuf.write(&p, sizeof(p));
	tmpbuf.write(f->szText, p.lSize);

	char szEscaped[CHAT_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szEscaped, sizeof(szEscaped), f->szText, strlen(f->szText));

	SendBulkWhisper(f->szText);
	P2P_MANAGER::instance().Send(tmpbuf.read_peek(), tmpbuf.size());
}

#ifdef __SASH_SYSTEM__
void CInputMain::Sash(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;
	
	TPacketSash * sPacket = (TPacketSash*) c_pData;
	switch (sPacket->subheader)
	{
		case SASH_SUBHEADER_CG_CLOSE:
			{
				pkChar->CloseSash();
			}
			break;
		case SASH_SUBHEADER_CG_ADD:
			{
				pkChar->AddSashMaterial(sPacket->tPos, sPacket->bPos);
			}
			break;
		case SASH_SUBHEADER_CG_REMOVE:
			{
				pkChar->RemoveSashMaterial(sPacket->bPos);
			}
			break;
		case SASH_SUBHEADER_CG_REFINE:
			{
				pkChar->RefineSashMaterials();
			}
			break;
		default:
			break;
	}
}
#endif

#ifdef ENABLE_DROP_ITEM_WORLDARD 
void CInputMain::DropItemInfoSend(LPCHARACTER ch, const char* data)
{
	struct packet_send_drop_item_system * pinfo = (struct packet_send_drop_item_system *) data;

	switch (pinfo->subheader)
	{
		case DROP_ITEM_SUB_HEADER_OPEN:
		{
			CDropItem::instance().Open(ch);
		}
		break;

		case DROP_ITEM_SUB_HEADER_SEARCH:
		{
			int vnum_item = pinfo->vnum_item;
			CDropItem::instance().Search(ch,vnum_item);
		}
		break;

	}
}
#endif

int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		d->SetPhase(PHASE_CLOSE);
		return (0);
	}

	int iExtraLen = 0;

	if (test_server && bHeader != HEADER_CG_MOVE)
		sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", bHeader);

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if (test_server)
			{
				char* pBuf = (char*)c_pData;
				sys_log(0, "%s", pBuf + sizeof(TPacketCGChat));
			}

			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MOVE:
			Move(ch, c_pData);

			if (LC_IsEurope())
			{
				if (g_bCheckClientVersion)
				{
					int version = atoi(g_stClientVersion.c_str());
					int date	= atoi(d->GetClientVersion());

					//if (0 != g_stClientVersion.compare(d->GetClientVersion()))
					if (version > date)
					{
						ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("Å¬¶óÀÌ¾ðÆ® ¹öÀüÀÌ Æ²·Á ·Î±×¾Æ¿ô µË´Ï´Ù. Á¤»óÀûÀ¸·Î ÆÐÄ¡ ÈÄ Á¢¼ÓÇÏ¼¼¿ä."));
						d->DelayedDisconnect(10);
						LogManager::instance().HackLog("VERSION_CONFLICT", d->GetAccountTable().login, ch->GetName(), d->GetHostName());
					}
				}
			}
			else
			{
				if (!*d->GetClientVersion())
				{   
					sys_err("Version not recieved name %s", ch->GetName());
					d->SetPhase(PHASE_CLOSE);
				}
			}
			break;

		case HEADER_CG_CHARACTER_POSITION:
			Position(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE:
			if (!ch->IsObserverMode())
				ItemUse(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DROP:
			if (!ch->IsObserverMode())
			{
				ItemDrop(ch, c_pData);
			}
			break;

		case HEADER_CG_ITEM_DROP2:
			if (!ch->IsObserverMode())
				ItemDrop2(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DESTROY:
			if (!ch->IsObserverMode())
				ItemDestroy(ch, c_pData);
			break;

		case HEADER_CG_ITEM_MOVE:
			if (!ch->IsObserverMode())
				ItemMove(ch, c_pData);
			break;

		case HEADER_CG_ITEM_PICKUP:
			if (!ch->IsObserverMode())
				ItemPickup(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE_TO_ITEM:
			if (!ch->IsObserverMode())
				ItemToItem(ch, c_pData);
			break;

		case HEADER_CG_ITEM_GIVE:
			if (!ch->IsObserverMode())
				ItemGive(ch, c_pData);
			break;

		case HEADER_CG_EXCHANGE:
			if(ch->GetMapIndex() == 200)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arena_engel"));
			}
			else
			{	
				if (!ch->IsObserverMode())
					Exchange(ch, c_pData);
			}
			break;

		case HEADER_CG_ATTACK:
		case HEADER_CG_SHOOT:
			if (!ch->IsObserverMode())
			{
				Attack(ch, bHeader, c_pData);
			}
			break;

		case HEADER_CG_USE_SKILL: // @fixme38
			if(!ch || ch == NULL)
				break;
			if(!c_pData || c_pData == NULL)
				break;
			if (!ch->IsObserverMode())
				UseSkill(ch, c_pData);
			break;

#ifdef __SKILL_COLOR_SYSTEM__
		case HEADER_CG_SKILL_COLOR:
			SetSkillColor(ch, c_pData);
			break;
#endif

		case HEADER_CG_QUICKSLOT_ADD:
			QuickslotAdd(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_DEL:
			QuickslotDelete(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_SWAP:
			QuickslotSwap(ch, c_pData);
			break;

#ifdef ENABLE_NEW_QUICK_SLOT_SYSTEM
		case HEADER_CG_NEWQUICKSLOT_ADD:
			NewQuickslotAdd(ch, c_pData);
			break;

		case HEADER_CG_NEWQUICKSLOT_DEL:
			NewQuickslotDelete(ch, c_pData);
			break;

		case HEADER_CG_NEWQUICKSLOT_SWAP:
			NewQuickslotSwap(ch, c_pData);
			break;
#endif

		case HEADER_CG_SHOP:
			if ((iExtraLen = Shop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_SHOP2:
			Shop2(ch, c_pData);
			break;
#ifdef WJ_OFFLINESHOP_SYSTEM
		case HEADER_CG_OFFLINE_SHOP:
			if ((iExtraLen = OfflineShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

		case HEADER_CG_MESSENGER:
			if ((iExtraLen = Messenger(ch, c_pData, m_iBufferLeft))<0)
				return -1;
			break;

		case HEADER_CG_ON_CLICK:
			OnClick(ch, c_pData);
			break;

		case HEADER_CG_SYNC_POSITION:
			if ((iExtraLen = SyncPosition(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_ADD_FLY_TARGETING:
		case HEADER_CG_FLY_TARGETING:
			FlyTarget(ch, c_pData, bHeader);
			break;

		case HEADER_CG_SCRIPT_BUTTON:
			ScriptButton(ch, c_pData);
			break;

			// SCRIPT_SELECT_ITEM
		case HEADER_CG_SCRIPT_SELECT_ITEM:
			ScriptSelectItem(ch, c_pData);
			break;
			// END_OF_SCRIPT_SELECT_ITEM

		case HEADER_CG_SCRIPT_ANSWER:
			ScriptAnswer(ch, c_pData);
			break;

		case HEADER_CG_QUEST_INPUT_STRING:
			QuestInputString(ch, c_pData);
			break;

		case HEADER_CG_QUEST_CONFIRM:
			QuestConfirm(ch, c_pData);
			break;

		case HEADER_CG_TARGET:
			Target(ch, c_pData);
			break;

		case HEADER_CG_WARP:
			Warp(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKIN:
			SafeboxCheckin(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKOUT:
			SafeboxCheckout(ch, c_pData, false);
			break;

		case HEADER_CG_SAFEBOX_ITEM_MOVE:
			SafeboxItemMove(ch, c_pData);
			break;

		case HEADER_CG_MALL_CHECKOUT:
			SafeboxCheckout(ch, c_pData, true);
			break;

		case HEADER_CG_PARTY_INVITE:
			if(ch->GetMapIndex() == 200)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("arena_engel"));
			}
			else
			{
				PartyInvite(ch, c_pData);
			}
			break;

		case HEADER_CG_PARTY_REMOVE:
			PartyRemove(ch, c_pData);
			break;

		case HEADER_CG_PARTY_INVITE_ANSWER:
			PartyInviteAnswer(ch, c_pData);
			break;

		case HEADER_CG_PARTY_SET_STATE:
			PartySetState(ch, c_pData);
			break;

		case HEADER_CG_PARTY_USE_SKILL: // @fixme38
			if(!ch || ch == NULL)
				break;
			if(!c_pData || c_pData == NULL)
				break;
			PartyUseSkill(ch, c_pData);
			break;

		case HEADER_CG_PARTY_PARAMETER:
			PartyParameter(ch, c_pData);
			break;

		case HEADER_CG_ANSWER_MAKE_GUILD:
			AnswerMakeGuild(ch, c_pData);
			break;

		case HEADER_CG_GUILD:
			if ((iExtraLen = Guild(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_FISHING:
			Fishing(ch, c_pData);
			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

#ifdef ENABLE_GROWTH_PET_SYSTEM
		case HEADER_CG_PetSetName:
			BraveRequestPetName(ch, c_pData);
			break;
#endif

		case HEADER_CG_MYSHOP:
			if ((iExtraLen = MyShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

#ifdef WJ_OFFLINESHOP_SYSTEM
		case HEADER_CG_MY_OFFLINE_SHOP:
			if ((iExtraLen = MyOfflineShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

		case HEADER_CG_REFINE:
			Refine(ch, c_pData);
			break;

#if defined(WJ_COMBAT_ZONE)
		case HEADER_CG_COMBAT_ZONE_REQUEST_ACTION:
			CCombatZoneManager::instance().RequestAction(ch, c_pData);
			break;
#endif


		case HEADER_CG_BULK_WHISPER:
			BulkWhisperManager(ch, c_pData);
			break;

#ifdef __SASH_SYSTEM__
		case HEADER_CG_SASH:
			{
				Sash(ch, c_pData);
			}
			break;
#endif

#ifdef ENABLE_ADMIN_BAN_MANAGER
		case HEADER_CG_ADMIN_BAN_MANAGER:
			SendAdminBanManager(ch, c_pData);
			break;
#endif

#ifdef ENABLE_FEATURES_OXEVENT
		case HEADER_CG_OXEVENT_MANAGER:
			COXEventManager::instance().Manager(ch, c_pData);
			break;
#endif

		case HEADER_CG_CLIENT_VERSION:
			Version(ch, c_pData);
			break;

#ifdef WJ_CHANGELOOK_SYSTEM
		case HEADER_CG_CL:
			ChangeLook(ch, c_pData);
			break;
#endif

#ifdef WJ_SHOP_SEARCH_SYSTEM
		case HEADER_CG_SHOP_SEARCH:
			ShopSearch(ch, c_pData, false);
			break;

		case HEADER_CG_SHOP_SEARCH_SUB:
			ShopSearch(ch, c_pData, true);
			break;

		case HEADER_CG_SHOP_SEARCH_BUY:
			ShopSearchBuy(ch, c_pData);
			break;
#endif

#ifdef __SEND_TARGET_INFO__
		case HEADER_CG_TARGET_INFO_LOAD:
			{
				TargetInfoLoad(ch, c_pData);
			}
			break;
#endif

#ifdef __ENABLE_FISH_EVENT_SYSTEM__
		case HEADER_CG_FISH_EVENT_SEND:
			if ((iExtraLen = FishEvent(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_SHOW_CHEST_DROP
		case HEADER_CG_CHEST_DROP_INFO:
			ChestDropInfo(ch, c_pData);
			break;
#endif

		case HEADER_CG_DRAGON_SOUL_REFINE:
			{
				TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
				switch(p->bSubType)
				{
				case DS_SUB_HEADER_CLOSE:
					ch->DragonSoul_RefineWindow_Close();
					break;
				case DS_SUB_HEADER_DO_REFINE_GRADE:
					{
						DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STEP:
					{
						DSManager::instance().DoRefineStep(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STRENGTH:
					{
						DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
					}
					break;
				}
			}
			break;

#ifdef ENABLE_DROP_ITEM_WORLDARD
		case HEADER_CG_DROP_ITEM_SYSTEM:
			DropItemInfoSend(ch, c_pData);
			break;
#endif

#ifdef ENABLE_SWITCHBOT
		case HEADER_CG_SWITCHBOT:
			if ((iExtraLen = Switchbot(ch, c_pData, m_iBufferLeft)) < 0)
			{
				return -1;
			}
			break;
#endif
	}
	return (iExtraLen);
}

int CInputDead::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		return 0;
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		default:
			return (0);
	}

	return (iExtraLen);
}
#ifdef ENABLE_SWITCHBOT
int CInputMain::Switchbot(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	const TPacketCGSwitchbot* p = reinterpret_cast<const TPacketCGSwitchbot*>(data);

	if (uiBytes < sizeof(TPacketCGSwitchbot))
	{
		return -1;
	}

	const char* c_pData = data + sizeof(TPacketCGSwitchbot);
	uiBytes -= sizeof(TPacketCGSwitchbot);

	switch (p->subheader)
	{
	case SUBHEADER_CG_SWITCHBOT_START:
	{
		size_t extraLen = sizeof(TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
		if (uiBytes < extraLen)
		{
			return -1;
		}

		std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives;

		for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
		{
			const TSwitchbotAttributeAlternativeTable* pAttr = reinterpret_cast<const TSwitchbotAttributeAlternativeTable*>(c_pData);
			c_pData += sizeof(TSwitchbotAttributeAlternativeTable);

			vec_alternatives.emplace_back(*pAttr);
		}

		CSwitchbotManager::Instance().Start(ch->GetPlayerID(), p->slot, vec_alternatives);
		return extraLen;
	}

	case SUBHEADER_CG_SWITCHBOT_STOP:
	{
		CSwitchbotManager::Instance().Stop(ch->GetPlayerID(), p->slot);
		return 0;
	}
	}

	return 0;
}
#endif
