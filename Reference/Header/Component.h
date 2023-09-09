#pragma once

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;

/// <summary>
/// 컴포넌트 클래스의 베이스
/// </summary>
class _declspec(dllexport) CComponent : public CBase
{
	DERIVED_CLASS(CComponent, CBase)
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();

public:
	virtual CComponent*		Clone() PURE;
	virtual void			Free();

protected:
	// 복사본 인스턴스 여부
	_bool					m_bClone;



public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}

protected:
	// 렌더 장치
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	


public:
	void Set_Owner(CGameObject* value) { m_pOwner = value; }
	CGameObject* Get_Owner() { return m_pOwner; }
	const CGameObject* const Get_Owner() const { return m_pOwner; }

protected:
	// 오너 오브젝트
	CGameObject*			m_pOwner;
	
};

END