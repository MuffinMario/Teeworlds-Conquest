/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/server/entities/pickup.h>
#include <game/server/entities/character.h>
#include "icq.h"

CGameControllerICQ::CGameControllerICQ(class CGameContext *pGameServer)
: CGameControllerCQ(pGameServer)
{

	m_pGameType = "iCQ";
}
void CGameControllerICQ::OnCharacterSpawn(class CCharacter *pChr)
{
    /* ALWAYS */
    pChr->IncreaseHealth(10);
    pChr->GiveWeapon(WEAPON_LASER,-1);
}
bool CGameControllerICQ::OnEntity(int Index,vec2 Pos)
{
    /* We don't want to handle other objects, since all entities are Spawns,CPs */
    bool Success = false;
    if(!(Success = OnEntityCQ(Index,Pos)))
    {
        /* Only add spawns */
        switch(Index)
        {
        case ENTITY_SPAWN:
            m_aaSpawnPoints[0][m_aNumSpawnPoints[0]++] = Pos;
            break;
        case ENTITY_SPAWN_RED:
            m_aaSpawnPoints[1][m_aNumSpawnPoints[1]++] = Pos;
            break;
        case ENTITY_SPAWN_BLUE:
            m_aaSpawnPoints[2][m_aNumSpawnPoints[2]++] = Pos;
            break;
        }

    }
    return Success;
}
