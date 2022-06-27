
#include "stdafx.h"
#include "../../common/tables.h"
#include "ClientManager.h"
#ifdef __SKILL_COLOR_SYSTEM__
	#include "../../common/length.h"
#endif
#include "Main.h"
#include "QID.h"
#include "ItemAwardManager.h"
#include "HB.h"
#include "Cache.h"
#include "../../common/service.h"
extern bool g_bHotBackup;

extern std::string g_stLocale;
extern int g_test_server;
extern int g_log;

//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!! IMPORTANT !!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// Check all SELECT syntax on item table before change this function!!!
//
bool CreateItemTableFromRes(MYSQL_RES * res, std::vector<TPlayerItem> * pVec, DWORD dwPID)
{
	if (!res)
	{
		pVec->clear();
		return true;
	}

	int rows;

	if ((rows = mysql_num_rows(res)) <= 0)	// ������ ����
	{
		pVec->clear();
		return true;
	}

	pVec->resize(rows);

	for (int i = 0; i < rows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		TPlayerItem & item = pVec->at(i);

		int cur = 0;

		// Check all SELECT syntax on item table before change this function!!!
		// Check all SELECT syntax on item table before change this function!!!
		// Check all SELECT syntax on item table before change this function!!!
		str_to_number(item.id, row[cur++]);
		str_to_number(item.window, row[cur++]);
		str_to_number(item.pos, row[cur++]);
		str_to_number(item.count, row[cur++]);
		str_to_number(item.vnum, row[cur++]);

#ifdef WJ_CHANGELOOK_SYSTEM
		str_to_number(item.transmutation, row[cur++]);
#endif
		str_to_number(item.alSockets[0], row[cur++]);
		str_to_number(item.alSockets[1], row[cur++]);
		str_to_number(item.alSockets[2], row[cur++]);
		str_to_number(item.alSockets[3], row[cur++]);


		for (int j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++)
		{
			str_to_number(item.aAttr[j].bType, row[cur++]);
			str_to_number(item.aAttr[j].sValue, row[cur++]);
		}

#ifdef ENABLE_SOULBIND_SYSTEM
		str_to_number(item.sealbind, row[cur++]);
#endif
		item.owner		= dwPID;
	}

	return true;
}

#ifdef __SKILL_COLOR_SYSTEM__
bool CreateSkillColorTableFromRes(MYSQL_RES * res, DWORD * dwSkillColor)
{
	if (mysql_num_rows(res) == 0)
		return false;

	MYSQL_ROW row = mysql_fetch_row(res);

	for (int x = 0; x < ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT; ++x)
	{
		for (int i = 0; i < ESkillColorLength::MAX_EFFECT_COUNT; ++i)
		{
			*(dwSkillColor++) = atoi(row[i + (x*ESkillColorLength::MAX_EFFECT_COUNT)]);
		}
	}

	return true;
}
#endif

size_t CreatePlayerSaveQuery(char * pszQuery, size_t querySize, TPlayerTable * pkTab)
{
	size_t queryLen;

	queryLen = snprintf(pszQuery, querySize,
			"UPDATE player%s SET "//4
			"job = %d, "//5
			"voice = %d, "//6
			"dir = %d, "//7
			"x = %d, "//8
			"y = %d, "//9
			"z = %d, "//10
			"map_index = %d, "//11
			"exit_x = %ld, "//12
			"exit_y = %ld, "//13
			"exit_map_index = %ld, "//14
			"hp = %ld, "//15
			"mp = %ld, "//16
			"stamina = %d, "//17
			"random_hp = %d, "//18
			"random_sp = %d, "//19
			"playtime = %d, "//20
			"level = %d, "//21
			"level_step = %d, "//22
			"st = %d, "//23
			"ht = %d, "//24
			"dx = %d, "//25
			"iq = %d, "//26
			"gold = %d, "//27
#ifdef ENABLE_CHEQUE_SYSTEM
			"cheque = %d, "//28
#endif
			"exp = %u, "//29
			"stat_point = %d, "//30
			"skill_point = %d, "//31
			"sub_skill_point = %d, "//32
			"stat_reset_count = %d, "//33
			"ip = '%s', "//34
			"part_main = %d, "//35
			"part_hair = %d, "//36
#ifdef __SASH_SYSTEM__
			"part_sash = %d, "//36
#endif
			"last_play = NOW(), "//37
			"skill_group = %d, "//38
			"alignment = %ld, "//39
#ifdef ENABLE_TITLE_SYSTEM
			"prestige = %ld, "//40
#endif
#ifdef ENABLE_REBORN_SYSTEM
			"reborn = %ld, "//41
#endif
			"horse_level = %d, "//42
			"horse_riding = %d, "//43
			"horse_hp = %d, "//44
			"horse_hp_droptime = %u, "//45
			"horse_stamina = %d, "//46
			"horse_skill_point = %d, "//47
#ifdef __GAYA__
			"gaya = %d, "//48
#endif
			"war_point = %ld, "//49
			,
		GetTablePostfix(),
		pkTab->job,
		pkTab->voice,
		pkTab->dir,
		pkTab->x,
		pkTab->y,
		pkTab->z,
		pkTab->lMapIndex,
		pkTab->lExitX,
		pkTab->lExitY,
		pkTab->lExitMapIndex,
		pkTab->hp,
		pkTab->sp,
		pkTab->stamina,
		pkTab->sRandomHP,
		pkTab->sRandomSP,
		pkTab->playtime,
		pkTab->level,
		pkTab->level_step,
		pkTab->st,
		pkTab->ht,
		pkTab->dx,
		pkTab->iq,
		pkTab->gold,
#ifdef ENABLE_CHEQUE_SYSTEM
		pkTab->cheque,
#endif
		pkTab->exp,
		pkTab->stat_point,
		pkTab->skill_point,
		pkTab->sub_skill_point,
		pkTab->stat_reset_count,
		pkTab->ip,
		pkTab->parts[PART_MAIN],
		pkTab->parts[PART_HAIR],
#ifdef __SASH_SYSTEM__
		pkTab->parts[PART_SASH],
#endif
		pkTab->skill_group,
		pkTab->lAlignment,
#ifdef ENABLE_TITLE_SYSTEM
		pkTab->lPrestige,
#endif
#ifdef ENABLE_REBORN_SYSTEM
		pkTab->lReborn,
#endif
		pkTab->horse.bLevel,
		pkTab->horse.bRiding,
		pkTab->horse.sHealth,
		pkTab->horse.dwHorseHealthDropTime,
		pkTab->horse.sStamina,
		pkTab->horse_skill_point,
#ifdef __GAYA__
		pkTab->gaya,
#endif
		pkTab->war_point
		);


	// Binary �� �ٲٱ� ���� �ӽ� ����
	static char text[8192 + 1];
	CDBManager::instance().EscapeString(text, pkTab->skills, sizeof(pkTab->skills));
	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, "skill_level = '%s', ", text);

	CDBManager::instance().EscapeString(text, pkTab->quickslot, sizeof(pkTab->quickslot));
	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, "quickslot = '%s', ", text);

