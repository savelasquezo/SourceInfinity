#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "char.h"
#include "char_manager.h"
#include "battle.h"
#include "item.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "vector.h"
#include "packet.h"
#include "pvp.h"
#include "profiler.h"
#include "guild.h"
#include "affect.h"
#include "unique_item.h"
#include "lua_incl.h"
#include "arena.h"
#include "castle.h"
#include "sectree.h"
#include "ani.h"
#include "locale_service.h"
#include "affect_flag.h"
#include "../../common/service.h"

int battle_hit(LPCHARACTER ch, LPCHARACTER victim, int & iRetDam);

bool battle_distance_valid_by_xy(long x, long y, long tx, long ty)
{
	long distance = DISTANCE_APPROX(x - tx, y - ty);

	if (distance > 170)
		return false;

	return true;
}

bool battle_distance_valid(LPCHARACTER ch, LPCHARACTER victim)
{
	return battle_distance_valid_by_xy(ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY());
}

bool timed_event_cancel(LPCHARACTER ch)
{
	if (ch->m_pkTimedEvent)
	{
		event_cancel(&ch->m_pkTimedEvent);
		return true;
	}

	/* RECALL_DELAY
	   차후 전투로 인해 귀환부 딜레이가 취소 되어야 할 경우 주석 해제
	   if (ch->m_pk_RecallEvent)
	   {
	   event_cancel(&ch->m_pkRecallEvent);
	   return true;
	   }
	   END_OF_RECALL_DELAY */

	return false;
}

bool battle_is_attackable(LPCHARACTER ch, LPCHARACTER victim)
{
	// 상대방이 죽었으면 중단한다.
	if (victim->IsDead())
		return false;

	// 안전지대면 중단
	{
		SECTREE	*sectree = NULL;

		sectree	= ch->GetSectree();
		if (sectree && sectree->IsAttr(ch->GetX(), ch->GetY(), ATTR_BANPK))
			return false;

		sectree = victim->GetSectree();
		if (sectree && sectree->IsAttr(victim->GetX(), victim->GetY(), ATTR_BANPK))
			return false;
	}

	// 내가 죽었으면 중단한다.
	if (ch->IsStun() || ch->IsDead())
		return false;

	if (ch->IsPC() && victim->IsPC())
	{
		CGuild* g1 = ch->GetGuild();
		CGuild* g2 = victim->GetGuild();

		if (g1 && g2)
		{
			if (g1->UnderWar(g2->GetID()))
				return true;
		}
	}

	if (IS_CASTLE_MAP(ch->GetMapIndex()) && false==castle_can_attack(ch, victim))
			return false;

	if (CArenaManager::instance().CanAttack(ch, victim) == true)
		return true;

#ifdef __DEFENSE_WAVE__
	if (victim->GetRaceNum() == 20434 && ch->IsMonster())
		return true;
#endif

	return CPVPManager::instance().CanAttack(ch, victim);
}

