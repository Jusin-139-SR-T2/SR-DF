#pragma once

#include "AceUnit.h"
#include "Engine_Macro.h"

#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
//임시용
#include "AceFood.h" 
#include "PlayerBullet.h"

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

/// <summary>
/// 플레이어 생성용 구조체
/// vPos, vRot, vScale, fPriority[3]
/// </summary>
struct FPlayer_Create
{
	_vec3 vPos = { 0.f, 0.f, 0.f };
	_vec3 vRot = { 0.f, 0.f, 0.f };
	_vec3 vScale = { 1.f, 1.f, 1.f };
	_float fPriority[static_cast<_uint>(EPRIORITY_TYPE::SIZE)] = {0.f, 0.f, 0.f};
};

class CPlayer : public CAceUnit
{
	DERIVED_CLASS(CAceUnit, CPlayer)
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, const FPlayer_Create& tCreate);

public:
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;

	// ===============================랜 더================================
	virtual void		Render_GameObject() override;
	// ====================================================================

	// =============================상태 추가==============================
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FPlayer_Create& tCreate);
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

	// 플레이어 오른손 상태값
	enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };
	// 플레이어 상태값
	enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, CHARGING, THROW_AWAY, DIE, JUMP, PLAYER_END };

	// 소영 추가 ---------------- 
	GETSET_EX2(STATE_RIGHTHAND, m_eRIGHTState, PlayerRightHand, GET, SET)   // 오른손 상태값 받아오는용도 
	GETSET_EX2(GAUGE<_float>, m_gHp, PlayerHP, GET, SET)   // 플레이어 hp용도 
	GETSET_EX2(STATE_PLAYER, ePlayerState, PlayerState, GET, SET)	// 라이터 조명
	STATE_RIGHTHAND   m_eRIGHTState;   // 오른손상태

		_bool* Get_DBugFrame() {
		return &bDbugFrame;
	} void Set_DBugFrame(_bool* value) {
		bDbugFrame = value;
	}	// 스위치

		
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
	// TEST
	enum class OBJECT_TYPE { NONE, TWO_HAND, TWO_OBJECT, RIGHT_OBJECT }; // 테스트(오브젝트)
	enum class OBJECT_NAME { NONE, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN };

	// TEST
	enum DASHDIR { LEFT, RIGHT, DOWN };	// 대쉬 방향 

private:
	enum class EACTION_KEY : _uint { RIGHT, LEFT, UP, DOWN, RUN, ATTACK, JUMP, SIT, SIZE };
	ACTION_SET<EACTION_KEY>			m_tActionKey;

public:
	//enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, CHARGING, KICK, THROW_AWAY, DIE, JUMP, PLAYER_END };
	//enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };
	enum class STATE_LEFTHAND { NONE, HAND, OPEN_HAND, RUN_HAND, RIGHTER, RUN_RIHGTER };

private: // 플레이어의 상태 머신
	STATE_SET<STATE_PLAYER, void(CPlayer*, float)> m_tPlayer_State;
	STATE_PLAYER ePlayerState;

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
	void				LeftLoadAnimationFromFile(const char* fileName);// 애니메이션 불러오기
	void				RightLoadAnimationFromFile(const char* fileName);// 애니메이션 불러오기
	void				LeftInterpolation();
	void				RightInterpolation();

private:
	void State_Update(float fTimeDelta);	// 상태 업데이트(상태 체크)

private: // 스위치
	_bool		bSpinOn = false;		// 총 회전 On/Off
	_bool		bRighter = false;		// 라이터 On/Off
	_bool		bRightHandOn = true;	// 오른손 출력 On/Off
	_bool		bGetAnimation = false;	// 애니메이션 불러오기 On/Off

	// 대쉬
	_bool		bDashOn = false;		// 플레이어 대쉬 여부
	_bool		bDashChange = false;	// 대쉬 증감 On/Off

	// 주먹 공격
	_bool		bLeftPunch = false;		// 왼주먹 On/Off
	_bool		bRightPunch = true;		// 오른주먹 On/Off

	_bool		bShieldOn = true;		// 쉴드 On/Off
	_bool		bChargeAttack = false;	// 일반 공격에서 차징 공격으로 변경
	_bool		bBackRighter = false;	// 라이터 되돌리기
	_bool		bDbugFrame = false;		// 디버그 프레임

