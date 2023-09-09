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
    //m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    //m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

    //m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    //m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    /*m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);*/

    // A(R.G.B)-> (AR AG AB) * (A`R A`G A`B)

    //(sAR sAG sAB) + (1.f - sA)(R G B)


    m_pTextureComp->Render_Texture(0);

    m_pBufferComp->Render_Buffer();

    //m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    //m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    //m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

HRESULT CPlayer::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcTex>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_Texture", L"Proto_PlayerTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransform>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculator>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

#pragma region 레거시
    //CComponent* pComponent = nullptr;

    // 버퍼 컴포넌트
    /*NULL_CHECK_RETURN((pComponent = m_pBufferCom = Engine::Clone_Proto<CRcTex>(L"Proto_RcTexComp")), E_FAIL);
    FAILED_CHECK_RETURN(Bind_Component(ID_STATIC, pComponent, L"Com_Buffer"), E_FAIL);*/
    //NULL_CHECK_RETURN(m_pBufferCom = Set_DefaultComponent_FromProto<CRcTex>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexComp"));

    // 텍스처 컴포넌트
    /*NULL_CHECK_RETURN((pComponent = m_pTextureCom = Engine::Clone_Proto<CTexture>(L"Proto_PlayerTextureComp")), E_FAIL);
    FAILED_CHECK_RETURN(Bind_Component(ID_STATIC, pComponent, L"Com_Texture"), E_FAIL);*/
    //NULL_CHECK_RETURN(m_pTextureCom = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_Texture", L"Proto_PlayerTextureComp"));

    // 트랜스폼 컴포넌트
    /*NULL_CHECK_RETURN((pComponent = m_pTransformCom = Engine::Clone_Proto<CTransform>(L"Proto_TransformComp")), E_FAIL);
    FAILED_CHECK_RETURN(Bind_Component(ID_DYNAMIC, pComponent, L"Com_Transform"), E_FAIL);*/
    //NULL_CHECK_RETURN(m_pTransformCom = Set_DefaultComponent_FromProto<CTransform>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"));

    // 지형 계산 컴포넌트
    /*NULL_CHECK_RETURN((pComponent = m_pCalculatorCom = Engine::Clone_Proto<CCalculator>(L"Proto_Calculator")), E_FAIL);
    FAILED_CHECK_RETURN(Bind_Component(ID_STATIC, pComponent, L"Com_Calculator"), E_FAIL);*/
    //NULL_CHECK_RETURN(m_pCalculatorCom = Set_DefaultComponent_FromProto<CCalculator>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"));
#pragma endregion

    return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
    _vec3	vLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vLook);
    vLook = { vLook.x, 0.f, vLook.z };

    if (Engine::Get_DIKeyState(DIK_W) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, 5.f);
    }

    if (Engine::Get_DIKeyState(DIK_S) & 0x80)
    {
        D3DXVec3Normalize(&vLook, &vLook);
        m_pTransformComp->Move_Pos(&vLook, fTimeDelta, -5.f);
    }

    _vec3 vInverse = (- m_pCamera->Get_At());
    
    m_pTransformComp->Compute_LootAtTarget(&vInverse);

    //if (GetAsyncKeyState('E') & 0x8000)
    //{
    //    D3DXVec3Normalize(&vDir, &vDir);
    //    m_pTransformCom->Move_Pos(&vDir, fTimeDelta, 5.f);
    //}

    //if (GetAsyncKeyState('Q') & 0x8000)
    //{
    //    D3DXVec3Normalize(&vDir, &vDir);
    //    m_pTransformCom->Move_Pos(&vDir, fTimeDelta, -5.f);
    //}

    ////D3DXMatrixRotationAxis(m_pTransformCom->Get_WorldMatrix(), &vDir, D3DXToRadian(-90.f * fTimeDelta));

    //if (GetAsyncKeyState('A') & 0x8000)
    //{
    //    m_pTransformCom->Rotation(ROT_Z, D3DXToRadian(90.f * fTimeDelta));
    //}

    //if (GetAsyncKeyState('D') & 0x8000)
    //{
    //    m_pTransformCom->Rotation(ROT_Z, D3DXToRadian(-90.f * fTimeDelta));
    //}

    /*if (GetAsyncKeyState('W') & 0x8000)
    {
        m_pTransformCom->Rotation(ROT_X, D3DXToRadian(90.f * fTimeDelta));
    }

    if (GetAsyncKeyState('S') & 0x8000)
    {
        m_pTransformCom->Rotation(ROT_X, D3DXToRadian(-90.f * fTimeDelta));
    }*/

    /*if (GetAsyncKeyState('Q') & 0x8000)
    {
        m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(90.f * fTimeDelta));
    }

    if (GetAsyncKeyState('E') & 0x8000)
    {
        m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(-90.f * fTimeDelta));
    }*/
}

void CPlayer::Mouse_Move()
{
    // 마우스로 이동하는 동작
    _long	dwMouseMove = 0;
    _matrix matWorld;

    

    /*if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
    {
        _vec3	vRight;
        memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));
        _vec3		vLook = m_vAt - m_vEye;
        _matrix		matRot;

        D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
        m_vAt = m_vEye + vLook;
    }

    if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
    {
        _vec3	vUp = { 0.f, 1.f, 0.f };
        _vec3		vLook = m_vAt - m_vEye;
        _matrix		matRot;

        D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
        m_vAt = m_vEye + vLook;
    }*/
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
