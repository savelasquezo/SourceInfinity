#ifndef __INC_SERVICE_H__
#define __INC_SERVICE_H__


//#define METIN2_LAMDA_100PVP						// Server 100% PvP Config


/*** YMIR Default Macros ***/
#define ENABLE_GAME_MASTER_FULL_SET					//GM /Full Set Komutu
#define _IMPROVED_PACKET_ENCRYPTION_				// YMIR Paket þifreleme
#define __PET_SYSTEM__								// Eski Evcil hayvan sistemi
#define __UDP_BLOCK__								// UDP Paket bloklamasý
#define ENABLE_MESSENGER_TEAM						// Messenger Team
#define __NEW_ENCHANT_ATTR__						// Objeto Encantado Block Especial
#define __NEW_ENCHANT2_ATTR__						// Objeto Encantado Selct Especial
#define __HIDE_COSTUME_SYSTEM__						// Hide costume part
#define ITEM_TALISMAN_EQUIPMENT						// Talismanes
#define ENABLE_NEW_GIFT_ANIMATIONS					// Animaciones/Memes
#define __DUNGEON_INFO_SYSTEM__						// informacion de Mazmorras
#define __KILL_BOSS_WITH_SKILLS__					// Mecanica de Matar Jefes con Habilidades
#define YAGONG_SCROLL_MODIFIC						// Pergamino del Herrero 75% Acierto/ No Destruye / No Disminuye
#define ENABLE_SWITCHBOT							// Nuevo Autodopador
//#define ENABLE_DROP_ITEM_WORLDARD					// Buscador de MonsterDrop
#define __ITEM_SHINING__							// Systema de Efectos
#define __DEFENSE_WAVE__							// Hidra Dungeon
#define __SKILL_COLOR_SYSTEM__						// Skill color system
/***************************************/
/*** Main Macros ***/
#define WJ_OFFLINESHOP_SYSTEM						// Çevrimdýþý pazar sistemi
#define WJ_SHOP_SEARCH_SYSTEM						// Ticaret (Arama) aynasý sistemi
#define	ENABLE_CUBE_RENEWAL							// Arýndýrma paneli güncellemesi
#define __GAYA__									// TR Tipi gaya sistemi
#define __CHANGE_CHANNEL_SYSTEM__					// TR Tipi kanal deðiþtirme sistemi
#define __ANTI_EXP_RING__							// Anillo AntiEXP
#define __WJ_SHOW_MOB_INFO__						// TR Tipi Mob seviye agresif göster gizle
#define ENABLE_PLAYER_PER_ACCOUNT5					// TR Tipi 5 karakter oluþturma sistemi
#define ENABLE_AFFECT_POLYMORPH_REMOVE				// Dönüþümden çýkma sistemi
#define ENABLE_POLY_SHOP							// Dönüþüm Maðazasý
#define ENABLE_DICE_SYSTEM							// TR Tipi Zar Sistemi
#define __HIGHLIGHT_SYSTEM__						// TR Tipi eþya parlama efekti sistemi
#define ENABLE_TITLE_SYSTEM							// Sitema de Titulos (Incompleto Titulos Premium)
#define ENABLE_LOCALIZATION_SYSTEM					// Konum kaydetme sistemi
#define ENABLE_PARTY_BONUS_FIX						// Grup ile yapýlan bonus fixi.
#define ENABLE_YMIR_AFFECT_FIX						// Þebnem bugunu engelleme sistemi
#define __EXPRESSING_EMOTIONS__						// New Emotes
#define ENABLE_ONLINE_COMMAND						// Aktif oyuncu gösterim sistemi
#define __SEND_TARGET_INFO__						// Yaratýklardan düþen nesneleri görme sistemi
#define ENABLE_OKAY_CARD							// TR Tipi Okey kart eventi sistemi
#define __BOSS_SECURITY__							// Patron (Boss) yaratýklarýn güvenlik sistemi
#define __SASH_SYSTEM__								// TR Tipi Skaleli kuþak sistemi
#define ENABLE_BIOLOG_SYSTEM						// Geliþtirilmiþ biyalog sistemi
#define ENABLE_BIOLOG_SYSTEM_TIME					// Biyalog ürenlerinin süre ve özelliklerini ayarlama sistemi
#define __ENABLE_FISH_EVENT_SYSTEM__				// MiniGame Puzzle Tetris
#define WJ_ITEM_COMBINATION_SYSTEM					// TR Tipi Kostüm bonuslarýný aktarma sistemi
#define WJ_SHOW_STROKE_INFO							// Yaratýk vuruþ sayacý
#define ENABLE_CHEQUE_SYSTEM						// Won parasý sistemi
#define ENABLE_SOULBIND_SYSTEM						// Ruha baðlama sistemi
#define ENABLE_SHOW_CHEST_DROP						// Sandýk aynasý sistemi