int battle_melee_attack(LPCHARACTER ch, LPCHARACTER victim)
{
	if (test_server&&ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	if (!victim || ch == victim)
		return BATTLE_NONE;

	if (test_server&&ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	if (!battle_is_attackable(ch, victim))
		return BATTLE_NONE;

	if (test_server&&ch->IsPC())
		sys_log(0, "battle_melee_attack : [%s] attack to [%s]", ch->GetName(), victim->GetName());

	// 거리 체크
	int distance = DISTANCE_APPROX(ch->GetX() - victim->GetX(), ch->GetY() - victim->GetY());

	if (!victim->IsBuilding())
	{
		int max = 300;

		if (false == ch->IsPC())
		{
			max = (int) (ch->GetMobAttackRange() * 1.15f);
		}
		else
		{
			if (false == victim->IsPC() && BATTLE_TYPE_MELEE == victim->GetMobBattleType())
				max = MAX(750, (int) (victim->GetMobAttackRange() * 1.15f));
		}

		if (ch->IsPC() && (victim->GetRaceNum() == 4053)) //Esfinge
			max += 500;

#ifdef __DEFENSE_WAVE__
		if (ch->IsPC() && (victim->GetRaceNum() == 3960 || victim->GetRaceNum() == 3961 || victim->GetRaceNum() == 3962 || victim->GetRaceNum() == 3963))
			max += 500;
#endif

		if (distance > max)
		{
			if (test_server)
				sys_log(0, "VICTIM_FAR: %s distance: %d max: %d", ch->GetName(), distance, max);

			return BATTLE_NONE;
		}
	}

	if (timed_event_cancel(ch))
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("전투가 시작 되어 취소 되었습니다."));

	if (timed_event_cancel(victim))
		victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("전투가 시작 되어 취소 되었습니다."));

	ch->SetPosition(POS_FIGHTING);
	ch->SetVictim(victim);

	const PIXEL_POSITION & vpos = victim->GetXYZ();
	ch->SetRotationToXY(vpos.x, vpos.y);

	int dam;
	int ret = battle_hit(ch, victim, dam);
	return (ret);
}

void battle_end_ex(LPCHARACTER ch)
{
	if (ch->IsPosition(POS_FIGHTING))
		ch->SetPosition(POS_STANDING);
}

void battle_end(LPCHARACTER ch)
{
	battle_end_ex(ch);
}

int CalcBattleDamage(int iDam, int iAttackerLev, int iVictimLev)
{
	if (iDam < 3)
		iDam = number(1, 5);

	return iDam;
}

int CalcMagicDamageWithValue(int iDam, LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
{
	return CalcBattleDamage(iDam, pkAttacker->GetLevel(), pkVictim->GetLevel());
}

int CalcMagicDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
{
	int iDam = 0;

	if (pkAttacker->IsNPC())
	{
		iDam = CalcMeleeDamage(pkAttacker, pkVictim, false, false);
	}

	iDam += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);

	return CalcMagicDamageWithValue(iDam, pkAttacker, pkVictim);
}

float CalcAttackRating(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, bool bIgnoreTargetRating)
{
	int iARSrc;
	int iERSrc;

	if (LC_IsYMIR()) // 천마
	{
		iARSrc = MIN(90, pkAttacker->GetPolymorphPoint(POINT_DX));
		iERSrc = MIN(90, pkVictim->GetPolymorphPoint(POINT_DX));
	}
	else 
	{
		int attacker_dx = pkAttacker->GetPolymorphPoint(POINT_DX);
		int attacker_lv = pkAttacker->GetLevel();

		int victim_dx = pkVictim->GetPolymorphPoint(POINT_DX);
		int victim_lv = pkAttacker->GetLevel();

		iARSrc = MIN(90, (attacker_dx * 4	+ attacker_lv * 2) / 6);
		iERSrc = MIN(90, (victim_dx	  * 4	+ victim_lv   * 2) / 6);
	}

	float fAR = ((float) iARSrc + 210.0f) / 300.0f; // fAR = 0.7 ~ 1.0

	if (bIgnoreTargetRating)
		return fAR;

	float fER = ((float) (iERSrc * 2 + 5) / (iERSrc + 95)) * 3.0f / 10.0f;

	return fAR - fER;
}

int CalcAttBonus(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, int iAtk)
{
	if (!pkVictim->IsPC())
		iAtk += pkAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_ATTACK_BONUS);

	if (!pkAttacker->IsPC())
	{
		int iReduceDamagePct = pkVictim->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_TRANSFER_DAMAGE);
		iAtk = iAtk * (100 + iReduceDamagePct) / 100;
	}

	if (pkAttacker->IsNPC() && pkVictim->IsPC())
	{
		iAtk = (iAtk * CHARACTER_MANAGER::instance().GetMobDamageRate(pkAttacker)) / 100;
	}

