#include "stdafx.h"
#include "Player.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"
#include "Calculator.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
    : Base(rhs)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->m_vScale.x = 1.f;

    m_pTransformComp->m_vInfo[INFO_POS] = { 15.f, 10.f, 10.f };

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Key_Input(fTimeDelta);

    // 플레이어 마우스 무브
    Mouse_Move();

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
}

void CPlayer::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pLeftTextureComp->Render_Texture(0);
    m_pBufferComp->Render_Buffer();

    m_pRightTextureComp->Render_Texture(0);
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlayer::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcTex>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransform>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculator>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CSphereColComp>(ID_STATIC, L"Comp_SphereCollider", L"Proto_SphereColComp"), E_FAIL);

    // 플레이어 왼손 텍스처
    NULL_CHECK_RETURN(m_pLeftTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_Texture", L"Proto_PlayerLeftTextureComp"), E_FAIL);
    // 플레이어 오른손 텍스처
    NULL_CHECK_RETURN(m_pRightTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_Texture", L"Proto_PlayerRightTextureComp"), E_FAIL);

    return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);
    vLook = { vLook.x, 0.f, vLook.z };

    _vec3 vInverse = (-m_pCamera->Get_At());

    m_pTransformComp->Compute_LootAtTarget(&vInverse);

    // 전진
    if (Engine::Get_DIKeyState(DIK_UP) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, 5.f);
    }

    // 후진
    if (Engine::Get_DIKeyState(DIK_DOWN) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -5.f);
    }

    // 오른쪽
    if (Engine::Get_DIKeyState(DIK_RIGHT) & 0x80)
    {
        m_pTransformComp->Rotation(ROT_Y, D3DXToRadian(90.f * fTimeDelta));
    }

    // 왼쪽
    if (Engine::Get_DIKeyState(DIK_LEFT) & 0x80)
    {
        m_pTransformComp->Rotation(ROT_Y, D3DXToRadian(-90.f * fTimeDelta));
    }
}

// 마우스 움직임
void CPlayer::Mouse_Move()
{
    // 마우스로 플레이어 회전
    // 상, 하
    if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
    {
        m_pTransformComp->Rotation(ROT_X, D3DXToRadian(dwMouseMove / 10.f));
    }
    // 좌, 우
    if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
    {
        m_pTransformComp->Rotation(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));
    }
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Player Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CPlayer::Free()
{
    SUPER::Free();
}
