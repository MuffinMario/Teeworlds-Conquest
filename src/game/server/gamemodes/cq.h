/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_MOD_H
#define GAME_SERVER_GAMEMODES_MOD_H
#include <game/server/gamecontroller.h>
#include <../../../base/vmath.h>
#include <game/server/entities/capture.h>
#include <vector>
#include <array>

#define CQ_VERSION "0.2.0"

// you can subclass GAMECONTROLLER_CTF, GAMECONTROLLER_TDM etc if you want
// todo a modification with their base as well.
class CGameControllerCQ : public IGameController
{
    std::vector<CCapture*> m_aCapturePoints;
public:
    static const constexpr std::array<int,6> m_sRadiiEntities = {
        100,150,200,250,350,450
    };

	CGameControllerCQ(class CGameContext *pGameServer);
	virtual void Tick();
	virtual bool OnChatCommand(int ClientID,const char* pCommand,const char* pParam);
	virtual void Reset();
	virtual bool OnEntity(int Index, vec2 Pos);
	// add more virtual functions here if you wish
};
#endif
