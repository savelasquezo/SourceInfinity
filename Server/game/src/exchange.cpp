#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "utils.h"
#include "desc.h"
#include "desc_client.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "locale_service.h"
#include "../../common/length.h"
#include "exchange.h"
#include "DragonSoul.h"
#include "config.h"
#include "questmanager.h"
#include "pvp.h"
#include "char_manager.h"
#include "utils.h"
#ifdef ENABLE_MESSENGER_BLOCK
#include "messenger_manager.h"
#endif
#if defined(WJ_COMBAT_ZONE)	
	#include "combat_zone.h"
#endif

void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL);

// 교환 패킷
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, DWORD arg3, void * pvData)
{
	if (!ch->GetDesc())
		return;

	struct packet_exchange pack_exchg;

	pack_exchg.header 		= HEADER_GC_EXCHANGE;
	pack_exchg.sub_header 	= sub_header;
	pack_exchg.is_me		= is_me;
	pack_exchg.arg1		= arg1;
	pack_exchg.arg2		= arg2;
	pack_exchg.arg3		= arg3;

	if (sub_header == EXCHANGE_SUBHEADER_GC_ITEM_ADD && pvData)
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(((LPITEM) pvData)->GetWindow(), ((LPITEM) pvData)->GetCell());
#endif
		thecore_memcpy(&pack_exchg.alSockets, ((LPITEM) pvData)->GetSockets(), sizeof(pack_exchg.alSockets));
		thecore_memcpy(&pack_exchg.aAttr, ((LPITEM) pvData)->GetAttributes(), sizeof(pack_exchg.aAttr));

#ifdef WJ_CHANGELOOK_SYSTEM
		pack_exchg.dwTransmutation = ((LPITEM) pvData)->GetTransmutation();
#endif
	}
	else
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(RESERVED_WINDOW, 0);
#endif
		memset(&pack_exchg.alSockets, 0, sizeof(pack_exchg.alSockets));
		memset(&pack_exchg.aAttr, 0, sizeof(pack_exchg.aAttr));

#ifdef WJ_CHANGELOOK_SYSTEM
		pack_exchg.dwTransmutation = 0;
#endif
	}

	ch->GetDesc()->Packet(&pack_exchg, sizeof(pack_exchg));
}

// 교환을 시작
bool CHARACTER::ExchangeStart(LPCHARACTER victim)
{
	if (this == victim)	// 자기 자신과는 교환을 못한다.
		return false;

	if (IsObserverMode())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("관전 상태에서는 교환을 할 수 없습니다."));
		return false;
	}

	//Donusumdeyken ticaret fix
	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("DONUSUMDEYKEN_TICARET_YAPAMASSIN"));
		return false;
	}
	
	if (victim->IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("KARSIDAKI_OYUNCU_DONUSUMDEYKEN_TICARET_YAPAMASSIN"));
		return false;
	}

	if (victim->IsNPC())
		return false;

#if defined(WJ_COMBAT_ZONE)
	if (CCombatZoneManager::Instance().IsCombatZoneMap(GetMapIndex()) || CCombatZoneManager::Instance().IsCombatZoneMap(victim->GetMapIndex()))
		return false;
#endif

	if (int(GetQuestFlag("Kilit.Enable")) == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GUVENLIK_ENGEL"));
		return false;
	}
	
	if (int(victim->GetQuestFlag("Kilit.Enable")) == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("guvenlik_engel_karsi"));
		return false;
	}

#ifdef ENABLE_PVP_ADVANCED
	if ((GetDuel("BlockExchange")))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("duel_block_function"));
		return false;
	}
	
	if ((victim->GetDuel("BlockExchange")))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("duel_block_victim"), victim->GetName());
		return false;
	}
#endif

#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::instance().IsBlocked_Target(GetName(), victim->GetName()))
	{
		//ben bloklad? hac?		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk"), victim->GetName());
		return false;
	}
	if (MessengerManager::instance().IsBlocked_Me(GetName(), victim->GetName()))
	{
		//o bloklad?hac?		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk_me"), victim->GetName());
		return false;
	}
#endif

	//PREVENT_TRADE_WINDOW

	if (IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetOfflineShopOwner())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 거래창이 열려있을경우 거래를 할수 없습니다."));
		return false;
	}

	if (victim->IsOpenSafebox() || victim->GetShopOwner() || victim->GetMyShop() || victim->IsCubeOpen() || victim->GetOfflineShopOwner())	
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 다른 거래중이라 거래를 할수 없습니다."));
		return false;
	}