#ifdef ENABLE_NEW_QUICK_SLOT_SYSTEM
	CDBManager::instance().EscapeString(text, pkTab->newquickslot, sizeof(pkTab->newquickslot));
	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, "newquickslot = '%s' ", text);
#endif

	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, " WHERE id=%d", pkTab->id);
	return queryLen;
}

CPlayerTableCache * CClientManager::GetPlayerCache(DWORD id)
{
	TPlayerTableCacheMap::iterator it = m_map_playerCache.find(id);

	if (it == m_map_playerCache.end())
		return NULL;

	TPlayerTable* pTable = it->second->Get(false);
	pTable->logoff_interval = GetCurrentTime() - it->second->GetLastUpdateTime();
	return it->second;
}

void CClientManager::PutPlayerCache(TPlayerTable * pNew)
{
	CPlayerTableCache * c;

	c = GetPlayerCache(pNew->id);

	if (!c)
	{
		c = new CPlayerTableCache;
		m_map_playerCache.insert(TPlayerTableCacheMap::value_type(pNew->id, c));
	}

	if (g_bHotBackup)
		PlayerHB::instance().Put(pNew->id);

	c->Put(pNew);
}

/*
 * PLAYER LOAD
 */
#ifdef __SKILL_COLOR_SYSTEM__
void CClientManager::QUERY_SKILL_COLOR_LOAD(CPeer* peer, DWORD dwHandle, TPlayerLoadPacket * packet)
{
	CSKillColorCache* c;
	TSkillColor* p;
	if ((c = GetSkillColorCache(packet->player_id)))
	{
		p = c->Get();
		peer->EncodeHeader(HEADER_DG_SKILL_COLOR_LOAD, dwHandle, sizeof(p->dwSkillColor));
		peer->Encode(p->dwSkillColor, sizeof(p->dwSkillColor));
	}
	else
	{
		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery), "SELECT "
			"s1_col1, s1_col2, s1_col3, s1_col4, s1_col5, "
			"s2_col1, s2_col2, s2_col3, s2_col4, s2_col5, "
			"s3_col1, s3_col2, s3_col3, s3_col4, s3_col5, "
			"s4_col1, s4_col2, s4_col3, s4_col4, s4_col5, "
			"s5_col1, s5_col2, s5_col3, s5_col4, s5_col5, "
			"s6_col1, s6_col2, s6_col3, s6_col4, s6_col5, "//end of skills (NewSkill)
			"s7_col1, s7_col2, s7_col3, s7_col4, s7_col5, "//begin buffs (NewSkill)
			"s8_col1, s8_col2, s8_col3, s8_col4, s8_col5, "
			"s9_col1, s9_col2, s9_col3, s9_col4, s9_col5, "
			"s10_col1, s10_col2, s10_col3, s10_col4, s10_col5 "
			"s11_col1, s11_col2, s11_col3, s11_col4, s11_col5 "
			"FROM skill_color%s WHERE player_id=%d",
			GetTablePostfix(), packet->player_id);
		CDBManager::instance().ReturnQuery(szQuery, QID_SKILL_COLOR, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));
	}
}
#endif

 
void CClientManager::QUERY_PLAYER_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket * packet)
{
	CPlayerTableCache * c;
	TPlayerTable * pTab;

	CLoginData * pLoginData = GetLoginDataByAID(packet->account_id);
	if (pLoginData)
	{
		for (int n = 0; n < PLAYER_PER_ACCOUNT; ++n)
			if (pLoginData->GetAccountRef().players[n].dwID != 0)
				DeleteLogoutPlayer(pLoginData->GetAccountRef().players[n].dwID);
	}

	if ((c = GetPlayerCache(packet->player_id)))
	{
		CLoginData * pkLD = GetLoginDataByAID(packet->account_id);
		if (!pkLD || pkLD->IsPlay())
		{
			sys_log(0, "PLAYER_LOAD_ERROR: LoginData %p IsPlay %d", pkLD, pkLD ? pkLD->IsPlay() : 0);
			peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, dwHandle, 0);
			return;
		}

		pTab = c->Get();
		pkLD->SetPlay(true);
		SendLoginToBilling(pkLD, true);
		thecore_memcpy(pTab->aiPremiumTimes, pkLD->GetPremiumPtr(), sizeof(pTab->aiPremiumTimes));

		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_SUCCESS, dwHandle, sizeof(TPlayerTable));
		peer->Encode(pTab, sizeof(TPlayerTable));

		if (packet->player_id != pkLD->GetLastPlayerID())
		{
			TPacketNeedLoginLogInfo logInfo;
			logInfo.dwPlayerID = packet->player_id;

			pkLD->SetLastPlayerID(packet->player_id);
			peer->EncodeHeader( HEADER_DG_NEED_LOGIN_LOG, dwHandle, sizeof(TPacketNeedLoginLogInfo) );
			peer->Encode( &logInfo, sizeof(TPacketNeedLoginLogInfo) );
		}

		char szQuery[1024] = { 0, };
		TItemCacheSet * pSet = GetItemCacheSet(pTab->id);
#ifdef ENABLE_CHEQUE_SYSTEM
		sys_log(0, "[PLAYER_LOAD] ID %s pid %d gold %d cheque %d ", pTab->name, pTab->id, pTab->gold, pTab->cheque);
#else
		sys_log(0, "[PLAYER_LOAD] ID %s pid %d gold %d ", pTab->name, pTab->id, pTab->gold);
