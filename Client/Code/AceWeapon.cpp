#include "stdafx.h"
#include "AceWeapon.h"

#include "Export_System.h"
#include "Export_Utility.h"

CAceWeapon::CAceWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceWeapon::CAceWeapon(const CAceWeapon& rhs)
    : Base(rhs)
{
}

CAceWeapon::~CAceWeapon()
{
}

CAceWeapon* CAceWeapon::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("WeaponObject Create Failed");
        return nullptr;
    }

    pInstance->WeaponName(pObjTag);
    return pInstance;
}

HRESULT CAceWeapon::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_Pos({ 25.f, 1.f, 25.f });

    return S_OK;
}

_int CAceWeapon::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 지형타기 
    Height_On_Terrain();

    // 빌보드 --------------------------------------
    BillBoard(fTimeDelta);


    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceWeapon::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceWeapon::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAceWeapon::Free()
{
    __super::Free();
}

HRESULT CAceWeapon::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ObjectTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    return S_OK;
}

void CAceWeapon::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

HRESULT CAceWeapon::BillBoard(const _float& fTimeDelta)
{
    //case1. 회전행렬 만들기 
    _matrix		matWorld;
    _vec3 vPlayerPos;

    matWorld = *m_pTransformComp->Get_Transform();

    CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    _vec3 Pos = m_pTransformComp->Get_Pos();

    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    // 회전행렬 생성
    _matrix rotationMatrix;
    D3DXMatrixRotationY(&rotationMatrix, rad);

    m_pTransformComp->Set_WorldMatrixS(&(rotationMatrix * matWorld));

    // case2. 빌보드 구성하기 
    //_matrix		matWorld, matView, matBill;

    //matWorld = *m_pTransformComp->Get_Transform();

    //m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    //D3DXMatrixIdentity(&matBill);

    //matBill._11 = matView._11;
    //matBill._13 = matView._13;
    //matBill._31 = matView._31;
    //matBill._33 = matView._33;

    //D3DXMatrixInverse(&matBill, 0, &matBill);

    //m_pTransformComp->Set_WorldMatrixS(&(matBill * matWorld));

   // m_pTransformComp->Set_ScaleY(1.9f);

    return S_OK;
}

void CAceWeapon::WeaponName(const _tchar* pObjTag)
{
    if (_tcscmp(pObjTag, _T("BOTTLE")) == 0)
    {
        m_pReceiveName = WEAPON_NAME::BOTTLE;
        //m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Apple");
    }
    else if (_tcscmp(pObjTag, _T("PIPE")) == 0)
    {
        m_pReceiveName = WEAPON_NAME::PIPE;
    }
    else if (_tcscmp(pObjTag, _T("FRYINGPAN")) == 0)
    {
        m_pReceiveName = WEAPON_NAME::FRYINGPAN;
    }
    else if (_tcscmp(pObjTag, _T("PISTOL")) == 0)
    {
        m_pReceiveName = WEAPON_NAME::PISTOL;
    }
    else if (_tcscmp(pObjTag, _T("TOMMYGUN")) == 0)
    {
        m_pReceiveName = WEAPON_NAME::TOMMYGUN;
    }
    else if (_tcscmp(pObjTag, _T("GASCANISTER")) == 0)
    {
        m_pReceiveName = WEAPON_NAME::GASCANISTER;
    }
    else
    {
        m_pReceiveName = WEAPON_NAME::WEAPON_END;
    }
}
