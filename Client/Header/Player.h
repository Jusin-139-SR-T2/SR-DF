#pragma once

#include "AceUnit.h"
#include "Engine_Macro.h"

#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
//�ӽÿ�
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

	// ===============================�� ��================================
	virtual void		Render_GameObject() override;
	// ====================================================================

	// =============================���� �߰�==============================
	virtual HRESULT		Ready_GameObject() override;
	// ====================================================================
private:
	// ============================�� ���� üũ============================
	void				Hand_Check();
	// ====================================================================

	// =============================��� �ָ�==============================
	void				Two_Hand();
	// ====================================================================

	// ===========================��� ������Ʈ============================
	void				Two_Object();
	// ====================================================================

	// ===========================�Ѽ� ������Ʈ============================
	void				Right_Object();
	// ====================================================================

	// ============================Ű���� �Է�=============================
	bool				Keyboard_Input(const _float& fTimeDelta);	// Ű���� �Է�
	// ====================================================================

	// ============================== ������ ==============================
	void				FrameManage(const _float& fTimeDelta);	// ������ ����
	// ====================================================================

	// =============================== ��¡ ===============================
	void				Charge(const _float& fTimeDelta);	// ��¡
	// ====================================================================

	// =============================== ��ŷ =============================== // �ҿ� �߰� 
	//_vec3				Picking_On_Object();	// ������Ʈ ���콺 ��ŷ 
	_bool				Picking_On_Object();
	// ====================================================================


//-------------------------------------------------------------------------------------------------

protected: // �浹 onoff
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
	GETSET_EX2(CColliderComponent*, m_pColliderComp, ColliderComponent, GET, SET) // �浹 �ʼ� 
	GETSET_EX2(CPlayerLighter*, m_PlayerLighter, PlayerLighter, GET, SET)	// ������ ����


private: // ������Ʈ
	// ���
	CRcBufferComp* m_pBufferComp = nullptr;					// Rc����
	CTransformComponent* m_pTransformComp = nullptr;		// �̵���� ����
	CCalculatorComponent* m_pCalculatorComp = nullptr;		// ���� Ÿ�� ������Ʈ
	CColliderComponent* m_pColliderComp = nullptr;				// �� �浹 �ݶ��̴�

	// �÷��̾� ��
	CTextureComponent* m_pLeftHandComp = nullptr;			// �޼� �ؽ�ó(������)
	CTextureComponent* m_pRightHandComp = nullptr;			// ������ �ؽ�ó(������)


//-------------------------------------------------------------------------------------------------

public:
	void		Update_BlackBoard();

private:
	FBlackBoardPtr<CBlackBoard_Player>		m_wpBlackBoard_Player;

//-------------------------------------------------------------------------------------------------

public:// �÷��̾� ���� ��
	// �÷��̾�
	enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, CHARGING, KICK, THROW_AWAY, DIE };
	// �޼�
	enum class STATE_LEFTHAND { NONE, HAND, OPEN_HAND, RUN_HAND, RIGHTER, RUN_RIHGTER };
	// ������
	enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };

	// TEST
	enum class OBJECT_TYPE { NONE, TWO_HAND, TWO_OBJECT, RIGHT_OBJECT }; // �׽�Ʈ(������Ʈ)
	enum class OBJECT_NAME { NONE, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN };

	// TEST
	enum DASHDIR { LEFT, RIGHT, DOWN };	// �뽬 ���� 

	// �ҿ� �߰� ---------------- 
	STATE_RIGHTHAND   m_eRIGHTState;   // �����ջ���
	GETSET_EX2(STATE_RIGHTHAND, m_eRIGHTState, PlayerRightHand, GET, SET)   // ������ ���°� �޾ƿ��¿뵵 
	GETSET_EX2(GAUGE<_float>, m_gHp, PlayerHP, GET, SET)   // �÷��̾� hp�뵵 