#ifdef WJ_ITEM_COMBINATION_SYSTEM
	if (IsCombOpen())
	{
		ChatPacket( CHAT_TYPE_INFO, LC_TEXT("다른 거래창이 열려있을경우 거래를 할수 없습니다." ) );
		return false;
	}
#endif

#ifdef WJ_ITEM_COMBINATION_SYSTEM
	if (victim->IsCombOpen())
	{
		ChatPacket( CHAT_TYPE_INFO, LC_TEXT("상대방이 다른 거래중이라 거래를 할수 없습니다." ) );
		return false;
	}
#endif

#ifdef WJ_SECURITY_SYSTEM
	if (IsActivateSecurity() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot start exchange with security key activate"));
		return false;
	}
	
	if (victim->IsActivateSecurity() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot start exchange with victim security key activate"));
		return false;
	}	
#endif

	//END_PREVENT_TRADE_WINDOW
	int iDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());

	// 거리 체크
#ifdef REMOTE_TRADE_SYSTEM
//	if (iDist >= EXCHANGE_MAX_DISTANCE)
//		return false;
#else
	if (iDist >= EXCHANGE_MAX_DISTANCE)
		return false;
#endif

	if (GetExchange())
		return false;

	if (victim->GetExchange())
	{
		exchange_packet(this, EXCHANGE_SUBHEADER_GC_ALREADY, 0, 0, NPOS, 0);
		return false;
	}

	if (victim->IsBlockMode(BLOCK_EXCHANGE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방이 교환 거부 상태입니다."));
		return false;
	}

#ifdef ENABLE_ACCOUNT_LOCK
	if (GetAccountLockSize() == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "Hesap kilitliyken ticaret yapamazsin. ");
		return false;
	}

	if (victim->GetAccountLockSize() == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, "Karsi oyuncunun hesabi kilitliyken ticaret yapamaz.");
		return false;
	}
#endif

	SetExchange(M2_NEW CExchange(this));
	victim->SetExchange(M2_NEW CExchange(victim));

	victim->GetExchange()->SetCompany(GetExchange());
	GetExchange()->SetCompany(victim->GetExchange());

	//
	SetExchangeTime();
	victim->SetExchangeTime();

	exchange_packet(victim, EXCHANGE_SUBHEADER_GC_START, 0, GetVID(), NPOS, 0);
	exchange_packet(this, EXCHANGE_SUBHEADER_GC_START, 0, victim->GetVID(), NPOS, 0);

	return true;
}

CExchange::CExchange(LPCHARACTER pOwner)
{
	m_pCompany = NULL;

	m_bAccept = false;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		m_apItems[i] = NULL;
		m_aItemPos[i] = NPOS;
		m_abItemDisplayPos[i] = 0;
	}

	m_lGold = 0;
#ifdef ENABLE_CHEQUE_SYSTEM
	m_bCheque = 0;
#endif

	m_pOwner = pOwner;
	pOwner->SetExchange(this);

	m_pGrid = M2_NEW CGrid(4,6);
}

CExchange::~CExchange()
{
	M2_DELETE(m_pGrid);
}

bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos)
{
	assert(m_pOwner != NULL && GetCompany());

	if (!item_pos.IsValidItemPosition())
		return false;

	// 장비는 교환할 수 없음
	if (item_pos.IsEquipPosition())
		return false;

	LPITEM item;

	if (!(item = m_pOwner->GetItem(item_pos)))
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
	{
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템을 건네줄 수 없습니다."));
		return false;
	}

	if (item->GetType() == ITEM_WEAPON || item->GetType() == ITEM_ARMOR || item->GetType() == ITEM_BELT)
	{
		char szEventFlag[30];
		snprintf(szEventFlag, sizeof(szEventFlag), "%d.Engel", item->GetID());
		if (*szEventFlag)
		{
			if (quest::CQuestManager::instance().GetEventFlag(szEventFlag))
			{
				m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("item_engel"));
				return false;
			}
		}
	}

	if (true == item->isLocked())
	{
		return false;
	}
#ifdef ENABLE_SOULBIND_SYSTEM
	if (item->IsSealed()){
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Can't trade with sealed item."));
		return false;
	}
