/*********************************************************************
* date        : 2017.11.26
* file        : reborn.cpp
* author      : BEST Production
* version	  : 2.1.6
*/
#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "desc.h"
#include "char.h"
#include "db.h"
#include "config.h"
#include "reborn.h"
#include "affect.h"

#define ENABLE_WOLFMAN_CHARACTER

RebornManager::RebornManager()
{
}

RebornManager::~RebornManager()
{
}
/*********
* Table with bonus name affect ( if u want to change bonus you can find in enum EPointTypes on char.h other bonus type )
*/
int affect_name[] = {POINT_MAX_HP, POINT_ATTBONUS_HUMAN, POINT_CRITICAL_PCT, POINT_ATTBONUS_MONSTER}; 

/*********
* Table with value bonus
* 1000 / 8 / 12 / 15 ==> POINT_MAX_HP / POINT_ATTBONUS_HUMAN / POINT_CRITICAL_PCT / POINT_ATTBONUS_MONSTER
*/
int affect_value_A[] = {1000, 8, 12, 15}; // Reborn I 
int affect_value_B[] = {2000, 8, 12, 15}; //  Reborn II
int affect_value_C[] = {3000, 8, 12, 15}; // Reborn III

/*********
* Settins for points/level
*/
int level_set = 1; //Level reset when you up in grade reborn
int level_max = 105; // Max level from your server to can activate reborn
int stat_points_max = 90; // Max points stat in character 

/*********
* Get type or make new grade reborn ( not change this )
*/
int type_level[] = {0, 1, 2, 3};

/*********
* The time for waiting can disable / enable anonymous way
*/
int time_anonymous = 10;

/*********
* Get type affect on reborn loaded from affect.h ( not change this )
*/
int get_affect[] = {
							AFFECT_REBORN_1_A, AFFECT_REBORN_1_B, AFFECT_REBORN_1_C, AFFECT_REBORN_1_D, 
							AFFECT_REBORN_2_A, AFFECT_REBORN_2_B, AFFECT_REBORN_2_C, AFFECT_REBORN_2_D,
							AFFECT_REBORN_3_A, AFFECT_REBORN_3_B, AFFECT_REBORN_3_C, AFFECT_REBORN_3_D};

/*********
* Get status from anonymous mode for insert in mysql log_reborn what mode have actual
*/		
const char* name_anonymous[] = {"Aktif", "Deaktif"};				
							
/*********
* Get race from insert in mysql log_rebron
*/													
const char* set_race = NULL;

/*********
* The name of each character races
*/
const char* name_race[] = {"Shaman", 
									"Sura", 
									"Ninja", 
									"Warrior",
									"Wolfman"};
								
/*********
* Global value for other function check
*/								
int global_value[] = {0, 1, 2, 3, 9, 10};	

/*********
* Translate small informations
*/	
#define EN  // Activate translate in language english

#ifdef EN
const char* translate[] = {"Hata ayiklama: Bagimsiz degisken nameLog gecersiz !", "[Reborn] Bu kadar hizli yapamazsin, %u saniye bekle!", "[Reborn] Zaten bir dereceniz mevcut. Reborn I !", "[Reborn] Zaten bir dereceniz mevcut. Reborn II !","[Reborn] Zaten bir dereceniz mevcut. Reborn III !","[Reborn] Maksimum reborn'a ulastin!","[Reborn] Reborn icin gerekli seviye %u !",	"[Reborn] Anonim mod '%s' !", "[Reborn] Bu komutu sadece yoneticinin kullanmasina izin verilir!"}; 
#endif	
#ifdef DE
const char* translate[] = {"Debug: Ihr Argument nameLog ist ungültig!", "<< Reborn >> Sie können nicht so schnell tun, warten %u eine Sekunde!", "<< Reborn >> Schon besitzen Sie Boni Grad Reborn I!", "<< Reborn >> Schon besitzen Sie Boni Grad Reborn II!","<< Reborn >> Schon besitzen Sie Boni Grad Reborn III!","<< Reborn >> Sie haben bereits die maximale Grad der neu geboren!", "<< Reborn >> Sie müssen level %u für neu geboren!", "<< Reborn >> Anonymous-Modus wurde '%s' !", "<< Reborn >> Nur der Administrator darf diesen Befehl zu verwenden!"}; 
#endif	
#ifdef IT
const char* translate[] = {"Debug: Il tuo argomento non è valido nameLog!", "<< Reborn >> Non si può fare che così in fretta, attendere %u di secondo!", "<< Reborn >> È già propri bonus grado Reborn I!", "<< Reborn >> È già propri bonus grado Reborn II!","<< Reborn >> È già propri bonus grado Reborn III!", "<< Reborn >> Hai già il massimo grado di rinascere!", "<< Reborn >> È necessario livello di %u per rinascere!", "<< Reborn >> Modalità Anonimo è stata '%s' !", "<< Reborn >> Solo l'amministratore è autorizzato ad utilizzare questo comando!"}; 
#endif