private: // �÷��̾��� ���� �ӽ�
	STATE_SET<STATE_PLAYER, void(CPlayer*, float)> m_tPlayer_State;

	void Idle(float fTimeDelta);
	void Move(float fTimeDelta);
	void Run(float fTimeDelta);
	void Down(float fTimeDelta);
	void Attack(float fTimeDelta);
	void Kick(float fTimeDelta);
	void Throw_Away(float fTimeDelta);
	void Die(float fTimeDelta);

private: // �÷��̾��� �޼� ���� �ӽ�
	STATE_SET<STATE_LEFTHAND, void(CPlayer*, float)> m_tLeftHand_State;

	void	Left_None(float fTimeDelta);
	void	Left_Hand(float fTimeDelta);
	void	Left_RunRighter(float fTimeDelta);
	void	Left_OpenHand(float fTimeDelta);
	void	Left_RunHand(float fTimeDelta);
	void	Left_Righter(float fTimeDelta);

private: // �÷��̾��� ������ ���� �ӽ�
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

private: // �Լ�
	HRESULT				Add_Component();							// ������Ʈ �߰�
	//bool				Keyboard_Input(const _float& fTimeDelta);	// Ű���� �Է�
	bool				Attack_Input(const _float& fTimeDelta);		// ���� �Է�(���콺)
	void				Mouse_Move();								// ���콺 ������
	void				Height_On_Terrain();						// ����Ÿ��
	void				Dash(const _float& fTimeDelta);
	//void				Hand_Check();								// �÷��̾� �� ���� üũ
	void				LoadAnimationFromFile(const char* fileName);// �ִϸ��̼� �ҷ�����
	void				Interpolation(float& _fFrame);

	//// ==============================��� �ָ�=============================
	//void				Two_Hand();
	//// ====================================================================
	//
	//// ===========================��� ������Ʈ============================
	//void				Two_Object();
	//// ====================================================================
	//
	//// ===========================�Ѽ� ������Ʈ============================
	//void				Right_Object();
	//// ====================================================================

private:
	void State_Update(float fTimeDelta);	// ���� ������Ʈ(���� üũ)

private: // ����ġ
	_bool		bAttackOn = false;		// ���� On/Off
	_bool		bLeftFrameOn = false;	// �޼� ������ On/Off
	_bool		bRightFrameOn = false;	// ������ ������ On/Off
	_bool		bGunOn = false;			// �� On/Off
	_bool		bSpinOn = false;		// �� ȸ�� On/Off
	_bool		bRighter = false;		// ������ On/Off
	_bool		bRighterSwitch = false; // ������ ����ġ On/Off
	_bool		bLeftHandOn = true;		// �޼� ��� On/Off
	_bool		bRightHandOn = true;	// ������ ��� On/Off
	_bool		bLightOn = false;		// ������ ���� On/Off
	_bool		bGetAnimation = false;	// �ִϸ��̼� �ҷ����� On/Off

	// �뽬
	_bool		bDashOn = false;		// �÷��̾� �뽬 ����
	_bool		bDashChange = false;	// �뽬 ���� On/Off

	// �ָ� ����
	_bool		bLeftHandFist = true;	// �޼� �ָԻ���
	_bool		bRightHandFist = true;	// ������ �ָԻ���
	_bool		bLeftPunch = false;		// ���ָ� On/Off
	_bool		bRightPunch = true;		// �����ָ� On/Off

	// ���
	_bool		bShieldOn = true;		// ���� On/Off

	// �÷��̾� �ൿ ����
	_bool		bShield = true;			// ���� ���� ����
	_bool		bChargingReady = true;	// ��¡ ���� ����
	_bool		bChargeAttack = false;	// �Ϲ� ���ݿ��� ��¡ �������� ����
	_bool		bFootAttack = false;	// ������ ����
	_bool		bRunOn = false;			// �÷��̾ �ٴ��� ����
	_bool		bDead = false;			// �÷��̾� ��� ����
	//_bool		bMove = false;			// �÷��̾ �����̴��� ����
	//_bool		bMouse_Button = false;	// ���콺 Ŭ�� ����

	// Test
	_bool		bBackRighter = false;

