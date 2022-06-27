#include "stdafx.h"
#include "../libgame/include/grid.h"
#include "constants.h"
#include "upgrade_items_storage.h"
#include "packet.h"
#include "char.h"
#include "desc_client.h"
#include "item.h"
#include "item_manager.h"
#include "log.h"

#include <algorithm>

CUpgradeItemsStorage::CUpgradeItemsStorage(LPCHARACTER pkChrOwner) : m_pkChrOwner(pkChrOwner), m_bIsOpen(0)
{
	assert(pkChrOwner != NULL);
	memset(m_pkItems, 0, sizeof(m_pkItems));
	m_pkGrid = M2_NEW CGrid(UPGRADE_ITEMS_STORAGE_SLOT_X_COUNT, UPGRADE_ITEMS_STORAGE_SLOT_Y_COUNT * UPGRADE_ITEMS_STORAGE_TAB_COUNT+1);
}

CUpgradeItemsStorage::~CUpgradeItemsStorage()
{
	__Destroy();
}

void CUpgradeItemsStorage::RemoveSpecifyItem(DWORD vnum, DWORD count)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < 90; ++i)
	{
		if (NULL == GetItem(i))
			continue;

		if (GetItem(i)->GetVnum() != vnum)
			continue;

		if (count >= GetItem(i)->GetCount())
		{
			count -= GetItem(i)->GetCount();
			GetItem(i)->SetCount(0);
			Remove(i);

			if (0 == count)
				return;
		}
		else
		{
			GetItem(i)->SetCount(GetItem(i)->GetCount() - count);
			return;
		}
	}
}

int CUpgradeItemsStorage::CountSpecifyItem(DWORD vnum)
{
	int	count = 0;
	LPITEM item;

	for (int i = 0; i < 90; ++i)
	{
		item = GetItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			count += item->GetCount();
		}
	}

	return count;
}

void CUpgradeItemsStorage::__Destroy()
{
	for (int i = 0; i < UPGRADE_ITEMS_STORAGE_TOTAL_SIZE; ++i)
	{
		if (m_pkItems[i] != NULL)
		{
			m_pkItems[i]->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(m_pkItems[i]);

			M2_DESTROY_ITEM(m_pkItems[i]->RemoveFromCharacter());
			m_pkItems[i] = NULL;
		}
	}

	if (m_pkGrid != NULL)
	{
		M2_DELETE(m_pkGrid);
		m_pkGrid = NULL;
	}
}

bool CUpgradeItemsStorage::Add(WORD dwPos, LPITEM pkItem, LPCHARACTER itemOwner)
{
	LPITEM _pkItem = ITEM_MANAGER::instance().CreateItem(pkItem->GetVnum(), pkItem->GetCount());

	if (!IsValidPosition(dwPos))
	{
		sys_err("UPGRADE_ITEMS_STORAGE: item on wrong position at %d (size of grid = %d)", dwPos, m_pkGrid->GetSize());
		return false;
	}

	_pkItem->SetOwnership(itemOwner);
	_pkItem->SetWindow(UPGRADE_ITEMS_STORAGE);
	_pkItem->SetCell(itemOwner, dwPos);
	_pkItem->ForceSave(m_pkChrOwner);
	ITEM_MANAGER::instance().FlushDelayedSave(_pkItem);

	m_pkGrid->Put(dwPos, 1, _pkItem->GetSize());
	m_pkItems[dwPos] = _pkItem;

	TPacketGCItemSet pack;

	pack.header	= HEADER_GC_UPGRADE_ITEMS_STORAGE_SET;
	pack.Cell	= TItemPos(UPGRADE_ITEMS_STORAGE, dwPos);
	pack.vnum	= _pkItem->GetVnum();
	pack.count	= _pkItem->GetCount();
	pack.flags	= _pkItem->GetFlag();
	pack.anti_flags	= _pkItem->GetAntiFlag();
	thecore_memcpy(pack.alSockets, _pkItem->GetSockets(), sizeof(pack.alSockets));
	thecore_memcpy(pack.aAttr, _pkItem->GetAttributes(), sizeof(pack.aAttr));

	DESC	*desc = m_pkChrOwner->GetDesc();
	if (!desc)
		return false;

	desc->Packet(&pack, sizeof(pack));
	sys_log(1, "UPGRADE_ITEMS_STORAGE: ADD %s %s count %d", m_pkChrOwner->GetName(), _pkItem->GetName(), _pkItem->GetCount());
	return true;
}

