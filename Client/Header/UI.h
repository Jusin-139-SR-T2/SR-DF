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
	GETSET_EX2(CRcTex*, m_pBufferComp, BufferComponent, GET, SET)
	GETSET_EX2(CTransform*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTexture*, m_pTextureComp, TextureComponent, GET, SET)
	GETSET_EX2(CCalculator*, m_pCalculatorComp, CalculatorComponent, GET, SET)

private:
	CRcTex* m_pBufferComp = nullptr;
	CTransform* m_pTransformComp = nullptr;
	CTexture* m_pTextureComp = nullptr;
	CCalculator* m_pCalculatorComp = nullptr;

	CGameObject* m_pTarget = nullptr;

public:
	void		Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	_matrix m_matRotAxis;

public:
	GETSET_EX2(CDynamicCamera*, m_pCamera, Camera, GET, SET)
	
protected:
	CDynamicCamera* m_pCamera = nullptr;

private: 
	float fHp;
};