private:
	_float		fRightFrameSpeed = 10.f;	// ������ ������ �ӵ�
	_float		fLeftFrameSpeed = 10.f;		// �޼� ������ �ӵ�
	_float		fStraightSpeed = 5.f;		// �÷��̾� ���� �ӵ�
	_float		fSpeed = 5.f;				// �÷��̾� �ӵ�
	_float		fDash = 20.f;				// �÷��̾� �뽬
	_float		fDownDash = 0.f;			// �÷��̾� �뽬�Ҷ� ����
	_float		fFullChage = 0.f;			// Ǯ��¡ ������
	_float		fShieldFrame = 0.f;			// ����� ������
	_float		fChageTime = 0.f;			// ��¡ ��ȯ �ð�
	_float		fFullChargeTime = 7.f;		// Ǯ��¡ �ð�
	_float		fCurrentTime = 0.f;			// ���� �ð�
	_float		fCurChangeTime = 1.f;		// �ð� �ӵ� ����(����)
	_float		fMaxChangeTime = 3.f;		// ����� �ִ� �ð�

	// �÷��̾� ����
	STATE_PLAYER	m_ePlayerState;	// �÷��̾�
	STATE_LEFTHAND	m_eLeftState;	// �޼�

	// Test
	OBJECT_TYPE m_eObjectType;	// ������Ʈ Ÿ��
	OBJECT_NAME m_eObjectName;	// ������Ʈ �̸�

	// �뽬 ����
	DASHDIR m_eDashDir;

	// ������ ����
	CPlayerLighter* m_PlayerLighter;

private:
	_long			dwMouseMove = 0;		// ���콺 ����

	_float			m_fLeftFrame = 0.f;		// �޼� ���� ������
	_float			m_fLeftMaxFrame = 0.f;	// �޼� �ִ� ������

	_float			m_fRightFrame = 0.f;	// ������ ���� ������
	_float			m_fRightMaxFrame = 0.f; // ������ �ִ� ������

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
	_matrix		m_ViewMatrix, m_ProjMatrix;	// ��� ���� ���� ����, ����, ����Ʈ�� �������� ���������� �ٲ�

private: //��
	//HRESULT			SetUp_Material();
	_bool			bTorch = false;

		
private: // ���ʹϾ�
	_long dwMouseMoveX;
	_long dwMouseMoveY;

	D3DXQUATERNION	m_quaternion;	// ���ʹϿ� ����

private:
	_float	m_fX, m_fY, m_fSizeX, m_fSizeY;

private: // ���� ����
	// dt ��
	_float fFrameTimeDelta, fCurFrameTimeDelta;

	// ũ��
	_float fSizeX_Delta, fSizeY_Delta;

	// ȸ��
	_float fRotX_Delta, fRotY_Delta, fRotZ_Delta;

	// �̵�
	_float fPosX_Delta, fPosY_Delta;

private:
	CGameObject* m_pPlayer;

private:
	// �ִϸ��̼� Ÿ�� ����
	std::vector<KEYFRAME> timeline;
};

/*	���� Ű ����

================ ī�޶� ================
F1 : 1��Ī ī�޶�
F2 : 3��Ī ī�޶�
F3 : �������� ī�޶�

����Ű : ī�޶� �̵�
========================================

=============== �÷��̾� ===============

-�̵�-
W : ����
S : ����
A : ��ȸ��
D : ��ȸ��

W + LShift : �޸���
A, S, D + LShift : �뽬

-�÷��̾� ���-
Q : ������
V : ������ (��� ������Ʈ�� ��� �Ұ���)
R : ���� ȸ��(���� �����϶��� ����)

-������Ʈ ����-
1 : �ָ�
2 : ����
3 : �轼 �����
4 : ��������
5 : ���ֺ�
6 : ��������

-����-
MouseLB 1Ŭ�� : ���� ����
MouseLB �� ������ : ��¡ ���� (��¡ ������ ������ ���¸�)
MouseRB : ���� (���尡 ������ ���¸�)
========================================

*/