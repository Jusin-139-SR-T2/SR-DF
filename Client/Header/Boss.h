#pragma once
#include "AceMonster.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

#include "MonsterPunch.h"

#include "Awareness.h"
#include "RedLaser.h"
#include "FallingStone.h"
#include "SlowThunder.h"
#include "EnergyBall.h"
#include "SpawnFire.h"
#include "BlueBuff.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CBoss : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CBoss)

private:
	explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBoss(const CBoss& rhs);
	virtual ~CBoss();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	void				Billboard(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 
	virtual void		Free();

	// Get, Set 함수 만들기 ---------------------------------------------------------------
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	
	GETSET_EX2(_float, m_fBossAwareness, BossAwareness, GET, SET)   
	GETSET_EX2(GAUGE<_float>, m_gHp, BossHP, GET, SET)   // 보스  hp용도 
		// 충돌 -----------------------------------------------------------------
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE			FCollisionBox* pShape;

	// 상태머신 셋팅 --------------------------------------------------
private:
	// 함수 ----------
	HRESULT     Get_PlayerPos(const _float& fTimeDelta); // 플레이어 dynamic_cast용도 	
	_bool		Detect_Player();					// 몬스터 시야각내에 플레이어가 있는지 체크
	_float		Calc_Distance();					// 몬스터와 플레이어 사이의 거리 체크하는 함수 
	void		Phase_Check();

	// 스킬패턴
	void Falling_Stone_Around(); // 랜덤위치에 만들어짐 
	void Pattern_Fire(_int number, _float radius); // radius 반지름의 원에서 number만큼 주위를 둘러싸고 만들어짐 
	void Pattern_EnergyBall(); // 보스앞에서 일렬로 만들어짐 - 유도 
	void Pattern_SlowThunder();

	// 변수 ----------
	wchar_t		debugString[100];	
	_float		m_fCheck = 0;						//Taunt 등 프레임 돌리는횟수 지정
	_float		m_iDazedHP = 25;					// 몬스터 기절하는 hp
	_float		phase1HP = 80;
	_float		phase2HP = 50;
	_int		m_iBulletCnt = 5;					// 보스몬스터 총알 갯수 

	_float		m_fFrame = 0.f;						// 이미지 돌리기위한 프레임변수 
	_float		m_fFrameEnd;						// 이미지마다 변수 넣어줘야함 
	_float		m_fFrameSpeed;						// 프레임 돌리는 속도

	// 몬스터 인식 관련 
	_float		m_fBossAwareness = 0;					// 의심게이지 숫자 
	_float		m_fMaxAwareness = 15.f;				// 의심게이지 max -> 추격으로 변함 
	_float		m_fConsider = 10.f;					// 플레이어 놓친뒤에 주변정찰 게이지 
	_float		m_fMaxConsider = 10.f;				// 플레이어 놓친뒤에 주변정찰 게이지 

	// 속도조절 
	_float		m_fRunSpeed = 3.0f;					// 뛰어오는 속도
	_float		m_fWalkSpeed = 2.0f;				// 걷는속도
	_float		m_fRollingSpeed = 3.5f;				// 옆으로 무빙하는  속도 

	// 시간조절용 
	_float		m_MaxTime = 12.f; 
	_float		m_ShortTime = 4.f;
	_float		m_ChaseTime = 0.f;
	_float		m_PreAttackTime = 0.f;
	_float		m_SideAttackTime = 0.f;

	_float		m_fPatternAge = 0.f;
	_float		m_fPatternLifeTime = 3.f;

	// 사거리 , 시야각
	_float		m_fMonsterFov = 90;					//시야각 - 반각 기준
	_float		m_fMonsterSightDistance = 18.f;		// 몬스터가 포착하는 사거리 
	
	//기본phase용 
	_float		m_fBasicRunDistance = 9.f;
	_float		m_fPH1basicDistance = 12.f;
	_float		m_fChaseDistance = 15.f; 

	_float		m_fRunDistance = 8.f;				// 사거리 ~ Run 사이 =  13~8 사이에 위치 
	_float		m_fWalkDistance = 7.f;				// run~walk 사이 = walk
	_float		m_fCloseAttackDistance = 2.f;		// Close ~ Eyes = 

	// 위치 조절 
	_vec3		vPlayerPos;							// 플레이어 위치 벡터
	_vec3		vDir;								// 몬스터가 플레이어 바라보는 벡터  
	_vec3		vPatrolPointZero;

	// bool on/off 
	_bool m_bGoHome = FALSE;
	_bool m_bDead = FALSE;
	_bool m_bArrive = FALSE;
	_bool m_bCheck = FALSE;
	_bool TimerReset = FALSE;
	_bool m_bPH1_RedLaser = TRUE;
	_bool m_bPH2_Buff = TRUE;
	_bool m_bPH2_Route = FALSE;

	// enum 
	MonsterPhase m_eCurrPhase;

public:
	enum class STATE_OBJ { 
	IDLE, SUSPICIOUS, RELOADING, BACKIDLE,
	CHASE, KEEP_DISTANCE, PATTERNCHECK,
	PRE_ATTACK, SIDE_READY,
	WALK, RUN, ROLL, CLOSEATTACK, SHOOTING,
	RECONNAISSANCE, GOHOME, TRACK, 
	
	//패턴체크 
	PH1_LASER
	};

	// 행동 상태머신
	enum class STATE_ACT { 
		IDLE, 
		APPROACHING, RUN,KEEPDISTANCE,
		ROLLING, 
		CLOSEATTACKING, SHOOTING, 
		GOHOME, TRACK,

		//패턴
		PHASE1_LASER	};

	// 행동키
	enum class ACTION_KEY { 
		IDLE, 
		WALK, RUN, KEEPDISTANCE,
		ROLL, TRACK,
		CLOSE_ATTACK, SHOOTING, 
		GOHOME,      
		//패턴
		NORMAL_LASER 
	};

private:
	STATE_SET<STATE_OBJ, void(CBoss*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBoss*, float)> m_tState_Act; // 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey; //가상 조작키 

#pragma region AI 

	void AI_Idle(float fDeltaTime); 
	void AI_Suspicious(float fDeltaTime);
	void AI_Reloading(float fDeltaTime); 
	void AI_BackIdle(float fDeltaTime); //품안에 총 갈무리 

	void AI_Chase(float fDeltaTime);
	void AI_KeepDistance(float fDeltaTime);
	void AI_Walk(float fDeltaTime);
	void AI_Run(float fDeltaTime);

	void AI_Pre_Attack(float fDeltaTime);
	void AI_Side_Ready(float fDeltaTime);
	void AI_Roll(float fDeltaTime);
	void AI_CloseAttack(float fDeltaTime);
	void AI_Shooting(float fDeltaTime);

	void AI_GoHome(float fDeltaTime);
	void AI_Reconnaissance(float fDeltaTime);
	void AI_Track(float fDeltaTime);

	//패턴쪽 AI 
	void AI_Ph1_Laser(float fDeltaTime);

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);
	void Approaching(float fDeltaTime); //  walk 
	void Run(float fDeltaTime); 
	void Rolling(float fDeltaTime); 
	void GoHome(float fDeltaTime); 
	void KeepDistance(float fDeltaTime); 
	void Phase1_LaserOn(float fDeltaTime);
	void Track(float fDeltaTime);

