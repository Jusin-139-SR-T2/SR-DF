#pragma once

#include "AceUnit.h"
#include "Engine_Macro.h"

#include "BlackBoard_Player.h"
#include "BlackBoardPtr.h"
//�ӽÿ�
#include "AceFood.h" 
#include "PlayerBullet.h"
#include "PlayerFist.h"

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
class CBrown;
class CGray;

/// <summary>
/// �÷��̾� ������ ����ü
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

	// ===============================�� ��================================
	virtual void		Render_GameObject() override;
	// ====================================================================

	// =============================���� �߰�==============================
	virtual HRESULT		Ready_GameObject() override;
	virtual HRESULT		Ready_GameObject(const FPlayer_Create& tCreate);
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

	// �÷��̾� ������ ���°�
	enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };
	// �÷��̾� ���°�
	enum class STATE_PLAYER { NONE, IDLE, SITDOWN, JUMP, DIE, PLAYER_STATE_SIZE_END };
	// �÷��̾� �ൿ ���°�
	enum class STATE_PLAYER_ACTION { IDLE, RUN, ATTACK, CHARGING, THROW_AWAY, PLAYER_ACTION_SIZE_END };

	// �ҿ� �߰� ---------------- 
	GETSET_EX2(STATE_RIGHTHAND, m_eRIGHTState, PlayerRightHand, GET, SET)   // ������ ���°� �޾ƿ��¿뵵 
	GETSET_EX2(GAUGE<_float>, m_gHp, PlayerHP, GET, SET)   // �÷��̾� hp�뵵 
	STATE_RIGHTHAND   m_eRIGHTState;   // �����ջ���
	// ���� ���� 
	_float m_fAge = 0.f;
	_float m_fLifeTime = 0.2;
	_bool m_bGunLight; // ���Ѵ� bool�� 
	// ------------------------- 

		_bool* Get_DBugFrame() {
		return &bDbugFrame;
	} void Set_DBugFrame(_bool* value) {
		bDbugFrame = value;
	}	// ����ġ

		
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
	// TEST
	enum class OBJECT_TYPE { NONE, TWO_HAND, TWO_OBJECT, RIGHT_OBJECT }; // �׽�Ʈ(������Ʈ)
	enum class OBJECT_NAME { NONE, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN };

	// TEST
	enum DASHDIR { LEFT, RIGHT, DOWN };	// �뽬 ���� 

private:
	enum class EACTION_KEY : _uint { RIGHT, LEFT, UP, DOWN, RUN, ATTACK, JUMP, SIT, SIZE };
	ACTION_SET<EACTION_KEY>			m_tActionKey;

public:
	//enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, CHARGING, KICK, THROW_AWAY, DIE, JUMP, PLAYER_END };
	//enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE, BEERBOTLE, FRYINGPAN, KICK };
	enum class STATE_LEFTHAND { NONE, HAND, OPEN_HAND, RUN_HAND, RIGHTER, RUN_RIHGTER };

private: // �÷��̾��� ���� �ӽ� (����x, �ٱ�, �ɱ�, ����, ����)
	STATE_SET<STATE_PLAYER, void(CPlayer*, float)> m_tPlayer_State;

	void Idle(float fTimeDelta);
	void Down(float fTimeDelta);
	void Kick(float fTimeDelta);
	void Die(float fTimeDelta);

private: // �÷��̾��� �ൿ �ӽ� (�ൿx, �̵�, ����, ��¡, ������)
	STATE_SET<STATE_PLAYER_ACTION, void(CPlayer*, float)> m_tPlayer_Action;

	void Action_Idle(float fTimeDelta);
	void Action_Move(float fTimeDelta);
	void Action_Run(float fTimeDelta);
	void Action_Charging(float fTimeDelta);
	void Action_ThrowAway(float fTimeDelta);

public:
	STATE_PLAYER Get_PlayerState() { return m_tPlayer_State.Get_State(); }

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
	//STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tRightState_Old;
	STATE_RIGHTHAND m_eRightState_Old;
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
	HRESULT				Add_Component();									// ������Ʈ �߰�
	//bool				Keyboard_Input(const _float& fTimeDelta);			// Ű���� �Է�
	bool				Attack_Input(const _float& fTimeDelta);				// ���� �Է�(���콺)
	void				Mouse_Move();										// ���콺 ������
	void				Height_On_Terrain();								// ����Ÿ��
	void				Dash(const _float& fTimeDelta);
	//void				Hand_Check();										// �÷��̾� �� ���� üũ
	void				LeftLoadAnimationFromFile(const char* fileName);	// �ִϸ��̼� �ҷ�����
	void				RightLoadAnimationFromFile(const char* fileName);	// �ִϸ��̼� �ҷ�����
	void				LeftInterpolation();
	void				RightInterpolation();

private:
	void State_Update(float fTimeDelta);	// ���� ������Ʈ(���� üũ)