#endif
		
		if (pSet)
		{
			static std::vector<TPlayerItem> s_items;
			s_items.resize(pSet->size());

			DWORD dwCount = 0;
			TItemCacheSet::iterator it = pSet->begin();
			while (it != pSet->end())
			{
				CItemCache * c = *it++;
				TPlayerItem * p = c->Get();

				if (p->vnum)
					thecore_memcpy(&s_items[dwCount++], p, sizeof(TPlayerItem));
			}

			if (g_test_server)
				sys_log(0, "ITEM_CACHE: HIT! %s count: %u", pTab->name, dwCount);

			peer->EncodeHeader(HEADER_DG_ITEM_LOAD, dwHandle, sizeof(DWORD) + sizeof(TPlayerItem) * dwCount);
			peer->EncodeDWORD(dwCount);

			if (dwCount)
				peer->Encode(&s_items[0], sizeof(TPlayerItem) * dwCount);

			snprintf(szQuery, sizeof(szQuery),
					"SELECT dwPID,szName,szState,lValue FROM quest%s WHERE dwPID=%d AND lValue<>0",
					GetTablePostfix(), pTab->id);
			
			CDBManager::instance().ReturnQuery(szQuery, QID_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle,0,packet->account_id));
			snprintf(szQuery, sizeof(szQuery), "SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost FROM affect%s WHERE dwPID=%d", GetTablePostfix(), pTab->id);
			CDBManager::instance().ReturnQuery(szQuery, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle));
		}
		else
		{
			snprintf(szQuery, sizeof(szQuery), 

					"SELECT id,window+0,pos,count,vnum,transmutation,socket0,socket1,socket2,socket3,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 "
#ifdef ENABLE_SOULBIND_SYSTEM
					",sealbind "
#endif
					"FROM item%s WHERE owner_id=%d AND (window < %d or window = %d)",


					GetTablePostfix(), pTab->id, SAFEBOX, DRAGON_SOUL_INVENTORY);

			CDBManager::instance().ReturnQuery(szQuery, QID_ITEM, peer->GetHandle(), new ClientHandleInfo(dwHandle, pTab->id));
			snprintf(szQuery, sizeof(szQuery), "SELECT dwPID, szName, szState, lValue FROM quest%s WHERE dwPID=%d", GetTablePostfix(), pTab->id);

			CDBManager::instance().ReturnQuery(szQuery, QID_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle, pTab->id));
			snprintf(szQuery, sizeof(szQuery), "SELECT dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost FROM affect%s WHERE dwPID=%d", GetTablePostfix(), pTab->id);
			
			CDBManager::instance().ReturnQuery(szQuery, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle, pTab->id));
		}
	}
	else
	{
		sys_log(0, "[PLAYER_LOAD] Load from PlayerDB pid[%d]", packet->player_id);
		char queryStr[QUERY_MAX_LEN + QUERY_MAX_LEN];
		snprintf(queryStr, sizeof(queryStr),
				"SELECT "
				"id,name,job,voice,dir,x,y,z,map_index,exit_x,exit_y,exit_map_index,hp,mp,stamina,random_hp,random_sp,playtime,"
#ifdef ENABLE_CHEQUE_SYSTEM
				"gold,cheque,level,level_step,st,ht,dx,iq,exp,"
#else
				"gold,level,level_step,st,ht,dx,iq,exp,"
#endif
				"stat_point,skill_point,sub_skill_point,stat_reset_count,part_base,part_hair,part_sash,"
				"skill_level,quickslot,newquickslot,skill_group,alignment,prestige,reborn,mobile,horse_level,horse_riding,horse_hp,horse_hp_droptime,horse_stamina,"
				"UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(last_play),horse_skill_point,gaya,war_point FROM player%s WHERE id=%d", 
				GetTablePostfix(), packet->player_id);

		ClientHandleInfo * pkInfo = new ClientHandleInfo(dwHandle, packet->player_id);
		pkInfo->account_id = packet->account_id;
		CDBManager::instance().ReturnQuery(queryStr, QID_PLAYER, peer->GetHandle(), pkInfo);

		snprintf(queryStr, sizeof(queryStr),
				"SELECT id,window+0,pos,count,vnum,transmutation,socket0,socket1,socket2,socket3,attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6,applytype0,applyvalue0,applytype1,applyvalue1,applytype2,applyvalue2,applytype3,applyvalue3,applytype4,applyvalue4,applytype5,applyvalue5,applytype6,applyvalue6,applytype7,applyvalue7 "	
#ifdef ENABLE_SOULBIND_SYSTEM
				",sealbind "
#endif
				"FROM item%s WHERE owner_id=%d AND (window < %d or window = %d)",
				GetTablePostfix(), packet->player_id, SAFEBOX, DRAGON_SOUL_INVENTORY);
		CDBManager::instance().ReturnQuery(queryStr, QID_ITEM, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));

		snprintf(queryStr, sizeof(queryStr),
				"SELECT dwPID,szName,szState,lValue FROM quest%s WHERE dwPID=%d",
				GetTablePostfix(), packet->player_id);
		CDBManager::instance().ReturnQuery(queryStr, QID_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id,packet->account_id));
		
		snprintf(queryStr, sizeof(queryStr),
				"SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost FROM affect%s WHERE dwPID=%d",
				GetTablePostfix(), packet->player_id);
		CDBManager::instance().ReturnQuery(queryStr, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));
	}
}

void CClientManager::ItemAward(CPeer * peer,char* login)
{
	char login_t[LOGIN_MAX_LEN + 1] = "";
	strlcpy(login_t,login,LOGIN_MAX_LEN + 1);
	std::set<TItemAward *> * pSet = ItemAwardManager::instance().GetByLogin(login_t);
	if(pSet == NULL)
		return;
	decltype(pSet->begin()) it = pSet->begin();	//taken_time�� NULL�ΰ͵� �о��	
	while(it != pSet->end() )
	{
		TItemAward * pItemAward = *(it++);
		char* whyStr = pItemAward->szWhy;	//why �ݷ� �б�
		char cmdStr[100] = "";	//why�ݷ뿡�� ���� ���� �ӽ� ���ڿ��� �����ص�
		strcpy(cmdStr,whyStr);	//���ɾ� ��� �������� ��ū���� ������ ��ūȭ �Ǳ� ����
		char command[20] = "";
		strcpy(command,GetCommand(cmdStr));	// command ���
		if( !(strcmp(command,"GIFT") ))	// command �� GIFT�̸�
		{
			TPacketItemAwardInfromer giftData;
			strcpy(giftData.login, pItemAward->szLogin);	//�α��� ���̵� ����
			strcpy(giftData.command, command);					//���ɾ� ����
			giftData.vnum = pItemAward->dwVnum;				//������ vnum�� ����
			ForwardPacket(HEADER_DG_ITEMAWARD_INFORMER,&giftData,sizeof(TPacketItemAwardInfromer));
		}
	}
}
char* CClientManager::GetCommand(char* str)
{
	char* command = new char[20];
	char* tok;

	if (str[0] == '[')
	{
		tok = strtok(str, "]");
		strcat(command, &tok[1]);
	}

	return command;
	free(command);
}

