#pragma once
#include "AceMonster.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

#include "MonsterPunch.h"

#include "Awareness.h"
#include "FallingStone.h"

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
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CColliderComponent* m_pColliderComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

	// Get, Set 함수 만들기 ---------------------------------------------------------------
public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

	// 상태머신 셋팅 --------------------------------------------------
private:
	// 함수 ----------
	_bool		Detect_Player();					// 몬스터 시야각내에 플레이어가 있는지 체크
	_float		Calc_Distance();					// 몬스터와 플레이어 사이의 거리 체크하는 함수 
	void		Billboard(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 

	// 변수 ----------
	_float		m_fCheck = 0;						//Taunt 등 프레임 돌리는횟수 지정
	_int		m_iHP;								// 몬스터 현재 hp 
	_int		m_iPreHP;							// 이전 HP 저장용도 
	_int		m_iDazedHP = 25;					// 몬스터 기절하는 hp
	_int		m_iMaxHP = 100;						// 몬스터 최대 hp 
	_int		m_iAttack = 15;						// 몬스터 공격력
	_int		m_iBulletCnt = 5;					// 보스몬스터 총알 갯수 

	_float		m_fFrame = 0.f;						// 이미지 돌리기위한 프레임변수 
	_float		m_fFrameEnd;						// 이미지마다 변수 넣어줘야함 
	_float		m_fFrameSpeed;						// 프레임 돌리는 속도

	// 몬스터 인식 관련 
	_float		m_fBossAwareness = 0;					// 의심게이지 숫자 
	_float		m_fMaxAwareness = 7.f;				// 의심게이지 max -> 추격으로 변함 
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


	// 사거리 , 시야각
	_float		m_fMonsterFov = 90;					//시야각 - 반각 기준
	_float		m_fMonsterSightDistance = 13.f;		// 몬스터가 포착하는 사거리 

	_float		m_fRunDistance = 8.f;				// 사거리 ~ Run 사이 =  13~8 사이에 위치 
	_float		m_fWalkDistance = 7.f;				// run~walk 사이 = walk
	_float		m_fCloseAttackDistance = 2.f;		// Close ~ Eyes = 

	// 위치 조절 
	_vec3		vPlayerPos;							// 플레이어 위치 벡터
	_vec3		vDir;								// 몬스터가 플레이어 바라보는 벡터  
	_vec3		vPatrolPointZero;
	//스위치 on/off 
	_bool m_bGoHome = FALSE;

public:
	// 시야는 좁게, 반응은 느리게, 의심게이지는 빠르게, 

	// 목표 상태머신(AI)
	enum class STATE_OBJ { 
	IDLE, SUSPICIOUS, RELOADING, CHASE, BACKIDLE,
	PRE_ATTACK, SIDE_READY,
	WALK, RUN, ROLL, CLOSEATTACK, SHOOTING,
	GOHOME};

	// 행동 상태머신
	enum class STATE_ACT { IDLE, APPROACHING, ROLLING, CLOSEATTACKING, SHOOTING, 
		GOHOME
	};

	// 행동키
	enum class ACTION_KEY { IDLE, WALK, RUN, ROLL, CLOSE_ATTACK, SHOOTING, 
		GOHOME
	};

private:
	STATE_SET<STATE_OBJ, void(CBoss*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBoss*, float)> m_tState_Act; // 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey; //가상 조작키 

#pragma region AI 

	void AI_Idle(float fDeltaTime); // idle <-> suspicious
	void AI_Suspicious(float fDeltaTime);
	void AI_Reloading(float fDeltaTime); // idle <-> sus <-> detect
	void AI_BackIdle(float fDeltaTime);// ▶작업필요 : 여기 들어가서 안빠져나옴 
	void AI_Chase(float fDeltaTime);

	void AI_Pre_Attack(float fDeltaTime);
	void AI_Side_Ready(float fDeltaTime);
	void AI_Walk(float fDeltaTime);
	void AI_Run(float fDeltaTime);
	void AI_Roll(float fDeltaTime);
	void AI_CloseAttack(float fDeltaTime);
	void AI_Shooting(float fDeltaTime);

	void AI_GoHome(float fDeltaTime);

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);
	void Approaching(float fDeltaTime);
	void Rolling(float fDeltaTime);
	void CloseAttacking(float fDeltaTime);
	void Shooting(float fDeltaTime);
	void GoHome(float fDeltaTime);
	

#pragma endregion
	// 액션키는 CPP쪽에 만들음
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