#pragma endregion

	void Get_RandomVector(_vec3* out, _vec3* min, _vec3* max)
	{
		out->x = Get_RandomFloat(min->x, max->x);
		out->y = Get_RandomFloat(min->y, max->y);
		out->z = Get_RandomFloat(min->z, max->z);
	}

	_float Get_RandomFloat(_float lowBound, _float hightBound)
	{
		if (lowBound >= hightBound)
			return lowBound;

		_float f = (rand() % 10000) * 0.0001f;

		return (f * (hightBound - lowBound)) + lowBound;
	}
	void GetRandomPointInCircle(_vec3* out, _vec3* center, float radius)
	{
		// 반지름 내에서 무작위 각도 및 반지름을 생성

		_float angle = static_cast<_float>(rand()) / RAND_MAX * 2 * D3DX_PI;
		_float r = static_cast<_float>(rand()) / RAND_MAX * radius;

		// 극좌표를 직교 좌표로 변환
		out->x = center->x + r * cosf(angle);
		out->y = center->y;
		out->z = center->z + r * sinf(angle);
	}


};

//기술설명

/*
RedLaser = 플레이어 점프 넣어주세요 피해야함 

1 phase 
- GetOuttaHere 사운드 재생 
- 보스가 구르면서 총만쏨 

2 phase 
- FallingStone : 일정시간간격으로 계속해서 소환
- SlowThunder : 플레이어 반경 2Radius 내에서 무작위 위치에 소환되는 Thunder	/ 단순히 소환되고 프레임다돌면 set_dead인데 닿으면 매우아픔
- Fire : 2초간 플레이어를 따라다니지만 이후에는 이동방향으로 계속 직진 수명은 약 4초 벽에닿아도 소멸

3 phase 
- YourLate 사운드 재생
-> 보스 자버프 획득 공이속 증가 / 총알 회복 
- 패턴은 유도 패턴 + 일반패턴 이 합쳐져서 나옴
- EnergyBall(유도) + RedLaser(일반)
- SlowThunder(일반) + Fire(반쯤 유도)
- Stone(일반) + Laser(일반) + Fire(반쯤 유도)
- 전멸기 : Holy랑 충돌하고있어야 살수있음 30초 쿨타임 

*/