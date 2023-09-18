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

class CMonster : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CMonster)

private:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CTransformComponent*	m_pPlayerTransformcomp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;
	CCalculatorComponent*	m_pCalculatorComp = nullptr;
	CGameObject*			m_pTarget = nullptr;
		
private:
	HRESULT			Add_Component();
	virtual void	Free();

private:
	void				Height_On_Terrain();
	_float				m_fFrame = 0.f; // 이미지 돌리기위한 프레임변수 
	_float				m_fsuspicious;
	
public:
	void				Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	static CMonster*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	_matrix				m_matRotAxis;


//---------------------------------------------------------------

public: //Get, Set 함수 만들기 
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

// 상태머신 셋팅 --------------------------------------------------
private:
	_bool				Monster_Capture(); // 몬스터 시야각내에 플레이어가 있는지 체크
	float				m_fDistance(); // 몬스터와 플레이어 사이의 거리 체크하는 함수 
	void				Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed);

public:
	// info 설정 (체, 공, 방x)
	_int m_iHP; // 몬스터 hp 
	_int m_iAttack = 15; // 몬스터 공격력
	
	_float m_fAwareness = 0; // 의심게이지 숫자 
	_float m_fMaxAwareness = 15.f; // 의심게이지 max -> 추격으로 변함 
	
	_float m_fMoveSpeed = 10.f;
	_int m_iFrameEnd;
	_float m_fWalkSpeed = 3.f;
	_bool SpinCheck = true;
	_float SpinAngle = 0.f;
	_float m_fMonsterFov = 90; //반각 기준 90도 
	_float m_fMonsterSightDistance = 7.f;


public: 
	// 목표 상태머신(AI)
	enum class STATE_OBJ { IDLE, SUSPICIOUS, CHASE, ATTACK, JUMP };
	// 행동 상태머신
	enum class STATE_ACT { IDLE, WALK, RUN, INCH, JUMP, 
						  PRE_ATTACK, ATTACK, HEAVY, PARRYING, FALLING, LANDING, DEAD };
	// 행동키
	enum class ACTION_KEY { RUN, WALK, JUMP, BASIC_ATTACK, HEAVY_ATTACK, INCH, STRAFING  };

private:
	STATE_SET<STATE_OBJ, void(CMonster*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CMonster*, float)> m_tState_Act; // 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey; //가상 조작키 

#pragma region AI : 판단하는애 

	void AI_Idle(float fDeltaTime); // idle <-> suspicious
	void AI_Suspicious(float fDeltaTime); // idle <-> sus <-> detect
	void AI_Chase(float fDeltaTime);
	void AI_Attack(float fDeltaTime);

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);

	void Walk(float fDeltaTime);
	void Run(float fDeltaTime);
	void Jump(float fDeltaTime);

	void Inch(float fDeltaTime);
	void Prepare_Atk(float fDeltaTime);
	void Attack(float fDeltaTime);
	void Heavy_Attack(float fDeltaTime);
	void Parrying(float fDeltaTime);
	void Falling(float fDeltaTime);
	
	void Dead(float fDeltaTime);
#pragma endregion
};

