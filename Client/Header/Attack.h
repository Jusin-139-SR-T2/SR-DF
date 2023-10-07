//#pragma once
//
//#include "GameObject.h"
//
//#include "Export_System.h"
//#include "Export_Utility.h"
//
//BEGIN(Engine)
//
//class CTransformComponent;
//
//END
//
//class CAttack abstract : public Engine::CGameObject
//{
//	DERIVED_CLASS(CGameObject, CAttack)
//
//protected:
//	explicit CAttack(LPDIRECT3DDEVICE9 pGraphicDev);
//	explicit CAttack(const CAttack& rhs);
//	virtual ~CAttack();
//
//public:
//	virtual HRESULT Ready_GameObject();
//	virtual _int	Update_GameObject(const _float& fTimeDelta);
//	virtual void	LateUpdate_GameObject();
//	virtual void	Render_GameObject()	PURE;
//
//private:
//	HRESULT				Add_Component();
//	virtual void		Free();
//
//protected:
//	CTransformComponent* m_pTransformComp = nullptr;
//
//};