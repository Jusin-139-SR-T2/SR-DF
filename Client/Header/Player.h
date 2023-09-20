#pragma once

#include "GameObject.h"

#include "Engine_Macro.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCameraComponent;
class CCalculatorComponent;
class CSphereColComp;

END

class CDynamicCamera;

class CPlayer : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CPlayer)
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	GETSET_EX2(CDynamicCamera*, m_pCamera, Camera, GET, SET)

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pLeftHandTextureComp, LeftHandTextureComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pRightHandTextureComp, RightHandTextureComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(CSphereColComp*, m_pColliderComp, SphereColComponent, GET, SET)


public:// �÷��̾� ���� ��
	// �÷��̾�
	enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, DIE };
	// �޼�
	enum class STATE_LEFTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, RIGHTER, RUN_RIHGTER, STEELPIPE };
	// ������
	enum class STATE_RIGHTHAND { NONE, HAND, RUN_HAND, GUN, THOMPSON, STEELPIPE };
	
// TEST
	enum class OBJECT_TYPE { NONE, GUN, THOMPSON, STEELPIPE  }; // �׽�Ʈ(������Ʈ)
// TEST
private: // �÷��̾��� ���� �ӽ�
	STATE_SET<STATE_PLAYER, void(CPlayer*, float)> m_tPlayer_State;

	void Idle(float fTimeDelta);
	void Move(float fTimeDelta);
	void Run(float fTimeDelta);
	void Down(float fTimeDelta);
	void Attack(float fTimeDelta);
	void Die(float fTimeDelta);

private: // �÷��̾��� �޼� ���� �ӽ�
	STATE_SET<STATE_LEFTHAND, void(CPlayer*, float)> m_tLeftHand_State;

	void	Left_Hand(float fTimeDelta);
	void	Left_Gun(float fTimeDelta);
	void	Left_Thompson(float fTimeDelta);
	void	Left_Steelpipe(float fTimeDelta);
	void	Left_Righter(float fTimeDelta);

private: // �÷��̾��� ������ ���� �ӽ�
	STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tRightHand_State;

	void	Right_Hand(float fTimeDelta);
	void	Right_Gun(float fTimeDelta);
	void	Right_Thompson(float fTimeDelta);
	void	Right_Steelpipe(float fTimeDelta);

private:
	HRESULT				Add_Component();							// ������Ʈ �߰�
	bool				Keyboard_Input(const _float& fTimeDelta);	// Ű���� �Է�
	bool				Attack_Input(const _float& fTimeDelta);		// ���� �Է�(���콺)
	void				Mouse_Move();								// ���콺 ������
	void				Height_On_Terrain();						// ����Ÿ��
	void				Hand_Check();								// �÷��̾� �� ���� üũ

private:
	void State_Update(float fTimeDelta);	// ���� ������Ʈ(���� üũ)

private:
	CRcBufferComp*				m_pBufferComp = nullptr;			// Rc����
	CTransformComponent*		m_pTransformComp = nullptr;			// �̵���� ����

	// �÷��̾� ��
	CTextureComponent*			m_pLeftHandComp = nullptr;			// �޼� �ؽ�ó(������)
	CTextureComponent*			m_pRightHandComp = nullptr;			// ������ �ؽ�ó(������)


	CTextureComponent*			m_pAttackTextureComp = nullptr;		// ����(��) �ؽ�ó
	CTextureComponent*			m_pAttackSpinTextureComp = nullptr;	// �� ȸ�� �ؽ�ó
	CCalculatorComponent*		m_pCalculatorComp = nullptr;		// ���� Ÿ�� ������Ʈ
	CSphereColComp*				m_pColliderComp = nullptr;			// �� �浹 �ݶ��̴�

	// Test
	CTextureComponent*			m_pLeftHandTextureComp = nullptr;			// �޼�(�̹���) �ؽ�ó
	CTextureComponent*			m_pRightHandTextureComp = nullptr;			// ������(�̹���) �ؽ�ó
	CTextureComponent*			m_pRightSteelPipeTextureComp = nullptr;		// ��������(�̹���) �ؽ�ó
	CTextureComponent*			m_pLeftZippoTextureComp = nullptr;			// ������(�̹���) �ؽ�ó

private:
	_bool		bAttackOn = false;		// ����
	_bool		bLeftFrameOn = false;
	_bool		bRightFrameOn = false;
	_bool		bGunOn = false;
	_bool		bSpinOn = false;
	_bool		bDead = false;
	_bool		bMove = false;
	_bool		bMouse_Button = false;
	_bool		bFootAttack = false;
	_bool		bRighter = false;
	_bool		bRunOn = false;

	_float		fSpeed = 5.f;

	// Test
	OBJECT_TYPE m_eObjectType;

private:
	_long			dwMouseMove = 0;
	_float			m_fLeftFrame = 0.f;
	_float			m_fRightFrame = 0.f;
	_float			m_fMaxFrame = 0.f;

private:
	virtual void		Free();

protected:
	CDynamicCamera*		m_pCamera = nullptr;

protected:
	_vec3		m_vEye, m_vAt, m_vUp;
	_matrix		m_matRot;


	//��
private:
	//HRESULT			SetUp_Material();
	_bool			bTorch = false;

};