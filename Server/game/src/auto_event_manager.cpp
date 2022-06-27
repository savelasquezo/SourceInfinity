#include "stdafx.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "utils.h"
#include "config.h"
#include "constants.h"
#include "AutoNotice.h"

#include "questmanager.h"

static LPEVENT running_event = NULL;

EVENTINFO(EventsManagerInfoData)
{
	CEventsManager *pEvents;

	EventsManagerInfoData()
	: pEvents(0)
	{
	}
};

EVENTFUNC(automatic_event_timer)
{
	if (event == NULL)
		return 0;

	if (event->info == NULL)
		return 0;

	EventsManagerInfoData* info = dynamic_cast<EventsManagerInfoData*>(event->info);

	if (info == NULL)
		return 0;
	
	CEventsManager* pInstance = info->pEvents;

	if (pInstance == NULL)
		return 0;
	

	CEventsManager::instance().PrepareChecker();
	return PASSES_PER_SEC(1);
}

void CEventsManager::PrepareChecker()
{
#ifdef __ENABLE_AUTO_EVENT_MANAGER__
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);

	int day = vKey.tm_wday;
	int hour = vKey.tm_hour;
	int minute = vKey.tm_min;
	int second = vKey.tm_sec;
	CAutoNotice::instance().Check(day, hour, minute, second);
#endif
	//CEventsManager::instance().Check(day, hour, minute, second);
}

void CEventsManager::Check(int day, int hour, int minute, int second)
{
	//if (day == MONDAY)
	//{
	//	if (hour == 18 && quest::CQuestManager::instance().GetEventFlag("EventMoonlight") == 0)
	//	{
	//		CAutoNotice::instance().SendNoticeLine(LC_TEXT("EVENT_MOONSDROP"));
	//		quest::CQuestManager::instance().SetEventFlag("EventMoonlight", 1);
	//	}
	//}
	//if (day == TUESDAY)
	//{
	//	if (hour == 18 && quest::CQuestManager::instance().GetEventFlag("EventMoonlight") == 0)
	//	{
	//		CAutoNotice::instance().SendNoticeLine(LC_TEXT("EVENT_MOONSDROP"));
	//		quest::CQuestManager::instance().SetEventFlag("EventMoonlight", 1);
	//	}
	//}
	//if (day == WEDNESDAY)
	//{
	//	if (hour == 18 && quest::CQuestManager::instance().GetEventFlag("EventMoonlight") == 0)
	//	{
	//		CAutoNotice::instance().SendNoticeLine(LC_TEXT("EVENT_MOONSDROP"));
	//		quest::CQuestManager::instance().SetEventFlag("EventMoonlight", 1);
	//	}
	//}
	//if (day == THURSDAY)
	//{
	//	if (hour == 18 && quest::CQuestManager::instance().GetEventFlag("EventMoonlight") == 0)
	//	{
	//		CAutoNotice::instance().SendNoticeLine(LC_TEXT("EVENT_MOONSDROP"));
	//		quest::CQuestManager::instance().SetEventFlag("EventMoonlight", 1);
	//	}
	//}
	//if (day == FRIDAY)
	//{
	//	if (hour == 18 && quest::CQuestManager::instance().GetEventFlag("EventMoonlight") == 0)
	//	{
	//		CAutoNotice::instance().SendNoticeLine(LC_TEXT("EVENT_MOONSDROP"));
	//		quest::CQuestManager::instance().SetEventFlag("EventMoonlight", 1);
	//	}
	//}
	//if (day == SATURDAY)
	//{
	//	if (hour == 18 && quest::CQuestManager::instance().GetEventFlag("EventMoonlight") == 0)
	//	{
	//		CAutoNotice::instance().SendNoticeLine(LC_TEXT("EVENT_MOONSDROP"));
	//		quest::CQuestManager::instance().SetEventFlag("EventMoonlight", 1);
	//	}
	//}
	//if (day == SATURDAY)
	//{
	//	if (hour == 18 && quest::CQuestManager::instance().GetEventFlag("EventMoonlight") == 0)
	//	{
	//		CAutoNotice::instance().SendNoticeLine(LC_TEXT("EVENT_MOONSDROP"));
	//		quest::CQuestManager::instance().SetEventFlag("EventMoonlight", 1);
	//	}
	//}
	//if (day == SUNDAY)
	//{
	//	if (hour == 18 && quest::CQuestManager::instance().GetEventFlag("EventMoonlight") == 0)
	//	{
	//		CAutoNotice::instance().SendNoticeLine(LC_TEXT("EVENT_MOONSDROP"));
	//		quest::CQuestManager::instance().SetEventFlag("EventMoonlight", 1);
	//	}
	//}
}

bool CEventsManager::Initialize()
{
	if (running_event != NULL)
	{
		event_cancel(&running_event);
		running_event = NULL;
	}
	
	EventsManagerInfoData* info = AllocEventInfo<EventsManagerInfoData>();
	info->pEvents = this;

	running_event = event_create(automatic_event_timer, info, PASSES_PER_SEC(30));
	return true;
}

void CEventsManager::Destroy()
{

	if (running_event != NULL)
	{
		event_cancel(&running_event);
		running_event = NULL;
	}
}