#pragma once

#include "GameObject.h"

#include "Engine_Macro.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCameraComponent;
class CCalculator;
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
private:
	virtual void		Free();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

// 플레이어 상태 머신1
public:
	enum class STATE_RIGHTHAND { IDLE, HAND, GUN };
private:
	STATE_SET<STATE_RIGHTHAND, void(CPlayer*, float)> m_tState;

	void Idle(float fTimeDelta);
	void HandAttack(float fTimeDelta);
	void GunAttack(float fTimeDelta);

// 플레이어 상태 머신2
public:
	enum class STATE2 { NONE };
private:
	STATE_SET<STATE2, void(CPlayer*, float)> m_tState2;

private:
	HRESULT				Add_Component();
	void				Key_Input(const _float& fTimeDelta);
	void				Mouse_Move();
	void				Height_On_Terrain();

public:
	GETSET_EX2(CRcTex*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransform*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTexture*, m_pLeftHandTextureComp, TextureComponent, GET, SET)
	//GETSET_EX2(CTexture*, m_pRightHandTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CCalculator*, m_pCalculatorComp, CalculatorComponent, GET, SET)
	GETSET_EX2(CSphereColComp*, m_pColliderComp, SphereColComponent, GET, SET)

private:
	CRcTex*				m_pBufferComp = nullptr;
	CTransform*			m_pTransformComp = nullptr;

	vector<CTexture*>	m_vecTexture;
	CTexture*			m_pLeftHandTextureComp = nullptr;
	CTexture*			m_pRightHandTextureComp = nullptr;
	CTexture*			m_pAttackTextureComp = nullptr;
	CTexture*			m_pAttackSpinTextureComp = nullptr;
	CCalculator*		m_pCalculatorComp = nullptr;
	CSphereColComp*		m_pColliderComp = nullptr;

private:
	_bool		bAttackOn = false;
	_bool		bFrameOn = false;
	_bool		bGunOn = false;
	_bool		bSpinOn = false;

private:
	_long			dwMouseMove = 0;
	_float			m_fFrame = 0.f;
	_float			m_fMaxFrame = 0.f;

public:
	GETSET_EX2(CDynamicCamera*, m_pCamera, Camera, GET, SET)

protected:
	CDynamicCamera*		m_pCamera = nullptr;

protected:
	_vec3		m_vEye, m_vAt, m_vUp;
	_matrix		m_matRot;
};