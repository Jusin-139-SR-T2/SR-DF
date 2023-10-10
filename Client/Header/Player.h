#pragma once

#include "AceUnit.h"
#include "Engine_Macro.h"

#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
//임시용
#include "AceFood.h" 

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCameraComponent;
class CCalculatorComponent;
class CSphereColComp;

END

class CDynamicCamera;
class CPlayerLighter;

class CPlayer : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CPlayer)
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;

	// ===============================랜 더================================
	virtual void		Render_GameObject() override;
	// ====================================================================

	// =============================상태 추가==============================
	virtual HRESULT		Ready_GameObject() override;
	// ====================================================================
private:
	// ============================손 상태 체크============================
	void				Hand_Check();
	// ====================================================================

	// =============================양손 주먹==============================
	void				Two_Hand();
	// ====================================================================

	// ===========================양손 오브젝트============================
	void				Two_Object();
	// ====================================================================

	// ===========================한손 오브젝트============================
	void				Right_Object();
	// ====================================================================

	// ============================키보드 입력=============================
	bool				Keyboard_Input(const _float& fTimeDelta);	// 키보드 입력
	// ====================================================================

	// ============================== 프레임 ==============================
	void				FrameManage(const _float& fTimeDelta);	// 프레임 관리
	// ====================================================================

	// =============================== 차징 ===============================
	void				Charge(const _float& fTimeDelta);	// 차징
	// ====================================================================

	// =============================== 피킹 =============================== // 소영 추가 
	//_vec3				Picking_On_Object();	// 오브젝트 마우스 피킹 
	_bool				Picking_On_Object();
	// ====================================================================


//-------------------------------------------------------------------------------------------------

protected: // 충돌 onoff
	virtual void	OnCollision(CGameObject* pDst) override;
	virtual void	OnCollisionEntered(CGameObject* pDst) override;
	virtual void	OnCollisionExited(CGameObject* pDst) override;

//-------------------------------------------------------------------------------------------------

public: // Camera
	GETSET_EX2(CDynamicCamera*, m_pCamera, Camera, GET, SET)


public: // Get_Set
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pLeftHandComp, m_pLeftHandComp, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pRightHandComp, m_pRightHandComp, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET) // 충돌 필수 
	GETSET_EX2(CPlayerLighter*, m_PlayerLighter, PlayerLighter, GET, SET)	// 라이터 조명


private: // 컴포넌트
	// 기능
	CRcBufferComp* m_pBufferComp = nullptr;					// Rc버퍼
	CTransformComponent* m_pTransformComp = nullptr;		// 이동행렬 버퍼
	CCalculatorComponent* m_pCalculatorComp = nullptr;		// 지형 타기 컴포넌트
	CColliderComponent* m_pColliderComp = nullptr;				// 구 충돌 콜라이더

	// 플레이어 손
	CTextureComponent* m_pLeftHandComp = nullptr;			// 왼손 텍스처(담을곳)
	CTextureComponent* m_pRightHandComp = nullptr;			// 오른손 텍스처(담을곳)


//-------------------------------------------------------------------------------------------------

public:
	void		Update_BlackBoard();

private:
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;

//-------------------------------------------------------------------------------------------------

public:// 플레이어 상태 값
	// 플레이어
	enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, CHARGING, KICK, THROW_AWAY, DIE };
	// 왼손
	enum class STATE_LEFTHAND { NONE, HAND, OPEN_HAND, RUN_HAND, RIGHTER, RUN_RIHGTER };
	// 오른손
	enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };

	// TEST
	enum class OBJECT_TYPE { NONE, TWO_HAND, TWO_OBJECT, RIGHT_OBJECT }; // 테스트(오브젝트)
	enum class OBJECT_NAME { NONE, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN };

	// TEST
	enum DASHDIR { LEFT, RIGHT, DOWN };	// 대쉬 방향 

	// 소영 추가 ---------------- 
	STATE_RIGHTHAND   m_eRIGHTState;   // 오른손상태
	GETSET_EX2(STATE_RIGHTHAND, m_eRIGHTState, PlayerRightHand, GET, SET)   // 오른손 상태값 받아오는용도 
	GETSET_EX2(GAUGE<_float>, m_gHp, PlayerHP, GET, SET)   // 플레이어 hp용도 

