#include "stdafx.h"
#include "AceFood.h"

#include "Export_System.h"
#include "Export_Utility.h"

CAceFood::CAceFood(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceFood::CAceFood(const CAceFood& rhs)
    : Base(rhs)
{
}

CAceFood::~CAceFood()
{
}

CAceFood* CAceFood::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);
    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }

    pInstance->FoodName(pObjTag);
    return pInstance;
}

HRESULT CAceFood::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_Pos({ 25.f, 1.f, 25.f });

    return S_OK;
}

_int CAceFood::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 지형타기 ------------------------------------
    Height_On_Terrain();

    // 빌보드 --------------------------------------
    BillBoard(fTimeDelta);

    // 상호작용 ------------------------------------


    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceFood::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceFood::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAceFood::Free()
{
    __super::Free();
}

HRESULT CAceFood::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ObjectTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    return S_OK;
}

void CAceFood::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 0.5f, vPos.z);
}

HRESULT CAceFood::BillBoard(const _float& fTimeDelta)
{
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

    return S_OK;
}

void CAceFood::FoodName(const _tchar* pObjTag)
{
    if ((wcscmp(pObjTag, L"APPLE") == 0) || (wcscmp(pObjTag, L"Apple") == 0))
    {
        m_pReceiveName = FOOD_NAME::APPLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Apple");
    }
    else if ((wcscmp(pObjTag, L"EATENAPPLE") == 0) || (wcscmp(pObjTag, L"EatenApple") == 0))
    {
        m_pReceiveName = FOOD_NAME::EATENAPPLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"EatenApple");
    }
    else if ((wcscmp(pObjTag, L"BANANA") == 0) || (wcscmp(pObjTag, L"Banana") == 0))
    {
        m_pReceiveName = FOOD_NAME::BANANA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Banana");
    }
    else if ((wcscmp(pObjTag, L"BANANAPEEL") == 0) || (wcscmp(pObjTag, L"BananaPeel") == 0))
    {
        m_pReceiveName = FOOD_NAME::BANANAPEEL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"BananaPeel");
    }
    else if ((wcscmp(pObjTag, L"COLA") == 0) || (wcscmp(pObjTag, L"Cola") == 0))
    {
        m_pReceiveName = FOOD_NAME::COLA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Cola");
    }
    else if ((wcscmp(pObjTag, L"MEDIKIT") == 0) || (wcscmp(pObjTag, L"Medkit") == 0))
    {
        m_pReceiveName = FOOD_NAME::MEDIKIT;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Food", L"Medkit");
    }
    else
    {
        m_pReceiveName = FOOD_NAME::FOOD_END;
    }
}
