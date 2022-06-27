MAKE = gmake
CC = g++49

INCDIR =
LIBDIR =
BINDIR = /usr/game/share/bin
OBJDIR = .obj

P4_VERSION = 40153
WOLF_ENABLE = lycan
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

TARGET = $(BINDIR)/db

CFLAGS = -g -Wall -O2 -pipe -fno-rtti -fno-exceptions -pthread -mtune=i686 -std=c++11 -D_THREAD_SAFE -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__DB_VERSION__=\"$(DB_VERSION)\"

# boost
INCDIR += -I../../boost

LIBDIR += -L../../libthecore/lib -L../../libsql -L../../libpoly -L../../libgame/lib -L/usr/local/lib/mysql

ifeq ($(BSD_VERSION), 7)
INCDIR += -I../../libmysql/7.x-5.1.35
LIBDIR += -L../../libmysql/7.x-5.1.35
else
INCDIR += -I../../libmysql/5.x-5.1.35
LIBDIR += -L../../libmysql/5.x-5.1.35
endif

LIBS = -lthecore -lmysqlclient -lsql -lpoly -lgame -lm -lz

SRCS =	Config.cpp NetBase.cpp Peer.cpp PeerBase.cpp Main.cpp Lock.cpp DBManager.cpp \
		Cache.cpp LoginData.cpp ClientManager.cpp ClientManagerPlayer.cpp ClientManagerLogin.cpp \
		ClientManagerBoot.cpp ClientManagerParty.cpp ClientManagerGuild.cpp GuildManager.cpp HB.cpp \
		PrivManager.cpp MoneyLog.cpp ItemAwardManager.cpp ClientManagerEventFlag.cpp Marriage.cpp \
		Monarch.cpp BlockCountry.cpp ItemIDRangeManager.cpp ClientManagerHorseName.cpp version.cpp \
		ProtoReader.cpp CsvReader.cpp

OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

default: $(TARGET)

$(TARGET): $(OBJS)
	@echo ----   Ha logrado compilar todo en su totalidad con exito.	----
	@echo ----   Simon Project DB: $(TARGET).... Generado
	@echo -------------------------------------------------
	@$(CC) -w $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS) -o $(TARGET)
	@touch version.cpp

$(OBJDIR)/%.o: %.cpp
	@echo -e "\033[0;32m [BEST Production] \033[0m       \033[0;33m [DB] Source Derleniyor:\033[0m" $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

$(OBJDIR)/version.o: version.cpp
	@$(CC) $(CFLAGS) -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__P4_VERSION__=\"$(P4_VERSION)\" -c $< -o $@
	@echo -e "\033[0;32m [BEST Production] \033[0m       \033[0;33m [DB] Source Derleniyor:\033[0m" $<

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	@rm -f $(OBJS) $(BINDIR)/db_r*

install:
	rm -rf $(INSTALL_DIR)/share/bin/db
	cp $(TARGET) $(INSTALL_DIR)/share/bin/db

dep:
	@touch Depend
	makedepend -fDepend $(INCDIR) -I/usr/include/c++/3.3 -I/usr/include/c++/4.2 -p$(OBJDIR)/ $(SRCS) 2> /dev/null

sinclude Depend