#ifdef __ENABLE_BONUS_STRONG_AGAINST_WEAPON__
	if (pkVictim->IsPC())
	{
		LPITEM pkWeapon = pkVictim->GetWear(WEAR_WEAPON);
		if (pkWeapon)
		{
			switch (pkWeapon->GetSubType())
			{
				case WEAPON_SWORD:
					iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_SWORD)) / 100;
					break;

				case WEAPON_TWO_HANDED:
					iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_TWOHANDED)) / 100;
					break;

				case WEAPON_DAGGER:
					iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DAGGER)) / 100;
					break;

				case WEAPON_BELL:
					iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_BELL)) / 100;
					break;

				case WEAPON_FAN:
					iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_FAN)) / 100;
					break;

				case WEAPON_BOW:
					iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_BOW)) / 100;
					break;
			}
		}
	}
#endif

	if (pkVictim->IsNPC())
	{
		if (pkVictim->IsRaceFlag(RACE_FLAG_ANIMAL))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ANIMAL)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_UNDEAD))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_UNDEAD)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_DEVIL))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DEVIL)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_HUMAN))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_HUMAN)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ORC))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ORC)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_MILGYO))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MILGYO)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_INSECT))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_INSECT)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_DESERT))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DESERT)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_TREE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_TREE)) / 100;
#ifdef __ENABLE_TARGET_ELEMENT_SYSTEM__
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_FIRE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_FIRE)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_ICE))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ICE)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_ELEC))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ELEC)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_WIND))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_WIND)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_EARTH))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_EARTH)) / 100;
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_DARK)) / 100;

#endif
		if (pkVictim->IsRaceFlag(RACE_FLAG_ATT_FIRE) || pkVictim->IsRaceFlag(RACE_FLAG_ATT_ICE) || pkVictim->IsRaceFlag(RACE_FLAG_ATT_ELEC) || pkVictim->IsRaceFlag(RACE_FLAG_ATT_WIND) || pkVictim->IsRaceFlag(RACE_FLAG_ATT_EARTH) || pkVictim->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_OMNIATTBONUS)) / 100;
		if (pkVictim->IsStone())
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_STONE)) / 100;

		iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_MONSTER)) / 100;


		if (pkVictim->GetMobRank() == MOB_RANK_BOSS)
			iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_BOSS)) / 100;

#ifdef __KILL_BOSS_WITH_SKILLS__
		if (pkAttacker->IsRiding() && pkVictim->GetMobRank() == MOB_RANK_BOSS)
			iAtk /= 100;
#endif
	}

	else if (pkVictim->IsPC())
	{

		iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_HUMAN)) / 100;

		switch (pkVictim->GetJob())
		{
			case JOB_WARRIOR:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_WARRIOR)) / 100;
				break;

			case JOB_ASSASSIN:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_ASSASSIN)) / 110;
				break;

			case JOB_SURA:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_SURA)) / 75;
				break;

			case JOB_SHAMAN:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_SHAMAN)) / 100;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				iAtk += (iAtk * pkAttacker->GetPoint(POINT_ATTBONUS_WOLFMAN)) / 100;
				break;
#endif
		}
	}

	if (pkAttacker->IsPC() == true)
	{
		switch (pkAttacker->GetJob())
		{
			iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_HUMAN)) / 175;

			case JOB_WARRIOR:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_WARRIOR)) / 175;
				break;

			case JOB_ASSASSIN:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_ASSASSIN)) / 165;
				break;

			case JOB_SURA:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_SURA)) / 195;
				break;

			case JOB_SHAMAN:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_SHAMAN)) / 175;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				iAtk -= (iAtk * pkVictim->GetPoint(POINT_RESIST_WOLFMAN)) / 175;
				break;
#endif
		}
	}

	return iAtk;
}