bool CreatePlayerTableFromRes(MYSQL_RES * res, TPlayerTable * pkTab)
{
	if (mysql_num_rows(res) == 0)	// ������ ����
		return false;

	memset(pkTab, 0, sizeof(TPlayerTable));

	MYSQL_ROW row = mysql_fetch_row(res);

	int	col = 0;

	// "id,name,job,voice,dir,x,y,z,map_index,exit_x,exit_y,exit_map_index,hp,mp,stamina,random_hp,random_sp,playtime,"
	// "gold,level,level_step,st,ht,dx,iq,exp,"
	// "stat_point,skill_point,sub_skill_point,stat_reset_count,part_base,part_hair,"
	// "skill_level,quickslot,skill_group,alignment,mobile,horse_level,horse_riding,horse_hp,horse_stamina FROM player%s WHERE id=%d",
	str_to_number(pkTab->id, row[col++]);
	strlcpy(pkTab->name, row[col++], sizeof(pkTab->name));
	str_to_number(pkTab->job, row[col++]);
	str_to_number(pkTab->voice, row[col++]);
	str_to_number(pkTab->dir, row[col++]);
	str_to_number(pkTab->x, row[col++]);
	str_to_number(pkTab->y, row[col++]);
	str_to_number(pkTab->z, row[col++]);
	str_to_number(pkTab->lMapIndex, row[col++]);
	str_to_number(pkTab->lExitX, row[col++]);
	str_to_number(pkTab->lExitY, row[col++]);
	str_to_number(pkTab->lExitMapIndex,  row[col++]);
	str_to_number(pkTab->hp, row[col++]);
	str_to_number(pkTab->sp, row[col++]);
	str_to_number(pkTab->stamina, row[col++]);
	str_to_number(pkTab->sRandomHP, row[col++]);
	str_to_number(pkTab->sRandomSP, row[col++]);
	str_to_number(pkTab->playtime, row[col++]);
	str_to_number(pkTab->gold, row[col++]);
#ifdef ENABLE_CHEQUE_SYSTEM
	str_to_number(pkTab->cheque, row[col++]);
#endif
	str_to_number(pkTab->level, row[col++]);
	str_to_number(pkTab->level_step, row[col++]);
	str_to_number(pkTab->st, row[col++]);
	str_to_number(pkTab->ht, row[col++]);
	str_to_number(pkTab->dx, row[col++]);
	str_to_number(pkTab->iq, row[col++]);
	str_to_number(pkTab->exp, row[col++]);
	str_to_number(pkTab->stat_point, row[col++]);
	str_to_number(pkTab->skill_point, row[col++]);
	str_to_number(pkTab->sub_skill_point, row[col++]);
	str_to_number(pkTab->stat_reset_count, row[col++]);
	str_to_number(pkTab->part_base, row[col++]);
	str_to_number(pkTab->parts[PART_HAIR], row[col++]);
#ifdef __SASH_SYSTEM__
	str_to_number(pkTab->parts[PART_SASH], row[col++]);
#endif

	if (row[col])
		thecore_memcpy(pkTab->skills, row[col], sizeof(pkTab->skills));
	else
		memset(&pkTab->skills, 0, sizeof(pkTab->skills));

	col++;

	if (row[col])
		thecore_memcpy(pkTab->quickslot, row[col], sizeof(pkTab->quickslot));
	else
		memset(pkTab->quickslot, 0, sizeof(pkTab->quickslot));
#ifdef ENABLE_NEW_QUICK_SLOT_SYSTEM
	col++;

	if (row[col])
		thecore_memcpy(pkTab->newquickslot, row[col], sizeof(pkTab->newquickslot));
	else
		memset(pkTab->newquickslot, 0, sizeof(pkTab->newquickslot));
#endif

	col++;

	str_to_number(pkTab->skill_group, row[col++]);
	str_to_number(pkTab->lAlignment, row[col++]);
#ifdef ENABLE_TITLE_SYSTEM
	str_to_number(pkTab->lPrestige, row[col++]);
#endif
#ifdef ENABLE_REBORN_SYSTEM
	str_to_number(pkTab->lReborn, row[col++]);
#endif

	if (row[col])
	{
		strlcpy(pkTab->szMobile, row[col], sizeof(pkTab->szMobile));
	}

	col++;

	str_to_number(pkTab->horse.bLevel, row[col++]);
	str_to_number(pkTab->horse.bRiding, row[col++]);
	str_to_number(pkTab->horse.sHealth, row[col++]);
	str_to_number(pkTab->horse.dwHorseHealthDropTime, row[col++]);
	str_to_number(pkTab->horse.sStamina, row[col++]);
	str_to_number(pkTab->logoff_interval, row[col++]);
	str_to_number(pkTab->horse_skill_point, row[col++]);
#ifdef __GAYA__
	str_to_number(pkTab->gaya, row[col++]);
#endif
	str_to_number(pkTab->war_point, row[col++]);

	// reset sub_skill_point
	{
		pkTab->skills[123].bLevel = 0; // SKILL_CREATE

		if (pkTab->level > 9)
		{
			int max_point = pkTab->level - 9;

			int skill_point =
				MIN(20, pkTab->skills[121].bLevel) +	// SKILL_LEADERSHIP			��ַ�
				MIN(20, pkTab->skills[124].bLevel) +	// SKILL_MINING				ä��
				MIN(10, pkTab->skills[131].bLevel) +	// SKILL_HORSE_SUMMON		����ȯ
				MIN(20, pkTab->skills[141].bLevel) +	// SKILL_ADD_HP				HP����
				MIN(20, pkTab->skills[142].bLevel);		// SKILL_RESIST_PENETRATE	��������

			pkTab->sub_skill_point = max_point - skill_point;
		}
		else
			pkTab->sub_skill_point = 0;
	}

	return true;
}

void CClientManager::RESULT_COMPOSITE_PLAYER(CPeer * peer, SQLMsg * pMsg, DWORD dwQID)
{
	CQueryInfo * qi = (CQueryInfo *) pMsg->pvUserData;
	std::unique_ptr<ClientHandleInfo> info((ClientHandleInfo *) qi->pvData);

	MYSQL_RES * pSQLResult = pMsg->Get()->pSQLResult;
	if (!pSQLResult)
	{
		sys_err("null MYSQL_RES QID %u", dwQID);
		return;
	}

	switch (dwQID)
	{
		case QID_PLAYER:
			sys_log(0, "QID_PLAYER %u %u", info->dwHandle, info->player_id);
			RESULT_PLAYER_LOAD(peer, pSQLResult, info.get());

			break;

		case QID_ITEM:
			sys_log(0, "QID_ITEM %u", info->dwHandle);
			RESULT_ITEM_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
			break;

#ifdef __SKILL_COLOR_SYSTEM__
		case QID_SKILL_COLOR:
			sys_log(0, "QID_SKILL_COLOR %u %u", info->dwHandle, info->player_id);
			RESULT_SKILL_COLOR_LOAD(peer, pSQLResult, info->dwHandle);
			break;
#endif

		case QID_QUEST:
			{
				sys_log(0, "QID_QUEST %u", info->dwHandle);
				RESULT_QUEST_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
				//aid���
				ClientHandleInfo*  temp1 = info.get();
				if (temp1 == NULL)
					break;

				CLoginData* pLoginData1 = GetLoginDataByAID(temp1->account_id);	//
				//���� ���� ���
				if( pLoginData1->GetAccountRef().login == NULL)
					break;
				if( pLoginData1 == NULL )
					break;
				sys_log(0,"info of pLoginData1 before call ItemAwardfunction %d",pLoginData1);
				ItemAward(peer,pLoginData1->GetAccountRef().login);
			}
			break;

		case QID_AFFECT:
			sys_log(0, "QID_AFFECT %u", info->dwHandle);
			RESULT_AFFECT_LOAD(peer, pSQLResult, info->dwHandle);
			break;
	}

}

