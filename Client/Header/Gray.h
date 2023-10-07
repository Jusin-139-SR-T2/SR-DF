
#pragma once
#include "GameObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Macro.h"

#include "ThrowPipe.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;
class CColliderComponent;

END

class CGray : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CGray)

private:
	explicit CGray(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGray(const CGray& rhs);
	virtual ~CGray();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

	static CGray* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z);

private:
	CRcBufferComp*			m_pBufferComp = nullptr;
	CTextureComponent*		m_pTextureComp = nullptr;
	CColliderComponent*		m_pColliderComp = nullptr;
	CTransformComponent*	m_pTransformComp = nullptr;
	CCalculatorComponent*	m_pCalculatorComp = nullptr;
	CTransformComponent*	m_pPlayerTransformcomp = nullptr;

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

	// 충돌
protected:
	virtual void	OnCollision(CGameObject* pDst);
	virtual void	OnCollisionEntered(CGameObject* pDst);
	virtual void	OnCollisionExited(CGameObject* pDst);

	// 상태머신 셋팅 --------------------------------------------------
private:
	// 함수 ----------					
	_bool		Detect_Player();					// 몬스터 시야각내에 플레이어가 있는지 체크
	_float		Calc_Distance();					// 몬스터와 플레이어 사이의 거리 체크하는 함수 
	void		Billboard(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 
	HRESULT     Get_PlayerPos(const _float& fTimeDelta); // 플레이어 dynamic_cast용도 
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

	// 몬스터 인식 관련 
	_float		m_fAwareness = 0;					// 의심게이지 숫자 
	_float		m_fMaxAwareness = 8.f;				// 의심게이지 max -> 추격으로 변함 
	_float		m_fConsider = 10.f;					// 플레이어 놓친뒤에 주변정찰 게이지 
	_float		m_fMaxConsider = 10.f;				// 플레이어 놓친뒤에 주변정찰 게이지 

	// 속도조절 
	_float		m_fRunSpeed = 2.0f;					// 뛰어오는 속도
	_float		m_fWalkSpeed = 1.0f;				// 걷는속도
	_float		m_fKeepEyeSpeed = 2.f;				// 옆으로 무빙하는  속도 
	_float		m_fSideWalkSpeed = 3.f;				// 옆으로 무빙하는  속도 

	_float		m_fUprightSpeed = 2.5f;				// 파이프들고 달려오는 속도 
	_float		m_fThrowSpeed = 4.f;				// 투사체 스피드 

	_float		m_fBasiCPlayerLighterSpeed = 3.f;			// 일반공격때 뛰어오는 속도 
	_float		m_fHeavAttackSpeed = 4.f;			// 강공격때 뛰어오는 속도 

	// 사거리 , 시야각
	_float		m_fMonsterFov = 80;					//시야각 - 반각 기준
	_float		m_fMonsterSightDistance = 13.f;		// 몬스터가 포착하는 사거리 
	_float		m_fRunDistance = 8.f;				// 사거리 ~ Run 사이 =  run
	_float		m_fWalkDistance = 7.5f;				// run~walk 사이 = walk
	_float		m_fEyesOnYouDistance = 6.f;			// Eyes ~ Walk = 옆으로 무빙 
	_float		m_fCloseToYouDistance = 2.f;		// Close ~ Eyes = 

	// 위치 조절 
	_vec3		vPlayerPos;							// 플레이어 위치 벡터
	_vec3		vDir;								// 몬스터가 플레이어 바라보는 벡터  
	_vec3		vPatrolPointZero;
	
	//스위치 on/off 
	_bool		m_bGoHome = false;

public:
	// 목표 상태머신(AI)
	enum class STATE_OBJ { 
		IDLE,		SUSPICIOUS,     REST,		CHASE,	  RECONNAISSANCE,
		YOUDIE,		TAUNT,			SIDEWALK,   KEEPEYE,  RUN,				 WALK, 
		THROW,		ATTACK,			UPRIGHTRUN, FRIGHTEN, HEAVYATTACK,		 BLOCK,
		CROTCHHIT,  FACEPUNCH,		FALLING,	DAZED,	  CHOPPED,
		HEADSHOT,	HEADLESS,		DEATH, 
		GOHOME};

	// 행동 상태머신
	enum class STATE_ACT { IDLE, APPROACH, SUDDENATTACK, SIDEMOVING, ATTACK,
						   GOHOME};

	// 행동키
	enum class ACTION_KEY {
		IDLE, RUN, WALK, KEEPEYE, SIDEWALK, 
		UPRIGHT, FRIGHTEN, BASIC_ATTACK, HEAVY_ATTACK, 
		GOHOME
	};

private:
	STATE_SET<STATE_OBJ, void(CGray*, float)> m_tState_Obj;				//AI
	STATE_SET<STATE_ACT, void(CGray*, float)> m_tState_Act;				// 행동
	ACTION_SET<ACTION_KEY> m_mapActionKey;								//가상 조작키 

#pragma region AI 

	void AI_Idle(float fDeltaTime); // 처음 서있는 용도 

	void AI_Suspicious(float fDeltaTime); // 견제값 추가용 
	void AI_Taunt(float fDeltaTime); //도발
	void AI_YouDie(float fDeltaTime); //도발
	void AI_Chase(float fDeltaTime); //  거리비교 움직임 시작
	void AI_Rest(float fDeltaTime); // idle ready상태 - 중간중간 넣기용 

	void AI_Run(float fDeltaTime); //빠르게 달려오기
	void AI_Walk(float fDeltaTime); //일반적인 걷기 
	void AI_KeepEye(float fDeltaTime); //플레이어 주시한채로 백스탭밟기 느리게 
	void AI_SideWalk(float fDeltaTime); //플레이어 주시한채로 옆으로 걸음 - 조금 빠르게 walk 
	void AI_UpRightRun(float fDeltaTime); //파이프 들고 공격하러 달려오는상태
	void AI_Frighten(float fDeltaTime); //점프해서 때림 - 점프하는것 여기에  공격이 바로 연타되어야할듯

	void AI_Throw(float fDeltaTime); //파이프 던짐 
	void AI_Attack(float fDeltaTime); // 파이프 들고 대각선으로 걍 떄리는거
	void AI_HeavyAttack(float fDeltaTime); //강공격 - 옆으로 스매시

	void AI_Reconnaissance(float fDeltaTime); // 플레이어 놓쳐서 주변 정찰하는중 
	void AI_GoHome(float fDeltaTime); // 파이프 들고 대각선으로 걍 떄리는거
	//void AI_Block(float fDeltaTime); //플레이어가 공격하면 막기함

	//void AI_CrotchHit( float fDeltaTime); // 앉기 + 피격
	//void AI_FacePunch(float fDeltaTime); //얼굴 피격
	//void AI_Falling(float fDeltaTime); //떨어지고 일어나는것
	//void AI_Dazed(float fDeltaTime); //일정피 이하가 되면 나오는상태
	//void AI_Chopped(float fDeltaTime); //뒤돌고 있을때 손날치기 -> sleep 
	//void AI_HeadShot(float fDeltaTime); // 주먹이나 기타 무기로 헤드샷
	//void AI_Headless(float fDeltaTime); //총으로 헤드어택
	//void AI_Death(float fDeltaTime); //일반공격으로 죽는상태 

#pragma endregion

#pragma region 행동 : AI 이후 넘어가는곳 
	void Idle(float fDeltaTime);
	void Approach(float fDeltaTime);
	void SuddenAttack(float fDeltaTime);
	void SideMoving(float fDeltaTime);
	void Attack(float fDeltaTime);
	void GoHome(float fDeltaTime);

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

