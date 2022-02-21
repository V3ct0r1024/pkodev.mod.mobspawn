#pragma once

namespace pkodev
{
	namespace address
	{
		// GameServer.exe 1.36
		namespace GAMESERVER_136
		{
			// bool CGameConfig::Load(char *pszFileName)
			const unsigned int CGameConfig__Load = 0x004A8FD0;

			// void COutMapCha::Run(unsigned long ulCurTick)
			const unsigned int COutMapCha__Run = 0x004BABE0;

			// void COutMapCha::ExecTimeCha(SMgrUnit *pChaInfo)
			const unsigned int COutMapCha__ExecTimeCha = 0x004BAF90;

			// CCharacter* SubMap::ChaSpawn(Long lChaInfoID, Char chCtrlType, Short sAngle, Point *pSPos, bool bEyeshotAbility, dbc::cChar *cszChaName, const long clSearchRadius)
			const unsigned int SubMap__ChaSpawn = 0x004B5060;

			// const char* SubMap::GetName(void)
			const unsigned int SubMap__GetName = 0x00416850;
		}

		// GameServer.exe 1.38
		namespace GAMESERVER_138
		{
			// bool CGameConfig::Load(char *pszFileName)
			const unsigned int CGameConfig__Load = 0x004B0200;

			// void COutMapCha::Run(unsigned long ulCurTick)
			const unsigned int COutMapCha__Run = 0x004C2170;

			// void COutMapCha::ExecTimeCha(SMgrUnit *pChaInfo)
			const unsigned int COutMapCha__ExecTimeCha = 0x004C2520;

			// CCharacter* SubMap::ChaSpawn(Long lChaInfoID, Char chCtrlType, Short sAngle, Point *pSPos, bool bEyeshotAbility, dbc::cChar *cszChaName, const long clSearchRadius)
			const unsigned int SubMap__ChaSpawn = 0x004BC5D0;

			// const char* SubMap::GetName(void)
			const unsigned int SubMap__GetName = 0x00417A70;
		}
	}
}