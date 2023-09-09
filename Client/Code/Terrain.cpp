#include "stdafx.h"
#include "Terrain.h"

#include "Export_System.h"
#include "Export_Utility.h"

using namespace Engine;

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : Base(rhs)
{
}

CTerrain::~CTerrain()
{
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

void CTerrain::Free()
{
    SUPER::Free();
}


HRESULT CTerrain::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->m_vScale.x = 1.f;

    return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Engine::Add_RenderGroup(RENDER_NONALPHA, this);

    return 0;
}

void CTerrain::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CTerrain::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  //  m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    m_pTextureComp->Render_Texture(0);
    m_pBufferComp->Render_Buffer();

   // m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTerrain::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CTerrainTexComponent>(ID_STATIC, L"Com_Buffer", L"Proto_TerrainBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_Texture", L"Proto_TerrainTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransform>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);

#pragma region ÄÄÆ÷³ÍÆ®
    //CComponent* pComponent = nullptr;

    // ¹öÆÛ ÄÄÆ÷³ÍÆ®
    /*pComponent = m_pBufferCom = dynamic_cast<CTerrainTexComponent*>(Engine::Clone_Proto(L"Proto_TerrainComp"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });*/
    //NULL_CHECK_RETURN(m_pBufferCom = Set_DefaultComponent_FromProto<CTerrainTexComponent>(ID_STATIC, L"Com_Buffer", L"Proto_TerrainComp"));

    //// ÅØ½ºÃ³ ÄÄÆ÷³ÍÆ®
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TerrainTexture"));
    //NULL_CHECK_RETURN(pComponent, E_FAIL);
    //m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
    //NULL_CHECK_RETURN(m_pTextureCom = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_Texture", L"Proto_TerrainTexture"));

    //// Æ®·£½ºÆû ÄÄÆ÷³ÍÆ®
    //pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    //NULL_CHECK_RETURN(pComponent, E_FAIL);
    //m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
    //NULL_CHECK_RETURN(m_pTransformCom = Set_DefaultComponent_FromProto<CTransform>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"));
#pragma endregion

    return S_OK;
}