#endif

	// 이미 교환창에 추가된 아이템인가?
	if (item->IsExchanging())
	{
		sys_log(0, "EXCHANGE under exchanging");
		return false;
	}

	if (!m_pGrid->IsEmpty(display_pos, 1, item->GetSize()))
	{
		sys_log(0, "EXCHANGE not empty item_pos %d %d %d", display_pos, 1, item->GetSize());
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			continue;

		m_apItems[i]		= item;
		m_aItemPos[i]		= item_pos;
		m_abItemDisplayPos[i]	= display_pos;
		m_pGrid->Put(display_pos, 1, item->GetSize());

		item->SetExchanging(true);

		exchange_packet(m_pOwner,
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				true,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		exchange_packet(GetCompany()->GetOwner(),
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				false,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		sys_log(0, "EXCHANGE AddItem success %s pos(%d, %d) %d", item->GetName(), item_pos.window_type, item_pos.cell, display_pos);

		return true;
	}

	// 추가할 공간이 없음
	return false;
}

bool CExchange::RemoveItem(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return false;

	if (!m_apItems[pos])
		return false;

	TItemPos PosOfInventory = m_aItemPos[pos];
	m_apItems[pos]->SetExchanging(false);

	m_pGrid->Get(m_abItemDisplayPos[pos], 1, m_apItems[pos]->GetSize());

	exchange_packet(GetOwner(),	EXCHANGE_SUBHEADER_GC_ITEM_DEL, true, pos, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ITEM_DEL, false, pos, PosOfInventory, 0);

	Accept(false);
	GetCompany()->Accept(false);

	m_apItems[pos]	    = NULL;
	m_aItemPos[pos]	    = NPOS;
	m_abItemDisplayPos[pos] = 0;
	return true;
}

bool CExchange::AddGold(long gold)
{
	if (gold <= 0)
		return false;

	if (GetOwner()->GetGold() < gold)
	{
		// 가지고 있는 돈이 부족.
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
		return false;
	}

	if ( LC_IsCanada() == true || LC_IsEurope() == true )
	{
		if ( m_lGold > 0 )
		{
			return false;
		}
	}

	Accept(false);
	GetCompany()->Accept(false);

	m_lGold = gold;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, true, m_lGold, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, false, m_lGold, NPOS, 0);
	return true;
}

#ifdef ENABLE_CHEQUE_SYSTEM
bool CExchange::AddCheque(int Cheque)
{
	if (Cheque <= 0)
		return false;

	if (GetOwner()->GetCheque() < Cheque)
	{
		// 가지고 있는 돈이 부족.
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_CHEQUE, 0, 0, NPOS, 0);
		return false;
	}

	if (m_bCheque > 0)
		return false;

	Accept(false);
	GetCompany()->Accept(false);

	m_bCheque = Cheque;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_CHEQUE_ADD, true, m_bCheque, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_CHEQUE_ADD, false, m_bCheque, NPOS, 0);
	return true;
}
#endif
// 돈이 충분히 있는지, 교환하려는 아이템이 실제로 있는지 확인 한다.
bool CExchange::Check(int * piItemCount)
{
	if (GetOwner()->GetGold() < m_lGold)
		return false;
#ifdef ENABLE_CHEQUE_SYSTEM
	if (GetOwner()->GetCheque() < m_bCheque)
		return false;
#endif

	int item_count = 0;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
			continue;

		if (!m_aItemPos[i].IsValidItemPosition())
			return false;

		if (m_apItems[i] != GetOwner()->GetItem(m_aItemPos[i]))
			return false;

		++item_count;
	}

	*piItemCount = item_count;
	return true;
}

