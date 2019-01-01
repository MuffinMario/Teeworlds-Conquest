#ifndef GAME_SERVER_ENTITIES_CAPTURE_H
#define GAME_SERVER_ENTITIES_CAPTURE_H

#include <game/server/entity.h>
#include <vector>
#include "laserdot.h"
/* Capture point for CQ */
class CCapture : public CEntity
{
private:
	int m_Team;
	vec2 m_StandPos;
	/* State */
	int m_PlayersOn[2];
	/* How much % does team X have. Each teams belonging percent is the complement on 1 to the others (so: for all x,y in double : x+y = 1) */
	double m_BelongingPercent[2];
	/* Current Degree of the Circle around the CP */
	double m_RotateCircleDegree = 0.0;
	size_t m_sDots = 10;
	/* Circle */
	std::vector<CLaserDot*> m_aDotsRed;
	std::vector<CLaserDot*> m_aDotsBlue;
    /* Contester */
    //TODO: CLaserDot* m_aContestingPoints[2][3];
    int m_CaptureRadius;

	unsigned m_TickLeftPoint;

public:

	/* Constructor */
	CCapture(CGameWorld *pGameWorld, int Team, vec2 StandPos,int CaptureRadius);
	CCapture(CGameWorld *pGameWorld, int Team, vec2 StandPos);

	/* Initiate the Dots */
    void Init();
	/* NULL-ify */
	void Reset();
	/* Getters */
	int GetTeam() const				{ return m_Team; }
	int GetPlayersOnPointTeam(int Team) { return m_PlayersOn[Team]; }
    /* Setters */
    void SetTeam(int Team) { m_Team = Team; }

    void UpdateCapturePointOwnage();
	virtual void Tick();
	virtual void TickDefered();
	virtual void Snap(int SnappingClient);
};

#endif
