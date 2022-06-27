#define _item_combination_cpp_
#include "stdafx.h"
#include "config.h"
#include "constants.h"
#include "item_combination.h"
#include "utils.h"
#include "log.h"
#include "char.h"
#include "dev_log.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include <stdlib.h>
#include <sstream>
#define RETURN_IF_COMB_IS_NOT_OPENED(ch) if (!(ch)->IsCombOpen()) return

extern int test_server;

void Comb_open_costume(LPCHARACTER ch)
{
	if (NULL == ch)
		return;

	LPCHARACTER	npc = ch->GetQuestNPC();
	if (NULL == npc)
	{
		if (test_server)
			dev_log(LOG_DEB0, "combination_npc is NULL");
		
		return;
	}

	if (ch->IsCombOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_ALREADY_OPENED_WINDOW"));
		return;
	}

	//if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCombOpen() || ch->GetOfflineShopOwner())
	if ( ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCombOpen() || ch->GetOfflineShopOwner() )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 거래중(창고,교환,상점)에는 사용할 수 없습니다."));
		return;
	}

	long distance = DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY());
	if (distance >= COMB_MAX_DISTANCE)
	{
		sys_log(1, "Combination: TOO_FAR: %s distance %d", ch->GetName(), distance);
		return;
	}

	Comb_clean_item(ch);
	ch->SetCombNpc(npc);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "combination open_costume %d", npc->GetRaceNum());
}

void Comb_open_skillbook(LPCHARACTER ch)
{
	if (NULL == ch)
		return;

	LPCHARACTER	npc = ch->GetQuestNPC();
	if (NULL == npc)
	{
		if (test_server)
			dev_log(LOG_DEB0, "combination_npc is NULL");
		
		return;
	}

	if (ch->IsCombOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_ALREADY_OPENED_WINDOW"));
		return;
	}

#ifdef WJ_OFFLINESHOP_SYSTEM
	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCombOpen() || ch->GetOfflineShopOwner())
#else
	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCombOpen())
#endif
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 거래중(창고,교환,상점)에는 사용할 수 없습니다."));
		return;
	}

	long distance = DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY());
	if (distance >= COMB_MAX_DISTANCE)
	{
		sys_log(1, "Combination: TOO_FAR: %s distance %d", ch->GetName(), distance);
		return;
	}

	Comb_clean_item(ch);
	ch->SetCombNpc(npc);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "combination open_skillbook %d", npc->GetRaceNum());
}

void Comb_add_item(LPCHARACTER ch, int combination_index, int inven_index)
{
	LPITEM	item;
	LPITEM	*combination_item;

	// RETURN_IF_COMB_IS_NOT_OPENED(ch);

	if (inven_index < 0 || INVENTORY_AND_EQUIP_SLOT_MAX <= inven_index)
		return;

	if (combination_index < 0 || COMB_MAX_NUM <= combination_index)
		return;

	item = ch->GetInventoryItem(inven_index);
	if (item == NULL)
		return;

	combination_item = ch->GetCombItem();
	for (int i = 0; i < COMB_MAX_NUM; ++i)
	{
		if (item == combination_item[i])
		{
			combination_item[i] = NULL;
			break;
		}
	}

	combination_item[combination_index] = item;

	return;
}

void Comb_add_item2(LPCHARACTER ch, int combination_index, int inven_index)
{
	LPITEM	item;
	LPITEM	*combination_item;

	// RETURN_IF_COMB_IS_NOT_OPENED(ch);

	if (inven_index < 0 || INVENTORY_AND_EQUIP_SLOT_MAX <= inven_index)
		return;

	if (combination_index < 0 || COMB_MAX_NUM <= combination_index)
		return;

	item = ch->GetInventoryItem(inven_index);
	if (item == NULL)
		return;

	combination_item = ch->GetCombItem();
	for (int i = 0; i < COMB_MAX_NUM; ++i)
	{
		if (item == combination_item[i])
		{
			combination_item[i] = NULL;
			break;
		}
	}

	combination_item[combination_index] = item;

	if (test_server)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("combination[%d]: inventory[%d]: %s added"), combination_index, inven_index, item->GetName());

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("combination[%d]: inventory[%d]: %s added"), combination_index, inven_index, item->GetName());

	return;
}

void Comb_del_item(LPCHARACTER ch, int combination_index)
{
	LPITEM	item;
	LPITEM	*combination_item;

	// RETURN_IF_COMB_IS_NOT_OPENED(ch);
	if (combination_index < 0 || COMB_MAX_NUM <= combination_index)
		return;

	combination_item = ch->GetCombItem();
	if (combination_item[combination_index] == NULL)
		return;

	item = combination_item[combination_index];

	combination_item[combination_index] = NULL;
	if (test_server)
		ch->ChatPacket(CHAT_TYPE_INFO, "combination[%d]: combination[%d]: %s deleted", combination_index, item->GetCell(), item->GetName());

	return;
}

