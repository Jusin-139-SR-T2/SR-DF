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


public:
// 플레이어 상태 머신
	enum class STATE_PLAYER { NONE, IDLE, MOVE, RUN, DOWN, ATTACK, DIE };
	enum class STATE_LEFTHAND { NONE, HAND, GUN };
	enum class STATE_RIGHTHAND { NONE, HAND, GUN };
	
// TEST
	enum class OBJECT_TYPE { NONE, GUN, NON_WEAPON, BOTH_HANDS,  };
// TEST
private:
	STATE_SET<STATE_PLAYER, void(CPlayer*, float)> m_tPlayer_State;

	void Idle(float fTimeDelta);
	void Move(float fTimeDelta);
	void Run(float fTimeDelta);
	void Down(float fTimeDelta);
	void Attack(float fTimeDelta);
	void Die(float fTimeDelta);

private:
	STATE_SET<STATE_LEFTHAND, void(CPlayer*, float)> m_tLeftHand_State;

	//void	Idle();
	//void	Attack();

private:
	STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tRightHand_State;

private:
	HRESULT				Add_Component();
	bool				Keyboard_Input(const _float& fTimeDelta);
	bool				Attack_Input(const _float& fTimeDelta);
	void				Mouse_Move();
	void				Height_On_Terrain();

private:
	void State_Update(float fTimeDelta);

private:
	CRcBufferComp*				m_pBufferComp = nullptr;
	CTransformComponent*			m_pTransformComp = nullptr;

	vector<CTextureComponent*>	m_vecTexture;
	CTextureComponent*			m_pLeftHandTextureComp = nullptr;
	CTextureComponent*			m_pRightHandTextureComp = nullptr;
	CTextureComponent*			m_pAttackTextureComp = nullptr;
	CTextureComponent*			m_pAttackSpinTextureComp = nullptr;
	CCalculatorComponent*		m_pCalculatorComp = nullptr;
	CSphereColComp*		m_pColliderComp = nullptr;

private:
	_bool		bAttackOn = false;
	_bool		bFrameOn = false;
	_bool		bGunOn = false;
	_bool		bSpinOn = false;
	_bool		bDead = false;
	_bool		bMove = false;
	_bool		bMouse_Button = false;
	_bool		bFootAttack = false;

	OBJECT_TYPE m_eObjectType;

private:
	_long			dwMouseMove = 0;
	_float			m_fFrame = 0.f;
	_float			m_fMaxFrame = 0.f;

private:
	virtual void		Free();

protected:
	CDynamicCamera*		m_pCamera = nullptr;

protected:
	_vec3		m_vEye, m_vAt, m_vUp;
	_matrix		m_matRot;


	//빛
private:
	//HRESULT			SetUp_Material();
	_bool			bTorch = false;

};