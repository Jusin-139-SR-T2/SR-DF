#include "Awareness.h"

CAwareness::CAwareness(LPDIRECT3DDEVICE9 pGraphicDev)
	:Base(pGraphicDev)
{
}

CAwareness::CAwareness(const CAwareness& rhs)
	:Base(rhs)
{
}

CAwareness::~CAwareness()
{
}

HRESULT CAwareness::Ready_GameObject()
{
	return S_OK;
}

_int CAwareness::Update_GameObject(const _float& fTimeDelta)
{
	return 0;
}

void CAwareness::LateUpdate_GameObject()
{
}

void CAwareness::Render_GameObject()
{
}

CAwareness* CAwareness::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z, CAwareness::TYPE pType, CGameObject* pOwner)
{
	return nullptr;
}

HRESULT CAwareness::Add_Component()
{
	return S_OK;
}

void CAwareness::Free()
{
}

HRESULT CAwareness::Billboard_Aware()
{
	return S_OK;
}

void CAwareness::Update_Awareness(CAwareness::TYPE pType)
{
}

void CAwareness::Update_BlackBoard()
{
}
