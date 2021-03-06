#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "locale_service.h"
#include "log.h"
#include "desc.h"

ACMD(do_guild_notice);
ACMD(do_loncatoplantis);
ACMD(do_loncatoplantisi);
ACMD(do_loncatoplantieve);
ACMD(do_loncatoplantievet);
ACMD(do_loncaistatisti);
ACMD(do_loncaistatistik);
ACMD(do_create_item);
ACMD(do_user_horse_ride);
ACMD(do_user_horse_back);
ACMD(do_user_horse_feed);
#ifdef ENABLE_RONARK_SYSTEM
ACMD(do_ronark_system);
#endif
#ifdef ENABLE_DS_CHANGE_ATTR
ACMD(do_ds_change_attr);
#endif
ACMD(do_pcbang_update);
ACMD(do_pcbang_check);
#ifdef __DUNGEON_INFO_SYSTEM__
ACMD(do_raking_finish);
//ACMD(do_raking_finish_ataque);
#endif
// ADD_COMMAND_SLOW_STUN
ACMD(do_npcac);
ACMD(do_ruhoku);
ACMD(do_bkoku);
ACMD(do_mysql_query_database_convert_sql);
ACMD(do_mysql_query_database_convert_sqll);
ACMD(do_slow);
ACMD(do_stun);
// END_OF_ADD_COMMAND_SLOW_STUN

#ifdef __BATTLE_PASS__
ACMD(final_reward_battlepass);
ACMD(open_battlepass);
#endif

ACMD(do_warp);
ACMD(do_goto);
ACMD(do_item);
ACMD(do_pets);
ACMD(do_mob);
ACMD(do_mob_ld);
ACMD(do_mob_aggresive);
ACMD(do_mob_coward);
ACMD(do_mob_map);
ACMD(do_purge);
#ifdef ENABLE_POLY_SHOP
ACMD(do_buy_marble);
#endif
#if defined(ENABLE_AFFECT_POLYMORPH_REMOVE)
ACMD(do_remove_polymorph);
#endif
ACMD(do_weaken);
ACMD(do_item_purge);
ACMD(do_state);
ACMD(do_notice);
ACMD(do_map_notice);
ACMD(do_big_notice);
ACMD(do_who);
ACMD(do_user);
ACMD(do_disconnect);
ACMD(do_kill);
ACMD(do_emotion_allow);
ACMD(do_emotion);
ACMD(do_transfer);
ACMD(do_set);
ACMD(do_cmd);
ACMD(do_reset);
ACMD(do_greset);
ACMD(do_mount);
ACMD(do_fishing);
ACMD(do_refine_rod);
#ifdef ENABLE_TITLE_SYSTEM
ACMD(do_prestige_title);
#endif
#ifdef ENABLE_LOCALIZATION_SYSTEM
	ACMD(do_localization);
#endif
ACMD(do_schimba_rasa);
ACMD(do_max_pick);
ACMD(do_refine_pick);
#ifdef ENABLE_BIOLOG_SYSTEM
ACMD(do_biolog);
#endif
ACMD(do_fishing_simul);
ACMD(do_console);
ACMD(do_restart);
ACMD(do_advance);
ACMD(do_stat);
ACMD(do_respawn);
ACMD(do_skillup);
ACMD(do_guildskillup);
ACMD(do_pvp);
#ifdef ENABLE_PVP_ADVANCED
ACMD(do_pvp_advanced);
ACMD(do_decline_pvp);
ACMD(do_block_equipment);
#endif
ACMD(do_point_reset);
ACMD(do_safebox_size);
ACMD(do_safebox_close);
ACMD(do_safebox_password);
ACMD(do_safebox_change_password);
ACMD(do_mall_password);
ACMD(do_mall_close);
ACMD(do_ajanikov);
ACMD(do_ungroup);
ACMD(do_makeguild);
ACMD(do_deleteguild);
ACMD(do_shutdown);
ACMD(do_group);
ACMD(do_group_random);
ACMD(do_invisibility);
ACMD(do_event_flag);

//BEST OTO AV
ACMD(do_otomatikav_komut1);
ACMD(do_otomatikav_komut2);
//BEST OTO AV

ACMD(do_get_event_flag);
ACMD(do_private);
ACMD(do_qf);
ACMD(do_clear_quest);
ACMD(do_book);
ACMD(do_reload);
ACMD(do_war);
ACMD(do_nowar);
ACMD(do_setskill);
ACMD(do_setskillother);
ACMD(do_level);
ACMD(do_polymorph);
ACMD(do_polymorph_item);
ACMD(do_close_shop);
ACMD(do_set_walk_mode);
ACMD(do_set_run_mode);
ACMD(do_set_skill_group);
ACMD(do_set_skill_point);
ACMD(do_cooltime);
ACMD(do_detaillog);
ACMD(do_monsterlog);

ACMD(do_gwlist);
ACMD(do_stop_guild_war);
ACMD(do_cancel_guild_war);
ACMD(do_guild_state);
ACMD(do_lonca_gecmis_history_req);

