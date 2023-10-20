#include "stdafx.h"
#include "Trigger_ToJumpMap.h"

#include <Layer.h>
#include <Scene.h>
#include <Scene_Parsed.h>

#include "Export_System.h"
#include "Export_Utility.h"

CTrigger_ToJumpMap::CTrigger_ToJumpMap(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTrigger_ToJumpMap::CTrigger_ToJumpMap(const CTrigger_ToJumpMap& rhs)
    : Base(rhs)
{
}

CTrigger_ToJumpMap::~CTrigger_ToJumpMap()
{
}

CTrigger_ToJumpMap* CTrigger_ToJumpMap::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
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

HRESULT CTrigger_ToJumpMap::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CTrigger_ToJumpMap::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
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

    return S_OK;
}

_int CTrigger_ToJumpMap::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return 0;
}

void CTrigger_ToJumpMap::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CTrigger_ToJumpMap::Free()
{
    SUPER::Free();
}

void CTrigger_ToJumpMap::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
}

void CTrigger_ToJumpMap::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
}

void CTrigger_ToJumpMap::OnCollision(CGameObject* pDst, const FContact* const pContact)
{

}

void CTrigger_ToJumpMap::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    if (!m_bIsTriggered)
    {
        CScene* pScene = CScene_Parsed::Create(m_pGraphicDev, "SeongHee");
        Engine::Set_Scene(pScene);
        pScene->Add_GameObject(L"UI", CUI_SceneChange::Create(m_pGraphicDev));
        Set_Dead();
        m_bIsTriggered = true;
    }
}

void CTrigger_ToJumpMap::OnCollisionExited(CGameObject* pDst)
{
    
}

HRESULT CTrigger_ToJumpMap::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}
