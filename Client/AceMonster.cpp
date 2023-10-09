#include "stdafx.h"
#include "AceMonster.h"

CAceMonster::CAceMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceMonster::CAceMonster(const CAceMonster& rhs)
    : Base(rhs)
{
}

CAceMonster::~CAceMonster()
{
}

HRESULT CAceMonster::Ready_GameObject()
{
    return S_OK;
}

_int CAceMonster::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    return S_OK;
}

void CAceMonster::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceMonster::Render_GameObject()
{
}

void CAceMonster::Free()
{
    SUPER::Free();
}
