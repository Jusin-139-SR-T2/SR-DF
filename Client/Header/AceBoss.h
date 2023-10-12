#pragma once
#include "AceMonster.h"
#include "BossLight.h"

#include "BlackBoard_Monster.h"
#include "BlackBoardPtr.h"

//공격 헤더 
#include "MonsterPunch.h"

//스킬 및 이펙트 헤더 
#include "Awareness.h"
#include "RedLaser.h"
#include "FallingStone.h"
#include "SlowThunder.h"
#include "EnergyBall.h"
#include "SpawnFire.h"
#include "BlueBuff.h"
#include "RedThunder.h"

typedef struct BossDistance
{
	//_flaot fCloseAtk = 3.f; 
	_int a = 0;
};


class CAceBoss : public CAceMonster
{
	DERIVED_CLASS(CAceMonster, CAceBoss)

private:
	explicit CAceBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceBoss(const CAceBoss& rhs);
	virtual ~CAceBoss();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CAceBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	HRESULT				Add_Component();
	virtual void		Free();

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(_float, m_tStat.fAwareness, Awareness, GET, SET)

	// 충돌 
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);
	PRIVATE FCollisionBox* pShape;
	
	//블랙보드 - Add BlackBoard에서 추가한 BoardName과 업로드, 다운로드 이름이 모두 같아야한다. 
private:
	void	Update_InternalData(); // 블랙보드로 데이터를 올린다. 
	void	Update_BlackBoard(); // 블랙보드로부터 데이터를 받아오는용도 

protected:
	FBlackBoardPtr<CBlackBoard_Monster>		m_wpBlackBoard_Monster;

	// 필요한 변수 
private:
	MonsterPhase m_ePhase;

	//거리조절 

	//조명 컨트롤역할 - 블랙보드 연결 완료 
	_bool m_bLightOn = FALSE;

#pragma region 상태머신 enum셋팅

public:
	// 목표 상태머신(AI)
	enum class STATE_OBJ {
		IDLE, SUSPICIOUS, REST, CHASE,				// 경계	  (SUS = BACKIDLE)
		RELOADING, RUN, WALK,						// 추격
		PRE_ATTACK, SIDE_READY, ROLL, SHOUT,				// 전조 
		CLOSEATTACK,  SHOOT, SKILLATTACK,			// 공격
		HIT, DAZED, FACEPUNCH, FALLING,	EVASION,	// 피격
		DEATH,										// 죽음
		RECONNAISSANCE, GOHOME						// 복귀
	};

	// 행동 상태머신
	enum class STATE_ACT {
		IDLE, APPROACH, MOVING, ATTACK, SKILL, GOHOME
	};

	// 액션키
	enum class ACTION_KEY {
		IDLE,
		RUN, WALK, ROLL, 
		ATTACK, SHOOT,
		SKILL_LASER, SKILL_STONE, SKILL_FIRE, SKILL_THUNDER, SKILL_ENERGYBALL, SKILL_BUFF,//스킬
		GOHOME
	};

private:
	STATE_SET<STATE_OBJ, void(CAceBoss*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CAceBoss*, float)> m_tState_Act;				// 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey;									//가상 조작키 

#pragma endregion

#pragma region AI 

	// 경계 
	void AI_Idle(float fDeltaTime); // SOUTH 바라보는중 
	void AI_Suspicious(float fDeltaTime); // 오 ? 의심중
	void AI_Rest(float fDeltaTime); // 제자리에서 들썩거림
	void AI_Chase(float fDeltaTime); // 거리 체크해서 각종 상태 보내버리는 일종의 허브역할 

	// 추격 
	void AI_Run(float fDeltaTime); // 달리기 
	void AI_Walk(float fDeltaTime); // 걷기
	void AI_Reloading(float fDeltaTime); // 총알 재장전

	//전조
	void AI_PreAttack(float fDeltaTime); // 근접공격 전조 
	void AI_SideReady(float fDeltaTime); // 벽에 등지고 총잡은듯 포즈 
	void AI_Roll(float fDeltaTime); // 옆으로 구르기 
	void AI_Shout(float fDeltaTime); // 옆으로 구르기 

	// 공격
	void AI_CloseAttack(float fDeltaTime); // 근접공격 - 개머리판으로 후리기  
	void AI_Shoot(float fDeltaTime); // 총으로 쏘는공격 
	
	// 스킬
	void AI_SkillAttack(float fDeltaTime); // 스킬 공격
	
	// 피격
	void AI_Hit(float fDeltaTime); // 맞은 히트판정 
	void AI_Dazed(float fDeltaTime); // hp 일정이상 닳은 상태 
	void AI_Falling(float fDeltaTime); // 발차기 맞았을경우 
	void AI_Evasion(float fDeltaTime); // 나름회피기 - 스킬 전조로 바꿀지도 ? 음   
	void AI_FacePunch(float fDeltaTime); // 얼굴에 맞았을경우 

	// 죽음 
	void AI_Death(float fDeltaTime); //ok // hp 0인상태 

	// 복귀
	void AI_Reconnaissance(float fDeltaTime); // 플레이어 놓쳐서 주변 정찰하는중 
	void AI_GoHome(float fDeltaTime);		 // 정찰마치고 원위치 복귀중 

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);		// AI_Run + AI_Walk
	void Moving(float fDeltaTime);			// AI_InchForward + AI_Strafing
	void Attack(float fDeltaTime);			// AI_NORMALATTACK + AI_HeavyAttack
	void Skill(float fDeltaTime);			// 각종 Skill처리 
	void GoHome(float fDeltaTime);			// Gohome
#pragma endregion

};


/* SPEED 정리

Speed 1 =
Speed 2 =
Speed 3 =
Speed 3.5 = WALK
Speed 4 =
Speed 5 = RUN
Speed 6 = GoHome
Speed 7 = Strafing
Speed 8 = Inch
Speed 9 =

디버그 라인

OutputDebugString(L"▶ : 충돌 관련 디버그
OutputDebugString(L"▷ : 상태머신 관련 디버그
OutputDebugString(L"★★★★★  중요 디버그라인 

swprintf_s(debugString, L"Boss - 변수 확인 m_fAwareness = %f\n", m_fAwareness);
OutputDebugStringW(debugString);

*/