#pragma once
#include "GameObject.h"
#include "Player.h"

#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

class CBrown : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CBrown)

private:
	explicit CBrown(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBrown(const CBrown& rhs);
	virtual ~CBrown();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CBrown*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CTransformComponent*	m_pPlayerTransformcomp = nullptr;
	CCalculatorComponent*	m_pCalculatorComp = nullptr;
	CPlayer*				m_pPlayer = nullptr;

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

	// Get, Set 함수 만들기 --------------------------------------------------
public: 
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(CPlayer*, m_pPlayer, Player, GET, SET)

	// 상태머신 셋팅 ---------------------------------------------------------
private:
	// 함수 -----------------------------------------------------------------
	_bool		Detect_Player();					// 몬스터 시야각내에 플레이어가 있는지 체크
	_float		m_fDistance();						// 몬스터와 플레이어 사이의 거리 체크하는 함수 
	void		FaceTurn(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 
	HRESULT     Get_PlayerPos(const _float& fTimeDelta); // 플레이어 dynamic_cast용도 
	//TestBoard -------------------------------------------------------------
	void		Monster_Jump(const _float& fTimeDelta); // 
	_bool		m_bJump = false;

	// 변수 -----------------------------------------------------------------
	_float		m_fCheck = 0;						//Taunt 등 프레임 돌리는횟수 지정
	_int		m_iHP;								// 몬스터 현재 hp 
	_int		m_iPreHP;							// 이전 HP 저장용도 
	_int		m_iDazedHP = 25;					// 몬스터 기절하는 hp
	_int		m_iMaxHP = 100;						// 몬스터 최대 hp 
	_int		m_iAttack = 15;						// 몬스터 공격력

	_float		m_fFrame = 0.f;						// 이미지 돌리기위한 프레임변수 
	_float		m_fFrameEnd;						// 이미지마다 변수 넣어줘야함 
	_float		m_fFrameSpeed;						// 프레임 돌리는 속도

	// 몬스터 인식 관련 
	_float		m_fAwareness = 0;					// 의심게이지 숫자 
	_float		m_fMaxAwareness = 10.f;				// 의심게이지 max -> 추격으로 변함 
	_float		m_fConsider = 10.f;					// 플레이어 놓친뒤에 주변정찰 게이지 
	_float		m_fMaxConsider = 10.f;				// 플레이어 놓친뒤에 주변정찰 게이지 

	// 속도조절 
	_float		m_fRunSpeed = 2.0f;					// 뛰어오는 속도
	_float		m_fWalkSpeed = 1.0f;				// 걷는속도
	_float		m_fInchSpeed = 4.f;					// 앞으로 전진하며 무빙하는 속도 
	_float		m_fStrafingSpeed = 8.f;				// 옆으로 무빙하는  속도 
	_float		m_fBasicAttackSpeed = 3.f;			// 일반공격때 뛰어오는 속도 
	_float		m_fHeavAttackSpeed = 4.f;			// 강공격때 뛰어오는 속도 

	// 사거리 , 시야각
	_float		m_fMonsterFov = 90;					//시야각 - 반각 기준
	_float		m_fMonsterSightDistance = 12.f;		// 몬스터가 포착하는 사거리 
	_float		m_fRunDistance = 7.f;				// 사거리 ~ Run 사이 =  run
	_float		m_fWalkDistance = 6.5f;				// run~walk 사이 = walk
	_float		m_fInchDistance = 1.5f;				// inch ~ walk 사이 = inch/strafy & 0 ~inch = attack 

	// 위치 조절 
	_vec3		vPlayerPos;							// 플레이어 위치 벡터
	_vec3		vDir;								// 몬스터가 플레이어 바라보는 벡터  

	//스위치 on/off 
	_bool Dead = false;
	_bool DeadSpin = true;
	_bool m_bGoHome = false;

	// 상태머신 enum --------------------------------------------------
public: 
	// 목표 상태머신(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, TAUNT, CHASE, REST,			// intro 
		RUN, WALK, INCHFORWARD, STRAFING, BASICATTACK, HEAVYATTACK, JUMP,	// 거리재는부분
		HIT, FACEPUNCH, CROTCHHIT,											// 피격판정 
		DAZED, FALLING, CHOPPED, DEATH, HEADLESS,							// 죽을때
		RECONNAISSANCE, GOHOME
	};

	// 행동 상태머신
	enum class STATE_ACT {
		IDLE, APPROACH, MOVING, ATTACK, GOHOME
	};

	// 행동키
	enum class ACTION_KEY { IDLE, RUN, WALK, INCHFORWARD, STRAFING, JUMP, 
		BASIC_ATTACK, HEAVY_ATTACK, GOHOME };

private:
	STATE_SET<STATE_OBJ, void(CBrown*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CBrown*, float)> m_tState_Act;				// 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//가상 조작키 

#pragma region AI 

	void AI_Idle(float fDeltaTime); 
	void AI_Suspicious(float fDeltaTime); 
	void AI_Taunt(float fDeltaTime);
	void AI_Chase(float fDeltaTime);
	void AI_Rest(float fDeltaTime);

	void AI_Run(float fDeltaTime);
	void AI_Walk(float fDeltaTime);
	void AI_InchForward(float fDeltaTime);
	void AI_Strafing(float fDeltaTime);
	void AI_BasicAttack(float fDeltaTime);
	void AI_HeavyAttack(float fDeltaTime);
	void AI_Jump(float fDeltaTime);

	void AI_Hit(float fDeltaTime);
	void AI_FacePunch(float fDeltaTime);
	void AI_HitByPitchedBall(float fDeltaTime);

	void AI_Dazed(float fDeltaTime);
	void AI_Chopped(float fDeltaTime);
	void AI_Headless(float fDeltaTime);
	void AI_Death(float fDeltaTime);

	void AI_Reconnaissance(float fDeltaTime); // 플레이어 놓쳐서 주변 정찰하는중 
	void AI_GoHome(float fDeltaTime);		 // 정찰마치고 원위치 복귀중 

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// AI_InchForward + AI_Strafing
	void Attack(float fDeltaTime);			// AI_BasicAttack + AI_HeavyAttack
	void GoHome(float fDeltaTime);			// Gohome
#pragma endregion
};

