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

    FadeInEnd = false;

    return S_OK;
}

_int CTrigger_ToBossMap::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Update_InternalData();

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
    if (FadeInEnd)
    {
        if (!m_bIsTriggered)
        {
            CScene* pScene = CScene_Parsed::Create(m_pGraphicDev, "Malone");
            Engine::Set_Scene(pScene);
            pScene->Add_GameObject(L"UI", CUI_FadeIn::Create(m_pGraphicDev));
            m_bIsTriggered = true;
            Update_InternalData();
            Set_Dead();
        }
    }
}

void CTrigger_ToBossMap::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    Engine::Add_GameObject(L"UI", CUI_FadeOut::Create(m_pGraphicDev));// 페이드아웃 먼저 생성
}

void CTrigger_ToBossMap::OnCollisionExited(CGameObject* pDst)
{
    
}

HRESULT CTrigger_ToBossMap::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}

void CTrigger_ToBossMap::Update_InternalData()
{ 
    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    {
        m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
        // 연결 실패
        if (!m_wpBlackBoard_Player.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // 여기서부터 블랙보드의 정보를 얻어온다.
    FadeInEnd = pBlackBoard->Get_SceneFade();

    if (m_bIsTriggered)
        pBlackBoard->Get_SceneFade() = false;
}