private: // 플레이어의 상태 머신
	STATE_SET<STATE_PLAYER, void(CPlayer*, float)> m_tPlayer_State;

	void Idle(float fTimeDelta);
	void Move(float fTimeDelta);
	void Run(float fTimeDelta);
	void Down(float fTimeDelta);
	void Attack(float fTimeDelta);
	void Kick(float fTimeDelta);
	void Throw_Away(float fTimeDelta);
	void Die(float fTimeDelta);

private: // 플레이어의 왼손 상태 머신
	STATE_SET<STATE_LEFTHAND, void(CPlayer*, float)> m_tLeftHand_State;

	void	Left_None(float fTimeDelta);
	void	Left_Hand(float fTimeDelta);
	void	Left_RunRighter(float fTimeDelta);
	void	Left_OpenHand(float fTimeDelta);
	void	Left_RunHand(float fTimeDelta);
	void	Left_Righter(float fTimeDelta);

private: // 플레이어의 오른손 상태 머신
	STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tRightHand_State;
	STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tRightState_Old;

	void	Right_None(float fTimeDelta);
	void	Right_Hand(float fTimeDelta);
	void	Right_RunHand(float fTimeDelta);
	void	Right_Gun(float fTimeDelta);
	void	Right_Thompson(float fTimeDelta);
	void	Right_Steelpipe(float fTimeDelta);
	void	Right_BeerBotle(float fTimeDelta);
	void	Right_FryingPan(float fTimeDelta);
	void	Right_Kick(float fTimeDelta);

private: // 함수
	HRESULT				Add_Component();							// 컴포넌트 추가
	//bool				Keyboard_Input(const _float& fTimeDelta);	// 키보드 입력
	bool				Attack_Input(const _float& fTimeDelta);		// 공격 입력(마우스)
	void				Mouse_Move();								// 마우스 움직임
	void				Height_On_Terrain();						// 지형타기
	void				Dash(const _float& fTimeDelta);
	//void				Hand_Check();								// 플레이어 손 상태 체크
	void				LoadAnimationFromFile(const char* fileName);// 애니메이션 불러오기
	void				Interpolation(float& _fFrame);

	//// ==============================양손 주먹=============================
	//void				Two_Hand();
	//// ====================================================================
	//
	//// ===========================양손 오브젝트============================
	//void				Two_Object();
	//// ====================================================================
	//
	//// ===========================한손 오브젝트============================
	//void				Right_Object();
	//// ====================================================================

private:
	void State_Update(float fTimeDelta);	// 상태 업데이트(상태 체크)

private: // 스위치
	_bool		bAttackOn = false;		// 공격 On/Off
	_bool		bLeftFrameOn = false;	// 왼손 프레임 On/Off
	_bool		bRightFrameOn = false;	// 오른손 프레임 On/Off
	_bool		bGunOn = false;			// 총 On/Off
	_bool		bSpinOn = false;		// 총 회전 On/Off
	_bool		bRighter = false;		// 라이터 On/Off
	_bool		bRighterSwitch = false; // 라이터 스위치 On/Off
	_bool		bLeftHandOn = true;		// 왼손 출력 On/Off
	_bool		bRightHandOn = true;	// 오른손 출력 On/Off
	_bool		bLightOn = false;		// 라이터 조명 On/Off
	_bool		bGetAnimation = false;	// 애니메이션 불러오기 On/Off

	// 대쉬
	_bool		bDashOn = false;		// 플레이어 대쉬 여부
	_bool		bDashChange = false;	// 대쉬 증감 On/Off

	// 주먹 공격
	_bool		bLeftHandFist = true;	// 왼손 주먹상태
	_bool		bRightHandFist = true;	// 오른손 주먹상태
	_bool		bLeftPunch = false;		// 왼주먹 On/Off
	_bool		bRightPunch = true;		// 오른주먹 On/Off

	// 방어
	_bool		bShieldOn = true;		// 쉴드 On/Off

	// 플레이어 행동 여부
	_bool		bShield = true;			// 쉴드 가능 여부
	_bool		bChargingReady = true;	// 차징 가능 여부
	_bool		bChargeAttack = false;	// 일반 공격에서 차징 공격으로 변경
	_bool		bFootAttack = false;	// 발차기 여부
	_bool		bRunOn = false;			// 플레이어가 뛰는지 여부
	_bool		bDead = false;			// 플레이어 사망 여부
	//_bool		bMove = false;			// 플레이어가 움직이는지 여부
	//_bool		bMouse_Button = false;	// 마우스 클릭 여부

	// Test
	_bool		bBackRighter = false;

