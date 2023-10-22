#include "stdafx.h"
#include "JumpBat.h"

#include <Layer.h>
#include <Scene.h>
#include <Scene_Parsed.h>

#include "Export_System.h"
#include "Export_Utility.h"

CJumpBat::CJumpBat(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CJumpBat::CJumpBat(const CJumpBat& rhs)
    : Base(rhs)
{
}

CJumpBat::~CJumpBat()
{
}

CJumpBat* CJumpBat::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
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

HRESULT CJumpBat::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CJumpBat::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
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

_int CJumpBat::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Update_InternalData();

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    return 0;
}

void CJumpBat::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CJumpBat::Free()
{
    SUPER::Free();
}

void CJumpBat::MeshBoxColider(_float _Width, _float _Height, _float Depth)
{
}

void CJumpBat::MeshSphereColider(_float Radius, _uint Slices, _uint Stacks)
{
}

void CJumpBat::OnCollision(CGameObject* pDst, const FContact* const pContact)
{

}

void CJumpBat::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
    // ������ ���� ����� ���� (3~5������ ������ ���ư�)
}

void CJumpBat::OnCollisionExited(CGameObject* pDst)
{

}

HRESULT CJumpBat::Add_Component()
{
    SUPER::Add_Component();

    return S_OK;
}

void CJumpBat::Update_InternalData()
{
    //// ������ ���� ���, ���� �ڵ�� �ʼ�
    //if (!m_wpBlackBoard_Player.Get_BlackBoard())
    //{
    //    m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
    //    // ���� ����
    //    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    //        return;
    //}

    //// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
    //CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    //// ���⼭���� �������� ������ ���´�.
    //FadeInEnd = pBlackBoard->Get_SceneFade();

    //if (m_bIsTriggered)
    //    pBlackBoard->Get_SceneFade() = false;
}
