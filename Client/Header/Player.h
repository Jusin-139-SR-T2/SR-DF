#pragma once

#include "GameObject.h"

#include "Engine_Macro.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCameraComponent;
class CCalculator;

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

private:
	HRESULT				Add_Component();
	void				Key_Input(const _float& fTimeDelta);
	void				Mouse_Move();

public:
	GETSET_EX2(CRcTex*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransform*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTexture*, m_pLeftTextureComp, TextureComponent, GET, SET)
	//GETSET_EX2(CTexture*, m_pRightTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CCalculator*, m_pCalculatorComp, CalculatorComponent, GET, SET)

private:
	CRcTex*				m_pBufferComp = nullptr;
	CTransform*			m_pTransformComp = nullptr;
	CTexture*			m_pLeftTextureComp = nullptr;
	CTexture*			m_pRightTextureComp = nullptr;
	CCalculator*		m_pCalculatorComp = nullptr;

private:
	_long			dwMouseMove = 0;

public:
	GETSET_EX2(CDynamicCamera*, m_pCamera, Camera, GET, SET)

protected:
	CDynamicCamera*		m_pCamera = nullptr;

protected:
	_vec3			m_vEye, m_vAt, m_vUp;
	_matrix		m_matRot;
};

