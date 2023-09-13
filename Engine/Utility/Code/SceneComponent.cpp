#include "SceneComponent.h"

CSceneComponent::CSceneComponent()
{
}

CSceneComponent::CSceneComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CSceneComponent::CSceneComponent(const CSceneComponent& rhs)
	: Base(rhs)
{
}

CSceneComponent::~CSceneComponent()
{
}

void CSceneComponent::Free()
{
	SUPER::Free();
}
