#pragma once
#include "Monster.h"

class CPunchGuy : public CMonster
{
    DERIVED_CLASS(CPunchGuy, CMonster)

private:
	explicit CPunchGuy(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPunchGuy(const CMonster& rhs);
	virtual ~CPunchGuy();


public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() override;

private:
	HRESULT			Add_Component();
	virtual void	Free();

private:
	CRcTex* m_pBufferComp = nullptr;
	CTexture* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CGameObject* m_pTarget = nullptr;

public:

};

