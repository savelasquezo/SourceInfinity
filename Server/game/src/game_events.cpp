#include "stdafx.h"

#include "../../common/service.h"

#ifdef __ENABLE_EVENT_SYSTEM__
#include "game_events.h"
#endif
#include "char.h"
#include "packet.h"
#include "desc.h"
#include "quest.h"
#include "questmanager.h"
#include "utils.h"

CGameEventsManager::CGameEventsManager()
{
	for (int i = 0; i < EVENT_MAX_NUM; ++i)
	{
		m_pkActivateEvents[i] = false;
	}
}

CGameEventsManager::~CGameEventsManager()
{
}

void CGameEventsManager::SendEventCharacter(LPCHARACTER ch)
{
	if (!ch)
		return;
	
	for (int i = 0; i < EVENT_MAX_NUM; ++i) 
	{
		TPacketGCEventInfo pPacket;
		pPacket.bHeader = HEADER_GC_EVENT_INFO;
		pPacket.event_id = i;
		pPacket.isActivate = IsActivateEvent(i);		
		ch->GetDesc()->Packet(&pPacket, sizeof(TPacketGCEventInfo));
	}
}

void CGameEventsManager::SetActivateEvent(BYTE bEventID, bool isActivate)
{
	if (bEventID > EVENT_MAX_NUM)
		return;
	
	m_pkActivateEvents[bEventID] = isActivate;
}

bool CGameEventsManager::IsActivateEvent(BYTE bEventID)
{
	return m_pkActivateEvents[bEventID];
}