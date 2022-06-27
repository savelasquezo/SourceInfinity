/*********************************************************************
* date        : 2018.02.25
* file        : refine.cpp
* author      : BEST Production
* version	  : 2.1.1
*/
#include "stdafx.h"
#include "refine.h"
#include "char.h"
#include "item_manager.h"
#include "item.h"
CRefineManager::CRefineManager()
{
}

CRefineManager::~CRefineManager()
{
}

#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	int EXTRA_REFINE_POTIONS_GRADE[3] = 
	{
		REFINE_VNUM_POTION_LOW,
		REFINE_VNUM_POTION_MEDIUM,
		REFINE_VNUM_POTION_EXTRA,
	};

	int CRefineManager::Result(LPCHARACTER ch)
	{
		int uninitialized = 0;
		int flag = ch->GetQuestFlag(REFINE_INCREASE);

		if (flag > 0)
			return flag;
		else
			return uninitialized;
	}

	bool CRefineManager::GetPercentage(LPCHARACTER ch, BYTE lLow, BYTE lMedium, BYTE lExtra, BYTE lTotal, LPITEM item)
	{
		BYTE ar_ListType[3] = {lLow, lMedium, lExtra};

		for (int it = 0; it <= JOURNAL_MAX_NUM; it++)
		{
			if (ar_ListType[it] > 0)
			{
				//@fix 12.01.2017
				if (item->GetType() == ITEM_METIN)
				{
					return false;
				}
				if (ch->CountSpecifyItem(EXTRA_REFINE_POTIONS_GRADE[it]) < 1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("refine_cannot_find_item"), ITEM_MANAGER::instance().GetTable(EXTRA_REFINE_POTIONS_GRADE[it])->szLocaleName);	
					return false;
				}
			}
		}	

		if (lTotal > 100 || lLow < 0 || lMedium < 0 || lExtra < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("refine_overflow"));		
			return false;
		}
		
		return true;
	}

	void CRefineManager::Reset(LPCHARACTER ch)
	{
		for (int it = 0; it <= JOURNAL_MAX_NUM; it++)
		{
			char buf[MAX_HOST_LENGTH + 1];
			snprintf(buf, sizeof(buf), "refine.mode_%d", it);
			
			if (ch->GetQuestFlag(buf) > 0)
			{
				ch->RemoveSpecifyItem(EXTRA_REFINE_POTIONS_GRADE[it], 1);
				ch->SetQuestFlag(REFINE_INCREASE, 0);
				ch->SetQuestFlag(buf, 0);
			}
		}
	}	

	void CRefineManager::Increase(LPCHARACTER ch, BYTE lLow, BYTE lMedium, BYTE lExtra)
	{
		int calcPercentage = 0;

		BYTE ar_ListType[3] = {lLow, lMedium, lExtra};
		int ar_ListPercentage[3] = {REFINE_PERCENTAGE_LOW, REFINE_PERCENTAGE_MEDIUM, REFINE_PERCENTAGE_EXTRA};
		
		for (int it = 0; it <= JOURNAL_MAX_NUM; it++)
		{
			if (ar_ListType[it] > 0)
			{
				char buf[MAX_HOST_LENGTH + 1];
				snprintf(buf, sizeof(buf), "refine.mode_%d", it);
				ch->SetQuestFlag(buf, 1);

				calcPercentage += ar_ListPercentage[it];		
			}
		}
		
		if (ch->GetQuestFlag(REFINE_INCREASE) < 1)
		{
			ch->SetQuestFlag(REFINE_INCREASE, calcPercentage);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("refine_already_is_increased"));
		}
	}
#endif

bool CRefineManager::Initialize(TRefineTable * table, int size)
{
	m_map_RefineRecipe.clear();
	for (int i = 0; i < size; ++i, ++table)
	{
		m_map_RefineRecipe.insert(std::make_pair(table->id, *table));
	}

	return true;
}

const TRefineTable* CRefineManager::GetRefineRecipe(DWORD vnum)
{
	if (vnum == 0)
		return NULL;

	itertype(m_map_RefineRecipe) it = m_map_RefineRecipe.find(vnum);

	if (it == m_map_RefineRecipe.end())
	{
		return NULL;
	}

	return &it->second;
}