void Item_GetDamage(LPITEM pkItem, int* pdamMin, int* pdamMax)
{
	*pdamMin = 0;
	*pdamMax = 1;

	if (!pkItem)
		return;

	switch (pkItem->GetType())
	{
		case ITEM_ROD:
		case ITEM_PICK:
			return;
	}

	if (pkItem->GetType() != ITEM_WEAPON)
		sys_err("Item_GetDamage - !ITEM_WEAPON vnum=%d, type=%d", pkItem->GetOriginalVnum(), pkItem->GetType());

	*pdamMin = pkItem->GetValue(3);
	*pdamMax = pkItem->GetValue(4);

}

int CalcMeleeDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, bool bIgnoreDefense, bool bIgnoreTargetRating)
{
	LPITEM pWeapon = pkAttacker->GetWear(WEAR_WEAPON);
	bool bPolymorphed = pkAttacker->IsPolymorphed();

	if (pWeapon && !(bPolymorphed && !pkAttacker->IsPolyMaintainStat()))
	{
		if (pWeapon->GetType() != ITEM_WEAPON)
			return 0;

		switch (pWeapon->GetSubType())
		{
			case WEAPON_SWORD:
			case WEAPON_DAGGER:
			case WEAPON_TWO_HANDED:
			case WEAPON_BELL:
			case WEAPON_FAN:
			case WEAPON_MOUNT_SPEAR:
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
#endif
				break;

			case WEAPON_BOW:
				sys_err("CalcMeleeDamage should not handle bows (name: %s)", pkAttacker->GetName());
				return 0;

			default:
				return 0;
		}
	}

	int iDam = 0;
	float fAR = CalcAttackRating(pkAttacker, pkVictim, bIgnoreTargetRating);
	int iDamMin = 0, iDamMax = 0;

	// TESTSERVER_SHOW_ATTACKINFO
	int DEBUG_iDamCur = 0;
	int DEBUG_iDamBonus = 0;
	// END_OF_TESTSERVER_SHOW_ATTACKINFO

	if (bPolymorphed && !pkAttacker->IsPolyMaintainStat())
	{
		Item_GetDamage(pWeapon, &iDamMin, &iDamMax);
		DWORD dwMobVnum = pkAttacker->GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);

		if (pMob)
		{
			int iPower = pkAttacker->GetPolymorphPower();
			iDamMin += pMob->m_table.dwDamageRange[0] * iPower / 100;
			iDamMax += pMob->m_table.dwDamageRange[1] * iPower / 100;
		}
	}
	else if (pWeapon)
	{
		Item_GetDamage(pWeapon, &iDamMin, &iDamMax);
	}
	else if (pkAttacker->IsNPC())
	{
		iDamMin = pkAttacker->GetMobDamageMin();
		iDamMax = pkAttacker->GetMobDamageMax();
	}

	iDam = number(iDamMin, iDamMax) * 2;
	DEBUG_iDamCur = iDam;

	int iAtk = 0;
	iAtk = pkAttacker->GetPoint(POINT_ATT_GRADE) + iDam - (pkAttacker->GetLevel() * 2);
	iAtk = (int) (iAtk * fAR);
	iAtk += pkAttacker->GetLevel() * 2;

	if (pWeapon)
	{
		iAtk += pWeapon->GetValue(5) * 2;
		DEBUG_iDamBonus = pWeapon->GetValue(5) * 2;
	}

	iAtk += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);
	iAtk = (int) (iAtk * (100 + (pkAttacker->GetPoint(POINT_ATT_BONUS) + pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100);

	iAtk = CalcAttBonus(pkAttacker, pkVictim, iAtk);

	int iDef = 0;
	if (!bIgnoreDefense)
	{
		iDef = (pkVictim->GetPoint(POINT_DEF_GRADE) * (100 + pkVictim->GetPoint(POINT_DEF_BONUS)) / 100);
		if (!pkAttacker->IsPC())
			iDef += pkVictim->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_DEFENSE_BONUS);
	}

	if (pkAttacker->IsNPC())
	{
		iAtk = (int) (iAtk * pkAttacker->GetMobDamageMultiply());

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_FIRE))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_FIRE)) / 350;

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ICE))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_ICE)) / 350;

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ELEC))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_ELEC)) / 350;
	
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_WIND))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_WIND)) / 350;
	
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_EARTH))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_EARTH)) / 350;

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_DARK)) / 350;

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_FIRE) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ICE) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ELEC) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_WIND) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_EARTH) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_OMNIRESIST)) / 300;
	}

	iDam = MAX(0, iAtk - iDef);

	if (test_server)
	{
		int DEBUG_iLV = pkAttacker->GetLevel()*2;
		int DEBUG_iST = int((pkAttacker->GetPoint(POINT_ATT_GRADE) - DEBUG_iLV) * fAR);
		int DEBUG_iPT = pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);
		int DEBUG_iWP = 0;
		int DEBUG_iPureAtk = 0;
		int DEBUG_iPureDam = 0;
		char szRB[32] = "";
		char szGradeAtkBonus[32] = "";

		DEBUG_iWP = int(DEBUG_iDamCur * fAR);
		DEBUG_iPureAtk = DEBUG_iLV + DEBUG_iST + DEBUG_iWP+DEBUG_iDamBonus;
		DEBUG_iPureDam = iAtk - iDef;

		if (pkAttacker->IsNPC())
		{
			snprintf(szGradeAtkBonus, sizeof(szGradeAtkBonus), "=%d*%.1f", DEBUG_iPureAtk, pkAttacker->GetMobDamageMultiply());
			DEBUG_iPureAtk = int(DEBUG_iPureAtk * pkAttacker->GetMobDamageMultiply());
		}

		if (DEBUG_iDamBonus != 0)
			snprintf(szRB, sizeof(szRB), "+RB(%d)", DEBUG_iDamBonus);

		char szPT[32] = "";

		if (DEBUG_iPT != 0)
			snprintf(szPT, sizeof(szPT), ", PT=%d", DEBUG_iPT);

		char szUnknownAtk[32] = "";

		if (iAtk != DEBUG_iPureAtk)
			snprintf(szUnknownAtk, sizeof(szUnknownAtk), "+?(%d)", iAtk-DEBUG_iPureAtk);

		char szUnknownDam[32] = "";

		if (iDam != DEBUG_iPureDam)
			snprintf(szUnknownDam, sizeof(szUnknownDam), "+?(%d)", iDam-DEBUG_iPureDam);

		char szMeleeAttack[128];

		snprintf(szMeleeAttack, sizeof(szMeleeAttack),
				"%s(%d)-%s(%d)=%d%s, ATK=LV(%d)+ST(%d)+WP(%d)%s%s%s, AR=%.3g%s",
				pkAttacker->GetName(),
				iAtk,
				pkVictim->GetName(),
				iDef,
				iDam,
				szUnknownDam,
				DEBUG_iLV,
				DEBUG_iST,
				DEBUG_iWP,
				szRB,
				szUnknownAtk,
				szGradeAtkBonus,
				fAR,
				szPT);

		pkAttacker->ChatPacket(CHAT_TYPE_TALKING, "%s", szMeleeAttack);
		pkVictim->ChatPacket(CHAT_TYPE_TALKING, "%s", szMeleeAttack);
	}

	return CalcBattleDamage(iDam, pkAttacker->GetLevel(), pkVictim->GetLevel());
}

int CalcArrowDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, LPITEM pkBow, LPITEM pkArrow, bool bIgnoreDefense)
{
	if (!pkBow || pkBow->GetType() != ITEM_WEAPON || pkBow->GetSubType() != WEAPON_BOW)
		return 0;

	//if (!pkArrow)
	//	return 0;

	int iDist = (int)(DISTANCE_SQRT(pkAttacker->GetX() - pkVictim->GetX(), pkAttacker->GetY() - pkVictim->GetY()));
	int iGap = (iDist / 100) - 5 - pkAttacker->GetPoint(POINT_BOW_DISTANCE);
	int iPercent = 100 - (iGap * 5);


#ifdef __QUIVER_SYSTEM__
	if (pkArrow->GetSubType() == WEAPON_QUIVER)
		iPercent = 100;
#endif

	if (iPercent <= 0)
		return 0;
	else if (iPercent > 100)
		iPercent = 100;

	int iDam = 0;
	float fAR = CalcAttackRating(pkAttacker, pkVictim, false);
	//iDam = number(pkBow->GetValue(3), pkBow->GetValue(4)) * 2 + pkArrow->GetValue(3);
	iDam = number(pkBow->GetValue(3), pkBow->GetValue(4)) * 2;


	int iAtk;
	iAtk = pkAttacker->GetPoint(POINT_ATT_GRADE) + iDam - (pkAttacker->GetLevel() * 2);
	iAtk = (int) (iAtk * fAR);
	iAtk += pkAttacker->GetLevel() * 2;

	iAtk += pkBow->GetValue(5) * 2;
	iAtk += pkAttacker->GetPoint(POINT_PARTY_ATTACKER_BONUS);
	iAtk = (int) (iAtk * (100 + (pkAttacker->GetPoint(POINT_ATT_BONUS) + pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100);
	iAtk = CalcAttBonus(pkAttacker, pkVictim, iAtk);

	int iDef = 0;
	if (!bIgnoreDefense)
		iDef = (pkVictim->GetPoint(POINT_DEF_GRADE) * (100 + pkAttacker->GetPoint(POINT_DEF_BONUS)) / 100);

	if (pkAttacker->IsNPC())
	{
		iAtk = (int) (iAtk * pkAttacker->GetMobDamageMultiply());

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_FIRE))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_FIRE)) / 350;

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ICE))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_ICE)) / 350;

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ELEC))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_ELEC)) / 350;
	
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_WIND))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_WIND)) / 350;
	
		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_EARTH))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_EARTH)) / 350;

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_RESIST_DARK)) / 350;

		if (pkAttacker->IsRaceFlag(RACE_FLAG_ATT_FIRE) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ICE) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_ELEC) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_WIND) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_EARTH) || pkAttacker->IsRaceFlag(RACE_FLAG_ATT_DARK))
			iAtk -= (iAtk * pkAttacker->GetPoint(POINT_OMNIRESIST)) / 350;
	}

	iDam = MAX(0, iAtk - iDef);

	int iPureDam = iDam;
	iPureDam = (iPureDam * iPercent) / 100;
	if (test_server)
	{
		pkAttacker->ChatPacket(CHAT_TYPE_INFO, "ARROW %s -> %s, DAM %d DIST %d GAP %d %% %d iATK %d ATT %d MAGIC %d",
				pkAttacker->GetName(),
				pkVictim->GetName(),
				iPureDam,
				iDist, iGap, iPercent, iAtk, pkAttacker->GetPoint(POINT_ATT_BONUS), pkAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER));
	}

	return iPureDam;
}

