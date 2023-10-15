#include "stdafx.h"
#include "AceUnit.h"

CAceUnit::CAceUnit(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceUnit::CAceUnit(const CAceUnit& rhs)
    : Base(rhs)
{
}

CAceUnit::~CAceUnit()
{
}

HRESULT CAceUnit::Ready_GameObject()
{
    return S_OK;
}

_int CAceUnit::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CAceUnit::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceUnit::Render_GameObject()
{
}

void CAceUnit::Free()
{
    SUPER::Free();
}

void CAceUnit::MeshBoxColider(FLOAT _Width, FLOAT _Height, FLOAT Depth)
{
    SUPER::MeshBoxColider(_Width, _Height, Depth);
}

void CAceUnit::MeshSphereColider(FLOAT Radius, UINT Slices, UINT Stacks)
{
    SUPER::MeshSphereColider(Radius, Slices, Stacks);
}