ACMD(do_pkmode);
ACMD(do_mobile);
ACMD(do_mobile_auth);
ACMD(do_messenger_auth);
#ifdef ENABLE_ONLINE_COMMAND
	ACMD(do_sananemk);
#endif
ACMD(do_tanitim);
ACMD(do_getqf);
ACMD(do_setqf);
ACMD(do_delqf);
ACMD(do_set_state);

ACMD(do_forgetme);
ACMD(do_aggregate);
ACMD(do_attract_ranger);
ACMD(do_pull_monster);
ACMD(do_setblockmode);
ACMD(do_priv_empire);
ACMD(do_priv_guild);
ACMD(do_mount_test);
ACMD(do_unmount);
ACMD(do_observer);
ACMD(do_observer_exit);
ACMD(do_socket_item);
ACMD(do_xmas);
ACMD(do_stat_minus);
ACMD(do_stat_reset);
ACMD(do_view_equip);
ACMD(do_block_chat);
ACMD(do_vote_block_chat);

// BLOCK_CHAT
ACMD(do_block_chat_list);
// END_OF_BLOCK_CHAT

ACMD(do_party_request);
ACMD(do_party_request_deny);
ACMD(do_party_request_accept);
ACMD(do_build);
ACMD(do_clear_land);

ACMD(do_horse_state);
ACMD(do_horse_level);
ACMD(do_horse_ride);
ACMD(do_horse_summon);
ACMD(do_horse_unsummon);
ACMD(do_horse_set_stat);
ACMD(do_pm_all_send);

ACMD(do_save_attribute_to_image);
ACMD(do_affect_remove);

ACMD(do_change_attr);
ACMD(do_add_attr);
ACMD(do_add_socket);

ACMD(do_inputall)
{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???????? ???? ??????????."));
}

ACMD(do_show_arena_list);
ACMD(do_end_all_duel);
ACMD(do_end_duel);
ACMD(do_duel);

ACMD(do_stat_plus_amount);

ACMD(do_break_marriage);

ACMD(do_oxevent_show_quiz);
ACMD(do_oxevent_log);
ACMD(do_oxevent_get_attender);

ACMD(do_effect);
ACMD(do_threeway_war_info );
ACMD(do_threeway_war_myinfo );
//
//???? ????????
ACMD(do_monarch_warpto);
ACMD(do_monarch_transfer);
ACMD(do_monarch_info);
ACMD(do_elect);
ACMD(do_monarch_tax);
ACMD(do_monarch_mob);
ACMD(do_monarch_notice);

//???? ???? ????
ACMD(do_rmcandidacy);
ACMD(do_setmonarch);
ACMD(do_rmmonarch);

ACMD(do_hair);
//gift notify quest command
ACMD(do_gift);
// ????????
ACMD(do_inventory);
#ifdef WJ_ITEM_COMBINATION_SYSTEM
ACMD(do_item_comb);
#endif
#ifdef __NEW_ENCHANT_ATTR__
ACMD(do_enchant_new);
ACMD(do_enchant_old);
#endif
#ifdef __NEW_ENCHANT2_ATTR__
ACMD(do_enchant2);
ACMD(do_enchant2_close);
#endif
ACMD(do_cube);
#ifdef ENABLE_OKAY_CARD
ACMD(do_cards);
#endif

ACMD(do_siege);
ACMD(do_temp);
ACMD(do_frog);

ACMD(do_check_monarch_money);

ACMD(do_reset_subskill );
ACMD(do_flush);

ACMD(do_eclipse);
ACMD(do_weeklyevent);

ACMD(do_event_helper);

ACMD(do_in_game_mall);

ACMD(do_get_mob_count);

ACMD(do_dice);
ACMD(do_special_item);

ACMD(do_click_mall);
ACMD(do_click_safebox);
ACMD(do_click_town);

ACMD(do_ride);
ACMD(do_get_item_id_list);
ACMD(do_set_socket);
ACMD(do_costume);
ACMD(do_set_stat);

// ????
ACMD (do_can_dead);
#ifdef ENABLE_USER_PANEL
ACMD(do_open_userpanel);
#endif
#ifdef ENABLE_MANAGER_BANK_SYSTEM
ACMD(do_bank_create);
ACMD(do_bank_open);
ACMD(do_bank_add);
ACMD(do_bank_withdraw);
ACMD(do_bank_send);
ACMD(do_bank_changepw);
#endif
#ifdef ENABLE_KILL_COMMAND_FOR_ALL_PLAYERS_KILL_FUN
ACMD(do_kill_players);
#endif
#ifdef ENABLE_GAME_MASTER_FULL_SET
ACMD (do_full_set);
ACMD (do_item_full_set);
ACMD (do_attr_full_set);
#endif
ACMD (do_all_skill_master);
ACMD (do_use_item);
#ifdef __GAYA__
ACMD(do_gaya_system);
#endif
#ifdef __HIDE_COSTUME_SYSTEM__
ACMD(do_hide_costume);
#endif

ACMD (do_dragon_soul);
ACMD (do_ds_list);
ACMD (do_clear_affect);
//ACMD(do_clear_search)
ACMD(do_user_mount_ride);
ACMD (do_add_new_attribute);