void CClientManager::RESULT_PLAYER_LOAD(CPeer * peer, MYSQL_RES * pRes, ClientHandleInfo * pkInfo)
{
	TPlayerTable tab;

	if (!CreatePlayerTableFromRes(pRes, &tab))
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, pkInfo->dwHandle, 0);
		return;
	}

	CLoginData * pkLD = GetLoginDataByAID(pkInfo->account_id);

	if (!pkLD || pkLD->IsPlay())
	{
		sys_log(0, "PLAYER_LOAD_ERROR: LoginData %p IsPlay %d", pkLD, pkLD ? pkLD->IsPlay() : 0);
		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, pkInfo->dwHandle, 0);
		return;
	}

	pkLD->SetPlay(true);
	SendLoginToBilling(pkLD, true);
	thecore_memcpy(tab.aiPremiumTimes, pkLD->GetPremiumPtr(), sizeof(tab.aiPremiumTimes));

	peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_SUCCESS, pkInfo->dwHandle, sizeof(TPlayerTable));
	peer->Encode(&tab, sizeof(TPlayerTable));

	if (tab.id != pkLD->GetLastPlayerID())
	{
		TPacketNeedLoginLogInfo logInfo;
		logInfo.dwPlayerID = tab.id;

		pkLD->SetLastPlayerID( tab.id );

		peer->EncodeHeader( HEADER_DG_NEED_LOGIN_LOG, pkInfo->dwHandle, sizeof(TPacketNeedLoginLogInfo) );
		peer->Encode( &logInfo, sizeof(TPacketNeedLoginLogInfo) );
	}
}

void CClientManager::RESULT_ITEM_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID)
{
	static std::vector<TPlayerItem> s_items;
	//DB���� ������ ������ �о�´�.
	CreateItemTableFromRes(pRes, &s_items, dwPID);
	DWORD dwCount = s_items.size();

	peer->EncodeHeader(HEADER_DG_ITEM_LOAD, dwHandle, sizeof(DWORD) + sizeof(TPlayerItem) * dwCount);
	peer->EncodeDWORD(dwCount);

	//CacheSet�� �����
	CreateItemCacheSet(dwPID);

	// ITEM_LOAD_LOG_ATTACH_PID
	sys_log(0, "ITEM_LOAD: count %u pid %u", dwCount, dwPID);
	// END_OF_ITEM_LOAD_LOG_ATTACH_PID

	if (dwCount)
	{
		peer->Encode(&s_items[0], sizeof(TPlayerItem) * dwCount);

		for (DWORD i = 0; i < dwCount; ++i)
			PutItemCache(&s_items[i], true); // �ε��� ���� ���� ������ �ʿ� �����Ƿ�, ���� bSkipQuery�� true�� �ִ´�.
	}
}

#ifdef __SKILL_COLOR_SYSTEM__
void CClientManager::RESULT_SKILL_COLOR_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle)
{
	DWORD dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memset(dwSkillColor, 0, sizeof(dwSkillColor));

	CreateSkillColorTableFromRes(pRes, *dwSkillColor);
	sys_log(0, "SKILL_COLOR_LOAD %i, %i, %i, %i,", dwSkillColor[0][0], dwSkillColor[1][1], dwSkillColor[2][2], dwSkillColor[3][3]);
	peer->EncodeHeader(HEADER_DG_SKILL_COLOR_LOAD, dwHandle, sizeof(dwSkillColor));
	peer->Encode(&dwSkillColor, sizeof(dwSkillColor));
}
#endif

void CClientManager::RESULT_AFFECT_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle)
{
	int iNumRows;

	if ((iNumRows = mysql_num_rows(pRes)) == 0) // ������ ����
		return;

	static std::vector<TPacketAffectElement> s_elements;
	s_elements.resize(iNumRows);

	DWORD dwPID = 0;

	MYSQL_ROW row;

	for (int i = 0; i < iNumRows; ++i)
	{
		TPacketAffectElement & r = s_elements[i];
		row = mysql_fetch_row(pRes);

		if (dwPID == 0)
			str_to_number(dwPID, row[0]);

		str_to_number(r.dwType, row[1]);
		str_to_number(r.bApplyOn, row[2]);
		str_to_number(r.lApplyValue, row[3]);
		str_to_number(r.dwFlag, row[4]);
		str_to_number(r.lDuration, row[5]);
		str_to_number(r.lSPCost, row[6]);
	}

	sys_log(0, "AFFECT_LOAD: count %d PID %u", s_elements.size(), dwPID);

	DWORD dwCount = s_elements.size();

	peer->EncodeHeader(HEADER_DG_AFFECT_LOAD, dwHandle, sizeof(DWORD) + sizeof(DWORD) + sizeof(TPacketAffectElement) * dwCount);
	peer->Encode(&dwPID, sizeof(DWORD));
	peer->Encode(&dwCount, sizeof(DWORD));
	peer->Encode(&s_elements[0], sizeof(TPacketAffectElement) * dwCount);
}

void CClientManager::RESULT_QUEST_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD pid)
{
	int iNumRows;

	if ((iNumRows = mysql_num_rows(pRes)) == 0)
	{
		DWORD dwCount = 0;
		peer->EncodeHeader(HEADER_DG_QUEST_LOAD, dwHandle, sizeof(DWORD));
		peer->Encode(&dwCount, sizeof(DWORD));
		return;
	}

	static std::vector<TQuestTable> s_table;
	s_table.resize(iNumRows);

	MYSQL_ROW row;

	for (int i = 0; i < iNumRows; ++i)
	{
		TQuestTable & r = s_table[i];

		row = mysql_fetch_row(pRes);

		str_to_number(r.dwPID, row[0]);
		strlcpy(r.szName, row[1], sizeof(r.szName));
		strlcpy(r.szState, row[2], sizeof(r.szState));
		str_to_number(r.lValue, row[3]);
	}

	sys_log(0, "QUEST_LOAD: count %d PID %u", s_table.size(), s_table[0].dwPID);

	DWORD dwCount = s_table.size();

	peer->EncodeHeader(HEADER_DG_QUEST_LOAD, dwHandle, sizeof(DWORD) + sizeof(TQuestTable) * dwCount);
	peer->Encode(&dwCount, sizeof(DWORD));
	peer->Encode(&s_table[0], sizeof(TQuestTable) * dwCount);
}

/*
 * PLAYER SAVE
 */
void CClientManager::QUERY_PLAYER_SAVE(CPeer * peer, DWORD dwHandle, TPlayerTable * pkTab)
{
	if (g_test_server)
		sys_log(0, "PLAYER_SAVE: %s", pkTab->name);

	PutPlayerCache(pkTab);
}

typedef std::map<DWORD, time_t> time_by_id_map_t;
static time_by_id_map_t s_createTimeByAccountID;

/*
 * PLAYER CREATE
 */
