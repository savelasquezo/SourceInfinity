#include "stdafx.h"
#include "utils.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "wedding.h"
#include "questmanager.h"
#ifdef ENABLE_MESSENGER_BLOCK
#include "messenger_manager.h"
#endif

#define NEED_TARGET	(1 << 0)
#define NEED_PC		(1 << 1)
#define WOMAN_ONLY	(1 << 2)
#define OTHER_SEX_ONLY	(1 << 3)
#define SELF_DISARM	(1 << 4)
#define TARGET_DISARM	(1 << 5)
#define BOTH_DISARM	(SELF_DISARM | TARGET_DISARM)

struct emotion_type_s
{
	const char *	command;
	const char *	command_to_client;
	long	flag;
	float	extra_delay;
} emotion_types[] = {
	{ "Ű??",	"french_kiss",	NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,		2.0f },
	{ "?ǻ?",	"kiss",		NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,		1.5f },
	{ "????",	"slap",		NEED_PC | SELF_DISARM,				1.5f },
	{ "?ڼ?",	"clap",		0,						1.0f },
	{ "??",		"cheer1",	0,						1.0f },
	{ "????",	"cheer2",	0,						1.0f },

	{ "????2",	"otur",		0,						1.0f },
	{ "????2",	"oturs",	0,						1.0f },

	// DANCE
	{ "????1",	"dance1",	0,						1.0f },
	{ "????2",	"dance2",	0,						1.0f },
	{ "????3",	"dance3",	0,						1.0f },
	{ "????4",	"dance4",	0,						1.0f },
	{ "????5",	"dance5",	0,						1.0f },
	{ "????6",	"dance6",	0,						1.0f },
	// END_OF_DANCE
#ifdef ENABLE_NEW_GIFT_ANIMATIONS
	// DANCE
	{ "????101",	"mixamo_101",	0,						1.0f },
	{ "????102",	"mixamo_102",	0,						1.0f },
	{ "????103",	"mixamo_103",	0,						1.0f },
	{ "????104",	"mixamo_104",	0,						1.0f },
	{ "????105",	"mixamo_105",	0,						1.0f },
	{ "????106",	"mixamo_106",	0,						1.0f },
	{ "????107",	"mixamo_107",	0,						1.0f },
	{ "????108",	"mixamo_108",	0,						1.0f },
	{ "????109",	"mixamo_109",	0,						1.0f },
	{ "????110",	"mixamo_110",	0,						1.0f },
	{ "????111",	"mixamo_111",	0,						1.0f },
	{ "????112",	"mixamo_112",	0,						1.0f },
	{ "????113",	"mixamo_113",	0,						1.0f },
	{ "????114",	"mixamo_114",	0,						1.0f },
	{ "????115",	"mixamo_115",	0,						1.0f },
	{ "????116",	"mixamo_116",	0,						1.0f },
	{ "????117",	"mixamo_117",	0,						1.0f },
	{ "????118",	"mixamo_118",	0,						1.0f },
	{ "????119",	"mixamo_119",	0,						1.0f },
	{ "????120",	"mixamo_120",	0,						1.0f },
	{ "????121",	"mixamo_121",	0,						1.0f },
	{ "????122",	"mixamo_122",	0,						1.0f },
	{ "????123",	"mixamo_123",	0,						1.0f },
	{ "????124",	"mixamo_124",	0,						1.0f },
	// END_OF_DANCE
	// GIFT
	{ " 齺101",	"emote101",	0,						1.0f },
	{ " 齺102",	"emote102",	0,						1.0f },
	{ " 齺103",	"emote103",	0,						1.0f },
	{ " 齺104",	"emote104",	0,						1.0f },
	{ " 齺105",	"emote105",	0,						1.0f },
	{ " 齺106",	"emote106",	0,						1.0f },
	{ " 齺107",	"emote107",	0,						1.0f },
	{ " 齺108",	"emote108",	0,						1.0f },
	{ " 齺109",	"emote109",	0,						1.0f },
	{ " 齺110",	"emote110",	0,						1.0f },
	{ " 齺111",	"emote111",	0,						1.0f },
	{ " 齺112",	"emote112",	0,						1.0f },
	{ " 齺113",	"emote113",	0,						1.0f },
	{ " 齺114",	"emote114",	0,						1.0f },
	{ " 齺115",	"emote115",	0,						1.0f },
	{ " 齺116",	"emote116",	0,						1.0f },
	{ " 齺117",	"emote117",	0,						1.0f },
	{ " 齺118",	"emote118",	0,						1.0f },
	{ " 齺119",	"emote119",	0,						1.0f },
	{ " 齺120",	"emote120",	0,						1.0f },
	{ " 齺121",	"emote121",	0,						1.0f },
	{ " 齺122",	"emote122",	0,						1.0f },
	{ " 齺123",	"emote123",	0,						1.0f },
	{ " 齺124",	"emote124",	0,						1.0f },
	// END_OF_GIFT
#endif
	{ "????",	"congratulation",	0,				1.0f	},
	{ "?뼭",	"forgive",			0,				1.0f	},
	{ "ȭ??",	"angry",			0,				1.0f	},
	{ "??Ȥ",	"attractive",		0,				1.0f	},
	{ "????",	"sad",				0,				1.0f	},
	{ "????",	"shy",				0,				1.0f	},
	{ "????",	"cheerup",			0,				1.0f	},
	{ "????",	"banter",			0,				1.0f	},
	{ "????",	"joy",				0,				1.0f	},
	{ "??",	"selfie",			0,				1.0f	},
	{ "??",	"dance7",			0,				1.0f	},
	{ "??",	"doze",				0,				1.0f	},
	{ "??",	"exercise",			0,				1.0f	},
	{ "??",	"pushup",			0,				1.0f	},
	{ "\n",	"\n",		0,						0.0f },
	/*
	//{ "Ű??",		NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,		MOTION_ACTION_FRENCH_KISS,	 1.0f },
	{ "?ǻ?",		NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,		MOTION_ACTION_KISS,		 1.0f },
	{ "???ȱ?",		NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,		MOTION_ACTION_SHORT_HUG,	 1.0f },
	{ "????",		NEED_PC | OTHER_SEX_ONLY | BOTH_DISARM,		MOTION_ACTION_LONG_HUG,		 1.0f },
	{ "????????",	NEED_PC | SELF_DISARM,				MOTION_ACTION_PUT_ARMS_SHOULDER, 0.0f },
	{ "??¯",		NEED_PC	| WOMAN_ONLY | SELF_DISARM,		MOTION_ACTION_FOLD_ARM,		 0.0f },
	{ "????",		NEED_PC | SELF_DISARM,				MOTION_ACTION_SLAP,		 1.5f },

	{ "???Ķ?",		0,						MOTION_ACTION_CHEER_01,		 0.0f },
	{ "????",		0,						MOTION_ACTION_CHEER_02,		 0.0f },
	{ "?ڼ?",		0,						MOTION_ACTION_CHEER_03,		 0.0f },

	{ "ȣȣ",		0,						MOTION_ACTION_LAUGH_01,		 0.0f },
	{ "űű",		0,						MOTION_ACTION_LAUGH_02,		 0.0f },
	{ "??????",		0,						MOTION_ACTION_LAUGH_03,		 0.0f },

	{ "????",		0,						MOTION_ACTION_CRY_01,		 0.0f },
	{ "????",		0,						MOTION_ACTION_CRY_02,		 0.0f },

	{ "?λ?",		0,						MOTION_ACTION_GREETING_01,	0.0f },
	{ "????",		0,						MOTION_ACTION_GREETING_02,	0.0f },
	{ "?????λ?",	0,						MOTION_ACTION_GREETING_03,	0.0f },

	{ "????",		0,						MOTION_ACTION_INSULT_01,	0.0f },
	{ "????",		SELF_DISARM,					MOTION_ACTION_INSULT_02,	0.0f },
	{ "????",		0,						MOTION_ACTION_INSULT_03,	0.0f },

	{ "??????",		0,						MOTION_ACTION_ETC_01,		0.0f },
	{ "????????",	0,						MOTION_ACTION_ETC_02,		0.0f },
	{ "????????",	0,						MOTION_ACTION_ETC_03,		0.0f },
	{ "????????",	0,						MOTION_ACTION_ETC_04,		0.0f },
	{ "ơ",		0,						MOTION_ACTION_ETC_05,		0.0f },
	{ "??",		0,						MOTION_ACTION_ETC_06,		0.0f },
	 */
};


