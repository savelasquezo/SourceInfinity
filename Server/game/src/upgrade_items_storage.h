#ifndef __INC_METIN_II_GAME_UPGRADE_ITEMS_STORAGE_H__
#define __INC_METIN_II_GAME_UPGRADE_ITEMS_STORAGE_H__
#include "db.h"

class CHARACTER;
class CItem;
class CGrid;

class CUpgradeItemsStorage
{
	public:
		enum
		{
			UPGRADE_ITEMS_STORAGE_SLOT_X_COUNT = 5,
			UPGRADE_ITEMS_STORAGE_SLOT_Y_COUNT = 9,
			UPGRADE_ITEMS_STORAGE_PAGE_SIZE = UPGRADE_ITEMS_STORAGE_SLOT_X_COUNT * UPGRADE_ITEMS_STORAGE_SLOT_Y_COUNT,
			UPGRADE_ITEMS_STORAGE_TAB_COUNT = 2,
			UPGRADE_ITEMS_STORAGE_TOTAL_SIZE = UPGRADE_ITEMS_STORAGE_TAB_COUNT * UPGRADE_ITEMS_STORAGE_PAGE_SIZE,
		};

		CUpgradeItemsStorage(LPCHARACTER pkChrOwner);
		~CUpgradeItemsStorage();

		bool Add(WORD dwPos, LPITEM pkItem, LPCHARACTER itemOwner);
		LPITEM Get(WORD dwPos);
		LPITEM Remove(WORD dwPos);

		bool MoveItem(WORD bCell, WORD bDestCell, BYTE count, LPCHARACTER ch);
		LPITEM		GetItem(WORD bCell);

		bool IsEmpty(WORD dwPos, BYTE bSize);
		bool IsValidPosition(WORD dwPos);
		int	 GetEmptyInventory(BYTE size);
		int CountSpecifyItem(DWORD vnum);

		void RemoveSpecifyItem(DWORD vnum, DWORD count);
		void LoadItems();
		void SendItems(LPCHARACTER pkCh);
		void Open();

	protected:
		void		__Destroy();

		LPCHARACTER	m_pkChrOwner;
		LPITEM		m_pkItems[UPGRADE_ITEMS_STORAGE_TOTAL_SIZE];
		CGrid *		m_pkGrid;
		bool		m_bIsOpen;
};

#endif