void CClientManager::__QUERY_PLAYER_CREATE(CPeer *peer, DWORD dwHandle, TPlayerCreatePacket* packet)
{
	char	queryStr[QUERY_MAX_LEN];
	int		queryLen;
	int		player_id;

	time_by_id_map_t::iterator it = s_createTimeByAccountID.find(packet->account_id);

	if (it != s_createTimeByAccountID.end())
	{
		time_t curtime = time(0);

		if (curtime - it->second < 30)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}
	}

	queryLen = snprintf(queryStr, sizeof(queryStr), "SELECT pid%u FROM player_index%s WHERE id=%d", packet->account_index + 1, GetTablePostfix(), packet->account_id);
	std::unique_ptr<SQLMsg> pMsg0(CDBManager::instance().DirectQuery(queryStr));

	if (pMsg0->Get()->uiNumRows != 0)
	{
		if (!pMsg0->Get()->pSQLResult)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(pMsg0->Get()->pSQLResult);

		DWORD dwPID = 0; str_to_number(dwPID, row[0]);
		if (row[0] && dwPID > 0)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
			sys_log(0, "ALREADY EXIST AccountChrIdx %d ID %d", packet->account_index, dwPID);
			return;
		}
	}
	else
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	if (g_stLocale == "sjis")
		snprintf(queryStr, sizeof(queryStr), "SELECT COUNT(*) as count FROM player%s WHERE name='%s' collate sjis_japanese_ci", GetTablePostfix(), packet->player_table.name);
	else
		snprintf(queryStr, sizeof(queryStr), "SELECT COUNT(*) as count FROM player%s WHERE name='%s'", GetTablePostfix(), packet->player_table.name);
	
	std::unique_ptr<SQLMsg> pMsg1(CDBManager::instance().DirectQuery(queryStr));

	if (pMsg1->Get()->uiNumRows)
	{
		if (!pMsg1->Get()->pSQLResult)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(pMsg1->Get()->pSQLResult);

		if (*row[0] != '0')
		{
			sys_log(0, "ALREADY EXIST name %s, row[0] %s query %s", packet->player_table.name, row[0], queryStr);
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
			return;
		}
	}
	else
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	queryLen = snprintf(queryStr, sizeof(queryStr),
			/*"INSERT INTO player%s "
			"(id, account_id, name, level, st, ht, dx, iq, "
			"job, voice, dir, x, y, z, "
			"hp, mp, random_hp, random_sp, stat_point, stamina, part_base, part_main, part_hair, part_sash, gold, cheque, playtime, "
			"skill_level, quickslot, newquickslot) "
			"VALUES(0, %u, '%s', %d, %d, %d, %d, %d, "
			"%d, %d, %d, %d, %d, %d, %d, "
			//"%d, %d, %d, %d, %d, %d, %d, %d, 0, %d, %d, 0, ",
			"%d, %d, %d, %d, %d, %d, 0, %d, 0, %d, %d, 0, ",*/

			"INSERT INTO player%s "
			"(id, account_id, name, level, st, ht, dx, iq, "
			"job, voice, dir, x, y, z, "
			"hp, mp, random_hp, random_sp, stat_point, stamina, part_base, part_main, part_hair, part_sash, gold, cheque, playtime, "
			"skill_level, quickslot, newquickslot) "
			"VALUES(0, %u, '%s', %u, %d, %d, %d, %d, %d, "
			"%d, %d, %d, %d, %d, %ld, %ld, "
			"%d, %d, %d, %d, %d, %d, %d, %d, 0, %d, 0, ",
			GetTablePostfix(),
			packet->account_id, packet->player_table.name, packet->player_table.level, packet->player_table.st, packet->player_table.ht, packet->player_table.dx, packet->player_table.iq,
			packet->player_table.job, packet->player_table.voice, packet->player_table.dir, packet->player_table.x, packet->player_table.y, packet->player_table.z,
			packet->player_table.hp, packet->player_table.sp, packet->player_table.sRandomHP, packet->player_table.sRandomSP, packet->player_table.stat_point, packet->player_table.stamina, packet->player_table.part_base, packet->player_table.part_base, packet->player_table.part_base, packet->player_table.gold
#ifdef ENABLE_CHEQUE_SYSTEM
			,packet->player_table.cheque);
#endif
	
#ifdef ENABLE_CHEQUE_SYSTEM
	sys_log(0, "PlayerCreate accountid %d name %s level %d gold %d cheque %d, st %d ht %d job %d",
#else
	sys_log(0, "PlayerCreate accountid %d name %s level %d gold %d, st %d ht %d job %d",
#endif
			packet->account_id,
			packet->player_table.name,
			packet->player_table.level,
			packet->player_table.gold,
#ifdef ENABLE_CHEQUE_SYSTEM
			packet->player_table.cheque,
#endif
			packet->player_table.st,
			packet->player_table.ht,
			packet->player_table.job);

	static char text[4096 + 1];

	CDBManager::instance().EscapeString(text, packet->player_table.skills, sizeof(packet->player_table.skills));
	queryLen += snprintf(queryStr + queryLen, sizeof(queryStr) - queryLen, "'%s', ", text);
	if (g_test_server)
		sys_log(0, "Create_Player queryLen[%d] TEXT[%s]", queryLen, text);

	CDBManager::instance().EscapeString(text, packet->player_table.quickslot, sizeof(packet->player_table.quickslot));
	queryLen += snprintf(queryStr + queryLen, sizeof(queryStr) - queryLen, "'%s', ", text);

#ifdef ENABLE_NEW_QUICK_SLOT_SYSTEM
	CDBManager::instance().EscapeString(text, packet->player_table.newquickslot, sizeof(packet->player_table.newquickslot));
	queryLen += snprintf(queryStr + queryLen, sizeof(queryStr) - queryLen, "'%s')", text);
#endif

	std::unique_ptr<SQLMsg> pMsg2(CDBManager::instance().DirectQuery(queryStr));
	if (g_test_server)
		sys_log(0, "Create_Player queryLen[%d] TEXT[%s]", queryLen, text);

	if (pMsg2->Get()->uiAffectedRows <= 0)
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
		sys_log(0, "ALREADY EXIST3 query: %s AffectedRows %lu", queryStr, pMsg2->Get()->uiAffectedRows);
		return;
	}

	player_id = pMsg2->Get()->uiInsertID;

	snprintf(queryStr, sizeof(queryStr), "UPDATE player_index%s SET pid%d=%d WHERE id=%d", GetTablePostfix(), packet->account_index + 1, player_id, packet->account_id);
	std::unique_ptr<SQLMsg> pMsg3(CDBManager::instance().DirectQuery(queryStr));
	if (pMsg3->Get()->uiAffectedRows <= 0)
	{
		sys_err("QUERY_ERROR: %s", queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM player%s WHERE id=%d", GetTablePostfix(), player_id);
		CDBManager::instance().DirectQuery(queryStr);

		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	TPacketDGCreateSuccess pack;
	memset(&pack, 0, sizeof(pack));

	pack.bAccountCharacterIndex = packet->account_index;

	pack.player.dwID			= player_id;
	strlcpy(pack.player.szName, packet->player_table.name, sizeof(pack.player.szName));
	pack.player.byJob			= packet->player_table.job;
	pack.player.byLevel			= 1;
	pack.player.dwPlayMinutes	= 0;
	pack.player.byST			= packet->player_table.st;
	pack.player.byHT			= packet->player_table.ht;
	pack.player.byDX 			= packet->player_table.dx;
	pack.player.byIQ			= packet->player_table.iq;
	pack.player.wMainPart		= packet->player_table.part_base;
	pack.player.x			= packet->player_table.x;
	pack.player.y			= packet->player_table.y;

	peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_SUCCESS, dwHandle, sizeof(TPacketDGCreateSuccess));
	peer->Encode(&pack, sizeof(TPacketDGCreateSuccess));

	sys_log(0, "7 name %s job %d", pack.player.szName, pack.player.byJob);
	s_createTimeByAccountID[packet->account_id] = time(0);
}