LPITEM CUpgradeItemsStorage::Get(WORD dwPos)
{
	if (dwPos >= m_pkGrid->GetSize())
		return NULL;

	return m_pkItems[dwPos];
}

LPITEM CUpgradeItemsStorage::Remove(WORD dwPos)
{
	LPITEM pkItem = Get(dwPos);

	if (!pkItem)
		return NULL;

	if (!m_pkGrid)
		sys_err("Upgrade Items Storage::Remove : nil grid");
	else
		m_pkGrid->Get(dwPos, 1, pkItem->GetSize());

	//pkItem->RemoveFromCharacter();
	pkItem->SetCell(NULL, 0);
	pkItem->SetWindow(RESERVED_WINDOW);
	pkItem->Save();

	m_pkItems[dwPos] = NULL;

	TPacketGCItemDel pack;

	pack.header	= HEADER_GC_UPGRADE_ITEMS_STORAGE_DEL;
	pack.pos	= dwPos;

	DESC	*desc = m_pkChrOwner->GetDesc();
	if (!desc)
		return false;

	desc->Packet(&pack, sizeof(pack));
	sys_log(1, "UPGRADE_ITEMS_STORAGE: REMOVE %s %s count %d", m_pkChrOwner->GetName(), pkItem->GetName(), pkItem->GetCount());
	return pkItem;
}

bool CUpgradeItemsStorage::IsEmpty(WORD dwPos, BYTE bSize)
{
	if (!m_pkGrid)
		return false;

	return m_pkGrid->IsEmpty(dwPos, 1, bSize);
}

LPITEM CUpgradeItemsStorage::GetItem(WORD bCell)
{
	if (bCell >= UPGRADE_ITEMS_STORAGE_TOTAL_SIZE)
	{
		sys_err("CHARACTER::GetItem: invalid item cell %d", bCell);
		return NULL;
	}

	return m_pkItems[bCell];
}

bool CUpgradeItemsStorage::MoveItem(WORD bCell, WORD bDestCell, BYTE count, LPCHARACTER ch)
{
	LPITEM item;

	int max_position = UPGRADE_ITEMS_STORAGE_TOTAL_SIZE;

	if (bCell >= max_position || bDestCell >= max_position)
		return false;

	if (!(item = GetItem(bCell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (item->GetCount() < count)
		return false;

	{
		LPITEM item2;

		if ((item2 = GetItem(bDestCell)) && item != item2 && item2->IsStackable() &&
				!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
				item2->GetVnum() == item->GetVnum())
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket(i) != item->GetSocket(i))
					return false;

			if (count == 0)
				count = item->GetCount();

			if (item->GetCount() == ITEM_MAX_COUNT && item2->GetCount() < ITEM_MAX_COUNT)
			{
				DWORD diafora = item->GetCount()-item2->GetCount();
				item->SetCount(item->GetCount() - diafora);
				item2->SetCount(item2->GetCount() + diafora);
				SendItems(ch);
				return true;
			}
			else if (item2->GetCount() == ITEM_MAX_COUNT && item->GetCount() < ITEM_MAX_COUNT)
			{
				DWORD diafora = item2->GetCount()-item->GetCount();
				item2->SetCount(item2->GetCount() - diafora);
				item->SetCount(item->GetCount() + diafora);
				SendItems(ch);
				return true;
			}
			
			count = MIN(ITEM_MAX_COUNT - item2->GetCount(), count);
			if (item->GetCount() >= count)
				Remove(bCell);

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);

			SendItems(ch);

			return true;
		}

		if (!IsEmpty(bDestCell, item->GetSize()))
			return false;

		m_pkGrid->Get(bCell, 1, item->GetSize());

		if (!m_pkGrid->Put(bDestCell, 1, item->GetSize()))
		{
			m_pkGrid->Put(bCell, 1, item->GetSize());
			return false;
		}
		else
		{
			m_pkGrid->Get(bDestCell, 1, item->GetSize());
			m_pkGrid->Put(bCell, 1, item->GetSize());
		}

		Remove(bCell);
		Add(bDestCell, item, ch);
	}

	return true;
}