void NormalAttackAffect(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
{
	// 독 공격은 특이하므로 특수 처리
	if (pkAttacker->GetPoint(POINT_POISON_PCT) && !pkVictim->IsAffectFlag(AFF_POISON) && !pkVictim->IsAffectFlag(AFF_BLEEDING))
	{
		if (number(1, 100) <= pkAttacker->GetPoint(POINT_POISON_PCT))
			pkVictim->AttackedByPoison(pkAttacker);
	}
#ifdef ENABLE_WOLFMAN_CHARACTER
	// BLEED
	if (pkAttacker->GetPoint(POINT_BLEEDING_PCT) && !pkVictim->IsAffectFlag(AFF_BLEEDING) && !pkVictim->IsAffectFlag(AFF_POISON))
	{
		if (number(1, 100) <= pkAttacker->GetPoint(POINT_BLEEDING_PCT))
			pkVictim->AttackedByBleeding(pkAttacker);
	}
#endif
	int iStunDuration = 2;
	if (pkAttacker->IsPC() && !pkVictim->IsPC())
		iStunDuration = 4;

	AttackAffect(pkAttacker, pkVictim, POINT_STUN_PCT, IMMUNE_STUN,  AFFECT_STUN, POINT_NONE,        0, AFF_STUN, iStunDuration, "STUN");
	AttackAffect(pkAttacker, pkVictim, POINT_SLOW_PCT, IMMUNE_SLOW,  AFFECT_SLOW, POINT_MOV_SPEED, -30, AFF_SLOW, 20,		"SLOW");
}

int battle_hit(LPCHARACTER pkAttacker, LPCHARACTER pkVictim, int & iRetDam)
{
	if (test_server)
		sys_log(0, "battle_hit : [%s] attack to [%s] : dam :%d type :%d", pkAttacker->GetName(), pkVictim->GetName(), iRetDam);

	int iDam = CalcMeleeDamage(pkAttacker, pkVictim);
	if (iDam <= 0)
		return (BATTLE_DAMAGE);

	NormalAttackAffect(pkAttacker, pkVictim);

	LPITEM pkWeapon = pkAttacker->GetWear(WEAR_WEAPON);
	if (pkWeapon)
		switch (pkWeapon->GetSubType())
		{
#ifdef __ENABLE_TARGET_ELEMENT_SYSTEM__

			case WEAPON_SWORD:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_SWORD) - pkAttacker->GetPoint(POINT_RESIST_SWORD_REDUCTION))) / 100;
				break;

			case WEAPON_TWO_HANDED:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_TWOHAND) - pkAttacker->GetPoint(POINT_RESIST_TWOHAND_REDUCTION))) / 100;
				break;

			case WEAPON_DAGGER:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_DAGGER) - pkAttacker->GetPoint(POINT_RESIST_DAGGER_REDUCTION))) / 100;
				break;

			case WEAPON_BELL:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_BELL) - pkAttacker->GetPoint(POINT_RESIST_BELL_REDUCTION))) / 100;
				break;

			case WEAPON_FAN:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_FAN) - pkAttacker->GetPoint(POINT_RESIST_FAN_REDUCTION))) / 100;
				break;

			case WEAPON_BOW:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_BOW) - pkAttacker->GetPoint(POINT_RESIST_BOW_REDUCTION))) / 100;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_CLAW) - pkAttacker->GetPoint(POINT_RESIST_CLAW_REDUCTION))) / 100;
