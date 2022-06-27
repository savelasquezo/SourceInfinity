#include <iostream>
#include <map>

class CRonark : public singleton<CRonark>
{
	public:
		CRonark();
		virtual ~CRonark();
		void		RonarkSet(LPCHARACTER pkAttacker, DWORD dam);
		void		RonarkClear(LPCHARACTER pkKiller);
};