#ifndef __INC_METIN_II_GAME_SHOP_H__
#define __INC_METIN_II_GAME_SHOP_H__

enum
{
	SHOP_MAX_DISTANCE = 1000
};

class CGrid;

/* ---------------------------------------------------------------------------------- */
class CShop
{
	public:
		typedef struct shop_item
		{
			DWORD	vnum;		// 아이템 번호
			long	price;		// 가격
			long	price_cheque;

			BYTE	count;		// 아이템 개수
#ifdef __MULTI_SHOP__
			DWORD	wPriceVnum;
			DWORD	wPrice;
#ifdef ENABLE_BUY_ITEMS_AND_YANG
			long long	llPriceYang;
#endif
#endif
			LPITEM	pkItem;
			int		itemid;		// 아이템 고유아이디

			shop_item()
			{
				vnum = 0;
				price = 0;
#ifdef __MULTI_SHOP__
				wPriceVnum = 0;
				wPrice = 0;
#ifdef ENABLE_BUY_ITEMS_AND_YANG
				llPriceYang=0;
#endif
#endif
				count = 0;
				itemid = 0;
				pkItem = NULL;
			}
		} SHOP_ITEM;

		CShop();
		~CShop();

		bool SearchItem(LPCHARACTER ch, const char* item_name, int iJob, int iType, int iSubType, int iMinLevel, int iMaxLevel, int iMinRefine, int iMaxRefine, int iMinPrice, int iMaxPrice, int iMinCheque, int iMaxCheque);
		bool	Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pItemTable);
		void	SetShopItems(TShopItemTable * pItemTable, BYTE bItemCount);
		void	SetShopItemsPrivateShop(TShopItemTable2 * pItemTable, BYTE bItemCount);

		virtual void		SetPCShop(LPCHARACTER ch) { m_pkPC = ch; };
		virtual LPCHARACTER GetPCShop() { return m_pkPC; }
		virtual bool		IsPCShop() { return m_pkPC ? true : false; };


		// 게스트 추가/삭제
		virtual bool	AddGuest(LPCHARACTER ch,DWORD owner_vid, bool bOtherEmpire);
		void	RemoveGuest(LPCHARACTER ch);
		void	RemoveAllGuests();

		// 물건 구입
		//virtual int	Buy(LPCHARACTER ch, BYTE pos);
		virtual int Buy(LPCHARACTER ch, BYTE pos, bool isSearchBuy = false);

		// 게스트에게 패킷을 보냄
		void	BroadcastUpdateItem(BYTE pos);

		// 판매중인 아이템의 갯수를 알려준다.
		int		GetNumberByVnum(DWORD dwVnum);

		// 아이템이 상점에 등록되어 있는지 알려준다.
		virtual bool	IsSellingItem(DWORD itemID);

		DWORD	GetVnum() { return m_dwVnum; }
		DWORD	GetNPCVnum() { return m_dwNPCVnum; }
		virtual int	BuyFromGlass(LPCHARACTER ch_buyer, BYTE pos);
		void	InsertInMysql(LPCHARACTER lpSeller, LPITEM item, BYTE bPos, DWORD dwItemPrice);
		void	RemoveFromMysql(DWORD dwSellerPID, BYTE bPos);

	protected:
		void	Broadcast(const void * data, int bytes);
		
	protected:
		DWORD				m_dwVnum;
		DWORD				m_dwNPCVnum;

		CGrid *				m_pGrid;

		typedef TR1_NS::unordered_map<LPCHARACTER, bool> GuestMapType;
		GuestMapType m_map_guest;
		std::vector<SHOP_ITEM>		m_itemVector;	// 이 상점에서 취급하는 물건들

		LPCHARACTER			m_pkPC;
};

#endif 
