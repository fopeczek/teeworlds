/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_PROJECTILE_H
#define GAME_SERVER_ENTITIES_PROJECTILE_H

#include <game/server/entity.h>

enum
{
	PLAYER_TEAM_BLUE = -2,
	PLAYER_TEAM_RED = -1
};

class CProjectile : public CEntity
{
public:
    CProjectile(CGameWorld *pGameWorld, int Type, int Owner, vec2 Pos, vec2 Dir, int Span,
                int Damage, bool Explosive, float Force, int SoundImpact, int Weapon, int MapID);

	vec2 GetPos(float Time);
	void FillInfo(CNetObj_Projectile *pProj);

    int GetOwner() const { return m_Owner; }
    int GetOwnerTeam() const { return m_OwnerTeam; }
    int GetWeapon() const { return m_Weapon; }
    bool GetExposive() const { return m_Explosive; }
    int GetStartTick() const { return m_StartTick; }
	void LoseOwner();

	virtual void Reset();
	virtual void Tick();
	virtual void TickPaused();
	virtual void Snap(int SnappingClient);

    bool Wall_Coll = false;

private:
	vec2 m_Direction;
	int m_LifeSpan;
	int m_Owner;
	int m_OwnerTeam;
	int m_Type;
	int m_Damage;
	int m_SoundImpact;
	int m_Weapon;
	float m_Force;
	int m_StartTick;
	bool m_Explosive;
};

#endif