bool CExchange::CheckSpace()
{
    static CGrid s_grid1(5, INVENTORY_MAX_NUM / 5 / 4); // inven page 1   9 Rows a 5 Columns
    static CGrid s_grid2(5, INVENTORY_MAX_NUM / 5 / 4); // inven page 2   9 Rows a 5 Columns
    static CGrid s_grid3(5, INVENTORY_MAX_NUM / 5 / 4); // inven page 3   9 Rows a 5 Columns
    static CGrid s_grid4(5, INVENTORY_MAX_NUM / 5 / 4); // inven page 4   9 Rows a 5 Columns

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	static CGrid s_grid5(5, SKILL_BOOK_INVENTORY_MAX_NUM / 5 / 5);
	static CGrid s_grid6(5, UPGRADE_ITEMS_INVENTORY_MAX_NUM / 5 / 5);
	static CGrid s_grid7(5, STONE_INVENTORY_MAX_NUM / 5 / 5);
	static CGrid s_grid8(5, GIFT_BOX_INVENTORY_MAX_NUM / 5 / 5);
#endif
 
    s_grid1.Clear();
    s_grid2.Clear();
    s_grid3.Clear();
    s_grid4.Clear();
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	s_grid5.Clear();
	s_grid6.Clear();
	s_grid7.Clear();
	s_grid8.Clear();
#endif

    LPCHARACTER victim = GetCompany()->GetOwner();
    LPITEM item;
 
    int i;
 
    const int perPageSlotCount = INVENTORY_MAX_NUM / 4;

    for (i = 0; i < INVENTORY_MAX_NUM; ++i) {
        if (!(item = victim->GetInventoryItem(i)))
            continue;
 
        BYTE itemSize = item->GetSize();
 
        if (i < perPageSlotCount) // Notice: This is adjusted for 4 Pages only!
            s_grid1.Put(i, 1, itemSize);
        else if (i < perPageSlotCount * 2)
            s_grid2.Put(i - perPageSlotCount, 1, itemSize);
        else if (i < perPageSlotCount * 3)
            s_grid3.Put(i - perPageSlotCount * 2, 1, itemSize);
        else
            s_grid4.Put(i - perPageSlotCount * 3, 1, itemSize);
    }
	
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	int x;
	int y;
	const int perPageSkillBookSlotCount = SKILL_BOOK_INVENTORY_MAX_NUM / 1;
	const int perPageUpgradeItemsSlotCount = UPGRADE_ITEMS_INVENTORY_MAX_NUM / 1;
	const int perPageStoneSlotCount = STONE_INVENTORY_MAX_NUM / 1;
	const int perPageGiftBoxSlotCount = GIFT_BOX_INVENTORY_MAX_NUM / 1;

	for (y = 0; y < SKILL_BOOK_INVENTORY_MAX_NUM; ++y) {
		if (!(item = victim->GetSkillBookInventoryItem(y)))
			continue;

		if (y < perPageSkillBookSlotCount)
			s_grid5.Put(y, 1, item->GetSize());
	}
	
	for (y = 0; y < UPGRADE_ITEMS_INVENTORY_MAX_NUM; ++y) {
		if (!(item = victim->GetUpgradeItemsInventoryItem(y)))
			continue;

		if (y < perPageUpgradeItemsSlotCount)
			s_grid6.Put(y, 1, item->GetSize());
	}

	for (y = 0; y < STONE_INVENTORY_MAX_NUM; ++y) {
		if (!(item = victim->GetStoneInventoryItem(y)))
			continue;

		if (y < perPageStoneSlotCount)
			s_grid7.Put(y, 1, item->GetSize());
	}

	for (y = 0; y < GIFT_BOX_INVENTORY_MAX_NUM; ++y)
	{
		if (!(item = victim->GetGiftBoxInventoryItem(y)))
			continue;

		if (y < perPageGiftBoxSlotCount)
			s_grid8.Put(y, 1, item->GetSize());
	}
#endif

    static std::vector <WORD> s_vDSGrid(DRAGON_SOUL_INVENTORY_MAX_NUM);

    bool bDSInitialized = false;

    for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
    {
        if (!(item = m_apItems[i]))
            continue;

        if (item->IsDragonSoul())
        {
            if (!victim->DragonSoul_IsQualified())
                return false;

            if (!bDSInitialized) {
                bDSInitialized = true;
                victim->CopyDragonSoulItemGrid(s_vDSGrid);
            }
 
            bool bExistEmptySpace = false;
            WORD wBasePos = DSManager::instance().GetBasePosition(item);
            if (wBasePos >= DRAGON_SOUL_INVENTORY_MAX_NUM)
                return false;
 
            for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; i++)
            {
                WORD wPos = wBasePos + i;
                if (0 == s_vDSGrid[wBasePos])
                {
                    bool bEmpty = true;
                    for (int j = 1; j < item->GetSize(); j++)
                    {
                        if (s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM])
                        {
                            bEmpty = false;
                            break;
                        }
                    }
                    if (bEmpty)
                    {
                        for (int j = 0; j < item->GetSize(); j++)
                        {
                            s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM] = wPos + 1;
                        }
                        bExistEmptySpace = true;
                        break;
                    }
                }
                if (bExistEmptySpace)
                    break;
            }
            if (!bExistEmptySpace)
                return false;
        }
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
		{
			int iPos;
			if ((iPos = s_grid5.FindBlank(1, item->GetSize())) >= 0)
				s_grid5.Put(iPos, 1, item->GetSize());
		}
		else if (item->IsUpgradeItem())
		{
			int iPos;
			if ((iPos = s_grid6.FindBlank(1, item->GetSize())) >= 0)
				s_grid6.Put(iPos, 1, item->GetSize());
		}
		else if (item->IsStone())
		{
			int iPos;
			if ((iPos = s_grid7.FindBlank(1, item->GetSize())) >= 0)
				s_grid7.Put(iPos, 1, item->GetSize());
		}
		else if (item->IsGiftBox())
		{
			int iPos;
			if ((iPos = s_grid8.FindBlank(1, item->GetSize())) >= 0)
				s_grid8.Put(iPos, 1, item->GetSize());
		}
