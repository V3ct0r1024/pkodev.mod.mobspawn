#pragma once
#include "structure.h"

namespace pkodev
{
	namespace hook
	{
		// bool CGameConfig::Load(char *pszFileName)
		bool __fastcall CGameConfig__Load(void* This, void* NotUsed, const char* pszFileName);

		// void COutMapCha::Run(unsigned long ulCurTick)
		void __fastcall COutMapCha__Run(void* This, void* NotUsed, unsigned int tick);

		// void COutMapCha::ExecTimeCha(SMgrUnit *pChaInfo)
		void __fastcall COutMapCha__ExecTimeCha(void* This, void* NotUsed, void* pChaInfo);

		// CCharacter* SubMap::ChaSpawn(Long lChaInfoID, Char chCtrlType, Short sAngle, Point *pSPos, bool bEyeshotAbility, dbc::cChar *cszChaName, const long clSearchRadius)
		character* __fastcall SubMap__ChaSpawn(void* This, void* NotUsed, unsigned int lChaInfoID, char chCtrlType, short int sAngle, point* pSPos, bool bEyeshotAbility, char* cszChaName, const int clSearchRadius);;
	}
}