#define ENABLE_REBORN_SYSTEM						// Yeniden doðuþ sistemi
#define ENABLE_RELOAD_QUEST_FIX						// Quest yüklenirken ekranda kalma sorunu fix
#define WJ_ENABLE_TRADABLE_ICON						// TR Tipi Ticarete koyulman nesneleri görme sistemi
#define __WEAPON_COSTUME_SYSTEM__					// Silah köstümü sistemi
#define WJ_CHANGELOOK_SYSTEM						// TR Tipi yansýtma sistemi
//#define ENABLE_SUPPORT_SYSTEM						// Support Shamana
#define WJ_SECURITY_SYSTEM							// Oyun giriþi güvenlik sistemi
#define WJ_SPLIT_INVENTORY_SYSTEM					// Special Storage
#define WJ_EXTENDED_PET_SYSTEM						// Envantere giymeli pet sistemi
#define ENABLE_MESSENGER_BLOCK						// TR Tipi Oyuncu bloklama sistemi
#define DEVILTOWER_BLACKSMITH_NOFAIL				// DemonBlackSmith Never Fail
#define __GUILD_LEADER_GRADE_NAME__					// Shows guild leader grade name on text tail
//#define GUILD_LEADER_GENERAL						// Lonca Lideri & Generali Tag Sistemi.
#define ENABLE_USER_PANEL							// Geliþtirilmiþ oyuncu kýsayol paneli sistemi
#define ENABLE_FEATURES_REFINE_SYSTEM				// Geliþtirilmiþ + basma sistemi
#define ENABLE_NEW_QUICK_SLOT_SYSTEM				// Geliþtirilmiþ ekstra slot sistemi
#define ENABLE_FEATURES_OXEVENT						// Geliþtirilmiþ OX event panel sistemi
#define ENABLE_SHOW_QUIZ_NUMBER_OXEVENT				// Ox soru sorma sayýsý sistemi
#define ENABLE_MANAGER_BANK_SYSTEM					// Geliþtirilmiþ banka sistemi
#define ENABLE_FIX_ELIXIR_TIMER						// Ýksirleri bugunu hesaplama ve engelleme sistemi
//#define __ENABLE_MINIGAME_BOSSHUNTERS__				// Patron avcýsý sistemi
#define __ENABLE_BLOCK_PICKUP__ 					// Toplanamayan özel nesnelerin engellenmesini saðlar
//#define WJ_DRAGONBONE_SYSTEM  						// Efectos Equipo Dragon [Hand LR]
#define ENABLE_MYSQL_QUERY_FUNCTION					// Veritabaný Query fonsksiyonu
#define ENABLE_ADMIN_BAN_MANAGER					// Geliþtirilmiþ Oyuncu engelleme sistemi
#define ENABLE_LIST_DENIED_TOURNAMENT_DAYS			// Turnuva kategori listesi sistemi
#define WJ_USER_CONTROL								// Oyuncu paneli sistemi
#define ENABLE_BLOCK_NEW_DUEL_SYSTEM_FOR_DOJANG		// Düello esnasýnda ýþýnlanma bugu fix
#define ENABLE_KILL_COMMAND_FOR_ALL_PLAYERS_KILL_FUN// Rakip öldürme leþ sistemi
#define ENABLE_BUFFIX_SKILL_MANA					// Silah olmadan becerileri kullanmax fix
#define ENABLE_BLOCK_SKILL_OXEVENT					// OX haritasýnda becerilerini kullanamaz fix
//#define BESTPRODUCTION_BOT_KONTROL_SYSTEM			// AntiBot
#define ENABLE_ACCOUNT_LOCK							// Hesap kilit sistemi
#define __FULL_NOTICE_SYSTEM__						// Duyuru ayarlarý sistemi
#define ENABLE_FIX_DESTROY_GUILD_IF_WAR_IS_ACTIVE	// Lonca daðýtma fix
#define ENABLE_CHECK_GHOSTMODE						// Ölüyken hareket edemessin fix
#define MARRIAGE_RING_DUNGEON_BLOCK					// Restricciones Anillo de Bodas
#define __ENABLE_EVENT_SYSTEM__						// Adminpanel Eventos
#define __ENABLE_AUTO_NOTICE_SYSTEM__				// Otomatik duyuru sistemi