#endif
		else
		{
			int iPos;
			if ((iPos = s_grid1.FindBlank(1, item->GetSize())) >= 0)
				s_grid1.Put(iPos, 1, item->GetSize());
			else if ((iPos = s_grid2.FindBlank(1, item->GetSize())) >= 0)
				s_grid2.Put(iPos, 1, item->GetSize());
			else if ((iPos = s_grid3.FindBlank(1, item->GetSize())) >= 0)
				s_grid3.Put(iPos, 1, item->GetSize());
			else if ((iPos = s_grid4.FindBlank(1, item->GetSize())) >= 0)
				s_grid4.Put(iPos, 1, item->GetSize());
			else
				return false;
		}
	}
	return true;
}

// 교환 끝 (아이템과 돈 등을 실제로 옮긴다)
bool CExchange::Done()
{
	int		empty_pos, i;
	LPITEM	item;

	LPCHARACTER	victim = GetCompany()->GetOwner();

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

		if (item->IsDragonSoul())
			empty_pos = victim->GetEmptyDragonSoulInventory(item);
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			empty_pos = victim->GetEmptySkillBookInventory(item->GetSize());
		else if (item->IsUpgradeItem())
			empty_pos = victim->GetEmptyUpgradeItemsInventory(item->GetSize());
		else if (item->IsStone())
			empty_pos = victim->GetEmptyStoneInventory(item->GetSize());
		else if (item->IsGiftBox())
			empty_pos = victim->GetEmptyGiftBoxInventory(item->GetSize());
#endif
		else
			empty_pos = victim->GetEmptyInventory(item->GetSize());

		if (empty_pos < 0)
		{
			sys_err("Exchange::Done : Cannot find blank position in inventory %s <-> %s item %s",
					m_pOwner->GetName(), victim->GetName(), item->GetName());
			continue;
		}

		assert(empty_pos >= 0);

		if (item->GetVnum() == 90008 || item->GetVnum() == 90009) // VCARD
		{
			VCardUse(m_pOwner, victim, item);
			continue;
		}

		m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 1000);
#ifdef ENABLE_NEW_QUICK_SLOT_SYSTEM
		m_pOwner->SyncNewQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 1000);
#endif

		item->RemoveFromCharacter();
		if (item->IsDragonSoul())
			item->AddToCharacter(victim, TItemPos(DRAGON_SOUL_INVENTORY, empty_pos));
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsStone())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		else if (item->IsGiftBox())
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
#endif
		else
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		item->SetExchanging(false);
		{
			char exchange_buf[51];

			snprintf(exchange_buf, sizeof(exchange_buf), "%s %u %u", item->GetName(), GetOwner()->GetPlayerID(), item->GetCount());
			LogManager::instance().ItemLog(victim, item, "EXCHANGE_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%s %u %u", item->GetName(), victim->GetPlayerID(), item->GetCount());
			LogManager::instance().ItemLog(GetOwner(), item, "EXCHANGE_GIVE", exchange_buf);

			if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
			{
				LogManager::instance().GoldBarLog(victim->GetPlayerID(), item->GetID(), EXCHANGE_TAKE, "");
				LogManager::instance().GoldBarLog(GetOwner()->GetPlayerID(), item->GetID(), EXCHANGE_GIVE, "");
			}

			m_pOwner->CreateFly(FLY_SP_MEDIUM, victim);
			for (int i = 0; i < 3; i++){
			m_pOwner->CreateFly(FLY_SP_SMALL, victim);
			}
		}

		m_apItems[i] = NULL;
	}

	if (m_lGold)
	{
		GetOwner()->PointChange(POINT_GOLD, -m_lGold, true);
		victim->PointChange(POINT_GOLD, m_lGold, true);

		if (m_lGold > 1000)
		{
			char exchange_buf[51];
			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", GetOwner()->GetPlayerID(), GetOwner()->GetName());
			LogManager::instance().CharLog(victim, m_lGold, "EXCHANGE_GOLD_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", victim->GetPlayerID(), victim->GetName());
			LogManager::instance().CharLog(GetOwner(), m_lGold, "EXCHANGE_GOLD_GIVE", exchange_buf);
		}
	}