void RebornManager::ResetCharacter(LPCHARACTER ch)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;

	ch->ResetPoint(MINMAX(1, level_set, PLAYER_MAX_LEVEL_CONST));  // Reset points
	ch->ClearSkill(); // Clear all skill
	ch->ClearSubSkill();
	ch->SetSkillGroup(0); // Clear skill group
	
	// Points: skills, sub-skills, and stats
	ch->PointChange(POINT_SKILL, level_set - ch->GetLevel());
	ch->PointChange(POINT_SUB_SKILL, level_set < global_value[5] ? global_value[0] : level_set - MAX(ch->GetLevel(), global_value[4]));
	ch->PointChange(POINT_STAT, ((MINMAX(global_value[1], level_set, stat_points_max) - ch->GetLevel()) * global_value[3]) + ch->GetPoint(POINT_LEVEL_STEP));
	// Level
	ch->PointChange(POINT_LEVEL, level_set - ch->GetLevel());
	// HP, SP
	ch->SetRandomHP((level_set - global_value[1]) * number(JobInitialPoints[ch->GetJob()].hp_per_lv_begin, JobInitialPoints[ch->GetJob()].hp_per_lv_end));
	ch->SetRandomSP((level_set - global_value[1]) * number(JobInitialPoints[ch->GetJob()].sp_per_lv_begin, JobInitialPoints[ch->GetJob()].sp_per_lv_end));
	// Recovery
	ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
	ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
	
	ch->ComputePoints(); // Update all points from character
	ch->PointsPacket(); // Update Packet points
	ch->SkillLevelPacket();	// Update packet level
}

void RebornManager::SendLogs(LPCHARACTER ch, const char* nameLog)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;

	if (!*nameLog)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, translate[0]);
		return;
	}	
	
	if(ch->GetJob() == JOB_SHAMAN)
		set_race = name_race[0];
	else if(ch->GetJob() == JOB_SURA)
		set_race = name_race[1];
	else if(ch->GetJob() == JOB_ASSASSIN)
		set_race = name_race[2];
	else if(ch->GetJob() == JOB_WARRIOR)
		set_race = name_race[3];
#ifdef ENABLE_WOLFMAN_CHARACTER
	else if(ch->GetJob() == JOB_WOLFMAN)
		set_race = name_race[4];
#endif

	if (!strcmp(nameLog, "send_logs_1"))
	{	
		char szQuery[512];
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.log_reborn (owner_id, type, name, time, x, y, mapIndex, channel, race) VALUES(%u, '%s', '%s', NOW(), %ld, %ld, %ld, %d, '%s')", 
																ch->GetPlayerID(), "Reborn I", ch->GetName(), ch->GetX(), ch->GetY(), ch->GetMapIndex(), g_bChannel, set_race, name_anonymous[1]);
		DBManager::Instance().DirectQuery(szQuery);
	}		
	
	else if (!strcmp(nameLog, "send_logs_2"))
	{		
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery("UPDATE player.log_reborn SET type='%s', time=NOW(), x=%ld, y=%ld, mapIndex=%ld, channel=%d WHERE owner_id = %u", "Reborn II", 
																ch->GetX(), ch->GetY(), ch->GetMapIndex(), g_bChannel, ch->GetPlayerID()));	
	}
	
	else if (!strcmp(nameLog, "send_logs_3"))
	{		
		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery("UPDATE player.log_reborn SET type='%s', time=NOW(), x=%ld, y=%ld, mapIndex=%ld, channel=%d WHERE owner_id = %u", "Reborn III", 
																ch->GetX(), ch->GetY(), ch->GetMapIndex(), g_bChannel, ch->GetPlayerID()));	
	}
}

