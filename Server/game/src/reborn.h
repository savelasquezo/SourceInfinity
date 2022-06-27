#ifndef __INC_METIN_II_GAME_REBORN_SYSTEM_H__
#define __INC_METIN_II_GAME_REBORN_SYSTEM_H__
#pragma once

class RebornManager : public singleton<RebornManager>
{
	public:
		RebornManager();
		~RebornManager();
	
	void	ResetCharacter(LPCHARACTER ch);	
	void	SetLevel(LPCHARACTER ch);
	void	SetAffect(LPCHARACTER ch, const char* valueAffect);	
	void	SetAnonymous(LPCHARACTER ch, const char* valueAnonymous);
	void	SendLogs(LPCHARACTER ch, const char* nameLog);	
};

#endif