#ifdef ENABLE_CHEQUE_SYSTEM
	if (m_bCheque)
	{
		GetOwner()->PointChange(POINT_CHEQUE, -m_bCheque, true);
		victim->PointChange(POINT_CHEQUE, m_bCheque, true);
	}
#endif

	m_pGrid->Clear();
	return true;
}

// 교환을 동의
bool CExchange::Accept(bool bAccept)
{
	if (m_bAccept == bAccept)
		return true;

	m_bAccept = bAccept;

	// 둘 다 동의 했으므로 교환 성립
	if (m_bAccept && GetCompany()->m_bAccept)
	{
		int	iItemCount;

		LPCHARACTER victim = GetCompany()->GetOwner();

		//PREVENT_PORTAL_AFTER_EXCHANGE
		GetOwner()->SetExchangeTime();
		victim->SetExchangeTime();		
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

		// exchange_check 에서는 교환할 아이템들이 제자리에 있나 확인하고,
		// 엘크도 충분히 있나 확인한다, 두번째 인자로 교환할 아이템 개수
		// 를 리턴한다.
		if (!Check(&iItemCount))
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈이 부족하거나 아이템이 제자리에 없습니다."));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방의 돈이 부족하거나 아이템이 제자리에 없습니다."));
			goto EXCHANGE_END;
		}

		// 리턴 받은 아이템 개수로 상대방의 소지품에 남은 자리가 있나 확인한다.
		if (!CheckSpace())
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방의 소지품에 빈 공간이 없습니다."));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지품에 빈 공간이 없습니다."));
			goto EXCHANGE_END;
		}

		// 상대방도 마찬가지로..
		if (!GetCompany()->Check(&iItemCount))
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈이 부족하거나 아이템이 제자리에 없습니다."));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상대방의 돈이 부족하거나 아이템이 제자리에 없습니다."));
			goto EXCHANGE_END;
		}

		if (!GetCompany()->CheckSpace())
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("El Jugador no tiene Espacio en el Inventario Principal."));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Espacio Insuficiente, Libera Espacio en el Inventario Principal."));
			goto EXCHANGE_END;
		}

		if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		{
			sys_err("Cannot use exchange feature while DB cache connection is dead.");
			victim->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			goto EXCHANGE_END;
		}

		if (Done())
		{
#ifdef ENABLE_CHEQUE_SYSTEM
			if (m_lGold || m_bCheque)
#else
			if (m_lGold) // 돈이 있을 떄만 저장
#endif
				GetOwner()->Save();


			if (GetCompany()->Done())
			{
#ifdef ENABLE_CHEQUE_SYSTEM
				if (GetCompany()->m_lGold || GetCompany()->m_bCheque)
#else
				if (GetCompany()->m_lGold) // 돈이 있을 때만 저장
#endif
					victim->Save();

				DBManager::instance().DirectQuery("UPDATE player SET cheque = %d WHERE name = '%s'", GetOwner()->GetCheque(), GetOwner()->GetName());
				DBManager::instance().DirectQuery("UPDATE player SET cheque = %d WHERE name = '%s'", victim->GetCheque(), victim->GetName());
				// INTERNATIONAL_VERSION
				GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님과의 교환이 성사 되었습니다."), victim->GetName());
				victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s 님과의 교환이 성사 되었습니다."), GetOwner()->GetName());
				// END_OF_INTERNATIONAL_VERSION
			}
		}

EXCHANGE_END:
		Cancel();
		return false;
	}
	else
	{
		// 아니면 accept에 대한 패킷을 보내자.
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, true, m_bAccept, NPOS, 0);
		exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, false, m_bAccept, NPOS, 0);
		return true;
	}
}

// 교환 취소
void CExchange::Cancel()
{
	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_END, 0, 0, NPOS, 0);
	GetOwner()->SetExchange(NULL);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			m_apItems[i]->SetExchanging(false);
	}

	if (GetCompany())
	{
		GetCompany()->SetCompany(NULL);
		GetCompany()->Cancel();
	}

	M2_DELETE(this);
}

