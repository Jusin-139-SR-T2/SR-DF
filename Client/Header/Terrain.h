#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CTerrainTexComponent;

END

class CTerrain : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CTerrain)
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain();

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void	Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	HRESULT			Add_Component();
	HRESULT			SetUp_Material();

private:
	CTerrainTexComponent*		m_pBufferComp = nullptr;
	CTexture*					m_pTextureComp = nullptr;
	CTransform*					m_pTransformComp = nullptr;

};