bool Comb_make_cotume(LPCHARACTER ch)
{
	LPCHARACTER	npc;
	LPITEM	*items;

	if (!ch->IsCombOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_WINDOW_NOT_OPENED"));
		return false;
	}

	npc = ch->GetQuestNPC();
	if (NULL == npc)
	{
		return false;
	}

	items = ch->GetCombItem();
	if (items[0] == NULL || items[1] == NULL || items[2] == NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_DRAG_ITEMS_WINDOW"));
		return false;
	}
	else if (items[0]->GetVnum() != 70065)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_NEED_TRANSFER_SCROLL"));
		return false;
	}
	else if (items[2] == NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_NEED_COSTUME_2"));
		return false;
	}
	else if (items[1]->GetType() != ITEM_COSTUME || items[1]->GetSubType() == COSTUME_SASH || items[2]->GetType() != ITEM_COSTUME || items[2]->GetSubType() == COSTUME_SASH )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_ONLY_COSTUME"));
		return false;
	}
	else if (items[1]->IsEquipped() || items[2]->IsEquipped())
	{
		(ch)->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_IS_EQUIPPED"));
		return false;
	}

	if (items[1]->GetSubType() == COSTUME_BODY && items[2]->GetSubType() != COSTUME_BODY)
		return false;
	
	if (items[1]->GetSubType() == COSTUME_HAIR && items[2]->GetSubType() != COSTUME_HAIR)
		return false;
	
	if (items[1]->GetSubType() == COSTUME_WEAPON && items[2]->GetSubType() != COSTUME_WEAPON)
		return false;

	if (items[1]->isLocked() || items[2]->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_IS_LOCKED"));
		return false;
	}

#ifdef ENABLE_SOULBIND_SYSTEM
	else if (items[0]->IsSealed() || items[1]->IsSealed())
	{
		(ch)->ChatPacket(CHAT_TYPE_INFO, "Voc?n? pode combinar itens bloqueados.");
		return false;
	}
#endif

	else if (items[1]->GetAttributeCount() < 1)
	{
		(ch)->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_NOT_HAVE_BONUS_COSTUME_2"));
		return false;
	}

	else if (items[2]->GetAttributeCount() < 1)
	{
		(ch)->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_NOT_HAVE_BONUS_COSTUME_2"));
		return false;
	}

	items[1]->ClearAttribute();
	for (int i = 0; i < items[2]->GetAttributeCount(); i++){
		items[1]->SetForceAttributeTransfer(i, items[2]->GetAttributeType(i), items[2]->GetAttributeValue(i));
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_SUCCESS"));
	ch->ChatPacket(CHAT_TYPE_COMMAND, "combination success_costume");

	if (items[0]->GetCount() == 1)
		items[0]->RemoveFromCharacter();
	else
		items[0]->SetCount(items[0]->GetCount() - 1);
	items[2]->RemoveFromCharacter();

	//items[2]->SetCount(0);
	items[2] = NULL;
	return true;
}

bool Comb_make_skillbook(LPCHARACTER ch)
{
	LPCHARACTER	npc;
	LPITEM	*items;
	int	i_number = 0;

	if (!ch->IsCombOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_WINDOW_NOT_OPENED"));
		return false;
	}

	npc = ch->GetQuestNPC();
	if (NULL == npc)
	{
		return false;
	}

	items = ch->GetCombItem();

	if (ch->GetGold() < 1000000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_NOT_ENOUGH_GOLD"));
		return false;
	}
	
	else if (ch->GetSkillGroup() == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_CH_NOT_HAVE_SKILL_GROUP"));
		return false;
	}

	for (int i = 0; i < 3; ++i)
	{
		if (items[i] == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_NEED_10_BOOKS"));
			return false;
		}
		else if (items[i]->GetVnum() != 50300)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_IS_BINDED"));
			return false;
		}

		if (items[i]->GetCount() != 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_COUNT_ERROR"));
			return false;
		}

#ifdef ENABLE_SOULBIND_SYSTEM
	else if (items[0]->IsSealed() || items[1]->IsSealed())
	{
		(ch)->ChatPacket(CHAT_TYPE_INFO, "Voc?n? pode combinar itens bloqueados.");
		return false;
	}
#endif

	}

	if (ch->GetJob() == JOB_WARRIOR)
	{
		if (ch->GetSkillGroup() == 1)
		{
			i_number = number(1,67);
		}
		else if (ch->GetSkillGroup() == 2)
		{
			i_number = number(16,22);
		}
	}
	else if (ch->GetJob() == JOB_ASSASSIN)
	{
		if (ch->GetSkillGroup() == 1)
		{
			i_number = number(31,37);
		}
		else if (ch->GetSkillGroup() == 2)
		{
			i_number = number(46,52);
		}
	}
	else if (ch->GetJob() == JOB_SURA)
	{
		if (ch->GetSkillGroup() == 1)
		{
			i_number = number(61,67);
		}
		else if (ch->GetSkillGroup() == 2)
		{
			i_number = number(76,82);
		}
	}
	else if (ch->GetJob() == JOB_SHAMAN)
	{
		if (ch->GetSkillGroup() == 1)
		{
			i_number = number(91,97);
		}
		else if (ch->GetSkillGroup() == 2)
		{
			i_number = number(106,112);
		}
	}
#ifdef ENABLE_WOLFMAN_CHARACTER
	else if (ch->GetJob() == JOB_WOLFMAN)
	{
		if (ch->GetSkillGroup() == 1)
		{
			i_number = number(170,176);
		}
	}
#endif

	for (int i = 0; i < COMB_MAX_NUM; ++i)
		items[i]->RemoveFromCharacter();

	ch->PointChange(POINT_GOLD, -1000000, false);

	ch->AutoGiveItem(50300)->SetSocket(0, i_number);

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_COMB_SUCCESS"));
	ch->ChatPacket(CHAT_TYPE_COMMAND, "combination success_skillbook");

	return true;
}

void Comb_close(LPCHARACTER ch)
{
	// RETURN_IF_COMB_IS_NOT_OPENED(ch);
	Comb_clean_item(ch);
	ch->SetCombNpc(NULL);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "combination close");
	dev_log(LOG_DEB0, "<combination> close (%s)", ch->GetName());
}

void Comb_clean_item(LPCHARACTER ch)
{
	LPITEM	*combination_item;

	combination_item = ch->GetCombItem();
	for (int i = 0; i < COMB_MAX_NUM; ++i)
	{
		if (combination_item[i] == NULL)
			continue;

		combination_item[i] = NULL;
	}
}

