#pragma once

enum
{
	EVENT_MOONSDROP				= 0,
	EVENT_CORDRACON				= 1,
	EVENT_HEXAGONAL				= 2,
	EVENT_CARDSOKEY				= 3,
	EVENT_FISHPUZLE				= 4,
	EVENT_RONARKGLD				= 5,
	EVENT_OXEVENTQZ				= 6,
	EVENT_BUDOKANPV				= 7,
	EVENT_TOURNAMNT				= 8,
	EVENT_TOTALWARS				= 9,
	EVENT_KINGDOMSW				= 10,
	EVENT_CHRISTMAS				= 11,
	EVENT_EASTEREGG				= 12,
	EVENT_SUMMERWKN				= 13,
	EVENT_HALLOWEEN				= 14,
	EVENT_VALENTINE				= 15,
	EVENT_TANAKASHT				= 16,
	EVENT_METINESEV				= 17,
	EVENT_BOSSEVENT				= 18,
	EVENT_FISHRANKG				= 19,
	EVENT_MININGMAP				= 20,
	EVENT_EXPDROPYG				= 21,
	EVENT_MAX_NUM				= 22,
};

class CGameEventsManager : public singleton<CGameEventsManager>
{
	public:
		CGameEventsManager();
		~CGameEventsManager();
		
		void	SendEventCharacter(LPCHARACTER ch);
		void	SetActivateEvent(BYTE bEventID, bool isActivate);
		bool	IsActivateEvent(BYTE bEventID);
	protected:
		bool	m_pkActivateEvents[EVENT_MAX_NUM];
	private:
};