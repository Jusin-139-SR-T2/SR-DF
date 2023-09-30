#pragma once

#include "AceObjectFactory.h"
#include "GameObject.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;
class CCalculatorComponent;

END

enum class WEAPON_NAME { BOTTLE, PIPE, FRYINGPAN, PISTOL, TOMMYGUN, GASCANISTER, WEAPON_END };

class CAceWeapon : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CAceWeapon)

private:
	explicit CAceWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAceWeapon(const CAceWeapon& rhs);
	virtual ~CAceWeapon();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;
	static CAceWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag);

private:
	virtual void		Free();
	HRESULT				Add_Component();
	void				Height_On_Terrain();
	HRESULT				BillBoard(const _float& fTimeDelta); // 플레이어쪽으로 향하는 함수 
	void				WeaponName(const _tchar* pObjTag);
	WEAPON_NAME			m_pReceiveName;

private:
	CRcBufferComp* m_pBufferComp = nullptr;
	CTextureComponent* m_pTextureComp = nullptr;
	CTransformComponent* m_pTransformComp = nullptr;
	CCalculatorComponent* m_pCalculatorComp = nullptr;
};

