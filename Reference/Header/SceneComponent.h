#pragma once

#include "ActorComponent.h"

BEGIN(Engine)

/// <summary>
/// ActorComponent���� ��ǥ�踦 �� �� �ֵ��� ����� �߰��� ������Ʈ�̴�.
/// TransformComponent�ʹ� ����
/// </summary>
class ENGINE_DLL CSceneComponent : public CActorComponent
{
	DERIVED_CLASS(CActorComponent, CSceneComponent)

protected:
	explicit CSceneComponent();
	explicit CSceneComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSceneComponent(const CSceneComponent& rhs);
	virtual ~CSceneComponent();

public:
	virtual CComponent*		Clone() PURE;
	virtual void			Free();

public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}
};

END