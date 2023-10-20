#include "stdafx.h"
#include "Trigger_ToBossMap.h"

#include <Layer.h>
#include <Scene.h>
#include <Scene_Parsed.h>

#include "Export_System.h"
#include "Export_Utility.h"

CTrigger_ToBossMap::CTrigger_ToBossMap(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTrigger_ToBossMap::CTrigger_ToBossMap(const CTrigger_ToBossMap& rhs)
    : Base(rhs)
{
}

CTrigger_ToBossMap::~CTrigger_ToBossMap()
{
}

CTrigger_ToBossMap* CTrigger_ToBossMap::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
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

HRESULT CTrigger_ToBossMap::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CTrigger_ToBossMap::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
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
}

_int CTrigger_ToBossMap::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return 0;
}

void CTrigger_ToBossMap::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CTrigger_ToBossMap::Free()
{
    SUPER::Free();
}

void CTrigger_ToBossMap::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
}

void CTrigger_ToBossMap::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
}

void CTrigger_ToBossMap::OnCollision(CGameObject* pDst, const FContact* const pContact)
{

}

void CTrigger_ToBossMap::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    if (!m_bIsTriggered)
    {
        CScene* pScene = CScene_Parsed::Create(m_pGraphicDev, "Malone");
        Engine::Set_Scene(pScene);
        Set_Dead();
        m_bIsTriggered = true;
    }
}

void CTrigger_ToBossMap::OnCollisionExited(CGameObject* pDst)
{
    
}

HRESULT CTrigger_ToBossMap::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}