private:
	_float		fRightFrameSpeed = 10.f;	// 오른손 프레임 속도
	_float		fLeftFrameSpeed = 10.f;		// 왼손 프레임 속도
	_float		fStraightSpeed = 5.f;		// 플레이어 전진 속도
	_float		fSpeed = 5.f;				// 플레이어 속도
	_float		fDash = 20.f;				// 플레이어 대쉬
	_float		fDownDash = 0.f;			// 플레이어 대쉬할때 높이
	_float		fFullChage = 0.f;			// 풀차징 프레임
	_float		fShieldFrame = 0.f;			// 쉴드시 프레임
	_float		fChageTime = 0.f;			// 차징 전환 시간
	_float		fFullChargeTime = 7.f;		// 풀자징 시간
	_float		fCurrentTime = 0.f;			// 현재 시간
	_float		fCurChangeTime = 1.f;		// 시간 속도 조절(배율)
	_float		fMaxChangeTime = 3.f;		// 변경될 최대 시간

	// 플레이어 상태
	STATE_PLAYER	m_ePlayerState;	// 플레이어
	STATE_LEFTHAND	m_eLeftState;	// 왼손

	// Test
	OBJECT_TYPE m_eObjectType;	// 오브젝트 타입
	OBJECT_NAME m_eObjectName;	// 오브젝트 이름

	// 대쉬 방향
	DASHDIR m_eDashDir;

	// 라이터 조명
	CPlayerLighter* m_PlayerLighter;

private:
	_long			dwMouseMove = 0;		// 마우스 무브

	_float			m_fLeftFrame = 0.f;		// 왼손 현재 프레임
	_float			m_fLeftMaxFrame = 0.f;	// 왼손 최대 프레임

	_float			m_fRightFrame = 0.f;	// 오른손 현재 프레임
	_float			m_fRightMaxFrame = 0.f; // 오른손 최대 프레임

private:
	virtual void		Free();

protected:
	CDynamicCamera* m_pCamera = nullptr;
	_vec3	m_vCameraAt;
	_vec3	m_vPlayerLook;
	_float	fDot;
	_vec3	vLook = { 0.f, 0.f, 1.f };
	_vec3	m_vPlayerPos;

protected:
	_matrix		m_matRot;
	_matrix		m_ViewMatrix, m_ProjMatrix;	// 요거 차후 제거 예정, 직교, 뷰포트가 렌더러에 종속적으로 바뀜

private: //빛
	//HRESULT			SetUp_Material();
	_bool			bTorch = false;

		
private: // 쿼터니언
	_long dwMouseMoveX;
	_long dwMouseMoveY;

	D3DXQUATERNION	m_quaternion;	// 쿼터니온 변수

private:
	_float	m_fX, m_fY, m_fSizeX, m_fSizeY;

private: // 보간 변수
	// dt 값
	_float fFrameTimeDelta, fCurFrameTimeDelta;

	// 크기
	_float fSizeX_Delta, fSizeY_Delta;

	// 회전
	_float fRotX_Delta, fRotY_Delta, fRotZ_Delta;

	// 이동
	_float fPosX_Delta, fPosY_Delta;

private:
	CGameObject* m_pPlayer;

private:
	// 애니메이션 타임 라인
	std::vector<KEYFRAME> timeline;
};

/*	현재 키 설명

================ 카메라 ================
F1 : 1인칭 카메라
F2 : 3인칭 카메라
F3 : 자유시점 카메라

방향키 : 카메라 이동
========================================

=============== 플레이어 ===============

-이동-
W : 전진
S : 후진
A : 좌회전
D : 우회전

W + LShift : 달리기
A, S, D + LShift : 대쉬

-플레이어 기능-
Q : 발차기
V : 라이터 (양손 오브젝트일 경우 불가능)
R : 권총 회전(권총 상태일때만 가능)

-오브젝트 변경-
1 : 주먹
2 : 권총
3 : 톰슨 기관총
4 : 쇠파이프
5 : 맥주병
6 : 프라이팬

-공격-
MouseLB 1클릭 : 단일 공격
MouseLB 꾹 누르기 : 차징 공격 (차징 공격이 가능한 상태만)
MouseRB : 가드 (가드가 가능한 상태만)
========================================

*/