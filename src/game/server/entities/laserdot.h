#include "../entity.h"
#include <iostream>
#include "../gamecontext.h"
class CLaserDot : public CEntity
{
    vec2 m_FromPos;
public:
    CLaserDot(CGameWorld* World, vec2 Pos) : CEntity(World,CGameWorld::ENTTYPE_LASER,Pos)
    {
        m_FromPos = vec2(0,0);
        GameServer()->m_World.InsertEntity(this);
    }
    void UpdateFromPos(vec2 FromPos)
    {
        m_FromPos = FromPos;
    }
    virtual void Snap(int SnappingClient)
    {
        if(NetworkClipped(SnappingClient) && NetworkClipped(SnappingClient, m_FromPos))
            return;
        CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, GetID(), sizeof(CNetObj_Laser)));
        if(!pObj)
            return;

        pObj->m_X = (int)m_Pos.x;
        pObj->m_Y = (int)m_Pos.y;
        pObj->m_FromX = (int)m_FromPos.x;
        pObj->m_FromY = (int)m_FromPos.y;
        pObj->m_StartTick = Server()->Tick();
    }
    vec2 GetPos() { return m_Pos; }
    void SetPos(vec2 Pos) { m_Pos = Pos; }
};