private:
	struct _LEFTHAND	// 왼손
	{
		_bool			bLeftHandOn = true;			// 왼손 출력 On/Off

		_float			fLeftFrameSpeed = 10.f;		// 왼손 프레임 속도
		_float			fLeftFrame = 0.f;			// 왼손 프레임
	};

	struct _RIGHTHAND	// 오른손
	{

		_float			fRightFrameSpeed = 10.f;	// 오른손 프레임 속도
		_float			fRightFrame = 0.f;			// 오른손 프레임

		_uint			iFullChargingIndex = 0.f;	// 풀차지시 인덱스
		_uint			iShieldIndex = 0.f;			// 쉴드시 인덱스
	};

	struct _PLAYER	// 플레이어
	{
		_float		fStraightSpeed = 5.f;		// 플레이어 전진 속도
		_float		fSpeed = 5.f;				// 플레이어 속도
	};

	struct _DASH	// 대쉬
	{
		_float		fDash = 20.f;				// 플레이어 대쉬
		_float		fDownDash = 0.f;			// 플레이어 대쉬할때 높이
	};

	struct _TIME	// 시간
	{
		_float		fCurrentTime = 0.f;			// 현재 시간
		_float		fCurChangeTime = 1.f;		// 시간 속도 조절(배율)

		_float		fLeftCurrentTime = 0.f;		// 왼손 현재 시간
		_float		fLeftMaxTime = 0.f;			// 왼손 최대 시간
		_float		fLeftChangeTime = 1.f;		// 시간 속도 조절(배율)

		_float		fRightCurrentTime = 0.f;	// 오른손 현재 시간
		_float		fRightMaxTime = 0.f;		// 오른손 최대 시간
		_float		fRightChangeTime = 1.f;		// 시간 속도 조절(배율)

		_float		fMaxChangeTime = 3.f;		// 변경될 최대 시간
	};



	_PLAYER			m_tPlayer;		// 플레이어
	_LEFTHAND		m_tLeftHand;	// 플레이어 왼손
	_RIGHTHAND		m_tRightHand;	// 플레이어 오른손
	_DASH			m_tDash;		// 대쉬
	_TIME			m_tTime;		// 시간

	GAUGE<_float>		m_fChage;	// 차지

	_float		fChageTime = 0.f;			// 차징 전환 시간
	_float		fFullChargeTime = 7.f;		// 풀자징 시간

	// Test
	OBJECT_TYPE m_eObjectType;	// 오브젝트 타입
	OBJECT_NAME m_eObjectName;	// 오브젝트 이름

	// 대쉬 방향
	DASHDIR m_eDashDir;

	// 라이터 조명
	CPlayerLighter* m_PlayerLighter;

private:
		// 애니메이션 타임 라인
		std::vector<KEYFRAME> timeline[KEYTYPE_END];

private:
	_long			dwMouseMove = 0;		// 마우스 무브


private:
	virtual void		Free();

protected:
	CDynamicCamera* m_pCamera = nullptr;

private: // 보간 변수
	// dt 값
	_float fFrameTimeDelta, fCurFrameTimeDelta;
	// 크기
	_float fSizeX_Delta, fSizeY_Delta;
	// 회전
	_float fRotX_Delta, fRotY_Delta, fRotZ_Delta;
	// 이동
	_float fPosX_Delta, fPosY_Delta;
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

//private: // 쿼터니언
	//_long dwMouseMoveX;
	//_long dwMouseMoveY;

	//D3DXQUATERNION	m_quaternion;	// 쿼터니온 변수