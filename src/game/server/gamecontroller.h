/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMECONTROLLER_H
#define GAME_SERVER_GAMECONTROLLER_H

#include <base/vmath.h>
#include <base/tl/array.h>

#include <game/commands.h>

#include <generated/protocol.h>
#include <vector>

/*
	Class: Game Controller
		Controls the main game logic. Keeping track of team and player score,
		winning conditions and specific game logic.
*/
class IGameController
{
public:

    // game
    enum EGameState
    {
        // internal game states
        IGS_WARMUP_GAME,		// warmup started by game because there're not enough players (infinite)
        IGS_WARMUP_USER,		// warmup started by user action via rcon or new match (infinite or timer)

        IGS_START_COUNTDOWN,	// start countown to unpause the game or start match/round (tick timer)

        IGS_GAME_PAUSED,		// game paused (infinite or tick timer)
        IGS_GAME_RUNNING,		// game running (infinite)

        IGS_END_MATCH,			// match is over (tick timer)
        IGS_END_ROUND,			// round is over (tick timer)
    };
    int m_GameStateTimer;

    EGameState m_GameState;
private:
	class CGameContext *m_pGameServer;
	class CConfig *m_pConfig;
	class IServer *m_pServer;

	// activity
	void DoActivityCheck();
	bool GetPlayersReadyState(int WithoutID = -1);
	void SetPlayersReadyState(bool ReadyState);
	void CheckReadyStates(int WithoutID = -1);

	// balancing
	enum
	{
		TBALANCE_CHECK=-2,
		TBALANCE_OK,
	};
    int m_UnbalancedTick;

	virtual bool CanBeMovedOnBalance(int ClientID) const;
	void CheckTeamBalance();
	void DoTeamBalance();

	virtual bool DoWincheckMatch();		// returns true when the match is over
	virtual void DoWincheckRound() {}
	bool HasEnoughPlayers() const { return (IsTeamplay() && m_aTeamSize[TEAM_RED] > 0 && m_aTeamSize[TEAM_BLUE] > 0) || (!IsTeamplay() && m_aTeamSize[TEAM_RED] > 1); }
	void ResetGame();
	void SetGameState(EGameState GameState, int Timer=0);
	void StartMatch();
	void StartRound();

	// map
	char m_aMapWish[128];

	void CycleMap();

	// spawn
	struct CSpawnEval
	{
		CSpawnEval()
		{
			m_Got = false;
			m_FriendlyTeam = -1;
			m_Pos = vec2(100,100);
		}

		vec2 m_Pos;
		bool m_Got;
		bool m_RandomSpawn;
		int m_FriendlyTeam;
		float m_Score;
	};
    //vec2 m_aaSpawnPoints[3][64];
    enum
    {
        NUM_SPAWN_TYPES = 3,
        NUM_SPAWN_PER_TYPE = 64,
        NUM_SPAWN_WORLD = 3*64,
    };

    struct SpawnData {
        vec2 m_aaSpawnPoints[3][64];
    };

    std::vector<SpawnData> m_vSpawnPoints;

    struct NumSpawnData {
        int m_aNumSpawnPoints[3];
    };
    std::vector<NumSpawnData> m_vNumSpawnPoints;

    float EvaluateSpawnPos(CSpawnEval *pEval, vec2 Pos, int MapID) const;
    void EvaluateSpawnType(CSpawnEval *pEval, int Type, int MapID) const;

	// team
	int ClampTeam(int Team) const;

protected:
	CGameContext *GameServer() const { return m_pGameServer; }
	CConfig *Config() const { return m_pConfig; }
	IServer *Server() const { return m_pServer; }

	// game
	int m_GameStartTick;
	int m_MatchCount;
	int m_RoundCount;
	int m_SuddenDeath;
	int m_aTeamscore[NUM_TEAMS];

	void EndMatch() { SetGameState(IGS_END_MATCH, TIMER_END); }
	void EndRound() { SetGameState(IGS_END_ROUND, TIMER_END/2); }

