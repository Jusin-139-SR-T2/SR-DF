#pragma once

#include "Component.h"

BEGIN(Engine)

/// <summary>
/// ������Ʈ�� �Ļ����� ���ӿ�����Ʈ�� �߰��Ǿ� ���̴� ������Ʈ�� �����̴�.
/// ����� �����Ǳ� ���̶� ���������� ���ӿ�����Ʈ���� ������ �ʰ� CComponent�� ����.
/// ���� �߰����� ����� �����ϱ� ���� �������� ���� ������ ������ �ӽ÷� ��������.
/// </summary>
class ENGINE_DLL CActorComponent : public CComponent
{
	DERIVED_CLASS(CComponent, CActorComponent)

protected:
	explicit CActorComponent();
	explicit CActorComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CActorComponent(const CActorComponent& rhs);
	virtual ~CActorComponent();

public:
	virtual CComponent*		Clone() PURE;
	virtual void			Free();

public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}
};

END