#define ENABLE_GOHOME_IFNOMAP
#define ATTR_BLOCK_OBJECT_FIX

//#define __ENABLE_COSTUME_EVOLUTION__				// Kostüm parlama sistemi
#define ENABLE_RONARK_SYSTEM						// Ronark sistemi
#define ZODIAC_NISAN_SYSTEM							// Zodyak niþan arýndýrma sistemi
#define __ENABLE_COSTUME_ENCHANT_SYSTEM__			// Kostüm efsunlama sistemi
#define ENABLE_PENDANT								// Talismanes
#define __ENABLE_ELEMENT_NEW_BONUSES__				// Yeni element bonuslarý
#define __ENABLE_TARGET_ELEMENT_SYSTEM__			// canavar element sistemi
#define __ANTI_RESIST_MAGIC_BONUS__					// Büyü bozma taþý sistemi
#define __ENABLE_DRAGON_SOUL_ACTIVE_EFFECT__		// Simya aktif efekti
//#define __7AND8TH_SKILLS__						// 7&8 skill beceri sistemi
#define TOURNAMENT_PVP_SYSTEM						// Oyuncular arasý turnuva savaþý sistemi
#define __ENABLE_BONUS_STRONG_AGAINST_WEAPON__		// Silahlara karþý güçlü
#define __ENABLE_CHARM_SYSTEM__						// TR Tipi týlsým sistemi

/***************************************/
/*** BEST PRODUCTION ***/
#define ENABLE_PVP_ADVANCED							// Geliþtirilmiþ düello sistemi
#define EQUIP_ENABLE_VIEW_SASH 						// Eðer Kuþak (Kanat) sisteminiz yoksa bu satýrý siliniz
#define BLOCK_CHANGEITEM "pvp.BlockChangeItem"		// Düello esnasýnda item deðiþimini engelleme
#define BLOCK_BUFF "pvp.BlockBuff"					// Düello esnasýnda þaman becerilerini engelleme
#define BLOCK_POTION "pvp.BlockPotion"				// Düello esnasýnda iksirleri engelleme
#define BLOCK_RIDE "pvp.BlockRide"					// Düello esnasýnda at (Binek) engelleme
#define BLOCK_PET "pvp.BlockPet"					// Düello esnasýnda evcil hayvan (Pet) engelleme
#define BLOCK_POLY "pvp.BlockPoly"					// Düello esnasýnda dönüþümü engelleme
#define BLOCK_PARTY "pvp.BlockParty"				// Düello esnasýnda grup kurma engelleme
#define BLOCK_EXCHANGE_ "pvp.BlockExchange"			// Düello esnasýnda ticaret penceresini engelleme
#define BLOCK_EQUIPMENT_ "pvp.BLOCK_VIEW_EQUIPMENT"	// Düello esnasýnda ekipman görüntülemeyi engelleme
#define BET_WINNER "pvp.BetMoney"					// Düello yaparken yang (para) bahisi
#define CHECK_IS_FIGHT "pvp.IsFight"				// Düello baþlarken 1-2-3 sayým efekti

