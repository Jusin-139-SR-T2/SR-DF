#pragma once

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;

/// <summary>
/// ��� ������Ʈ Ŭ������ ���̽� Ŭ�����̴�.
/// </summary>
class _declspec(dllexport) CComponent : public CBase
{
	DERIVED_CLASS(CBase, CComponent)
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();

public:
	virtual CComponent*		Clone() PURE;
	virtual void			Free();

protected:
	// ���纻 �ν��Ͻ� ����
	_bool					m_bClone;

public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}

protected:
	// ���� ��ġ
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	
public:
	GETSET_EX3(CGameObject*, m_pOwner, Owner, SET, GET, GET_C_C)

protected:
	// ���� ������Ʈ
	CGameObject*			m_pOwner;
	
};

END