#ifdef BESTPRODUCTION_BOT_KONTROL_SYSTEM
ACMD(do_captcha);
#endif

#ifdef __CHANGE_CHANNEL_SYSTEM__
ACMD (do_change_channel);
#endif

#ifdef __FULL_NOTICE_SYSTEM__
ACMD(do_notice_test);
ACMD(do_big_notice_test);
ACMD(do_map_big_notice);
#endif

#ifdef WJ_OFFLINESHOP_SYSTEM
ACMD(do_kick_offline_shop);
ACMD(do_open_offline_shop);
#endif

#ifdef WJ_SECURITY_SYSTEM
ACMD (do_create_security);
ACMD (do_input_security);
ACMD (do_change_security);
ACMD (do_open_security);
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
ACMD(do_CubePetAdd);
ACMD(do_FeedCubePet);
ACMD(do_PetSkill);
ACMD(do_PetEvo);
ACMD(do_PetChangeName);
#endif

#if defined(WJ_USER_CONTROL)
ACMD(do_user_control);
#endif
ACMD(do_envanter_duzenle);
#ifdef ENABLE_REBORN_SYSTEM
	ACMD(do_set_reborn);
#endif
#ifdef ZODIAC_NISAN_SYSTEM
ACMD(do_zodyakstart);
#endif