#if defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_ITEM_CLAW_AS_DAGGER)
				iDam = iDam * (100 - (pkVictim->GetPoint(POINT_RESIST_DAGGER) - pkAttacker->GetPoint(POINT_RESIST_DAGGER_REDUCTION))) / 100;
#endif
				break;
#endif

#else
			case WEAPON_SWORD:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_SWORD)) / 100;
				break;

			case WEAPON_TWO_HANDED:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_TWOHAND)) / 100;
				break;

			case WEAPON_DAGGER:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_DAGGER)) / 100;
				break;

			case WEAPON_BELL:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_BELL)) / 100;
				break;

			case WEAPON_FAN:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_FAN)) / 100;
				break;

			case WEAPON_BOW:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_BOW)) / 100;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_CLAW)) / 100;
#if defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_ITEM_CLAW_AS_DAGGER)
				iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_DAGGER)) / 100;
#endif
				break;
#endif

#endif
		}

	float attMul = pkAttacker->GetAttMul();
	float tempIDam = iDam;
	iDam = attMul * tempIDam + 0.5f;
	iRetDam = iDam;

	//PROF_UNIT puDam("Dam");
	if (pkVictim->Damage(pkAttacker, iDam, DAMAGE_TYPE_NORMAL))
		return (BATTLE_DEAD);

	return (BATTLE_DAMAGE);
}

