#include "stdafx.h"
#include "Trigger_Box.h"

#include <Layer.h>
#include <Scene.h>
#include <Scene_Parsed.h>

#include "Export_System.h"
#include "Export_Utility.h"

CTrigger_Box::CTrigger_Box(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTrigger_Box::CTrigger_Box(const CTrigger_Box& rhs)
    : Base(rhs)
{
}

CTrigger_Box::~CTrigger_Box()
{
}

CTrigger_Box* CTrigger_Box::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("RectObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CTrigger_Box::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CTrigger_Box::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(D3DXToRadian(tObjectSerial.vRotation));
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    FadeInEnd = false;

    return S_OK;
}

_int CTrigger_Box::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);


    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return 0;
}

void CTrigger_Box::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CTrigger_Box::Free()
{
    SUPER::Free();
}

void CTrigger_Box::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
}

void CTrigger_Box::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
}

void CTrigger_Box::OnCollision(CGameObject* pDst, const FContact* const pContact)
{

}

void CTrigger_Box::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{

}

void CTrigger_Box::OnCollisionExited(CGameObject* pDst)
{

}

HRESULT CTrigger_Box::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}