struct command_info cmd_info[] =
{
	{ "!RESERVED!",	NULL,			0,			POS_DEAD,	GM_IMPLEMENTOR	}, /* ?????? ?? ???? ?????????? ????. */
	{ "who",		do_who,			0,			POS_DEAD,	GM_LOW_WIZARD	},
#if defined(ENABLE_AFFECT_POLYMORPH_REMOVE)
	{ "remove_polymorph",	do_remove_polymorph,	0,		POS_DEAD,	GM_PLAYER	},
#endif
#ifdef ENABLE_POLY_SHOP
	{ "buy_marble_item", 	do_buy_marble,		 	0, 		POS_DEAD, 	GM_PLAYER },
#endif
	{ "war",		do_war,			0,			POS_DEAD,	GM_PLAYER	},
	{ "warp",		do_warp,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "user",		do_user,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "notice",		do_notice,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "notice_map",	do_map_notice,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "big_notice",	do_big_notice,	0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "nowar",		do_nowar,		0,			POS_DEAD,	GM_PLAYER	},
#ifdef __DUNGEON_INFO_SYSTEM__
	{ "raking_finish", do_raking_finish, 0, POS_DEAD, GM_PLAYER },
	//{ "raking_ataque", do_raking_finish_ataque, 0, POS_DEAD, GM_PLAYER },
#endif
	{ "purge",		do_purge,		0,			POS_DEAD,	GM_WIZARD	},
	{ "weaken",		do_weaken,		0,			POS_DEAD,	GM_GOD		},
	{ "dc",		do_disconnect,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "transfer",	do_transfer,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "goto",		do_goto,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "level",		do_level,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "eventflag",	do_event_flag,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "geteventflag",	do_get_event_flag,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

//BEST OTO AV
	{ "otomatikav_komut1",	do_otomatikav_komut1,	0,			POS_DEAD,	GM_PLAYER	},
	{ "otomatikav_komut2",	do_otomatikav_komut2,	0,			POS_DEAD,	GM_PLAYER	},
//BEST OTO AV

#if defined(WJ_USER_CONTROL)
	{ "user_control",	do_user_control,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
#endif

	{"create_item",	do_create_item, 0,			POS_DEAD, GM_IMPLEMENTOR},
	{ "item",		do_item,		0,			POS_DEAD,	GM_IMPLEMENTOR		},
	{ "pets",		do_pets,		0,			POS_DEAD,	GM_PLAYER		},
	{ "mob",		do_mob,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mob_ld",		do_mob_ld,			0,			POS_DEAD,	GM_IMPLEMENTOR	}, /* ???? ?????? ?????? ?????? ???? /mob_ld vnum x y dir */
	{ "ma",		do_mob_aggresive,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mc",		do_mob_coward,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mm",		do_mob_map,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "kill",		do_kill,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "ipurge",		do_item_purge,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "group",		do_group,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "grrandom",	do_group_random,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "set",		do_set,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reset",		do_reset,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "greset",		do_greset,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "advance",	do_advance,		0,			POS_DEAD,	GM_IMPLEMENTOR		},
	{ "book",		do_book,		0,			POS_DEAD,	GM_IMPLEMENTOR  },
#ifdef ENABLE_KILL_COMMAND_FOR_ALL_PLAYERS_KILL_FUN
	{ "kill_players",		do_kill_players,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
#endif
	{ "console",	do_console,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "shutdow",	do_inputall,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "shutdown",	do_shutdown,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "stat",		do_stat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "stat-",		do_stat_minus,		0,			POS_DEAD,	GM_PLAYER	},
	{ "stat_reset",	do_stat_reset,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "state",		do_state,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "stun",		do_stun,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "slow",		do_slow,		0,			POS_DEAD,	GM_LOW_WIZARD	},

#ifdef __BATTLE_PASS__
	{ "open_battlepass",		open_battlepass,		0,			POS_DEAD,	GM_PLAYER	},
	{ "final_reward_battlepass",		final_reward_battlepass,		0,			POS_DEAD,	GM_PLAYER	},
#endif

	{ "respawn",	do_respawn,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "makeguild",	do_makeguild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "deleteguild",	do_deleteguild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "mount",		do_mount,		0,			POS_MOUNTING,	GM_PLAYER	},
	{ "restart_here",	do_restart,		SCMD_RESTART_HERE,	POS_DEAD,	GM_PLAYER	},
	{ "restart_town",	do_restart,		SCMD_RESTART_TOWN,	POS_DEAD,	GM_PLAYER	},
#if defined(WJ_COMBAT_ZONE)
	{ "restart_combat_zone",	do_restart,		SCMD_RESTART_COMBAT_ZONE,	POS_DEAD,	GM_PLAYER	},
#endif
	{ "phase_selec",	do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "phase_select",	do_cmd,			SCMD_PHASE_SELECT,	POS_DEAD,	GM_PLAYER	},
	{ "qui",		do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "schimba_rasa",    do_schimba_rasa, 0,  POS_DEAD,   GM_IMPLEMENTOR },
	{ "quit",		do_cmd,			SCMD_QUIT,		POS_DEAD,	GM_PLAYER	},
	{ "logou",		do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "logout",		do_cmd,			SCMD_LOGOUT,		POS_DEAD,	GM_PLAYER	},
	{ "skillup",	do_skillup,		0,			POS_DEAD,	GM_PLAYER	},
	{ "gskillup",	do_guildskillup,	0,			POS_DEAD,	GM_PLAYER	},
	{ "guild_notice", do_guild_notice, 0, POS_DEAD, GM_PLAYER },
	{ "loncatoplantis",	do_loncatoplantis,	0,			POS_DEAD,	GM_PLAYER	},
	{ "loncatoplantisi",	do_loncatoplantisi,	0,			POS_DEAD,	GM_PLAYER	},
	{ "loncatoplantieve",	do_loncatoplantieve,	0,			POS_DEAD,	GM_PLAYER	},
	{ "loncatoplantievet",	do_loncatoplantievet,	0,			POS_DEAD,	GM_PLAYER	},
	{ "loncaistatisti",			do_loncaistatisti,		0,	POS_DEAD,	GM_PLAYER	},
	{ "loncaistatistik",		do_loncaistatistik,		0,	POS_DEAD,	GM_PLAYER	},

	{ "pvp",		do_pvp,			0,			POS_DEAD,	GM_PLAYER	},
#ifdef ENABLE_PVP_ADVANCED
	{ "pvp_advanced",	do_pvp_advanced,	0,	POS_DEAD,	GM_PLAYER	},	
	{ "decline_pvp",	do_decline_pvp,		0,	POS_DEAD,	GM_PLAYER	},
	{ "pvp_block_equipment",	do_block_equipment,		0,	POS_DEAD,	GM_PLAYER	},
#endif
	{ "safebox",	do_safebox_size,	0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "safebox_close",	do_safebox_close,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_passwor",do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_password",do_safebox_password,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_change_passwor", do_inputall,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_change_password", do_safebox_change_password,	0,	POS_DEAD,	GM_PLAYER	},
	{ "mall_passwor",	do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "mall_password",	do_mall_password,	0,			POS_DEAD,	GM_PLAYER	},
	{ "mall_close",	do_mall_close,		0,			POS_DEAD,	GM_PLAYER	},
#ifdef ENABLE_TITLE_SYSTEM
	{ "prestige_title_name",	do_prestige_title,	0,	POS_DEAD,	GM_PLAYER},	
#endif
	{ "ajanikov",	do_ajanikov,		0,			POS_DEAD,	GM_PLAYER	}, //EviLsHow
#ifdef ENABLE_LOCALIZATION_SYSTEM
	{ "localization",	do_localization,	0,	POS_DEAD,	GM_PLAYER		},
#endif

	{ "ungroup",	do_ungroup,		0,			POS_DEAD,	GM_PLAYER	},

	{ "refine_rod",	do_refine_rod,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "refine_pick",	do_refine_pick,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef ENABLE_BIOLOG_SYSTEM
	{ "biolog",		do_biolog,		0,		POS_DEAD,		GM_PLAYER	},
#endif
	{ "max_pick",	do_max_pick,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "fish_simul",	do_fishing_simul,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "invisible",	do_invisibility,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "qf",		do_qf,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "clear_quest",	do_clear_quest,		0,			POS_DEAD,	GM_HIGH_WIZARD	},

	{ "close_shop",	do_close_shop,		0,			POS_DEAD,	GM_PLAYER	},

	{ "set_walk_mode",	do_set_walk_mode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "set_run_mode",	do_set_run_mode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "setjob",do_set_skill_group,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskill",	do_setskill,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskillother",	do_setskillother,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskillpoint",  do_set_skill_point,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reload",		do_reload,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "cooltime",	do_cooltime,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "gwlist",		do_gwlist,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "gwstop",		do_stop_guild_war,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "gwcancel",	do_cancel_guild_war, 0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "gstate",		do_guild_state,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "lonca_gecmis_history_req",		do_lonca_gecmis_history_req,		0,			POS_DEAD,	GM_PLAYER	},

	{ "pkmode",		do_pkmode,		0,			POS_DEAD,	GM_PLAYER	},
	{ "messenger_auth",	do_messenger_auth,	0,			POS_DEAD,	GM_PLAYER	},
#ifdef ENABLE_ONLINE_COMMAND
	{ "sananemk",	do_sananemk,	0,			POS_DEAD,	GM_PLAYER	},
#endif
	{ "tanitim",		do_tanitim,		0,			POS_DEAD,	GM_PLAYER	},

	{ "getqf",		do_getqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setqf",		do_setqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "delqf",		do_delqf,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "set_state",	do_set_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "detaillog",	do_detaillog,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "monsterlog",	do_monsterlog,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "forgetme",	do_forgetme,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "aggregate",	do_aggregate,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "attract_ranger",	do_attract_ranger,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "pull_monster",	do_pull_monster,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setblockmode",	do_setblockmode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "polymorph",	do_polymorph,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "polyitem",	do_polymorph_item,	0,			POS_DEAD,	GM_IMPLEMENTOR },
	{ "priv_empire",	do_priv_empire,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "priv_guild",	do_priv_guild,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mount_test",	do_mount_test,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "unmount",	do_unmount,		0,			POS_DEAD,	GM_PLAYER	},
	{ "private",	do_private,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "party_request",	do_party_request,	0,			POS_DEAD,	GM_PLAYER	},
	{ "party_request_accept", do_party_request_accept,0,		POS_DEAD,	GM_PLAYER	},
	{ "party_request_deny", do_party_request_deny,0,			POS_DEAD,	GM_PLAYER	},
	{ "observer",	do_observer,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "observer_exit",	do_observer_exit,	0,			POS_DEAD,	GM_PLAYER	},
	{ "socketitem",	do_socket_item,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "saveati",	do_save_attribute_to_image, 0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "xmas_boom",	do_xmas,		SCMD_XMAS_BOOM,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "xmas_snow",	do_xmas,		SCMD_XMAS_SNOW,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "xmas_santa",	do_xmas,		SCMD_XMAS_SANTA,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "view_equip",	do_view_equip,		0,			POS_DEAD,	GM_PLAYER   	},
	{ "jy",				do_block_chat,		0,			POS_DEAD,	GM_LOW_WIZARD	},

	// BLOCK_CHAT
	{ "vote_block_chat", do_vote_block_chat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "block_chat",		do_block_chat,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "block_chat_list",do_block_chat_list,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	// END_OF_BLOCK_CHAT

	{ "build",		do_build,		0,		POS_DEAD,	GM_PLAYER	},
	{ "clear_land", do_clear_land,	0,		POS_DEAD,	GM_IMPLEMENTOR	},

	{ "affect_remove",	do_affect_remove,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "horse_state",	do_horse_state,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_level",	do_horse_level,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_ride_gellan",	do_horse_ride,		0,			POS_DEAD,	GM_PLAYER	},
	{ "horse_summon_gellan",	do_horse_summon,		0,			POS_DEAD,	GM_PLAYER	},
	{ "horse_ride",	do_horse_ride,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_summon",	do_horse_summon,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_unsummon",	do_horse_unsummon,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "horse_set_stat", do_horse_set_stat,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "pm_all_send", do_pm_all_send,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "pcbang_update", 	do_pcbang_update,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "pcbang_check", 	do_pcbang_check,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "npcac",	do_npcac,	0,	POS_DEAD,	GM_PLAYER	},
	{ "ruhoku",	do_ruhoku,	0,	POS_DEAD,	GM_PLAYER	},

	{ "bkoku",	do_bkoku,	0,	POS_DEAD,	GM_PLAYER	},

	{ "emotion_allow",	do_emotion_allow,	0,			POS_FIGHTING,	GM_PLAYER	},
	{ "kiss",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "slap",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "french_kiss",	do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "clap",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "cheer1",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "cheer2",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},

	{ "otur",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "oturs",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},

	// DANCE
	{ "dance1",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance2",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance3",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance4",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance5",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance6",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	// END_OF_DANCE


#ifdef ENABLE_NEW_GIFT_ANIMATIONS
	// DANCE
	{ "mixamo_101",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_102",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_103",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_104",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_105",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_106",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_107",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_108",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_109",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_110",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_111",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_112",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_113",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_114",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_115",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_116",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_117",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_118",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_119",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_120",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_121",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_122",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_123",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "mixamo_124",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	// END_OF_DANCE
	// GIFT
	{ "emote101",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote102",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote103",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote104",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote105",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote106",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote107",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote108",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote109",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote110",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote111",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote112",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote113",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote114",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote115",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote116",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote117",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote118",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote119",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote120",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote121",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote122",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote123",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "emote124",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
#endif

	{ "congratulation",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "forgive",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "angry",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "attractive",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "sad",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "shy",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "cheerup",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "banter",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "joy",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
#ifdef __EXPRESSING_EMOTIONS__
	{ "charging",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "nosay",			do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "weather1",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "weather2",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "weather3",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "hungry",			do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "siren",			do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "letter",			do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "call",			do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "celebration",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "alcohol",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "busy",			do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "whirl",			do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
#endif
	{ "selfie",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "dance7",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "doze",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "exercise",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "pushup",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},


	{ "change_attr",	do_change_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "add_attr",	do_add_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "add_socket",	do_add_socket,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "user_horse_ride",	do_user_horse_ride,		0,		POS_FISHING,	GM_PLAYER	},
	{ "user_horse_back",	do_user_horse_back,		0,		POS_FISHING,	GM_PLAYER	},
	{ "user_horse_feed",	do_user_horse_feed,		0,		POS_FISHING,	GM_PLAYER	},

	{ "show_arena_list",	do_show_arena_list,		0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "end_all_duel",		do_end_all_duel,		0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "end_duel",			do_end_duel,			0,		POS_DEAD,	GM_IMPLEMENTOR	},
	{ "duel",				do_duel,				0,		POS_DEAD,	GM_IMPLEMENTOR	},

	{ "con+",			do_stat_plus_amount,	POINT_HT,	POS_DEAD,	GM_PLAYER	},
	{ "int+",			do_stat_plus_amount,	POINT_IQ,	POS_DEAD,	GM_PLAYER	},
	{ "str+",			do_stat_plus_amount,	POINT_ST,	POS_DEAD,	GM_PLAYER	},
	{ "dex+",			do_stat_plus_amount,	POINT_DX,	POS_DEAD,	GM_PLAYER	},

	{ "break_marriage",	do_break_marriage,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "show_quiz",			do_oxevent_show_quiz,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "log_oxevent",		do_oxevent_log,			0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "get_oxevent_att",	do_oxevent_get_attender,0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "effect",				do_effect,				0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "threeway_info",		do_threeway_war_info,	0,	POS_DEAD,	GM_IMPLEMENTOR},
	{ "threeway_myinfo",	do_threeway_war_myinfo, 0,	POS_DEAD,	GM_IMPLEMENTOR},
	{ "mto",				do_monarch_warpto,		0, 	POS_DEAD,	GM_PLAYER},
	{ "mtr",				do_monarch_transfer,	0,	POS_DEAD,	GM_PLAYER},
	{ "minfo",		do_monarch_info,		0,  POS_DEAD,   GM_PLAYER},
	{ "mtax",			do_monarch_tax,			0,	POS_DEAD,	GM_PLAYER},
	{ "mmob",			do_monarch_mob,			0, 	POS_DEAD,	GM_PLAYER},
	{ "elect",				do_elect,				0,	POS_DEAD,	GM_IMPLEMENTOR},
	{ "rmcandidacy",		do_rmcandidacy,			0, 	POS_DEAD,	GM_IMPLEMENTOR},
	{ "setmonarch",			do_setmonarch,			0, 	POS_DEAD,	GM_IMPLEMENTOR},
	{ "rmmonarch",			do_rmmonarch,			0, 	POS_DEAD, 	GM_IMPLEMENTOR},
	{ "hair",				do_hair,				0,	POS_DEAD,	GM_PLAYER	},
	{ "inventory",			do_inventory,			0,	POS_DEAD,	GM_IMPLEMENTOR	},
#ifdef WJ_ITEM_COMBINATION_SYSTEM
	{ "item_comb",			do_item_comb,			0,	POS_DEAD,	GM_PLAYER	},
#endif
#ifdef __NEW_ENCHANT_ATTR__
	{"enchant_new", do_enchant_new, 0, POS_DEAD, GM_PLAYER},
	{"enchant_old", do_enchant_old, 0, POS_DEAD, GM_PLAYER},
#endif
#ifdef __NEW_ENCHANT2_ATTR__
	{"enchant2", do_enchant2, 0, POS_DEAD, GM_PLAYER},
	{"enchant2_close", do_enchant2_close, 0, POS_DEAD, GM_PLAYER},
#endif
	{ "cube",				do_cube,				0,	POS_DEAD,	GM_PLAYER	},
#ifdef ENABLE_OKAY_CARD
	{ "cards",				do_cards,				0,	POS_DEAD,	GM_PLAYER	},
#endif
#ifdef __HIDE_COSTUME_SYSTEM__
	{ "hide_costume", do_hide_costume, 0, POS_DEAD, GM_PLAYER },
#endif

	{ "siege",				do_siege,				0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "temp",				do_temp,				0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "frog",				do_frog,				0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "check_mmoney",		do_check_monarch_money,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reset_subskill",		do_reset_subskill,		0,	POS_DEAD,	GM_HIGH_WIZARD },
	{ "flush",				do_flush,				0,	POS_DEAD,	GM_IMPLEMENTOR },
#ifdef ENABLE_USER_PANEL
	{ "open_userpanel",		do_open_userpanel,		0, POS_DEAD,	GM_PLAYER },
#endif
#ifdef ENABLE_MANAGER_BANK_SYSTEM
	{ "bank_create",	do_bank_create,		0, POS_DEAD, GM_PLAYER },
	{ "bank_open",		do_bank_open,		0, POS_DEAD, GM_PLAYER },
	{ "bank_add",		do_bank_add,		0, POS_DEAD, GM_PLAYER },
	{ "bank_withdraw",	do_bank_withdraw,	0, POS_DEAD, GM_PLAYER },
	{ "bank_send",		do_bank_send,		0, POS_DEAD, GM_PLAYER },
	{ "bank_changepw",	do_bank_changepw,	0, POS_DEAD, GM_PLAYER },
#endif
	{ "gift",				do_gift,				0,  POS_DEAD,   GM_PLAYER	},

	{ "mnotice",			do_monarch_notice,		0,	POS_DEAD,	GM_PLAYER	},

	{ "eclipse",			do_eclipse,				0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "weeklyevent",		do_weeklyevent,			0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "eventhelper",		do_event_helper,		0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "in_game_mall",		do_in_game_mall,		0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "get_mob_count",		do_get_mob_count,		0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "dice",				do_dice,				0,	POS_DEAD,	GM_IMPLEMENTOR		},
	{ "special_item",			do_special_item,	0,	POS_DEAD,	GM_IMPLEMENTOR		},

	{ "click_mall",			do_click_mall,			0,	POS_DEAD,	GM_PLAYER		},
	{ "click_safebox",      do_click_safebox,       0,  POS_DEAD,   GM_PLAYER       },
	{ "click_town",			do_click_town,       	0,  POS_DEAD,   GM_PLAYER       },
	{ "ride",				do_ride,				0,	POS_DEAD,	GM_PLAYER	},

	{ "item_id_list",	do_get_item_id_list,	0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "set_socket",		do_set_socket,			0,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "costume",			do_costume, 			0,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "tcon",			do_set_stat,	POINT_HT,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tint",			do_set_stat,	POINT_IQ,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tstr",			do_set_stat,	POINT_ST,	POS_DEAD,	GM_IMPLEMENTOR	},
	{ "tdex",			do_set_stat,	POINT_DX,	POS_DEAD,	GM_IMPLEMENTOR	},

	{ "cannot_dead",			do_can_dead,	1,	POS_DEAD,		GM_IMPLEMENTOR},
	{ "can_dead",				do_can_dead,	0,	POS_DEAD,		GM_IMPLEMENTOR},

	#ifdef ENABLE_GAME_MASTER_FULL_SET
	{ "full_set",	do_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "item_full_set",	do_item_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	{ "attr_full_set",	do_attr_full_set, 0, POS_DEAD,		GM_IMPLEMENTOR},
	#endif
	
	{ "all_skill_master",	do_all_skill_master,	0,	POS_DEAD,	GM_IMPLEMENTOR},
	{ "use_item",		do_use_item,	0, POS_DEAD,		GM_IMPLEMENTOR},

#ifdef __GAYA__
	{ "w_gaya",			do_gaya_system,		0,			POS_DEAD,		GM_PLAYER		},
#endif

	{ "dragon_soul",				do_dragon_soul,				0,	POS_DEAD,	GM_PLAYER	},
	{ "ds_list",				do_ds_list,				0,	POS_DEAD,	GM_PLAYER	},
	{ "do_clear_affect", do_clear_affect, 	0, POS_DEAD,		GM_IMPLEMENTOR},

#ifdef BESTPRODUCTION_BOT_KONTROL_SYSTEM
    { "captcha",    do_captcha,    0,    POS_DEAD,    GM_PLAYER    },
#endif
#ifdef ENABLE_DS_CHANGE_ATTR
    { "ds_change_attr", do_ds_change_attr, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef __FULL_NOTICE_SYSTEM__
	{ "big_notice_map",	do_map_big_notice,	0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "notice_test",	do_notice_test,		0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "big_notice_test",do_big_notice_test,	0,	POS_DEAD,	GM_HIGH_WIZARD	},
#endif

#ifdef WJ_OFFLINESHOP_SYSTEM
	{ "kick_offlineshop", do_kick_offline_shop, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "open_offlineshop",		do_open_offline_shop,		0,	POS_DEAD,	GM_PLAYER	},
#endif
	{ "user_mount_ride",		do_user_mount_ride,			0,	POS_DEAD,	GM_PLAYER	},
	{ "add_new_attribute",		do_add_new_attribute,		0,	POS_DEAD,	GM_PLAYER	},
#ifdef WJ_SECURITY_SYSTEM
	{ "create_security",		do_create_security,			0,	POS_DEAD,	GM_PLAYER	},
	{ "input_security",			do_input_security,			0,	POS_DEAD,	GM_PLAYER	},
	{ "change_security",		do_change_security,			0,	POS_DEAD,	GM_PLAYER	},
	{ "open_security",			do_open_security,			0,	POS_DEAD,	GM_PLAYER	},
#endif

#ifdef __CHANGE_CHANNEL_SYSTEM__
	{ "channel",						do_change_channel,				0,					POS_DEAD,		GM_PLAYER		},
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	{ "cubepetadd",		do_CubePetAdd,	0,	POS_DEAD,	GM_PLAYER },
	{ "feedcubepet",	do_FeedCubePet,	0,	POS_DEAD,	GM_PLAYER },
	{ "petskills",		do_PetSkill,	0,	POS_DEAD,	GM_PLAYER },
	{ "petvoincrease",	do_PetEvo,		0,	POS_DEAD,	GM_PLAYER },
	{ "pet_change_name",	do_PetChangeName,	0,	POS_DEAD,	GM_PLAYER	},
#endif

	{ "envanter_duzenle",	do_envanter_duzenle,	0,	POS_DEAD,	GM_PLAYER	},
#ifdef ENABLE_REBORN_SYSTEM
	{ "set_reborn",	do_set_reborn,		0,			POS_DEAD,	GM_PLAYER	},
#endif
#ifdef ENABLE_RONARK_SYSTEM
	{ "ronarksystem",	do_ronark_system,	0,			POS_DEAD,	GM_PLAYER	},
#endif
#ifdef ZODIAC_NISAN_SYSTEM
	{ "zodiak_gui_start",	do_zodyakstart,		0,			POS_DEAD,	GM_PLAYER	},
#endif


	{ "\n",		NULL,			0,			POS_DEAD,	GM_IMPLEMENTOR	} 
};

void interpreter_set_privilege(const char *cmd, int lvl)
{
	int i;

	for (i = 0; *cmd_info[i].command != '\n'; ++i)
	{
		if (!str_cmp(cmd, cmd_info[i].command))
		{
			cmd_info[i].gm_level = lvl;
			sys_log(0, "Setting command privilege: %s -> %d", cmd, lvl);
			break;
		}
	}
}

void double_dollar(const char *src, size_t src_len, char *dest, size_t dest_len)
{
	const char * tmp = src;
	size_t cur_len = 0;

	// \0 ???? ???? ????
	dest_len -= 1;

	while (src_len-- && *tmp)
	{
		if (*tmp == '$')
		{
			if (cur_len + 1 >= dest_len)
				break;

			*(dest++) = '$';
			*(dest++) = *(tmp++);
			cur_len += 2;
		}
		else
		{
			if (cur_len >= dest_len)
				break;

			*(dest++) = *(tmp++);
			cur_len += 1;
		}
	}

	*dest = '\0';
}

void interpret_command(LPCHARACTER ch, const char * argument, size_t len)
{
	if (NULL == ch)
	{
		sys_err ("NULL CHRACTER");
		return ;
	}

	char cmd[1024 + 1];  // buffer overflow ?????? ?????? ?????? ?????? ?????? ???? ????
	char new_line[2048 + 1];
	const char * line;
	int icmd;

	if (len == 0 || !*argument)
		return;

	double_dollar(argument, len, new_line, sizeof(new_line));

	size_t cmdlen;
	line = first_cmd(new_line, cmd, sizeof(cmd), &cmdlen);

	for (icmd = 1; *cmd_info[icmd].command != '\n'; ++icmd)
	{
		if (cmd_info[icmd].command_pointer == do_cmd)
		{
			if (!strcmp(cmd_info[icmd].command, cmd)) // do_cmd?? ???? ???????? ???? ?? ?? ????.
				break;
		}
		else if (!strncmp(cmd_info[icmd].command, cmd, cmdlen))
			break;
	}

	if (ch->GetPosition() < cmd_info[icmd].minimum_position)
	{
		switch (ch->GetPosition())
		{
			case POS_MOUNTING:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?? ?????????? ?? ?? ????????."));
				break;

			case POS_DEAD:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?????? ?????????? ?? ?? ????????."));
				break;

			case POS_SLEEPING:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???????? ?????????"));
				break;

			case POS_RESTING:
			case POS_SITTING:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???? ???? ??????."));
				break;
				/*
				   case POS_FIGHTING:
				   ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?????? ???? ???? ?? ??????. ???? ??????."));
				   break;
				 */
			default:
				sys_err("unknown position %d", ch->GetPosition());
				break;
		}

		return;
	}

	if (*cmd_info[icmd].command == '\n')
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???? ???????? ????????"));
		return;
	}

	if (cmd_info[icmd].gm_level && cmd_info[icmd].gm_level > ch->GetGMLevel())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("???? ???????? ????????"));
		return;
	}

	if (strncmp("phase", cmd_info[icmd].command, 5) != 0) // ???? ?????? ???? 
		sys_log(0, "COMMAND: %s: %s", ch->GetName(), cmd_info[icmd].command);

	((*cmd_info[icmd].command_pointer) (ch, line, icmd, cmd_info[icmd].subcmd));

	if (ch->GetGMLevel() >= GM_LOW_WIZARD)
	{
		if (cmd_info[icmd].gm_level >= GM_LOW_WIZARD)
		{
			if (LC_IsEurope() == true || /*LC_IsNewCIBN() == true || */LC_IsCanada() == true || LC_IsBrazil() == true)
			{
				char buf[1024];
				snprintf( buf, sizeof(buf), "%s", argument );

				LogManager::instance().GMCommandLog(ch->GetPlayerID(), ch->GetName(), ch->GetDesc()->GetHostName(), g_bChannel, buf);
			}
		}
	}
}

