#pragma once

#include "GameObject.h"

#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CTransformComponent;

END

class CAceLighter : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceLighter)

private:
	explicit CAceLighter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceLighter(const CAceLighter& rhs);
	virtual ~CAceLighter();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CAceLighter* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void				Height_On_Terrain();
	HRESULT				Add_Component();
	virtual void		Free();

private:
	CTransformComponent* m_pTransformComp = nullptr;
	CTransformComponent* m_pPlayerTransformcomp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;

	// Get, Set �Լ� ����� ---------------------------------------------------------------
public:
	GETSET_EX2(CTransformComponent*, m_pTransformComp, TransformComponent, GET, SET)
	GETSET_EX2(CTransformComponent*, m_pPlayerTransformcomp, PlayerTransformComponent, GET, SET)
	GETSET_EX2(CCalculatorComponent*, m_pCalculatorComp, CalculatorComponent, GET, SET)

private:
	HRESULT Create_Light(); // ������ ����� 
	D3DLIGHT9 AceLight; // ������ 
	_bool m_bLightOn = false; // ������ �����ؼ� ������ �÷��̾� ������ on���� 

};