void RebornManager::SetAnonymous(LPCHARACTER ch, const char* valueAnonymous)
{
	int isAnonymous = ch->GetQuestFlag("reborn.valueAnonymous");
	int isGetLastTime = ch->GetQuestFlag("reborn.flood_database");
	
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;

	if (!*valueAnonymous)
	{	
		return;
	}
	
	if (!strcmp(valueAnonymous, "anonymous"))
	{	
		if (isAnonymous == global_value[1]) 
		{
			if (get_global_time() < isGetLastTime + time_anonymous)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, translate[1], time_anonymous);
				return;
			}
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SetReborn pro_1");
				ch->ChatPacket(CHAT_TYPE_NOTICE, translate[7], name_anonymous[0]);
				ch->ShowReborn(false);
				std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery("UPDATE player.log_reborn SET mode_anonymous='%s' WHERE owner_id = %u", name_anonymous[0], ch->GetPlayerID()));		
				ch->SetQuestFlag("reborn.valueAnonymous", global_value[2]);
				ch->SetQuestFlag("reborn.flood_database", get_global_time());
		}
		
		else if (isAnonymous == global_value[2]) 
		{	
			if (get_global_time() < isGetLastTime + time_anonymous)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, translate[1], time_anonymous);
				return;
			}
				ch->ChatPacket(CHAT_TYPE_COMMAND, "SetReborn pro_0");
				ch->ChatPacket(CHAT_TYPE_NOTICE, translate[7], name_anonymous[1]);
				ch->ShowReborn(true);
				std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery("UPDATE player.log_reborn SET mode_anonymous='%s' WHERE owner_id = %u", name_anonymous[1], ch->GetPlayerID()));				
				ch->SetQuestFlag("reborn.valueAnonymous", global_value[0]);
				ch->SetQuestFlag("reborn.flood_database", get_global_time());
		}
	}	
}

