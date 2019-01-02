/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_GCQ_H
#define GAME_SERVER_GAMEMODES_GCQ_H
#include "cq.h"
#include <../../../base/vmath.h>
#include <game/server/entities/capture.h>
#include <vector>
#include <array>


class CGameControllerGCQ : public CGameControllerCQ
{
public:
	CGameControllerGCQ(class CGameContext *pGameServer);
	virtual bool OnEntity(int Index, vec2 Pos);
	virtual void OnCharacterSpawn(class CCharacter *pChr);
};
#endif
