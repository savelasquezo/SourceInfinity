PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)

GCC_VERSION = $(shell $(CC) --version 2>&1 | grep "(GCC)" | cut -d' ' -f3  | cut -d'.' -f1)
BSD_VERSION = $(shell uname -v 2>&1 | cut -d' ' -f2 | cut -d'.' -f1)
SVN_VERSION = 40250
WOLF_ENABLE = lycan

CC = g++49

INCDIR =
LIBDIR =
BINDIR = /usr/game/share/bin
OBJDIR = OBJDIR
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

# Standard Setting
LIBS = -pthread -lm -lmd
# Removed -fno-rtti 
CFLAGS = -g -Wall -O3 -m32 -w -pipe -fexceptions -D_THREAD_SAFE -DNDEBUG -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__GAME_VERSION__=\"$(GAME_VERSION)\"
CFLAGS += -mtune=i686 -fstack-protector-all -std=c++11

# boost
INCDIR += -I../../../Extern/include/boost

# DevIL
INCDIR += -I../../libdevil
LIBDIR += -L../../libdevil
LIBS += -lIL -lpng -ltiff -lmng -llcms -ljpeg

# MySQL
INCDIR += -I../../libmysql/7.x-5.1.35
LIBDIR += -L../../libmysql/7.x-5.1.35

LIBS += -lmysqlclient -L/usr/local/lib/mysql -lz

# Miscellaneous external libraries
INCDIR += -I../../../Extern/include
LIBDIR += -L../../../Extern/lib
LIBS += -lcryptopp -lgtest

# openssl
#INCDIR += -I/usr/include
#LIBS += -lssl
#LIBS += /usr/lib/libssl.a

# Project Library
INCDIR += -I../../liblua/include
INCDIR += -I/usr/local/include
INCDIR += -L/usr/local/lib/mysql
LIBDIR += -L../../libthecore/lib -L../../libpoly -L../../libsql -L../../libgame/lib -L../../liblua/lib
LIBDIR += -L/usr/local/lib
LIBS += -lthecore -lpoly -llua -llualib -lsql -lgame
USE_STACKTRACE = 0
ifeq ($(USE_STACKTRACE), 1)
LIBS += /usr/local/lib/libexecinfo.a
endif

TARGET  = $(BINDIR)/game

CFILE	= minilzo.c

CPPFILE = BattleArena.cpp FSM.cpp MarkConvert.cpp MarkImage.cpp MarkManager.cpp OXEvent.cpp TrafficProfiler.cpp ani.cpp\
		  arena.cpp banword.cpp battle.cpp blend_item.cpp block_country.cpp buffer_manager.cpp building.cpp castle.cpp\
		  char.cpp char_affect.cpp char_battle.cpp char_change_empire.cpp char_horse.cpp char_item.cpp char_manager.cpp\
		  char_quickslot.cpp char_resist.cpp char_skill.cpp char_state.cpp PetSystem.cpp cmd.cpp cmd_emotion.cpp cmd_general.cpp\
		  cmd_gm.cpp cmd_oxevent.cpp config.cpp constants.cpp crc32.cpp cube.cpp db.cpp desc.cpp\
		  desc_client.cpp desc_manager.cpp desc_p2p.cpp dev_log.cpp dungeon.cpp empire_text_convert.cpp entity.cpp\
		  entity_view.cpp event.cpp event_queue.cpp exchange.cpp file_loader.cpp fishing.cpp gm.cpp guild.cpp\
		  guild_manager.cpp guild_war.cpp horse_rider.cpp horsename_manager.cpp input.cpp input_auth.cpp input_db.cpp\
		  input_login.cpp input_main.cpp input_p2p.cpp input_udp.cpp ip_ban.cpp\
		  item.cpp item_addon.cpp item_attribute.cpp item_manager.cpp item_manager_idrange.cpp locale.cpp\
		  locale_service.cpp log.cpp login_data.cpp lzo_manager.cpp marriage.cpp matrix_card.cpp\
		  messenger_manager.cpp mining.cpp mob_manager.cpp monarch.cpp motion.cpp over9refine.cpp p2p.cpp packet_info.cpp\
		  party.cpp pcbang.cpp polymorph.cpp priv_manager.cpp pvp.cpp\
		  questevent.cpp questlua.cpp questlua_affect.cpp questlua_arena.cpp questlua_ba.cpp questlua_building.cpp\
		  questlua_danceevent.cpp questlua_dungeon.cpp questlua_forked.cpp new_switchbot.cpp questlua_game.cpp questlua_global.cpp\
		  questlua_guild.cpp questlua_horse.cpp questlua_pet.cpp questlua_item.cpp questlua_marriage.cpp questlua_mgmt.cpp\
		  questlua_monarch.cpp questlua_npc.cpp questlua_oxevent.cpp questlua_party.cpp questlua_pc.cpp\
		  questlua_quest.cpp questlua_target.cpp questmanager.cpp questnpc.cpp questpc.cpp\
		  refine.cpp TempleOchao.cpp questlua_TempleOchao.cpp regen.cpp safebox.cpp sectree.cpp sectree_manager.cpp sequence.cpp shop.cpp\
		  skill.cpp start_position.cpp target.cpp text_file_loader.cpp trigger.cpp utils.cpp vector.cpp war_map.cpp\
		  wedding.cpp xmas_event.cpp version.cpp panama.cpp threeway_war.cpp map_location.cpp\
		  BlueDragon.cpp BlueDragon_Binder.cpp DragonLair.cpp questlua_dragonlair.cpp\
		  skill_power.cpp affect.cpp SpeedServer.cpp questlua_speedserver.cpp\
		  FileMonitor_FreeBSD.cpp ClientPackageCryptInfo.cpp cipher.cpp\
		  buff_on_attributes.cpp dragon_soul_table.cpp DungeonInfo.cpp drop_item.cpp DragonSoul.cpp\
		  group_text_parse_tree.cpp char_dragonsoul.cpp questlua_dragonsoul.cpp item_combination.cpp\
		  shop_manager.cpp shopEx.cpp item_manager_read_tables.cpp title.cpp bank.cpp New_PetSystem.cpp questlua_petnew.cpp questlua_mysql.cpp char_cards.cpp\
		  ronark_system.cpp char_gaya.cpp new_events.cpp questlua_new_events.cpp MeleyLair.cpp questlua_MeleyLair.cpp\
		  offline_shop.cpp offlineshop_manager.cpp biolog.cpp localization.cpp char_keyboard.cpp\
		  user_control.cpp reborn.cpp combat_zone.cpp AutoNotice.cpp game_events.cpp tournament.cpp


