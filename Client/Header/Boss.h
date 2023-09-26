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
private:
	// 함수 ----------
	_bool		Detect_Player();					// 몬스터 시야각내에 플레이어가 있는지 체크
	_float		Calc_Distance();					// 몬스터와 플레이어 사이의 거리 체크하는 함수 
	void		FaceTurn(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 

	// 변수 ----------
	_float		m_fCheck = 0;						//Taunt 등 프레임 돌리는횟수 지정
	_int		m_iHP;								// 몬스터 현재 hp 
	_int		m_iPreHP;							// 이전 HP 저장용도 
	_int		m_iDazedHP = 25;					// 몬스터 기절하는 hp
	_int		m_iMaxHP = 100;						// 몬스터 최대 hp 
	_int		m_iAttack = 15;						// 몬스터 공격력

	_float		m_fFrame = 0.f;						// 이미지 돌리기위한 프레임변수 
	_float		m_fFrameEnd;						// 이미지마다 변수 넣어줘야함 
	_float		m_fFrameSpeed;						// 프레임 돌리는 속도

	_float		m_fAwareness = 0;					// 의심게이지 숫자 
	_float		m_fMaxAwareness = 10.f;				// 의심게이지 max -> 추격으로 변함 
	_float		m_fConsider = 10.f;					// 플레이어 놓친뒤에 주변정찰 게이지 
	_float		m_fMaxConsider = 10.f;				// 플레이어 놓친뒤에 주변정찰 게이지 

	// 속도조절 
	_float		m_fRunSpeed = 2.0f;					// 뛰어오는 속도
	_float		m_fWalkSpeed = 1.0f;				// 걷는속도
	_float		m_fKeepEyeSpeed = 2.f;				// 옆으로 무빙하는  속도 
	_float		m_fSideWalkSpeed = 3.f;				// 옆으로 무빙하는  속도 

	_float		m_fUprightSpeed = 2.5f;				// 파이프들고 달려오는 속도 
	_float		m_fThrowSpeed = 4.f;				// 투사체 스피드 

	_float		m_fBasicAttackSpeed = 3.f;			// 일반공격때 뛰어오는 속도 
	_float		m_fHeavAttackSpeed = 4.f;			// 강공격때 뛰어오는 속도 

	// 사거리 , 시야각
	_float		m_fMonsterFov = 90;					//시야각 - 반각 기준
	_float		m_fMonsterSightDistance = 13.f;		// 몬스터가 포착하는 사거리 
	_float		m_fRunDistance = 8.f;				// 사거리 ~ Run 사이 =  run
	_float		m_fWalkDistance = 7.5f;				// run~walk 사이 = walk
	_float		m_fEyesOnYouDistance = 6.f;			// Eyes ~ Walk = 옆으로 무빙 
	_float		m_fCloseToYouDistance = 3.f;		// Close ~ Eyes = 

	// 위치 조절 
	_vec3		vPlayerPos;							// 플레이어 위치 벡터
	_vec3		vDir;								// 몬스터가 플레이어 바라보는 벡터  

	//스위치 on/off 

public:
	// 목표 상태머신(AI)
	enum class STATE_OBJ { 
		IDLE, SUSPICIOUS, TEST };

	// 행동 상태머신
	enum class STATE_ACT { IDLE, APPROACH };

	// 행동키
	enum class ACTION_KEY { IDLE };

private:
	STATE_SET<STATE_OBJ, void(CBoss*, float)> m_tState_Obj; //AI
	STATE_SET<STATE_ACT, void(CBoss*, float)> m_tState_Act; // 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey; //가상 조작키 

#pragma region AI 

	void AI_Idle(float fDeltaTime); // idle <-> suspicious
	void AI_Suspicious(float fDeltaTime); // idle <-> sus <-> detect
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
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);

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

