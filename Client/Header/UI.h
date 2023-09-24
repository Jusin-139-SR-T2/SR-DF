#pragma once

#include "GameObject.h"
#include "Engine_Macro.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCameraComponent;
class CCalculatorComponent;

END

class CDynamicCamera;
class CPlayer;

class CUI : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CUI)
private:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI(const CUI& rhs);
	virtual ~CUI();

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private: 
	virtual void Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int Update_GameObject(const _float& fTimeDelta) override;
	virtual void LateUpdate_GameObject() override;
	virtual void Render_GameObject() override;

private:
	HRESULT Add_Component();
	void Key_Input(const _float& fTimeDelta);

public:
	GETSET_EX2(CRcBufferComp*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTextureComponent*, m_pTextureComp, TextureComponent, GET, SET)

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;

	CGameObject* m_pTarget = nullptr;

public:
	void		Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	_matrix		m_matRotAxis;

private: 
	float fHp;
};

