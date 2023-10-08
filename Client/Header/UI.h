#pragma once

#include "GameObject.h"


/// <summary>
/// UI�� �⺻�� �Ǵ� �߻� Ŭ����
/// </summary>
class CUI abstract : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CUI)

protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUI(const CUI& rhs);
	virtual ~CUI();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdate_GameObject() override;
	virtual void	Render_GameObject() PURE;
};