/*
 * PLAYER DELETE
 */
void CClientManager::__QUERY_PLAYER_DELETE(CPeer* peer, DWORD dwHandle, TPlayerDeletePacket* packet)
{
	if (!packet->login[0] || !packet->player_id || packet->account_index >= PLAYER_PER_ACCOUNT)
		return;

	CLoginData * ld = GetLoginDataByLogin(packet->login);

	if (!ld)
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
		peer->EncodeBYTE(packet->account_index);
		return;
	}

	TAccountTable & r = ld->GetAccountRef();

	// block for japan
	if (g_stLocale != "sjis")
	{
		if (!IsChinaEventServer())
		{
			if (strlen(r.social_id) < 7 || strncmp(packet->private_code, r.social_id + strlen(r.social_id) - 7, 7))
			{
				sys_log(0, "PLAYER_DELETE FAILED len(%d)", strlen(r.social_id));
				peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
				peer->EncodeBYTE(packet->account_index);
				return;
			}

			CPlayerTableCache * pkPlayerCache = GetPlayerCache(packet->player_id);
			if (pkPlayerCache)
			{
				TPlayerTable * pTab = pkPlayerCache->Get();

				if (pTab->level >= m_iPlayerDeleteLevelLimit)
				{
					sys_log(0, "PLAYER_DELETE FAILED LEVEL %u >= DELETE LIMIT %d", pTab->level, m_iPlayerDeleteLevelLimit);
					peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
					peer->EncodeBYTE(packet->account_index);
					return;
				}

				if (pTab->level < m_iPlayerDeleteLevelLimitLower)
				{
					sys_log(0, "PLAYER_DELETE FAILED LEVEL %u < DELETE LIMIT %d", pTab->level, m_iPlayerDeleteLevelLimitLower);
					peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
					peer->EncodeBYTE(packet->account_index);
					return;
				}
			}
		}
	}

	char szQuery[128];
	snprintf(szQuery, sizeof(szQuery), "SELECT p.id, p.level, p.name FROM player_index%s AS i, player%s AS p WHERE pid%u=%u AND pid%u=p.id",
			GetTablePostfix(), GetTablePostfix(), packet->account_index + 1, packet->player_id, packet->account_index + 1);

	ClientHandleInfo * pi = new ClientHandleInfo(dwHandle, packet->player_id);
	pi->account_index = packet->account_index;

	sys_log(0, "PLAYER_DELETE TRY: %s %d pid%d", packet->login, packet->player_id, packet->account_index + 1);
	CDBManager::instance().ReturnQuery(szQuery, QID_PLAYER_DELETE, peer->GetHandle(), pi);
}

//
// @version	05/06/10 Bang2ni - �÷��̾� ������ �������� ����Ʈ ���� �߰�.
//
void CClientManager::__RESULT_PLAYER_DELETE(CPeer *peer, SQLMsg* msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * pi = (ClientHandleInfo *) qi->pvData;

	if (msg->Get() && msg->Get()->uiNumRows)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		DWORD dwPID = 0;
		str_to_number(dwPID, row[0]);

		int deletedLevelLimit = 0;
		str_to_number(deletedLevelLimit, row[1]);

		char szName[64];
		strlcpy(szName, row[2], sizeof(szName));

		if (deletedLevelLimit >= m_iPlayerDeleteLevelLimit && !IsChinaEventServer())
		{
			sys_log(0, "PLAYER_DELETE FAILED LEVEL %u >= DELETE LIMIT %d", deletedLevelLimit, m_iPlayerDeleteLevelLimit);
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		if (deletedLevelLimit < m_iPlayerDeleteLevelLimitLower)
		{
			sys_log(0, "PLAYER_DELETE FAILED LEVEL %u < DELETE LIMIT %d", deletedLevelLimit, m_iPlayerDeleteLevelLimitLower);
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		char queryStr[QUERY_MAX_LEN];

		snprintf(queryStr, sizeof(queryStr), "INSERT INTO player_deleted%s SELECT * FROM player%s WHERE id=%d",
				GetTablePostfix(), GetTablePostfix(), pi->player_id);
		std::unique_ptr<SQLMsg> pIns(CDBManager::instance().DirectQuery(queryStr));

		if (pIns->Get()->uiAffectedRows == 0 || pIns->Get()->uiAffectedRows == (uint32_t)-1)
		{
			sys_log(0, "PLAYER_DELETE FAILED %u CANNOT INSERT TO player_deleted%s", dwPID, GetTablePostfix());

			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		// ���� ����
		sys_log(0, "PLAYER_DELETE SUCCESS %u", dwPID);

		char account_index_string[16];

		snprintf(account_index_string, sizeof(account_index_string), "player_id%d", m_iPlayerIDStart + pi->account_index);

		// �÷��̾� ���̺��� ĳ������ �����Ѵ�.
		CPlayerTableCache * pkPlayerCache = GetPlayerCache(pi->player_id);

		if (pkPlayerCache)
		{
			m_map_playerCache.erase(pi->player_id);
			delete pkPlayerCache;
		}

		// �����۵��� ĳ������ �����Ѵ�.
		TItemCacheSet * pSet = GetItemCacheSet(pi->player_id);

		if (pSet)
		{
			TItemCacheSet::iterator it = pSet->begin();

			while (it != pSet->end())
			{
				CItemCache * pkItemCache = *it++;
				DeleteItemCache(pkItemCache->Get()->id);
			}

			pSet->clear();
			delete pSet;

			m_map_pkItemCacheSetPtr.erase(pi->player_id);
		}

		snprintf(queryStr, sizeof(queryStr), "UPDATE player_index%s SET pid%u=0 WHERE pid%u=%d",
				GetTablePostfix(),
				pi->account_index + 1,
				pi->account_index + 1,
				pi->player_id);

		std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(queryStr));

		if (pMsg->Get()->uiAffectedRows == 0 || pMsg->Get()->uiAffectedRows == (uint32_t)-1)
		{
			sys_log(0, "PLAYER_DELETE FAIL WHEN UPDATE account table");
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM player%s WHERE id=%d", GetTablePostfix(), pi->player_id);
		delete CDBManager::instance().DirectQuery(queryStr);

#ifdef ENABLE_SWITCHBOT
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM item%s WHERE owner_id=%d AND (window < %d or window = %d or window %d)", GetTablePostfix(), pi->player_id, SAFEBOX, DRAGON_SOUL_INVENTORY, SWITCHBOT);
#else
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM item%s WHERE owner_id=%d AND (window < %d or window = %d)", GetTablePostfix(), pi->player_id, SAFEBOX, DRAGON_SOUL_INVENTORY);
#endif
		delete CDBManager::instance().DirectQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM quest%s WHERE dwPID=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM affect%s WHERE dwPID=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM guild_member%s WHERE pid=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		// MYSHOP_PRICE_LIST
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM myshop_pricelist%s WHERE owner_id=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);
		// END_OF_MYSHOP_PRICE_LIST

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM messenger_list%s WHERE account='%s' OR companion='%s'", GetTablePostfix(), szName, szName);
		CDBManager::instance().AsyncQuery(queryStr);

		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_SUCCESS, pi->dwHandle, 1);
		peer->EncodeBYTE(pi->account_index);
	}
	else
	{
		// ���� ����
		sys_log(0, "PLAYER_DELETE FAIL NO ROW");
		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
		peer->EncodeBYTE(pi->account_index);
	}
}