bool CUpgradeItemsStorage::IsValidPosition(WORD dwPos)
{
	if (!m_pkGrid)
		return false;

	if (dwPos >= m_pkGrid->GetSize())
		return false;

	return true;
}

void CUpgradeItemsStorage::LoadItems()
{
	char szQuery[512];
	snprintf(szQuery, sizeof(szQuery),
		"SELECT id, pos, count, vnum "
		"FROM item WHERE owner_id = %d AND window = 'UPGRADE_ITEMS_STORAGE'", m_pkChrOwner->GetID());

	SQLMsg *msg = DBManager::instance().DirectQuery(szQuery);

	MYSQL_RES *res = msg->Get()->pSQLResult;
	if (!res)
		return;

	int rows;

	if ((rows = mysql_num_rows(res)) <= 0)
		return;

	for (int i = 0; i < rows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		TPlayerItem item;

		// int cur = 0;

		str_to_number(item.id, row[0]);
		str_to_number(item.pos, row[1]);
		str_to_number(item.count, row[2]);
		str_to_number(item.vnum, row[3]);


		LPITEM gItem = ITEM_MANAGER::instance().CreateItem(item.vnum, item.count, item.id);

		if (!gItem)
		{
			sys_err("CUpgradeItemsStorage cannot create item vnum %d id %u (char_id: %d)", item.vnum, item.id, m_pkChrOwner->GetID());
			continue;
		}

		// gItem->SetSkipSave(true);
		gItem->SetWindow(7);
		gItem->SetCell(NULL, item.pos);
		gItem->ForceSave(m_pkChrOwner);
		// m_pkChrOwner->ChatPacket(7, "load items window %d", gItem->GetWindow());
		// ITEM_MANAGER::instance().FlushDelayedSave(gItem);

		m_pkGrid->Put(item.pos, 1, gItem->GetSize());
		m_pkItems[item.pos] = gItem;
	}
}

int CUpgradeItemsStorage::GetEmptyInventory(BYTE size)
{
	for (int i = 0; i < 90; ++i)
		if (IsEmpty(i, size))
			return i;
	return -1;
}

void CUpgradeItemsStorage::Open()
{
	if (!m_bIsOpen)
	{
		LoadItems();
		m_bIsOpen = 1;
	}
}

void CUpgradeItemsStorage::SendItems(LPCHARACTER pkCh)
{
	for (int i = 0; i < UPGRADE_ITEMS_STORAGE_TOTAL_SIZE; ++i)
	{
		if (m_pkItems &&
			m_pkItems[i] != NULL)
		{
			LPITEM pkItem = m_pkItems[i];
			TPacketGCItemSet pack;

			pack.header = HEADER_GC_UPGRADE_ITEMS_STORAGE_SET;
			pack.Cell = TItemPos(UPGRADE_ITEMS_STORAGE, pkItem->GetCell());
			pack.vnum = pkItem->GetVnum();
			pack.count = pkItem->GetCount() == 0 ? 1 : pkItem->GetCount();
			pack.flags = pkItem->GetFlag();
			pack.anti_flags = pkItem->GetAntiFlag();
			thecore_memcpy(pack.alSockets, pkItem->GetSockets(), sizeof(pack.alSockets));
			thecore_memcpy(pack.aAttr, pkItem->GetAttributes(), sizeof(pack.aAttr));
			pkCh->GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
}
