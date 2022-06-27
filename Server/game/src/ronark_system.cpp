#include "stdafx.h"
#include "char.h"
#include "guild.h"
#include "questmanager.h"
#include "ronark_system.h"
#include "guild_manager.h"
#include "dungeon.h"

void CRonark::RonarkSet(LPCHARACTER pkAttacker, DWORD dam)
{
	if (NULL == pkAttacker)
		return;

	if(!pkAttacker->IsPC())
		return;
	
	CGuild* GetGuildRonark = CGuildManager::instance().FindGuild(pkAttacker->GetGuild()->GetID());
	quest::CQuestManager& R = quest::CQuestManager::instance();
	int GetRonarkHP =  R.GetEventFlag("RonarkHP");

	if (GetRonarkHP == 0)
	{
		R.SetEventFlag("RonarkHP", 1000000000);
		R.SetEventFlag("RonarkKill",0);
	}
	else
	{

		float Ronarkpercent = (double)(dam) / (double)(GetRonarkHP) * 100;
		int GetRonarkPoint = R.RonarkGetQuestFlag("RonarkPoint", GetGuildRonark->GetID());
		pkAttacker->GetGuild()->RonarkAmountDamage += Ronarkpercent;
		
		if (GetRonarkPoint > 99)
			return;
		
		if (pkAttacker->GetGuild()->RonarkAmountDamage > 1){
			int IRonarkpercent = pkAttacker->GetGuild()->RonarkAmountDamage;
			pkAttacker->GetGuild()->RonarkAmountDamage -= IRonarkpercent;
			R.RonarkSetQuestFlag("RonarkPoint",GetRonarkPoint+IRonarkpercent , GetGuildRonark->GetID());
		}

		int GetOneGuildPoint = R.GetEventFlag("OneGuildPoint");
		int GetTwoGuildPoint = R.GetEventFlag("TwoGuildPoint");
		int GetThreeGuildPoint = R.GetEventFlag("ThreeGuildPoint");
		int GetFourGuildPoint = R.GetEventFlag("FourGuildPoint");
		int GetFiveGuildPoint = R.GetEventFlag("FiveGuildPoint");
		int GetOneGuildID = R.GetEventFlag("OneGuildIDRanked");
		int GetTwoGuildID = R.GetEventFlag("TwoGuildIDRanked");
		int GetThreeGuildID = R.GetEventFlag("ThreeGuildIDRanked");
		int GetFourGuildID = R.GetEventFlag("FourGuildIDRanked");
		int GetFiveGuildID = R.GetEventFlag("FiveGuildIDRanked");
		int GetRonarkGuildCount = R.RonarkGetQuestFlag("RonarkGuildCount", GetGuildRonark->GetID());
		
		if (GetRonarkPoint > GetOneGuildPoint){
			if (GetOneGuildID != 0 && GetOneGuildID != GetGuildRonark->GetID()){
				R.SetEventFlag("TwoGuildPoint", GetOneGuildPoint);
				R.SetEventFlag("TwoGuildIDRanked", GetOneGuildID);
				R.SetEventFlag("OneGuildPoint", GetRonarkPoint);
				R.SetEventFlag("OneGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("OneGuildMemberCount", GetRonarkGuildCount);
			}
			else{
				R.SetEventFlag("OneGuildPoint", GetRonarkPoint);
				R.SetEventFlag("OneGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("OneGuildMemberCount", GetRonarkGuildCount);
			}
		}
		else if (GetRonarkPoint > GetTwoGuildPoint && GetRonarkPoint < GetOneGuildPoint){
			if (GetTwoGuildID != 0 && GetTwoGuildID != GetGuildRonark->GetID()){
				R.SetEventFlag("ThreeGuildPoint", GetTwoGuildPoint);
				R.SetEventFlag("ThreeGuildIDRanked", GetTwoGuildID);
				R.SetEventFlag("TwoGuildPoint", GetRonarkPoint);
				R.SetEventFlag("TwoGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("TwoGuildMemberCount", GetRonarkGuildCount);
			}
			else{
				R.SetEventFlag("TwoGuildPoint", GetRonarkPoint);
				R.SetEventFlag("TwoGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("TwoGuildMemberCount", GetRonarkGuildCount);
			}
		}
		else if (GetRonarkPoint > GetThreeGuildPoint && GetRonarkPoint < GetOneGuildPoint && GetRonarkPoint < GetTwoGuildPoint){
			if (GetThreeGuildID != 0 && GetThreeGuildID != GetGuildRonark->GetID()){
				R.SetEventFlag("FourGuildPoint", GetThreeGuildPoint);
				R.SetEventFlag("FourGuildIDRanked", GetThreeGuildID);
				R.SetEventFlag("ThreeGuildPoint", GetRonarkPoint);
				R.SetEventFlag("ThreeGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("ThreeGuildMemberCount", GetRonarkGuildCount);
			}
			else{
				R.SetEventFlag("ThreeGuildPoint", GetRonarkPoint);
				R.SetEventFlag("ThreeGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("ThreeGuildMemberCount", GetRonarkGuildCount);
			}
		}
		else if (GetRonarkPoint > GetFourGuildPoint && GetRonarkPoint < GetOneGuildPoint && GetRonarkPoint < GetTwoGuildPoint && GetRonarkPoint < GetThreeGuildPoint){
			if (GetFourGuildID != 0 && GetFourGuildID != GetGuildRonark->GetID()){
				R.SetEventFlag("FiveGuildPoint", GetFourGuildPoint);
				R.SetEventFlag("FiveGuildIDRanked", GetFourGuildID);
				R.SetEventFlag("FourGuildPoint", GetRonarkPoint);
				R.SetEventFlag("FourGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("FourGuildMemberCount", GetRonarkGuildCount);
			}
			else{
				R.SetEventFlag("FourGuildPoint", GetRonarkPoint);
				R.SetEventFlag("FourGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("FourGuildMemberCount", GetRonarkGuildCount);
			}
		}
		else if (GetRonarkPoint > GetFiveGuildPoint && GetRonarkPoint < GetOneGuildPoint && GetRonarkPoint < GetTwoGuildPoint && GetRonarkPoint < GetThreeGuildPoint && GetRonarkPoint < GetFourGuildPoint){
				R.SetEventFlag("FiveGuildPoint", GetRonarkPoint);
				R.SetEventFlag("FiveGuildIDRanked", GetGuildRonark->GetID());
				R.SetEventFlag("FiveGuildMemberCount", GetRonarkGuildCount);
		}
	}
}

void CRonark::RonarkClear(LPCHARACTER pkKiller)
{
	if (NULL == pkKiller)
		return;

	if(!pkKiller->IsPC())
		return;

	quest::CQuestManager& KillRonark = quest::CQuestManager::instance();
	KillRonark.SetEventFlag("RonarkHP", 0);
	KillRonark.RonarkSetQuestFlag("RonarkPoint",0 , pkKiller->GetGuild()->GetID());
	KillRonark.SetEventFlag("FiveGuildPoint",0);
	KillRonark.SetEventFlag("FourGuildPoint",0);
	KillRonark.SetEventFlag("ThreeGuildPoint",0);
	KillRonark.SetEventFlag("TwoGuildPoint",0);
	KillRonark.SetEventFlag("OneGuildPoint",0);

	KillRonark.SetEventFlag("FiveGuildIDRanked",0);
	KillRonark.SetEventFlag("FourGuildIDRanked",0);
	KillRonark.SetEventFlag("ThreeGuildIDRanked",0);
	KillRonark.SetEventFlag("TwoGuildIDRanked",0);
	KillRonark.SetEventFlag("OneGuildIDRanked",0);
	
	KillRonark.SetEventFlag("OneGuildMemberCount",0);
	KillRonark.SetEventFlag("TwoGuildMemberCount",0);
	KillRonark.SetEventFlag("ThreeGuildMemberCount",0);
	KillRonark.SetEventFlag("FourGuildMemberCount",0);
	KillRonark.SetEventFlag("FiveGuildMemberCount",0);
	
}