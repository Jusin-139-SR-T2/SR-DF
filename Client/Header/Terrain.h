#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CTerrainBufferComp;

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
	CTerrainBufferComp*		m_pBufferComp = nullptr;
	CTextureComponent*					m_pTextureComp = nullptr;
	CTransformComponent*					m_pTransformComp = nullptr;

};

