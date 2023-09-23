#pragma once
#include "GameObject.h"
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
		
private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

// Get, Set 함수 만들기 ---------------------------------------------------------------
public: 
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

// 상태머신 셋팅 --------------------------------------------------
private:
	_bool		Monster_Capture();  // 몬스터 시야각내에 플레이어가 있는지 체크
	_float		m_fDistance();		// 몬스터와 플레이어 사이의 거리 체크하는 함수 
	_float		m_fCheck = 0;		//Taunt 등 프레임 돌리는횟수 지정

	_int		m_iHP;				// 몬스터 hp 
	_int		m_iAttack = 15;		// 몬스터 공격력

	_float		m_fFrame = 0.f;		// 이미지 돌리기위한 프레임변수 
	_float		m_fFrameEnd;		// 이미지마다 변수 넣어줘야함 
	_float		m_fFrameSpeed;		// 프레임 돌리는 속도

	_float		m_fAwareness = 0;	// 의심게이지 숫자 
	_float		m_fMaxAwareness = 10.f; // 의심게이지 max -> 추격으로 변함 

	// 속도조절 
	_float		m_fRunSpeed = 1.5f; // 뛰어오는 속도
	_float		m_fWalkSpeed = 0.7f;  // 걷는속도
	_float		m_fInchSpeed = 3.f;  // 앞으로 전진하며 무빙하는 속도 
	_float		m_fStrafingSpeed = 5.f;  // 옆으로 무빙하는  속도 
	_float		m_fBasicAttackSpeed = 3.f;  // 일반공격때 뛰어오는 속도 
	_float		m_fHeavAttackSpeed = 4.f;  // 강공격때 뛰어오는 속도 

	// 사거리 , 시야각
	_float		m_fMonsterFov = 90;		//시야각 - 반각 기준
	_float		m_fMonsterSightDistance = 12.f; // 몬스터가 포착하는 사거리 
	_float		m_fRunDistance = 8.f;
	_float		m_fWalkDistance = 7.5f;
	_float		m_fInchDistance = 3.f;

	// 위치 조절 
	_vec3	vPlayerPos, vDir;

	//스위치 on/off 
public: 
	// 목표 상태머신(AI)
	enum class STATE_OBJ { IDLE, SUSPICIOUS, TAUNT, CHASE, RUN, WALK, INCHFORWARD, STRAFING, BASICATTACK, HEAVYATTACK };
	// 행동 상태머신
	enum class STATE_ACT { IDLE, APPROACH , MOVING};
	// 행동키
	enum class ACTION_KEY { IDLE, RUN, WALK, INCHFORWARD, STRAFING, JUMP, BASIC_ATTACK, HEAVY_ATTACK,  };

private:
	STATE_SET<STATE_OBJ, void(CBrown*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBrown*, float)> m_tState_Act; // 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey; //가상 조작키 

#pragma region AI 

	void AI_Idle(float fDeltaTime); // idle <-> suspicious
	void AI_Suspicious(float fDeltaTime); // idle <-> sus <-> detect
	void AI_Taunt(float fDeltaTime);
	void AI_Chase(float fDeltaTime);

	void AI_Run(float fDeltaTime);
	void AI_Walk(float fDeltaTime);
	void AI_InchForward(float fDeltaTime);
	void AI_Strafing(float fDeltaTime);
	void AI_BasicAttack(float fDeltaTime);
	void AI_HeavyAttack(float fDeltaTime);

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);

	void Moving(float fDeltaTime);
#pragma endregion
	// 액션키는 CPP쪽에 만들음
};