#define ENABLE_GROWTH_PET_SYSTEM					// TR Tipi Evrimli evcil hayvan sistemi
#ifdef ENABLE_GROWTH_PET_SYSTEM						
//	#define DISABLE_TRADE_UNSUMMON					// Bu, bir takas / ticaret / dükkan / dükkaným / kasa penceresi açýldýðýnda evcil hayvanýn ahlaksýzlýðýný iptal eder,
													// Vnum ile öðeleri ayarladýðýnýzdan emin olun 55401/55402/55403/55404 with antiflag ANTI_SAFEBOX | ANTI_PKDROP | ANTI_DROP | ANTI_SELL | ANTI_GIVE | ANTI_STACK | ANTI_MYSHOP
#endif
#define ENABLE_MULTI_RANKING
#define ENABLE_WOLFMAN_CHARACTER					//
#ifdef ENABLE_WOLFMAN_CHARACTER						// 5.Karakter Lycan (Wolfman)
#define USE_MOB_BLEEDING_AS_POISON					// Mob kanama vurmasý aktif eder
#define USE_MOB_CLAW_AS_DAGGER						// Moblara karþý býçak ve pençeyi aktif eder
#define USE_ITEM_BLEEDING_AS_POISON					// Ýtemlere kanama efsunu gelir aktif eder
#define USE_ITEM_CLAW_AS_DAGGER						// Wolfman býçak savunmasý aktif eder
#define USE_WOLFMAN_STONES							// Metinlerden Wolfman taþlarý düþmesini aktif eder
#define USE_WOLFMAN_BOOKS							// Metinlerden Wolfman beceri kitaplarýn düþmesini aktif eder
#endif

#define ENABLE_DS_GRADE_MYTH						// Alquimia Grado Mitica
#define ENABLE_DS_CHANGE_ATTR						// Cambiador de Bonus Alquimias
#define __MOUNT_SYSTEM__							// yanýnda gezdirmeli binek sistemi
#ifdef __MOUNT_SYSTEM__
	#define __MS_BUG_WITH_MAPS__					// binek eþya bugu çözümü.
#endif

#define	__VIEW_TARGET_PLAYER_HP__					// Oyuncunun yaþam puanýný görme sistemi
#ifdef	__VIEW_TARGET_PLAYER_HP__
	#define __VIEW_TARGET_DECIMAL_HP__				// Oyuncunun yaþam puanýný yüzdeli þekilde görme
#endif

#define WJ_COMBAT_ZONE // Enable Combat Zone

#define __DUNGEON_FOR_GUILD__						// Loncalar için zindan
#ifdef __DUNGEON_FOR_GUILD__
	#define __MELEY_LAIR_DUNGEON__					// Meley zindaný
	#ifdef __MELEY_LAIR_DUNGEON__
		#define __LASER_EFFECT_ON_75HP__			// Yüzde 75 yaþam puanýnda efekt
		#define __LASER_EFFECT_ON_50HP__			// Yüzde 50 yaþam puanýnda efekt
	#endif
#endif

#define __NEW_EVENTS__								// Ýmparatorluk savaþý eventi sistemi
#ifdef __NEW_EVENTS__
	#define __KINGDOMS_WAR__						// Kral Ýmparatorluðu
#endif

#define __VERSION_162__								// Büyülü orman tapýnaðý
#ifdef __VERSION_162__
	#define HEALING_SKILL_VNUM 265					// Yaþam becerisi kodu
#endif


#define __MULTI_SHOP__
#ifdef __MULTI_SHOP__
	#define ENABLE_BUY_ITEMS_AND_YANG
#endif

/***************************************/
/*** Hack Macros ***/
#define __ENABLE_CAMP_FIRE_FIX__			// kamp ateþi hilesi
/***************************************/
/*** Aktif olmayan sistemler ***/
//#define ENABLE_COUNTER_FAKE 						// Sahte aktif oyuncu gösterim sistemi
#endif

#define __EREBUS_DUNGEON__ // Erebus dungeon
#ifdef __EREBUS_DUNGEON__
#define EREBUS_FINAL_BOSS 6192
#define EREBUS_HEALER 6409
#endif