COBJS	= $(CFILE:%.c=$(OBJDIR)/%.o)
CPPOBJS	= $(CPPFILE:%.cpp=$(OBJDIR)/%.o)

MAINOBJ = $(OBJDIR)/main.o
MAINCPP = main.cpp

default: $(TARGET) $(TEST_TARGET)

$(OBJDIR)/minilzo.o: minilzo.c
	@$(CC) -w $(CFLAGS) $(INCDIR) -c $< -o $@
	@echo -e "\033[0;32m [Simon Project] \033[0m       \033[0;33m [Game] Source compile:\033[0m" $<

$(OBJDIR)/version.o: version.cpp
	@$(CC) -w $(CFLAGS) -D__SVN_VERSION__=\"$(SVN_VERSION)\" -c $< -o $@
	@echo -e "\033[0;32m [Simon Project] \033[0m       \033[0;33m [Game] Source compile:\033[0m" $<

$(OBJDIR)/%.o: %.cpp
	@echo -e "\033[0;32m [Simon Project] \033[0m       \033[0;33m [Game] Source compile:\033[0m" $<
	@$(CC) -w $(CFLAGS) $(INCDIR) -c $< -o $@

$(TARGET): $(CPPOBJS) $(COBJS) $(MAINOBJ)
	@echo ----     Ha logrado compilar todo en su totalidad con exito. ----
	@echo ----     Simon Project GAME: $(TARGET).... Generado
	@echo -------------------------------------------------
	@$(CC) -w $(CFLAGS) $(LIBDIR) $(COBJS) $(CPPOBJS) $(MAINOBJ) $(LIBS) -o $(TARGET)
	@touch version.cpp


clean:
	@rm -f $(COBJS) $(CPPOBJS)
	@rm -f $(BINDIR)/game_r* $(BINDIR)/conv

install:
	rm -rf $(INSTALL_DIR)/share/bin/game
	cp $(TARGET) $(INSTALL_DIR)/share/bin/game

tag:
	ctags *.cpp *.h *.c

dep:
	makedepend -f Depend $(INCDIR) -I/usr/include/c++/3.3 -I/usr/include/c++/4.2 -p$(OBJDIR)/ $(CPPFILE) $(CFILE) $(MAINCPP) $(TESTCPP) 2> /dev/null > Depend

sinclude Depend
