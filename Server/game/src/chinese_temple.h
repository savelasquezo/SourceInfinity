
namespace ChineseTemple
{
	enum EChineseTemple
	{
		CHINESE_TEMPLE_DEFAULT_MAP_INDEX			= 361,
		CHINESE_TEMPLE_MAIN_NPC_VNUM				= 20438,
		CHINESE_TEMPLE_METIN0_VNUM					= 8019,
		CHINESE_TEMPLE_METIN1_VNUM					= 8026,
		DAILY_ZODIAC_POINT							= 12,
		DAILY_ZODIAC_POINT_WAIT_TIME				= 86400,
		START_ZODIAC_POINT							= 36,
		AWAKENING_PRISM								= 33025,
	};
	
	class CDungeonMgr : public singleton<CDungeonMgr>
	{
		public:
			void	Initialize();
			void	Destroy();
			void	Prepare();
			void	Restart();
		protected:
		private:
			LPCHARACTER	pkMetin0, pkMetin1;
	};
}