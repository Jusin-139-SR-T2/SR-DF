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

class CBoss : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CBoss)

private:
	explicit CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBoss(const CBoss& rhs);
	virtual ~CBoss();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
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
		GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
		GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

		// 상태머신 셋팅 --------------------------------------------------
private:
	_int		m_iHP;				// 몬스터 hp 
	_int		m_iAttack = 15;		// 몬스터 공격력

	_float		m_fFrame = 0.f;		// 이미지 돌리기위한 프레임변수 
	_float		m_fFrameEnd;		// 이미지마다 변수 넣어줘야함 
	_float		m_fFrameSpeed;		// 프레임 돌리는 속도

public:
	// 목표 상태머신(AI)
	enum class STATE_OBJ { IDLE, SUSPICIOUS, TAUNT, CHASE, RUN, WALK, INCHFORWARD, STRAFYING, BASICATTACK, HEAVYATTACK };
	// 행동 상태머신
	enum class STATE_ACT { IDLE, APPROACH };
	// 행동키
	enum class ACTION_KEY { IDLE, RUN, WALK, INCH, STRAFING, JUMP, BASIC_ATTACK, HEAVY_ATTACK, };

private:
	STATE_SET<STATE_OBJ, void(CBoss*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBoss*, float)> m_tState_Act; // 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey; //가상 조작키 

#pragma region AI 

	//void AI_Idle(float fDeltaTime); // idle <-> suspicious
	//void AI_Suspicious(float fDeltaTime); // idle <-> sus <-> detect
	//void AI_Taunt(float fDeltaTime);
	//void AI_Chase(float fDeltaTime);

	//void AI_Run(float fDeltaTime);
	//void AI_Walk(float fDeltaTime);
	//void AI_InchForward(float fDeltaTime);
	//void AI_Strafing(float fDeltaTime);
	//void AI_BasicAttack(float fDeltaTime);
	//void AI_HeavyAttack(float fDeltaTime);

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	//void Idle(float fDeltaTime);
	//void Approach(float fDeltaTime);

	//void Walk(float fDeltaTime);
	//void Inch(float fDeltaTime);

	//void Heavy_Attack(float fDeltaTime);
	//void Prepare_Atk(float fDeltaTime);
	//void Attack(float fDeltaTime);
	//void Parrying(float fDeltaTime);
	//
	//void Jump(float fDeltaTime);
	//void Falling(float fDeltaTime);
	//void Dead(float fDeltaTime);
#pragma endregion
	// 액션키는 CPP쪽에 만들음
};

