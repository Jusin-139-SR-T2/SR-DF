#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CCubeTexComponent;
class CTexture;
class CTransform;
class CCalculator;

END

class CSkyBox : public Engine::CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	HRESULT			Add_Component();

private:
	CTransform* m_pTransformComp = nullptr;
	CCalculator* m_pCalculatorComp = nullptr;
	CTexture* m_pTextureComp = nullptr;
	CCubeTexComponent* m_pCubeBufferComp = nullptr;

public:
	static CSkyBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();


};