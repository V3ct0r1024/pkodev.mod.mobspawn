#pragma once
#include "address.h"
#include "structure.h"

namespace pkodev
{
	namespace pointer
	{

		// bool CGameConfig::Load(char *pszFileName)
		typedef bool(__thiscall* CGameConfig__Load__Ptr)(void*, const char*);
		CGameConfig__Load__Ptr CGameConfig__Load = (CGameConfig__Load__Ptr)(void*)(address::MOD_EXE_VERSION::CGameConfig__Load);

		// void COutMapCha::Run(unsigned long ulCurTick)
		typedef void(__thiscall* COutMapCha__Run__Ptr)(void*, unsigned int);
		COutMapCha__Run__Ptr COutMapCha__Run = (COutMapCha__Run__Ptr)(void*)(address::MOD_EXE_VERSION::COutMapCha__Run);

		// void COutMapCha::ExecTimeCha(SMgrUnit *pChaInfo)
		typedef void(__thiscall* COutMapCha__ExecTimeCha__Ptr)(void*, void*);
		COutMapCha__ExecTimeCha__Ptr COutMapCha__ExecTimeCha = (COutMapCha__ExecTimeCha__Ptr)(void*)(address::MOD_EXE_VERSION::COutMapCha__ExecTimeCha);

		// CCharacter* SubMap::ChaSpawn(Long lChaInfoID, Char chCtrlType, Short sAngle, Point *pSPos, bool bEyeshotAbility, dbc::cChar *cszChaName, const long clSearchRadius)
		typedef character* (__thiscall* SubMap__ChaSpawn__Ptr)(void*, unsigned int, char, short int, point*, bool, const char*, const int);
		SubMap__ChaSpawn__Ptr SubMap__ChaSpawn = (SubMap__ChaSpawn__Ptr)(void*)(address::MOD_EXE_VERSION::SubMap__ChaSpawn);
		
		// const char*  SubMap::GetName(void)
		typedef const char* (__thiscall* SubMap__GetName__Ptr)(void*);
		SubMap__GetName__Ptr SubMap__GetName = (SubMap__GetName__Ptr)(void*)(address::MOD_EXE_VERSION::SubMap__GetName);
	}
}