private: // ����ġ
	_bool		bSpinOn = false;			// �� ȸ�� On/Off
	_bool		bRighter = false;			// ������ On/Off
	_bool		bRightHandOn = true;		// ������ ��� On/Off
	_bool		bRightGetAnimation = false;	// �ִϸ��̼� �ҷ����� On/Off
	_bool		bLeftGetAnimation = false;	// �ִϸ��̼� �ҷ����� On/Off

	// �뽬
	_bool		bDashOn = false;		// �÷��̾� �뽬 ����
	_bool		bDashChange = false;	// �뽬 ���� On/Off

	// �ָ� ����
	_bool		bLeftPunch = false;		// ���ָ� On/Off
	_bool		bRightPunch = true;		// �����ָ� On/Off

	_bool		bShieldOn = true;		// ���� On/Off
	_bool		bChargeAttack = false;	// �Ϲ� ���ݿ��� ��¡ �������� ����
	_bool		bBackRighter = false;	// ������ �ǵ�����
	_bool		bDbugFrame = false;		// ����� ������
	_bool		m_bAttack = false;		// ���� On/Off

private:
	struct _LEFTHAND	// �޼�
	{
		_bool			bLeftHandOn = true;			// �޼� ��� On/Off
		_bool			bLeftFrameOn = false;

		_float			fLeftFrameSpeed = 10.f;		// �޼� ������ �ӵ�
		_float			fLeftFrame = 0.f;			// �޼� ������
	};

	struct _RIGHTHAND	// ������
	{
		_bool			bRightFrameOn = false;
		_bool			bPickUpState = false;

		_float			fRightFrameSpeed = 1.f;		// ������ ������ �ӵ�
		_float			fRightFrame = 0.f;			// ������ ������

		_uint			iFullChargingIndex = 0.f;	// Ǯ������ �ε���
		_uint			iShieldIndex = 0.f;			// ����� �ε���
	};

	struct _PLAYER	// �÷��̾�
	{
		_float		fStraightSpeed = 5.f;		// �÷��̾� ���� �ӵ�
		_float		fSpeed = 5.f;				// �÷��̾� �ӵ�
	};

	struct _DASH	// �뽬
	{
		_float		fDash = 20.f;				// �÷��̾� �뽬
		_float		fDownDash = 0.f;			// �÷��̾� �뽬�Ҷ� ����
	};

	struct _TIME	// �ð�
	{
		_float		fCurrentTime = 0.f;			// ���� �ð�
		_float		fCurChangeTime = 1.f;		// �ð� �ӵ� ����(����)

		_float		fLeftCurrentTime = 0.f;		// �޼� ���� �ð�
		_float		fLeftMaxTime = 0.f;			// �޼� �ִ� �ð�
		_float		fLeftChangeTime = 1.f;		// �ð� �ӵ� ����(����)

		_float		fRightCurrentTime = 0.f;	// ������ ���� �ð�
		_float		fRightMaxTime = 0.f;		// ������ �ִ� �ð�
		_float		fRightChangeTime = 1.f;		// �ð� �ӵ� ����(����)

		_float		fMaxChangeTime = 3.f;		// ����� �ִ� �ð�
		_float		fChargeStartTime = 0.f;		// ������ ������ �ð�
	};


	PLAYER_ATTACK_STATE m_eAttackState; // �÷��̾� ���� ����

	_PLAYER			m_tPlayer;		// �÷��̾�
	_LEFTHAND		m_tLeftHand;	// �÷��̾� �޼�
	_RIGHTHAND		m_tRightHand;	// �÷��̾� ������
	_DASH			m_tDash;		// �뽬
	_TIME			m_tTime;		// �ð�

	GAUGE<_float>		m_fChage;	// ����

	_float		fChageTime = 0.f;			// ��¡ ��ȯ �ð�
	_float		fFullChargeTime = 7.f;		// Ǯ��¡ �ð�

	// Test
	OBJECT_TYPE m_eObjectType;	// ������Ʈ Ÿ��
	OBJECT_NAME m_eObjectName;	// ������Ʈ �̸�

	// �뽬 ����
	DASHDIR m_eDashDir;

	// ������ ����
	CPlayerLighter* m_PlayerLighter;
private:
		// �ִϸ��̼� Ÿ�� ����
		std::vector<KEYFRAME> timeline[KEYTYPE_END];

private:
	_long			dwMouseMove = 0;		// ���콺 ����


private:
	virtual void		Free();

protected:
	CDynamicCamera* m_pCamera = nullptr;

private: // ���� ����
	// dt ��
	_float fFrameTimeDelta, fCurFrameTimeDelta;
	// ũ��
	_float fSizeX_Delta, fSizeY_Delta;
	// ȸ��
	_float fRotX_Delta, fRotY_Delta, fRotZ_Delta;
	// �̵�
	_float fPosX_Delta, fPosY_Delta;
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

//private: // ���ʹϾ�
	//_long dwMouseMoveX;
	//_long dwMouseMoveY;

	//D3DXQUATERNION	m_quaternion;	// ���ʹϿ� ����