DWORD GET_ATTACK_SPEED(LPCHARACTER ch)
{
	if (NULL == ch)
		return 1000;

	LPITEM item = ch->GetWear(WEAR_WEAPON);
	DWORD default_bonus = SPEEDHACK_LIMIT_BONUS;
	DWORD riding_bonus = 0;
	if (ch->IsRiding())
	{
		riding_bonus = 50;
	}

	DWORD ani_speed = ani_attack_speed(ch);
	DWORD real_speed = (ani_speed * 100) / (default_bonus + ch->GetPoint(POINT_ATT_SPEED) + riding_bonus);
	if (item && item->GetSubType() == WEAPON_DAGGER)
		real_speed /= 2;
#ifdef ENABLE_WOLFMAN_CHARACTER
	else if (item && item->GetSubType() == WEAPON_CLAW)
		real_speed /= 2;
#endif

	return real_speed;
}

void SET_ATTACK_TIME(LPCHARACTER ch, LPCHARACTER victim, DWORD current_time)
{
	if (NULL == ch || NULL == victim)
		return;

	if (!ch->IsPC())
		return;

	ch->m_kAttackLog.dwVID = victim->GetVID();
	ch->m_kAttackLog.dwTime = current_time;
}

void SET_ATTACKED_TIME(LPCHARACTER ch, LPCHARACTER victim, DWORD current_time)
{
	if (NULL == ch || NULL == victim)
		return;

	if (!ch->IsPC())
		return;

	victim->m_AttackedLog.dwPID			= ch->GetPlayerID();
	victim->m_AttackedLog.dwAttackedTime= current_time;
}

bool IS_SPEED_HACK(LPCHARACTER ch, LPCHARACTER victim, DWORD current_time)
{
	if (ch->m_kAttackLog.dwVID == victim->GetVID())
	{
		if (current_time - ch->m_kAttackLog.dwTime < GET_ATTACK_SPEED(ch))
		{
			INCREASE_SPEED_HACK_COUNT(ch);

			if (test_server)
			{
				sys_log(0, "%s attack hack! time (delta, limit)=(%u, %u) hack_count %d",
						ch->GetName(),
						current_time - ch->m_kAttackLog.dwTime,
						GET_ATTACK_SPEED(ch),
						ch->m_speed_hack_count);

				ch->ChatPacket(CHAT_TYPE_INFO, "%s attack hack! time (delta, limit)=(%u, %u) hack_count %d",
						ch->GetName(),
						current_time - ch->m_kAttackLog.dwTime,
						GET_ATTACK_SPEED(ch),
						ch->m_speed_hack_count);
			}

			SET_ATTACK_TIME(ch, victim, current_time);
			SET_ATTACKED_TIME(ch, victim, current_time);
			return true;
		}
	}

	SET_ATTACK_TIME(ch, victim, current_time);

	if (victim->m_AttackedLog.dwPID == ch->GetPlayerID())
	{
		if (current_time - victim->m_AttackedLog.dwAttackedTime < GET_ATTACK_SPEED(ch))
		{
			INCREASE_SPEED_HACK_COUNT(ch);

			if (test_server)
			{
				sys_log(0, "%s Attack Speed HACK! time (delta, limit)=(%u, %u), hack_count = %d",
						ch->GetName(),
						current_time - victim->m_AttackedLog.dwAttackedTime,
						GET_ATTACK_SPEED(ch),
						ch->m_speed_hack_count);

				ch->ChatPacket(CHAT_TYPE_INFO, "Attack Speed Hack(%s), (delta, limit)=(%u, %u)",
						ch->GetName(),
						current_time - victim->m_AttackedLog.dwAttackedTime,
						GET_ATTACK_SPEED(ch));
			}

			SET_ATTACKED_TIME(ch, victim, current_time);
			return true;
		}
	}

	SET_ATTACKED_TIME(ch, victim, current_time);
	return false;
}
