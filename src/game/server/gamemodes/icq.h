/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_ICQ_H
#define GAME_SERVER_GAMEMODES_ICQ_H
#include "cq.h"


class CGameControllerICQ : public CGameControllerCQ
{
public:
	CGameControllerICQ(class CGameContext *pGameServer);
	virtual bool OnEntity(int Index, vec2 Pos);
	virtual void OnCharacterSpawn(class CCharacter *pChr);

};
#endif
