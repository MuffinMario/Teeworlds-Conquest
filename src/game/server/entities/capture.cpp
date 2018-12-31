
#include <generated/server_data.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/character.h>
#include <game/server/entities/laser.h>
#include <game/server/player.h>
#include <base/math.h>
#include <string>
#include <iostream>
#include "capture.h"

const size_t CCapture::m_sDots = 20;
CCapture::CCapture(CGameWorld *pGameWorld, int Team, vec2 StandPos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_FLAG, StandPos)
{
    m_Team = Team;
    m_StandPos = StandPos;

    GameServer()->m_World.InsertEntity(this);

    Init();
}
void CCapture::UpdateCapturePointOwnage()
{
    int i = 0;
    const double DPD = 360.0 / m_sDots; // DEGREE PER DOT
    double MaxPercentPerDot = 1.0/m_sDots;
    double Percent = m_BelongingPercent[0] + MaxPercentPerDot;
    double CaptureRadius = GameServer()->Tuning()->m_CaptureRadius;
    /* Red capture poitns */
    for(auto& pLaserDot : m_aDotsRed)
    {
        Percent -= MaxPercentPerDot;
        double DotPercent =  clamp(min(Percent,MaxPercentPerDot),0.0,1.0);
        double Degree = m_RotateCircleDegree + DPD*i/180.0*M_PI;
        double DegreeFrom = Degree + (360.0*DotPercent)/180.0*M_PI;
        vec2 NewLaserPos(m_StandPos);
        vec2 NewLaserFromPos(m_StandPos);

        NewLaserPos += vec2(CaptureRadius*std::sin(Degree),CaptureRadius*std::cos(Degree));
        NewLaserFromPos += vec2(CaptureRadius*std::sin(DegreeFrom),CaptureRadius*std::cos(DegreeFrom));

        pLaserDot->SetPos(NewLaserPos);
        pLaserDot->UpdateFromPos(NewLaserFromPos);
        ++i;
    }

    /* Blue capture points */
    i = 0;
    Percent = m_BelongingPercent[1] + MaxPercentPerDot;
    CaptureRadius = GameServer()->Tuning()->m_CaptureRadius + 20; // 20 units more radius for team blue
    for(auto& pLaserDot : m_aDotsBlue)
    {
        Percent -= MaxPercentPerDot;
        double DotPercent =  clamp(min(Percent,MaxPercentPerDot),0.0,1.0);
        double Degree = m_RotateCircleDegree + DPD*i/180.0*M_PI;
        double DegreeFrom = Degree + (360.0*DotPercent)/180.0*M_PI;
        vec2 NewLaserPos(m_StandPos);
        vec2 NewLaserFromPos(m_StandPos);

        NewLaserPos += vec2(CaptureRadius*std::sin(Degree),CaptureRadius*std::cos(Degree));
        NewLaserFromPos += vec2(CaptureRadius*std::sin(DegreeFrom),CaptureRadius*std::cos(DegreeFrom));

        pLaserDot->SetPos(NewLaserPos);
        pLaserDot->UpdateFromPos(NewLaserFromPos);
        ++i;
    }
}
void CCapture::Init()
{
    Reset();

    const double DPD = 360.0 / m_sDots;
    /* RED */
    double PercentRed = m_BelongingPercent[0];
    double MaxPercentPerDot = 100.0/m_sDots;
    for(size_t i = 0; i < m_sDots;i++)
    {
        PercentRed -= MaxPercentPerDot;
        double DotPercent =  min(PercentRed,MaxPercentPerDot);

        double Degree = m_RotateCircleDegree + DPD*i/180.0*M_PI;
        double DegreeFrom = Degree + 360.0*DotPercent;
        double CaptureRadius = GameServer()->Tuning()->m_CaptureRadius;
        vec2 LaserPos(m_StandPos);
        vec2 LaserFromPos(m_StandPos);

        LaserPos += vec2(CaptureRadius*std::sin(Degree),CaptureRadius*std::cos(Degree));
        LaserFromPos += vec2(CaptureRadius*std::sin(DegreeFrom),CaptureRadius*std::cos(DegreeFrom));

        CLaserDot* LaserDotEnt = new CLaserDot(&GameServer()->m_World,LaserPos);

        LaserDotEnt->UpdateFromPos(LaserFromPos);

        m_aDotsRed.push_back(LaserDotEnt);
    }
    /* BLUE */
    double PercentBlue = m_BelongingPercent[1];
    MaxPercentPerDot = 100.0/m_sDots;
    for(size_t i = 0; i < m_sDots;i++)
    {
        PercentBlue -= MaxPercentPerDot;
        double DotPercent =  min(PercentBlue,MaxPercentPerDot);

        double Degree = m_RotateCircleDegree + DPD*i/180.0*M_PI;
        double DegreeFrom = Degree + 360.0*DotPercent;
        double CaptureRadius = GameServer()->Tuning()->m_CaptureRadius + 20;
        vec2 LaserPos(m_StandPos);
        vec2 LaserFromPos(m_StandPos);

        LaserPos += vec2(CaptureRadius*std::sin(Degree),CaptureRadius*std::cos(Degree));
        LaserFromPos += vec2(CaptureRadius*std::sin(DegreeFrom),CaptureRadius*std::cos(DegreeFrom));

        CLaserDot* LaserDotEnt = new CLaserDot(&GameServer()->m_World,LaserPos);

        LaserDotEnt->UpdateFromPos(LaserFromPos);

        m_aDotsBlue.push_back(LaserDotEnt);
    }
}
void CCapture::Reset()
{
    m_RotateCircleDegree = 0.0;

    m_Team = -1;

    m_PlayersOn[0] = 0;
    m_PlayersOn[1] = 0;

    m_BelongingPercent[0] = 0.0;
    m_BelongingPercent[1] = 0.0;
}
void CCapture::Snap(int SnappingClient)
{
    if(m_Team == TEAM_RED || m_Team == TEAM_BLUE)
    {
        if(NetworkClipped(SnappingClient))
            return;
        CNetObj_Flag *pFlag = (CNetObj_Flag *)Server()->SnapNewItem(NETOBJTYPE_FLAG, GetID(), sizeof(CNetObj_Flag));
        if(!pFlag)
            return;

        pFlag->m_X = (int)m_StandPos.x;
        pFlag->m_Y = (int)m_StandPos.y +
                        8*std::sin(Server()->Tick() / 30.0); //hover animation. 6/10
        pFlag->m_Team = m_Team;
    }

}
void CCapture::TickDefered()
{
    m_RotateCircleDegree += GameServer()->Tuning()->m_CaptureRotateSpeed / 180.0 * M_PI;

    UpdateCapturePointOwnage();
}
void CCapture::Tick()
{
    /* Perhaps I can move the logic to the GameMode class, but this does it as well. */
    m_PlayersOn[0] = 0;
    m_PlayersOn[1] = 0;
    bool anybodyOn = false;
    for(int i = 0; i < MAX_CLIENTS;i++)
    {
        CPlayer* pPlayer = GameServer()->m_apPlayers[i];
        if(pPlayer)
        {
            CCharacter* pChar = pPlayer->GetCharacter();
            if(pChar)
            {
                const vec2& PlayerPos = pPlayer->GetCharacter()->GetPos();
                vec2 StandPos = m_StandPos;
                float Dist = std::sqrt((PlayerPos.x - StandPos.x) * (PlayerPos.x - StandPos.x) + (PlayerPos.y - StandPos.y) *(PlayerPos.y - StandPos.y)) ;
                if(Dist < GameServer()->Tuning()->m_CaptureRadius)
                {
                    int Team = pPlayer->GetTeam();
                    m_PlayersOn[Team] = m_PlayersOn[Team] + 1;
                    anybodyOn = true;
                }
            }

        }
    }
    m_PlayersOn[0] = clamp(m_PlayersOn[0],0,3);
    m_PlayersOn[1] = clamp(m_PlayersOn[1],0,3);
    // this could be optimized yes
    if(anybodyOn)
    {
        if(m_PlayersOn[0] == 0 || m_PlayersOn[1] == 0)
        {
            int ContestingTeam = m_PlayersOn[0]==0?1:0;
            if(m_Team != ContestingTeam)
            {
              double CaptureSpeedPerTick = GameServer()->Tuning()->m_CaptureSpeedPerTick/10;
              m_BelongingPercent[ContestingTeam] += CaptureSpeedPerTick*m_PlayersOn[ContestingTeam];
              m_BelongingPercent[~ContestingTeam&1] -= CaptureSpeedPerTick*m_PlayersOn[ContestingTeam];

              /* Clamp to 0.0-1.0 */
              m_BelongingPercent[ContestingTeam] = clamp(m_BelongingPercent[ContestingTeam],0.0,1.0);
              m_BelongingPercent[~ContestingTeam&1] = clamp(m_BelongingPercent[~ContestingTeam&1],0.0,1.0);
              if(m_BelongingPercent[ContestingTeam] == 1 && m_BelongingPercent[~ContestingTeam&1] == 0) // if they are not logically inverse this algorithm would be flawed
              {
                  std::cout << "new flag owner team " << ContestingTeam << std::endl;
                  std::string message = "Team ";
                  m_Team = ContestingTeam;
                  message += m_Team==TEAM_RED?"red":"blue";
                  message += " captured a point";

                  GameServer()->SendChat(-1,CHAT_ALL,-1,message.c_str());
                  //simulate SOUND
                  for(int i = 0; i < MAX_CLIENTS;i++)
                  {
                      CPlayer* pPlayer = GameServer()->m_apPlayers[i];
                      if(pPlayer)
                      {
                          // player & char nessesary or rather only char?
                          CCharacter* cChar = GameServer()->GetPlayerChar(i);
                          if(cChar)
                            GameServer()->CreateSound(cChar->GetPos(),SOUND_CTF_CAPTURE,CmaskOne(pPlayer->GetCID()));
                      }
                  }
              }
              else
              {
                  GameServer()->CreateSound(m_StandPos,SOUND_PLAYER_JUMP,CmaskAll());
              }
              //std::cout << "[" << ContestingTeam << " - " << m_belongingPercent[ContestingTeam] << "] [" << (~ContestingTeam&1 )<< " - " << m_belongingPercent[~ContestingTeam&1]  << "]"<< std::endl;
            }
            else
            {
                int EnemyTeam = ~m_Team&1;
                if(m_BelongingPercent[m_Team] != 1.0 || m_BelongingPercent[EnemyTeam] != 0.0)
                {
                    double CaptureSpeed = GameServer()->Tuning()->m_CaptureSpeedPerTick/10 * m_PlayersOn[ContestingTeam];
                    int EnemyTeam = ~m_Team&1;
                    GameServer()->CreateSound(m_StandPos,SOUND_PLAYER_JUMP,CmaskAll());
                    if(m_BelongingPercent[m_Team] != 1.0)
                        m_BelongingPercent[m_Team] = clamp(m_BelongingPercent[m_Team] + CaptureSpeed,0.0,1.0);
                    if(m_BelongingPercent[EnemyTeam ] != 0.0)
                        m_BelongingPercent[EnemyTeam ] = clamp(m_BelongingPercent[EnemyTeam ] - CaptureSpeed,0.0,1.0);
                }
            }
        }
        else
        {
            /* Both teams on */
            if(Server()->Tick() % 15 == 0)
                GameServer()->CreateSound(m_StandPos,SOUND_PLAYER_JUMP,CmaskAll());

        }
    }
    else
    {
        if(m_Team == TEAM_RED)
        {
            if(m_BelongingPercent[TEAM_RED] != 1.0 && m_BelongingPercent[TEAM_BLUE] != 0.0)
            {
                GameServer()->CreateSound(m_StandPos,SOUND_PLAYER_JUMP,CmaskAll());
                if(m_BelongingPercent[TEAM_RED] != 1.0)
                    m_BelongingPercent[TEAM_RED] = clamp(m_BelongingPercent[TEAM_RED] + GameServer()->Tuning()->m_CaptureSpeedPerTick/75.0,0.0,1.0);
                if(m_BelongingPercent[TEAM_BLUE] != 0.0)
                    m_BelongingPercent[TEAM_BLUE] = clamp(m_BelongingPercent[TEAM_BLUE] - GameServer()->Tuning()->m_CaptureSpeedPerTick/75.0,0.0,1.0);
            }
        }
        else if(m_Team == TEAM_BLUE)
        {
            if(m_BelongingPercent[TEAM_BLUE] != 1.0 && m_BelongingPercent[TEAM_RED] != 0.0)
            {
                GameServer()->CreateSound(m_StandPos,SOUND_PLAYER_JUMP,CmaskAll());
            if(m_BelongingPercent[TEAM_RED] != 0.0)
                m_BelongingPercent[TEAM_RED] = clamp(m_BelongingPercent[TEAM_RED] - GameServer()->Tuning()->m_CaptureSpeedPerTick/75.0,0.0,1.0);
            if(m_BelongingPercent[TEAM_BLUE] != 1.0)
                m_BelongingPercent[TEAM_BLUE] = clamp(m_BelongingPercent[TEAM_BLUE] + GameServer()->Tuning()->m_CaptureSpeedPerTick/75.0,0.0,1.0);
            }
        }
    }
}