void CClientManager::QUERY_ADD_AFFECT(CPeer * peer, TPacketGDAddAffect * p)
{
	char queryStr[QUERY_MAX_LEN];
	/*
	   snprintf(queryStr, sizeof(queryStr),
	   "INSERT INTO affect%s (dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost) "
	   "VALUES(%u, %u, %u, %d, %u, %d, %d) "
	   "ON DUPLICATE KEY UPDATE lApplyValue=%d, dwFlag=%u, lDuration=%d, lSPCost=%d",
	   GetTablePostfix(),
	   p->dwPID,
	   p->elem.dwType,
	   p->elem.bApplyOn,
	   p->elem.lApplyValue,
	   p->elem.dwFlag,
	   p->elem.lDuration,
	   p->elem.lSPCost,
	   p->elem.lApplyValue,
	   p->elem.dwFlag,
	   p->elem.lDuration,
	   p->elem.lSPCost);
	   */
	snprintf(queryStr, sizeof(queryStr),
			"REPLACE INTO affect%s (dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost) "
			"VALUES(%u, %u, %u, %ld, %u, %ld, %ld)",
			GetTablePostfix(),
			p->dwPID,
			p->elem.dwType,
			p->elem.bApplyOn,
			p->elem.lApplyValue,
			p->elem.dwFlag,
			p->elem.lDuration,
			p->elem.lSPCost);

	CDBManager::instance().AsyncQuery(queryStr);
}

void CClientManager::QUERY_REMOVE_AFFECT(CPeer * peer, TPacketGDRemoveAffect * p)
{
	char queryStr[QUERY_MAX_LEN];

	snprintf(queryStr, sizeof(queryStr),
			"DELETE FROM affect%s WHERE dwPID=%u AND bType=%u AND bApplyOn=%u",
			GetTablePostfix(), p->dwPID, p->dwType, p->bApplyOn);

	CDBManager::instance().AsyncQuery(queryStr);
}


void CClientManager::QUERY_HIGHSCORE_REGISTER(CPeer* peer, TPacketGDHighscore * data)
{
	char szQuery[128];
	snprintf(szQuery, sizeof(szQuery), "SELECT value FROM highscore%s WHERE board='%s' AND pid = %u", GetTablePostfix(), data->szBoard, data->dwPID);

	sys_log(0, "HEADER_GD_HIGHSCORE_REGISTER: PID %u", data->dwPID);

	ClientHandleInfo * pi = new ClientHandleInfo(0);
	strlcpy(pi->login, data->szBoard, sizeof(pi->login));
	pi->account_id = (DWORD)data->lValue;
	pi->player_id = data->dwPID;
	pi->account_index = (data->cDir > 0);

	CDBManager::instance().ReturnQuery(szQuery, QID_HIGHSCORE_REGISTER, peer->GetHandle(), pi);
}

void CClientManager::RESULT_HIGHSCORE_REGISTER(CPeer * pkPeer, SQLMsg * msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * pi = (ClientHandleInfo *) qi->pvData;
	//DWORD dwHandle = pi->dwHandle;

	char szBoard[21];
	strlcpy(szBoard, pi->login, sizeof(szBoard));
	int value = (int)pi->account_id;

	SQLResult * res = msg->Get();

	if (res->uiNumRows == 0)
	{
		// ���ο� ���̽��ھ ����
		char buf[256];
		snprintf(buf, sizeof(buf), "INSERT INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
		CDBManager::instance().AsyncQuery(buf);
	}
	else
	{
		if (!res->pSQLResult)
		{
			delete pi;
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(res->pSQLResult);
		if (row && row[0])
		{
			int current_value = 0; str_to_number(current_value, row[0]);
			if (pi->account_index && (current_value >= value || !pi->account_index) && (current_value <= value))
			{
				value = current_value;
			}
			else
			{
				char buf[256];
				snprintf(buf, sizeof(buf), "REPLACE INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
				CDBManager::instance().AsyncQuery(buf);
			}
		}
		else
		{
			char buf[256];
			snprintf(buf, sizeof(buf), "INSERT INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
			CDBManager::instance().AsyncQuery(buf);
		}
	}
	// TODO: �̰����� ���̽��ھ ������Ʈ �Ǿ����� üũ�Ͽ� ������ �ѷ����Ѵ�.
	delete pi;
}

void CClientManager::InsertLogoutPlayer(DWORD pid)
{
	TLogoutPlayerMap::iterator it = m_map_logout.find(pid);

	// �������� ������� �߰�
	if (it != m_map_logout.end())
	{
		// �����Ұ�� �ð��� ����
		if (g_log)
			sys_log(0, "LOGOUT: Update player time pid(%d)", pid);

		it->second->time = time(0);
		return;
	}

	TLogoutPlayer * pLogout = new TLogoutPlayer;
	pLogout->pid = pid;
	pLogout->time = time(0);
	m_map_logout.insert(std::make_pair(pid, pLogout));

	if (g_log)
		sys_log(0, "LOGOUT: Insert player pid(%d)", pid);
}

void CClientManager::DeleteLogoutPlayer(DWORD pid)
{
	TLogoutPlayerMap::iterator it = m_map_logout.find(pid);

	if (it != m_map_logout.end())
	{
		delete it->second;
		m_map_logout.erase(it);
	}
}

extern int g_iLogoutSeconds;

void CClientManager::UpdateLogoutPlayer()
{
	time_t now = time(0);

	TLogoutPlayerMap::iterator it = m_map_logout.begin();

	while (it != m_map_logout.end())
	{
		TLogoutPlayer* pLogout = it->second;

		if (now - g_iLogoutSeconds > pLogout->time)
		{
			FlushItemCacheSet(pLogout->pid);
			FlushPlayerCacheSet(pLogout->pid);

			delete pLogout;
			m_map_logout.erase(it++);
		}
		else
			++it;
	}
}

void CClientManager::FlushPlayerCacheSet(DWORD pid)
{
	TPlayerTableCacheMap::iterator it = m_map_playerCache.find(pid);

	if (it != m_map_playerCache.end())
	{
		CPlayerTableCache * c = it->second;
		m_map_playerCache.erase(it);

		c->Flush();
		delete c;
	}
}