std::set<std::pair<DWORD, DWORD> > s_emotion_set;

ACMD(do_emotion_allow)
{
	if ( ch->GetArena() )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?????忡?? ?????Ͻ? ?? ?????ϴ?."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD	val = 0; str_to_number(val, arg1);
	#ifdef ENABLE_MESSENGER_BLOCK
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(val);
	if (!tch)
		return;
	
	if (MessengerManager::instance().IsBlocked_Target(ch->GetName(), tch->GetName()))
	{
		//ben bloklad?m hac?
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk"), tch->GetName());
		return;
	}
	if (MessengerManager::instance().IsBlocked_Me(ch->GetName(), tch->GetName()))
	{
		//o bloklad? hac?
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s blokkk_me"), tch->GetName());
		return;
	}
	#endif
	s_emotion_set.insert(std::make_pair(ch->GetVID(), val));
}

bool CHARACTER_CanEmotion(CHARACTER& rch)
{
	return true;
}

ACMD(do_emotion)
{
	int i;
	{
		if (ch->IsRiding())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???? ź ???¿??? ????ǥ???? ?? ?? ?????ϴ?."));
			return;
		}
	}

	quest::PC * onti = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
	DWORD tekrar = get_global_time();
	if (onti)
	{
		DWORD sontime = onti->GetFlag("ademkodlar.sonduygu");
		if (sontime + 5 > tekrar)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("DUYGULAR_ARASINDA_5_SANIYE_BEKLEMELISIN"));
			return;
		}
	}

	for (i = 0; *emotion_types[i].command != '\n'; ++i)
	{
		if (!strcmp(cmd_info[cmd].command, emotion_types[i].command))
			break;

		if (!strcmp(cmd_info[cmd].command, emotion_types[i].command_to_client))
			break;
	}

	if (*emotion_types[i].command == '\n')
	{
		sys_err("cannot find emotion");
		return;
	}

	if (!CHARACTER_CanEmotion(*ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?????? ?????? ?????ÿ??? ?? ?? ?ֽ??ϴ?."));
		return;
	}

	if (IS_SET(emotion_types[i].flag, WOMAN_ONLY) && SEX_MALE==GET_SEX(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???ڸ? ?? ?? ?ֽ??ϴ?."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	LPCHARACTER victim = NULL;

	if (*arg1)
		victim = ch->FindCharacterInView(arg1, IS_SET(emotion_types[i].flag, NEED_PC));

	if (IS_SET(emotion_types[i].flag, NEED_TARGET | NEED_PC))
	{
		if (!victim)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?׷? ?????? ?????ϴ?."));
			return;
		}
	}

	if (victim)
	{
		if (!victim->IsPC() || victim == ch)
			return;

		if (victim->IsRiding())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???? ź ?????? ????ǥ???? ?? ?? ?????ϴ?."));
			return;
		}

		long distance = DISTANCE_APPROX(ch->GetX() - victim->GetX(), ch->GetY() - victim->GetY());

		if (distance < 10)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?ʹ? ?????? ?ֽ??ϴ?."));
			return;
		}

		if (distance > 500)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?ʹ? ?ָ? ?ֽ??ϴ?"));
			return;
		}

		if (IS_SET(emotion_types[i].flag, OTHER_SEX_ONLY))
		{
			if (GET_SEX(ch)==GET_SEX(victim))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?̼??????? ?? ?? ?ֽ??ϴ?."));
				return;
			}
		}

		if (IS_SET(emotion_types[i].flag, NEED_PC))
		{
			if (s_emotion_set.find(std::make_pair(victim->GetVID(), ch->GetVID())) == s_emotion_set.end())
			{
				if (true == marriage::CManager::instance().IsMarried( ch->GetPlayerID() ))
				{
					const marriage::TMarriage* marriageInfo = marriage::CManager::instance().Get( ch->GetPlayerID() );

					const DWORD other = marriageInfo->GetOther( ch->GetPlayerID() );

					if (0 == other || other != victim->GetPlayerID())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?? ?ൿ?? ??ȣ???? ?Ͽ? ???? ?մϴ?."));
						return;
					}
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?? ?ൿ?? ??ȣ???? ?Ͽ? ???? ?մϴ?."));
					return;
				}
			}

			s_emotion_set.insert(std::make_pair(ch->GetVID(), victim->GetVID()));
		}
	}

	char chatbuf[256+1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s %u %u",
			emotion_types[i].command_to_client,
			(DWORD) ch->GetVID(), victim ? (DWORD) victim->GetVID() : 0);

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	++len;  // \0 ???? ????

	TPacketGCChat pack_chat;
	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = CHAT_TYPE_COMMAND;
	pack_chat.id = 0;
	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	ch->PacketAround(buf.read_peek(), buf.size());

	/*if (victim)
		sys_log(1, "ACTION: %s TO %s", emotion_types[i].command, victim->GetName());
	else
		sys_log(1, "ACTION: %s", emotion_types[i].command);*/

	onti->SetFlag("ademkodlar.sonduygu", tekrar);
}

