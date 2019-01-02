/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>

#include <generated/protocol.h>
#include <game/version.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/character.h>
#include <game/server/player.h>
#include "cq.h"
#include <sstream>

bool between(int left, int min,int max)
{
    return left >= min && left <= max;
}
#define HACKERMODE 0
/*****************/
/*****************/
/*****************/
/*****************/
const constexpr std::array<int,6> CGameControllerCQ::m_sRadiiEntities;
CGameControllerCQ::CGameControllerCQ(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
    #if HACKERMODE == 1
    system("color 0a");
    #endif // HACKERMODE
	m_pGameType = "CQ";
    m_GameFlags = GAMEFLAG_TEAMS;
}
int CGameControllerCQ::OnCharacterDeath(CCharacter* pVictim,CPlayer* pKiller, int Weapon) {
    IGameController::OnCharacterDeath(pVictim,pKiller,Weapon);
	pVictim->GetPlayer()->m_RespawnTick = max(pVictim->GetPlayer()->m_RespawnTick, Server()->Tick()+Server()->TickSpeed()*g_Config.m_SvRespawnDelayCQ);
	return 0;
}
bool CGameControllerCQ::OnEntityCQ(int Index, vec2 Pos) {

    if(Index == 4 || Index == 5) // FLAGS = CAPTURE POINTS, NO TEAM
    {
        m_aCapturePoints.push_back(new CCapture(&(GameServer()->m_World),-1,Pos));
        return true;
    }
    else if(between(Index,17,17+CGameControllerCQ::m_sRadiiEntities.size()-1)) // 17 = first "row 2" item
    {
        int Radius = CGameControllerCQ::m_sRadiiEntities.at(Index-17);
        m_aCapturePoints.push_back(new CCapture(&(GameServer()->m_World),-1,Pos,Radius));
        return true;
    }
    return false;
}
bool CGameControllerCQ::OnEntity(int Index, vec2 Pos) {
    bool IOnEntity = IGameController::OnEntity(Index,Pos);
    if(!IOnEntity)
    {
        OnEntityCQ(Index,Pos);
    }
    return IOnEntity;
}
bool CGameControllerCQ::OnChatCommand(int ClientID,const char* pCommand, const char* pParam)
{
    if(strcasecmp(pCommand,"about") == 0)
    {
        std::stringstream ss;
        ss << "ConQuest mod by MuffinMario. Version " << CQ_VERSION << " (Compiled " << __DATE__<< " for " << GAME_VERSION << ")";
        GameServer()->SendChat(-1,CHAT_COMMAND,ClientID,ss.str().c_str());
    }
    else if(strcasecmp(pCommand,"cmdlist") == 0 || strcasecmp(pCommand,"help") == 0)
    {
        GameServer()->SendChat(-1,CHAT_COMMAND,ClientID,"**** CMD LIST *****");
        GameServer()->SendChat(-1,CHAT_COMMAND,ClientID,"/about - About the mod");
    }
    else
    {
        GameServer()->SendChat(-1,CHAT_COMMAND,ClientID,std::string("Unknown Command /").operator+=(pCommand).append(". Use /cmdlist for all commands.").c_str());
    }
    return true;
}

void CGameControllerCQ::Reset()
{
    m_aCapturePoints.clear();
}
void CGameControllerCQ::Tick()
{
	IGameController::Tick();
    /* Account a teams points only when the game is running. Once per second (60 ticks == 1 sec i assume) */
	if(Server()->Tick()%60 == 0 &&
        IsGameRunning())
    {
        for(auto& cps : m_aCapturePoints)
        {
            int Team = cps->GetTeam();

            /* Handle points awarding */
            if(Team != -1)
            {
                m_aTeamscore[Team]++;
            }
        }
    }
    if(Server()->Tick()%(60*60*4) == 0)
	{
	    // TODO: 4 minute rotation of text? no idea if this is useful.
	}
}