	// info
	int m_GameFlags;
	const char *m_pGameType;
	struct CGameInfo
	{
		int m_MatchCurrent;
		int m_MatchNum;
		int m_ScoreLimit;
		int m_TimeLimit;
	} m_GameInfo;

	void UpdateGameInfo(int ClientID);

public:

	IGameController(class CGameContext *pGameServer);
	virtual ~IGameController() {}

	// event
	/*
		Function: on_CCharacter_death
			Called when a CCharacter in the world dies.

		Arguments:
			victim - The CCharacter that died.
			killer - The player that killed it.
			weapon - What weapon that killed it. Can be -1 for undefined
				weapon when switching team or player suicides.
	*/
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	/*
		Function: on_CCharacter_spawn
			Called when a CCharacter spawns into the game world.

		Arguments:
			chr - The CCharacter that was spawned.
	*/
	virtual void OnCharacterSpawn(class CCharacter *pChr);

	virtual void OnFlagReturn(class CFlag *pFlag);

	/*
		Function: on_entity
			Called when the map is loaded to process an entity
			in the map.

		Arguments:
			index - Entity index.
			pos - Where the entity is located in the world.

		Returns:
			bool?
	*/
    virtual bool OnEntity(int Index, vec2 Pos, int MapID);

	virtual void OnPlayerConnect(class CPlayer *pPlayer);
	virtual void OnPlayerDisconnect(class CPlayer *pPlayer);
	virtual void OnPlayerInfoChange(class CPlayer *pPlayer);
	virtual void OnPlayerReadyChange(class CPlayer *pPlayer);
	void OnPlayerCommand(class CPlayer *pPlayer, const char *pCommandName, const char *pCommandArgs);

	void OnReset();

    void SetSpawnNum(int MapNum);

	// game
	enum
	{
		TIMER_INFINITE = -1,
		TIMER_END = 10,
	};

	void DoPause(int Seconds) { SetGameState(IGS_GAME_PAUSED, Seconds); }
	void DoWarmup(int Seconds)
	{
		SetGameState(IGS_WARMUP_USER, Seconds);
	}
	void AbortWarmup()
	{
		if((m_GameState == IGS_WARMUP_GAME || m_GameState == IGS_WARMUP_USER)
			&& m_GameStateTimer != TIMER_INFINITE)
		{
			SetGameState(IGS_GAME_RUNNING);
		}
	}
	void SwapTeamscore();

	// general
	virtual void Snap(int SnappingClient);
	virtual void Tick();

	// info
	void CheckGameInfo();
	bool IsFriendlyFire(int ClientID1, int ClientID2) const;
	bool IsFriendlyTeamFire(int Team1, int Team2) const;
	bool IsGamePaused() const { return m_GameState == IGS_GAME_PAUSED || m_GameState == IGS_START_COUNTDOWN; }
	bool IsGameRunning() const { return m_GameState == IGS_GAME_RUNNING; }
	bool IsPlayerReadyMode() const;
	bool IsTeamChangeAllowed() const;
	bool IsTeamplay() const { return m_GameFlags&GAMEFLAG_TEAMS; }
	bool IsSurvival() const { return m_GameFlags&GAMEFLAG_SURVIVAL; }

	const char *GetGameType() const { return m_pGameType; }

	// map
	void ChangeMap(const char *pToMap);

	//spawn
    bool CanSpawn(int Team, vec2 *pPos, int MapID) const;
	bool GetStartRespawnState() const;

	// team
	bool CanJoinTeam(int Team, int NotThisID) const;
	bool CanChangeTeam(CPlayer *pPplayer, int JoinTeam) const;

	void DoTeamChange(class CPlayer *pPlayer, int Team, bool DoChatMsg=true);
	void ForceTeamBalance() { if(!(m_GameFlags&GAMEFLAG_SURVIVAL)) DoTeamBalance(); }

	int GetRealPlayerNum() const { return m_aTeamSize[TEAM_RED]+m_aTeamSize[TEAM_BLUE]; }
	int GetStartTeam();

    static void Com_reset_class(IConsole::IResult *pResult, void *pContext);
	virtual void RegisterChatCommands(CCommandManager *pManager);

    int m_aTeamSize[NUM_TEAMS];
};

#endif
