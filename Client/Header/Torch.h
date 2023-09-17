// 플레이어가 가지는 점광원 구현할곳 
// -> 감쇠에 대해 값을 넣어줘야지 점광원이됨 
#pragma once

#include "GameObject.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CTransformComponent;

END


class CTorch : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CTorch)

private:
	explicit CTorch(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTorch(const CTorch& rhs);
	virtual ~CTorch();

private:
	virtual HRESULT Ready_GameObject() override;
	virtual _int	Update_GameObject(const _float& fTimeDelta) override;
	virtual void	Render_GameObject() override;

private:
	HRESULT			Add_Component();
	_bool			bActive = false;

private:
	CTransformComponent* m_pTransformComp = nullptr;

public:
	static CTorch* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();

};