void RebornManager::SetAffect(LPCHARACTER ch, const char* valueAffect)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;
	
	if (!strcmp(valueAffect, "send_affect_1"))
	{
		if (!ch->FindAffect(get_affect[0]) && !ch->FindAffect(get_affect[1]) && !ch->FindAffect(get_affect[2]) && !ch->FindAffect(get_affect[3]))
		{
			ch->AddAffect(get_affect[0], affect_name[0], affect_value_A[0], 0, 60*60*60*365, 0, true);
			ch->AddAffect(get_affect[1], affect_name[1], affect_value_A[1], 0, 60*60*60*365, 0, true);
			ch->AddAffect(get_affect[2], affect_name[2], affect_value_A[2], 0, 60*60*60*365, 0, true);	
			ch->AddAffect(get_affect[3], affect_name[3], affect_value_A[3], 0, 60*60*60*365, 0, true);
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, translate[2]);		
	}	
	
	else if (!strcmp(valueAffect, "send_affect_2"))
	{	
		if (!ch->FindAffect(get_affect[4]) && !ch->FindAffect(get_affect[5]) && !ch->FindAffect(get_affect[6]) && !ch->FindAffect(get_affect[7]))
		{
			ch->AddAffect(get_affect[4], affect_name[0], affect_value_B[0], 0, 60*60*60*365, 0, true);
			ch->AddAffect(get_affect[5], affect_name[1], affect_value_B[1], 0, 60*60*60*365, 0, true);
			ch->AddAffect(get_affect[6], affect_name[2], affect_value_B[2], 0, 60*60*60*365, 0, true);	
			ch->AddAffect(get_affect[7], affect_name[3], affect_value_B[3], 0, 60*60*60*365, 0, true);
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, translate[3]);
	}
	
	else if (!strcmp(valueAffect, "send_affect_3"))
	{	
		if (!ch->FindAffect(get_affect[8]) && !ch->FindAffect(get_affect[9]) && !ch->FindAffect(get_affect[10]) && !ch->FindAffect(get_affect[11]))
		{
			ch->AddAffect(get_affect[8], affect_name[0], affect_value_C[0], 0, 60*60*60*365, 0, true);
			ch->AddAffect(get_affect[9], affect_name[1], affect_value_C[1], 0, 60*60*60*365, 0, true);
			ch->AddAffect(get_affect[10], affect_name[2], affect_value_C[2], 0, 60*60*60*365, 0, true);	
			ch->AddAffect(get_affect[11], affect_name[3], affect_value_C[3], 0, 60*60*60*365, 0, true);
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, translate[4]);
	}	
	
	else if (!strcmp(valueAffect, "remove_affect_administrator"))
	{
		if (ch->GetGMLevel() == GM_IMPLEMENTOR)	
		{				
			ch->RemoveAffect(get_affect[0]);
			ch->RemoveAffect(get_affect[1]);	
			ch->RemoveAffect(get_affect[2]);	
			ch->RemoveAffect(get_affect[3]);	
			ch->RemoveAffect(get_affect[4]);	
			ch->RemoveAffect(get_affect[5]);	
			ch->RemoveAffect(get_affect[6]);	
			ch->RemoveAffect(get_affect[7]);	
			ch->RemoveAffect(get_affect[8]);	
			ch->RemoveAffect(get_affect[9]);
			ch->RemoveAffect(get_affect[10]);	
			ch->RemoveAffect(get_affect[11]);			
		}
		else
																	// If u want to get command from this you need to use in chat: /set_reborn remove_affect_administrator		
			ch->ChatPacket(CHAT_TYPE_INFO, translate[8]);	
	}
}

void RebornManager::SetLevel(LPCHARACTER ch)
{
	if (NULL == ch)
		return;

	if (!ch->IsPC())
		return;
	
	if (ch->GetRealReborn() == type_level[3])
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE, translate[5]);	
		return;	
	}	
	
	else if (ch->GetRealReborn() == type_level[0])
	{
		if (ch->GetLevel() == level_max)
		{
			ResetCharacter(ch); // Reset Character
			SetAffect(ch, "send_affect_1"); // Set bonus on reborn I
			SendLogs(ch, "send_logs_1"); // Send logs in table log_reborn with actual informations on reborn name
			
			ch->UpdateReborn(type_level[1] - ch->GetRealReborn()); // Set Level Reborn
			ch->ChatPacket(CHAT_TYPE_COMMAND, "SetReborn arg1"); // Set argument in client command for big notice
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, translate[6], level_max);			
	}
	
	else if (ch->GetRealReborn() == type_level[1])
	{	
		if (ch->GetLevel() == level_max)
		{
			ResetCharacter(ch); // Reset Character
			SetAffect(ch, "send_affect_2"); // Set bonus on reborn II
			SendLogs(ch, "send_logs_2"); // Send logs in table log_reborn with actual informations on reborn name	
			
			ch->UpdateReborn(type_level[2] - ch->GetRealReborn()); // Set Level Reborn	
			ch->ChatPacket(CHAT_TYPE_COMMAND, "SetReborn arg2"); // Set argument in client command for big notice
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, translate[6], level_max);			
	}
	
	else if (ch->GetRealReborn() == type_level[2])
	{	
		if (ch->GetLevel() == level_max)
		{
			ResetCharacter(ch); // Reset Character
			SetAffect(ch, "send_affect_3"); // Set bonus on reborn III 
			SendLogs(ch, "send_logs_3"); // Send logs in table log_reborn with actual informations on reborn name	
			
			ch->UpdateReborn(type_level[3] - ch->GetRealReborn()); // Set Level Reborn	
			ch->ChatPacket(CHAT_TYPE_COMMAND, "SetReborn arg3"); // Set argument in client command for big notice
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, translate[6], level_max);				
	}
}	